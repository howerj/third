
CC     ?= gcc
CCOPTS ?= -ansi -Wall -Wextra -pedantic -Os

SOURCES := ${wildcard *.c}
DEPS    := ${SOURCES:%.c=%.d}
OBJECTS := ${SOURCES:%.c=%.o}

all: third 

run: third
	@cat third.fth - | ./third

third: ${OBJECTS}

-include ${DEPS}


third.o: third.c third.h
	${CC} ${CCOPTS} -c third.c -o third.o

clean:
	@rm -vf third *.o

