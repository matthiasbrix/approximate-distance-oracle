CC = gcc
CFLAGS =-O3 -Wall -Wextra -pedantic -std=gnu11

BINDIR = bin
SOURCEDIR = src
TESTSDIR = tests

.PHONY: clean

all : minheap graph filehandling thorupzwick main

rebuild : clean all

minheap : $(SOURCEDIR)/minheap.c $(SOURCEDIR)/minheap.h
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

graph : $(SOURCEDIR)/graph.c $(SOURCEDIR)/graph.h $(BINDIR)/minheap.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

filehandling : $(SOURCEDIR)/filehandling.c $(SOURCEDIR)/filehandling.h $(BINDIR)/graph.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

thorupzwick : $(SOURCEDIR)/thorupzwick.c $(SOURCEDIR)/thorupzwick.h $(BINDIR)/graph.o $(BINDIR)/minheap.o $(BINDIR)/filehandling.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

main : $(SOURCEDIR)/main.c $(SOURCEDIR)/main.h $(BINDIR)/thorupzwick.o $(BINDIR)/graph.o $(BINDIR)/minheap.o $(BINDIR)/filehandling.o
	$(CC) $(CFLAGS) $< $(BINDIR)/*.o -o $(BINDIR)/$@ -lm

clean :
	rm -rf $(BINDIR)/*.o $(BINDIR)/main
