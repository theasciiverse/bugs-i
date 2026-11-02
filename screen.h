#ifndef __SCREEN_H
#define __SCREEN_H

#include <curses.h>

/* The 1991 layout, kept as it was: the stats from column 0, the next
   piece on row 1 above the board, the board at row 2, column 32, and the
   signature in the bottom right corner. It is drawn as one frame of
   FRAME_W x FRAME_H, centred in the window from (oy, ox). */
#define FRAME_W 42
#define FRAME_H 21

extern int oy, ox;

void colours(void);
chtype glyph(chtype letter, int bug);
void layout(void);
void redraw(void);
void stats(void);
void show_next(chtype a, chtype b);
void board_show(void);
int key(void);

#endif // __SCREEN_H
