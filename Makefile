#makefile for BUGS I - SRN 7/12/91 
# modified for Linux on 6/15/92 Ken Corey

CC= gcc
CFLAGS= -O3
OBJS= main.o startup.o loop.o meat.o

#bugs : &  $(OBJS)
bugs :  $(OBJS)   
#use above line if parallel processing is not supported
	${CC} ${CFLAGS} -o bugs $(OBJS) -lcurses -ltermcap 

main.o: main.c info.h
	${CC} ${CFLAGS} -DLINUX -o main.o main.c

$(OBJS):  info.h	

clean: 
	rm -f *.o core temp.c a.out
