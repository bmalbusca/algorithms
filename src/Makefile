#
#Makefile 
#	target:dependencies
#	     action

CC=gcc
CFLAGS2=-g -Wall -Wextra -pedantic -std=c99 
CFLAGS= -Wall -std=c99 -O3 

walker: num.c files.c 
	$(CC) $(CFLAGS) files.c num.c  -o walker

# Make (compiling) for testing 
clean:
	rm -r -f run walker *.vrf *.out *.dSYM *~

run: num.c files.c 
	$(CC) $(CFLAGS2) files.c num.c  -o walker


# Test files directories 
FILES_docker = $(shell ls ./docker_test/*)
FILES = $(shell ls ../test/*)
OUT_FILES = $(shell ls ../test/*.path)

# Valgrind flags
VALG = valgrind --leak-check=full -v --track-origins=yes 
HEAP = valgrind --tool=massif  --time-unit=B --massif-out-file=./walker.out 

# docker tests
docker:
	make clean; make; for F in ${FILES_docker}; do ${VALG} ./walker $${F} ; done;

heap_docker:
	rm *.out; for F in ${FILES_docker}; do ${HEAP} ./walker $${F} ; done; ms_print walker.out ;


# linux terminal 
t:
	make clean; make; for F in ${FILES}; do time ./walker $${F} ;done; for T in ${OUT_FILES}; do echo $${T} && cat $${T}; done; 

tt: 	
	make clean; make; for F in ${FILES}; do /usr/bin/time -l ./walker $${F} ;done; for T in ${OUT_FILES}; do echo $${T} && cat $${T}; done; 

	
v:
	for F in ${FILES}; do ${VALG} ./walker $${F} ; done;

h: 	
	rm *.out; for F in ${FILES}; do ${HEAP} ./walker $${F} ; done; ms_print walker.out ;



