#include <stdlib.h>
#include <stdio.h>
#include "uthash.h"

struct my_struct {
	int id;                    /* key */
	char name[10];
	UT_hash_handle hh;         /* makes this structure hashable */
};

int main(int argc, char *argv[]) {
	argc = argc;
	argv = argv;

	return 0;
}
