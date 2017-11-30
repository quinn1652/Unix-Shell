#Quinn Murphy
#November 11, 2016
#CSCI 352 MTWF 9am
#Assignment 4

#	$Id: Makefile,v 1.8 2016/11/15 06:53:01 murphyq Exp $
FILES=msh.o builtin.o arg_parse.o exp.o
CC=gcc
CFLAGS= -g -Wall


msh: ${FILES}
	${CC} ${CFLAGS} ${FILES} -o $@

msh.o: msh.c proto.h global.h
	${CC} ${CFLAGS} -c msh.c

builtin.o: builtin.c proto.h global.h
	${CC} ${CFLAGS} -c builtin.c

arg_parse.o: arg_parse.c proto.h global.h
	${CC} ${CFLAGS} -c arg_parse.c

exp.o: exp.c proto.h global.h
	${CC} ${CFLAGS} -c exp.c

clean:
	rm ${FILES} msh
