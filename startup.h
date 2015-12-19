#ifndef __STARTUP_H
#define __STARTUP_H

#include <curses.h>
#include <stdlib.h>

#ifndef chtype                  /*some curses define chtype, others.. */
#define chtype unsigned char
#endif

#define rx(a,b) ((random()%a)+b)        /*probably should use function here */

void nextpiece();
void newstart();
void start();

#endif // __STARTUP_H
