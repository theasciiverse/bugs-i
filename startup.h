#ifndef __STARTUP_H
#define __STARTUP_H

#include <curses.h>
#include <stdlib.h>

#define rx(a,b) ((random()%a)+b)        /*probably should use function here */

void nextpiece(void);
void newstart(void);
void start(void);

#endif // __STARTUP_H
