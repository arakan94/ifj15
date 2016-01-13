# Makefile
# FIT VUT, IFJ 089
# Author: David Novák

CC=gcc
CFLAGS= -std=c11 -Wall -Wextra -pedantic -O3

MODULES = scanner.o semantic.o str.o stack.o sym_tab.o ial.o fce.o parser.o psa.o tac.o psa_stack.o functions.o main.o

all: build

build: $(MODULES) error_codes.h
	$(CC) $(CFLAGS) $(MODULES) -o ifj15

main.o: main.c scanner.o parser.o semantic.o
	$(CC) $(CFLAGS) -c main.c

scanner.o: scanner.c scanner.h str.o
	$(CC) $(CFLAGS) -c scanner.c

str.o: str.c str.h
	$(CC) $(CFLAGS) -c str.c

parser.o: parser.c parser.h scanner.o functions.o str.o psa.o
	$(CC) $(CFLAGS) -c parser.c

psa.o: psa.c psa.h functions.o scanner.o psa_stack.o semantic.o
	$(CC) $(CFLAGS) -c psa.c

psa_stack.o: psa_stack.c psa_stack.h semantic.o
	$(CC) $(CFLAGS) -c psa_stack.c

semantic.o: semantic.c semantic.h sym_tab.o
	$(CC) $(CFLAGS) -c semantic.c

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c

sym_tab.o: sym_tab.c sym_tab.h ial.o stack.o
	$(CC) $(CFLAGS) -c sym_tab.c

ial.o: ial.c ial.h
	$(CC) $(CFLAGS) -c ial.c

fce.o: fce.c fce.h ial.o
	$(CC) $(CFLAGS) -c fce.c

tac.o: tac.c tac.h
	$(CC) $(CFLAGS) -c tac.c

functions.o: functions.c functions.h
	$(CC) $(CFLAGS) -c functions.c

scanner_selftest: scanner_selftest.o str.o
	$(CC) $(CFLAGS) scanner_selftest.o str.o -o scanner_selftest

scanner_selftest.o: scanner.c scanner.h debug.h
	$(CC) $(CFLAGS) -DSCANNER_DEBUG -DSCANNER_SELFTEST -c scanner.c -o scanner_selftest.o

clean:
	rm -f *.o ifj scanner_selftest *~
