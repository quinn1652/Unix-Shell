#Quinn Murphy
#October 18, 2016
#CSCI 352 MTWF 9am
#Assignment 3

#	$Id: Makefile,v 1.6 2016/10/18 21:02:33 murphyq Exp $
FILES=msh.o builtin.o arg_parse.o exp.o
CC=gcc
CFLAGS= -g -Wall

msh: ${FILES}
	${CC} ${CFLAGS} ${FILES} -o $@

clean:
	rm ${FILES} msh
