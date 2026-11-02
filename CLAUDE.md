# BUGS I (Asciiverse)

Scott Noecker's 1991 curses game (public domain, his 1991-11-15 header),
restored and brought to today's terminals; catalog id `bugs-i`, shipped as
the command `bugs-i` (the Makefile builds `bugs`). C and ncurses. Shown as
BUGS I only.

- History: tag `1.3` is Ken Corey's 1992 Linux release (credited to
  Scott), tag `grandpas-d280cc3f22a5` the 2015 C99 cleanup, then our
  commits; PATCHES.md lists them. Branch `asciiverse`, `origin`
  github.com/theasciiverse/bugs-i. No upstream remote (none exists).
- The board's contents are the game: what to clear is decided by reading
  the screen back, so every comparison goes through `letter()` (no
  attributes), and the board is a pad (`board_show()` puts it on screen;
  ncurses would shrink a window on resize). Keys are read from stdscr
  through `key()`, which handles resizes.
- `make test` runs `tests/match.c` on a dummy terminal.
- Test with the bot: `PYTHONPATH=<pyte> asciiverse/bot.py <tmux target>
  <seconds>`; build with `CFLAGS="-O1 -g -fsanitize=address,undefined"`
  for soaks. Run tests under a memory cap (`systemd-run --user --scope -p
  MemoryMax=1G ...`).
- Static build: `docker run --rm -v "$PWD":/src -w /src alpine:3.22 sh -c
  'apk add build-base ncurses-dev ncurses-static && make CURSES_CFLAGS=
  CURSES_LIBS="-lncurses -ltinfo" LDFLAGS="-static -s" CFLAGS=-O2 bugs'`.
- Preview: `asciiverse/record-preview.sh <binary> [seconds]` (tmux,
  asciinema 3, pyte); games are random, so record until a take clears a
  level (`grep -q "for Next" asciiverse/bugs-i.cast`).
- Release: tag `v1.3-av.<n>`; the workflow runs on `*-av.*`.
