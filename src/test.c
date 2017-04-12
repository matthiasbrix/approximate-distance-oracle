#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "uthash.h"
#include "sodium.h"

struct my_struct {
	int id;            /* we'll use this field as the key */
	int weight;
	UT_hash_handle hh; /* makes this structure hashable */
};

struct my_struct *users = NULL;

void add_user(struct my_struct *s, int user_id, int weight) {
	s->id = user_id;
	s->weight = weight;
	HASH_ADD_INT(users, id, s);
}

/* Random integer in [0, limit) */
/* unsigned int random_uint(unsigned int limit) { */
/* 	union { */
/* 		unsigned int i; */
/* 		unsigned char c[sizeof(unsigned int)]; */
/* 	} u; */

/* 	do { */
/* 		if (!RAND_bytes(u.c, sizeof(u.c))) { */
/* 			fprintf(stderr, "Can't get random bytes!\n"); */
/* 			exit(1); */
/* 		} */
/* 	} while (u.i < (-limit % limit)); /\* u.i < (2**size % limit) *\/ */
/* 	return u.i % limit; */
/* } */

int main () {
	/* int a = 0; */
	/* struct my_struct *s; */
	/* s = malloc(sizeof(struct my_struct)); */
	/* add_user (s, a, 14); */
	/* int b = 1; */
	/* // s is output pointer */
	/* HASH_FIND_INT(users, &b, s); */
	/* HASH_DEL(users, s); */
	/* unsigned int num_users; */
	/* num_users = HASH_COUNT(users); */
	/* printf("there are %u objects\n", num_users); */
	// ************************************
	/* int *pos = malloc (sizeof (int)); */
	/* unsigned int i = 0; */
	/* pos[i] = 1; */
	/* i += 1; */
	/* pos[i] = 2; */
	/* 	i += 1; */
	/* pos[i] = 2; */
	/* 	i += 1; */
	/* pos[i] = 2; */
	/* 	i += 1; */
	/* pos[i] = 2; */
	/* 	i += 1; */
	/* pos[i] = 2; */
	/* for (i = 0; i < 10; i++) */
	/* 	printf ("%d\n", pos[i]); */

	// ************************************
	/* heap_t *heap = malloc (sizeof (heap_t)); */
	/* heap->nodes = malloc(5 * sizeof(struct node)); */
	/* for (unsigned i = 0; i < 5; i++) */
	/* 	printf ("%lu\n", sizeof(heap->nodes[i])); */

	// ************************************
	/* int t[] = {3, 2, 1}; */
	/* t[0] = t[0]; */
	/* printf ("%d\n", t[0]); */

	/* for (unsigned int i = 0; i < 30; i++) { */
	/* 	int divisor = RAND_MAX/(1000+1); */
	/* 	int retval; */

	/* 	do { */
	/* 		retval = rand() / divisor; */
	/* 	} while (retval > 1000); */

	/* 	printf ("r: %d\n", retval); */
	/* } */


	/* for (unsigned int i = 0; i < 30; i++) */
	/* 	printf ("%d\n", rand() % (10 - 0 + 1) + 0); */


	int p = 0, i;
	for (i = 0; i < 20; i++) {
		p = rand() % 400;
		if (p > 360)
			printf("%d ", 0);
		else if (p < 0)
			printf("%d ", 0);
		else
			printf("%f ", (p * 0.1 / 360)*10);

	}
	printf("...\n");

	srand((unsigned)time(NULL)); // Set the seed
	for (int i = 0; i < 20; i++) {
		double sample = (double)rand()/RAND_MAX;
		printf ("%f\n", sample);
	}

	char myString[32];
	uint32_t myInt;

	randombytes_buf(myString, 32);
	/* myString will be an array of 32 random bytes, not null-terminated */
	myInt = randombytes_uniform(10);
	/* myInt will be a random number between 0 and 9 */

	printf ("%d\n", myInt);

	printf ("%f %f\n", (-1.0/3.0), (pow (5, -1.0/(double)3)));

	// gcc test.c -o test /usr/local/lib/libsodium.a

	return 0;
}
