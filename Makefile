CC = gcc
CFLAGS =-g -O3 -Wall -Wextra -pedantic -std=gnu11
LDFLAGS=-lm -lsodium

BINDIR = bin
SOURCEDIR = src

.PHONY: clean all

all : minheap graph sssp filehandling thorupzwick main

rebuild : clean all

minheap : $(SOURCEDIR)/minheap.c $(SOURCEDIR)/minheap.h
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

graph : $(SOURCEDIR)/graph.c $(SOURCEDIR)/graph.h $(BINDIR)/minheap.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

sssp : $(SOURCEDIR)/sssp.c $(SOURCEDIR)/sssp.h $(BINDIR)/graph.o $(BINDIR)/minheap.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

filehandling : $(SOURCEDIR)/filehandling.c $(SOURCEDIR)/filehandling.h $(BINDIR)/graph.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

thorupzwick : $(SOURCEDIR)/thorupzwick.c $(SOURCEDIR)/thorupzwick.h $(BINDIR)/graph.o $(BINDIR)/minheap.o $(BINDIR)/filehandling.o
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@.o

main : $(SOURCEDIR)/main.c $(SOURCEDIR)/main.h $(BINDIR)/thorupzwick.o $(BINDIR)/graph.o $(BINDIR)/minheap.o $(BINDIR)/filehandling.o
	$(CC) $(CFLAGS) $< $(BINDIR)/*.o -o $(BINDIR)/$@ $(LDFLAGS)

clean :
	rm -rf $(BINDIR)/*.o $(BINDIR)/main
