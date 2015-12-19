#ifndef __BUGS_H
#define __BUGS_H

void bleed(int filed, int xof);
void fatal(char *s);
void fatal_err(int e);
int get_option(char *s, int min, int max);

#endif // __BUGS_H
