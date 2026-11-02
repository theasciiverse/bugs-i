# What Asciiverse changed

BUGS I is Scott Noecker's curses game of 1991 (begun 1991-07-10 at SUNY
CT at Utica/Rome, later work at UNCW), which he placed in the public
domain on 1991-11-15: "I, being of a somewhat sound mind, hearby place
this in the great world of public domain" (header of `bugs.c`, `main.c`
in 1991). It never had a repository; this one is built in the order
Asciiverse uses for such titles:

1. **1.3, Ken Corey's Linux release** (tag `1.3`, credited to Scott
   Noecker): `dr_mario.tar.Z` as Ken Corey released it for Linux on
   1992-06-15, imported unchanged (sha256
   `7376e4e4bd39ca7466c7d3d367ac11c34d4e8d39c63e984f83ae4c0f0fc15263`); the
   same files as `dr_mario.tar.gz` in sunsite.unc.edu's games folder as
   archived on 1996-09-29 at ibiblio.org/pub/historic-linux.
2. **Grandpa's Code's 2015 cleanup** (tag `grandpas-d280cc3f22a5`): C99
   and POSIX, nanosleep for usleep, a README.rst with the history, from
   the archive of Mercurial node d280cc3f22a5 of bitbucket.org/grandpas/code
   (sha256 `1cb5c83637a91152e418521ca1f7f611f39a2e856b2aac6b99b785f4ac114b1c`).
   That repository is gone, so its ten commits are not available.
3. **Ours**, below, one commit each.

The game is shown as BUGS I, its own name.

## Restored

- **Bugs can be cleared again.** A bug is drawn in reverse video and a
  piece is not, and the game decides what to clear by comparing what the
  screen holds. 1991's BSD curses returned only the letter; ncurses
  returns the attributes too, so a bug never matched its own letter, no
  bug could be removed and no level won. Every comparison now reads the
  letter alone (`letter()` in `info.h`). `tests/match.c` (`make test`)
  checks it on a dummy terminal.
- **Builds with current compilers.** The globals were defined in a
  header that four files include (only links with GCC before 10); they
  are defined once in `state.c`. A `#define chtype unsigned char` meant
  for old curses fired against ncurses' typedef and made the pieces one
  byte wide in `startup.c`. The Makefile asked `ncurses5-config`; it now
  asks pkg-config and falls back to `-lncurses`.
- **The right cells settle after a removal.** `setac()` had lost the
  braces round its left-hand case (in 1992 already), adding a cell with a
  stale row; the list of cells (`t[48]`) could overflow in a long chain
  and now has room for every cell three times over, written only through
  `mark()`.

## For today's terminals

The rules and the 1991 look (the letters A, L and S, reverse-video bugs,
the board with its `___!  !___` top, the stats and the S. signature) are
kept.

- **Keys:** the arrow keys alongside h, j and l; Up, x or s turn
  clockwise and z or a the other way; space drops; p pauses (BUGS I 1.3
  shows under the stats); q or Esc leaves the game. Read through curses
  instead of switching stdin to non-blocking with fcntl.
- **Colour:** A red, L yellow, S cyan; pieces bold, bugs in reverse video
  in their colour; standout without colour, as in 1991.
- **Centred, and redrawn on resize,** with a note while the window is
  smaller than the 42x21 frame. The board is a curses pad: ncurses shrinks
  windows that no longer fit and never grows them back, and the board's
  contents are the game.
- **A title screen** for level (0 to 20) and speed (1 to 3); **game over**
  shown on the board with the score and the best score, kept in
  `$XDG_DATA_HOME/bugs-i/best` (`~/.local/share/bugs-i/best`); then the
  title screen again.
- **Quitting:** Ctrl+C, Ctrl+\, SIGTERM and SIGHUP put the terminal back
  and exit with status 0; Ctrl+Z suspends as usual instead of quitting.
  The random seed takes the time too.
- **Speed up every ten pieces,** as the 1992 README says the game does
  ("play speed will increase every 10 pieces"); the code only counted
  them. Each ten pieces of a level take a tenth off the pause between
  steps, down to 40 ms; each level starts at the chosen speed. Its own
  commit, so it can be taken back alone.
- Scott's own account (uid 952) still has the k key.

## Packaging

- `.github/workflows/release.yml`: av archives `bugs-i-<target>` for Linux
  (static musl with ncurses linked in, x86-64 and ARM64, built and tested
  in Alpine) and macOS (x86-64 and ARM64, the system ncurses), with the
  game as `bugs-i`, the READMEs and this file, plus SHA256SUMS; and .deb,
  .rpm, .apk and Arch packages of the static Linux binary by nfpm
  (`asciiverse/nfpm.yaml`): `/usr/bin/bugs-i`, no menu entry. No Windows
  (POSIX signals, getuid, mkdir). Runs on tags `*-av.*`.
- `asciiverse/bot.py`, a player that reads the board from a tmux pane and
  places each piece, used for testing and the preview;
  `asciiverse/record-preview.sh` records `asciiverse/bugs-i.cast` with it
  (100x30: the title screen, then levels 0 and 1 cleared).

## Checked

- `make test`, and the bot through many games and levels, with resizes
  from 1x1 to 150x45, under AddressSanitizer and UBSan: no report (UBSan
  found the shrunken-board fault above). Static musl build in Alpine 3.22
  (282 KB). macOS not built here.
