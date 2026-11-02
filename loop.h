#ifndef __LOOP_H
#define __LOOP_H

/* how a level ended */
#define LEVEL_DONE 0
#define GAME_OVER 1
#define QUIT 2

int loop(void);
int loop2(void);
int mvcheck(int y1, int x1);

#endif // __LOOP_H
