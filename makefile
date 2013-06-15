#
# Third: Makefile
# @author         Richard James Howe.
# @email          howe.r.j.89@gmail.com
#


## Colors

BLUE=\e[1;34m
GREEN=\e[1;32m
RED=\e[1;31m
DEFAULT=\e[0m

## Compiler options

CC=gcc
CCOPTS=-ansi -g -Wall -Wno-write-strings -Wshadow -Wextra -pedantic -O2
#CCOPTS=-ansi --coverage -g -Wall -Wno-write-strings -Wshadow -Wextra -pedantic -O2

## Help

all: banner third run

banner:
	@echo -e "Third, GNU Makefile\n"
	@echo -e "Author:    $(BLUE)Richard James Howe$(DEFAULT)."
	@echo -e "Email:     $(BLUE)howe.r.j.89@gmail.com$(DEFAULT)."
	@echo -e "  This program is derived from a program availble from the"
	@echo -e "  the IOCCC, see entry buzzard.2 from 1992."
	@echo -e "\n"


help:
	@echo "Options:"
	@echo "make"
	@echo "     Print out banner, this help message and compile program."
	@echo "make help"
	@echo "     This help message."
	@echo "make run"
	@echo "		  Run the program."
	@echo "make third"
	@echo "     Compile third."
	@echo "make pretty"
	@echo "     Indent source, print out word count, clean up directory."
	@echo "make clean"
	@echo "     Clean up directory."
	@echo "make gcov"
	@echo "     Compile program with coverage options, run program, run gcov."
	@echo "make splint"
	@echo "     Run splint on all *.c and *.h source files."
	@echo "make html"
	@echo "     Compile the documentation."
	@echo "make valgrind"
	@echo "     Run program with valgrind on start up forth file only."

run: third
	@echo "cat third.fs - | ./third"
	@cat third.fs - | ./third

## Main forth program

third: main.c third.h third.o
	$(CC) $(CCOPTS) main.c third.o -o third 

third.o: third.c third.h
	$(CC) $(CCOPTS) -c third.c -o third.o

## Optional extras, helper functions

# Indent the files, clean up directory, word count.
pretty: 
	@echo -e "$(BLUE)"
	@echo -e "Indent files and clean up system.$(DEFAULT)"
	@echo -e "$(GREEN)"
	@echo "indent -nut -linux *.h *.c";
	@indent -nut -linux *.h *c;
	@echo -e "$(RED)"
	@rm -vf third memory.txt *.log *.swo *.swp *.o *~ *.gcov *.gcda *.gcno *.html *.htm;
	@echo -e "$(DEFAULT)"
	@wc *.c *.h *.fs makefile

# Clean up directory.
clean:
	@echo -e "$(RED)"
	@rm -vf third memory.txt *.log *.swo *.swp *.o *~ *.gcov *.gcda *.gcno *.html *.htm;
	@echo -e "$(DEFAULT)"

# Static checking.
splint:
	@echo "Running \"splint *.c *.h &> splint.log\""
	-splint *.c *.h &> splint.log 

html:
	@echo -e "Compiling markdown to html."
	@for i in *.md; do echo "$$i > $$i.html"; markdown $$i > $$i.html; done

valgrind: third
	@echo "Running valgrind on ./third"
	@echo "  This command needs changing in the makefile"
	-cat third.fs | valgrind ./third &> valgrind.log << EOF

gcov: CCOPTS:=$(CCOPTS) --coverage
gcov: clean third
	@echo "Providing gcov statistics for third program."
	@cat third.fs | ./third << EOF
	@gcov third.c main.c


