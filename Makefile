CC = gcc
CFLAGS =-Wall -Wextra -pedantic -std=gnu11

BINDIR = bin
SOURCEDIR = src
TESTINGDIR = tests

.PHONY: clean

run : ./run_tests.sh

all : priorityqueue graph filehandling thorupzwick

rebuild : clean all

priorityqueue : $(SOURCEDIR)/priorityqueue.c $(SOURCEDIR)/priorityqueue.h
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

graph : $(SOURCEDIR)/graph.c $(SOURCEDIR)/graph.h $(BINDIR)/priorityqueue.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

filehandling : $(SOURCEDIR)/filehandling.c $(SOURCEDIR)/filehandling.h $(BINDIR)/graph.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

thorupzwick : $(SOURCEDIR)/thorupzwick.c $(SOURCEDIR)/thorupzwick.h $(BINDIR)/graph.o $(BINDIR)/priorityqueue.o $(BINDIR)/filehandling.o
	$(CC) $(CFLAGS) $< $(BINDIR)/*.o -o $(BINDIR)/$@ -lm

clean :
	rm -rf $(BINDIR)/*.o $(BINDIR)/thorupzwick
