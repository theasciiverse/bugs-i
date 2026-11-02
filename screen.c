/* Screen handling (Asciiverse): colours, the frame centred in the window
   and redrawn on resize, and key reading that copes with both. */

#include "screen.h"
#include "info.h"

int oy, ox;                     /* top left corner of the frame */

static int have_colour;
static chtype next_a = ' ', next_b = ' ';

/* One colour per letter, as the three colours of the pieces:
   A red, L yellow, S cyan. */
void colours(void)
{
    if (!has_colors())
        return;
    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_YELLOW, -1);
    init_pair(3, COLOR_CYAN, -1);
    have_colour = 1;
}

/* A letter as it is drawn: a piece bold in its colour, a bug in reverse
   video in its colour. Without colour, a bug is in standout, as in 1991. */
chtype glyph(chtype letter, int bug)
{
    chtype ch = letter & A_CHARTEXT;
    if (!have_colour)
        return bug ? (ch | A_STANDOUT) : ch;
    int pair = ch == 'A' ? 1 : ch == 'L' ? 2 : 3;
    return ch | COLOR_PAIR(pair) | (bug ? A_REVERSE : A_BOLD);
}

/* Centre the frame. While the window is too small, say so and wait. */
void layout(void)
{
    while (LINES < FRAME_H || COLS < FRAME_W)
    {
        clear();
        mvprintw(0, 0, "Make the window bigger");
        mvprintw(1, 0, "(%d x %d needed)", FRAME_W, FRAME_H);
        refresh();
        int ch;
        while ((ch = getch()) != KEY_RESIZE)
            if (ch == 'q' || ch == 27)
                break;
        if (ch != KEY_RESIZE)
            break;
    }
    oy = LINES > FRAME_H ? (LINES - FRAME_H) / 2 : 0;
    ox = COLS > FRAME_W ? (COLS - FRAME_W) / 2 : 0;
}

void stats(void)
{
    mvprintw(oy + 1, ox, "Score: %-8u", score);
    mvprintw(oy + 3, ox, "Level: %-2u", lv);
    mvprintw(oy + 5, ox, "Bugs left: %02u", v);
    mvprintw(oy + 7, ox, "Speed: %u", sp);
    refresh();
}

void show_next(chtype a, chtype b)
{
    next_a = a;
    next_b = b;
    mvaddch(oy + 1, ox + 36, glyph(a, 0));
    addch(glyph(b, 0));
    refresh();
}

/* Everything again, after a resize: the frame recentred, the board moved
   with its contents. */
void redraw(void)
{
    layout();
    clear();
    mvprintw(oy + FRAME_H - 1, ox + FRAME_W - 4, "S."); /*signature! */
    stats();
    if (next_a != ' ')
        show_next(next_a, next_b);
    refresh();
    if (w != NULL)
    {
        touchwin(w);
        board_show();
    }
}

/* The board pad, shown at its place in the frame. */
void board_show(void)
{
    if (w != NULL && LINES >= FRAME_H && COLS >= FRAME_W)
        prefresh(w, 0, 0, oy + 2, ox + 32, oy + 19, ox + 41);
}

/* A key, ERR if none is waiting (during play nothing blocks); a resize
   is dealt with here and reported as ERR. */
int key(void)
{
    int ch = getch();
    if (ch == KEY_RESIZE)
    {
        redraw();
        return ERR;
    }
    return ch;
}
