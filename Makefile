CC = gcc
CFLAGS =-Wall -Wextra -pedantic -std=gnu11

BINDIR = bin
SOURCEDIR = src
TESTSDIR = tests

.PHONY: clean

all : priorityqueue graph filehandling thorupzwick testinglibrary main

rebuild : clean all

priorityqueue : $(SOURCEDIR)/priorityqueue.c $(SOURCEDIR)/priorityqueue.h
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

graph : $(SOURCEDIR)/graph.c $(SOURCEDIR)/graph.h $(BINDIR)/priorityqueue.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

filehandling : $(SOURCEDIR)/filehandling.c $(SOURCEDIR)/filehandling.h $(BINDIR)/graph.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

thorupzwick : $(SOURCEDIR)/thorupzwick.c $(SOURCEDIR)/thorupzwick.h $(BINDIR)/graph.o $(BINDIR)/priorityqueue.o $(BINDIR)/filehandling.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

testinglibrary : $(SOURCEDIR)/testinglibrary.c $(SOURCEDIR)/testinglibrary.h $(BINDIR)/graph.o $(BINDIR)/priorityqueue.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

main : $(SOURCEDIR)/main.c $(SOURCEDIR)/main.h $(BINDIR)/thorupzwick.o $(BINDIR)/graph.o $(BINDIR)/priorityqueue.o $(BINDIR)/filehandling.o $(BINDIR)/testinglibrary.o
	$(CC) $(CFLAGS) $< $(BINDIR)/*.o -o $(BINDIR)/$@ -lm

clean :
	rm -rf $(BINDIR)/*.o $(BINDIR)/main
