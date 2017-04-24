#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

struct Test {
	int a;
};

int main () {

	int **ipp;
	int i = 5, j = 6;
	int *ip1 = &i;
	int *ip2 = &j;

	struct Test* test = malloc (sizeof (struct Test));
	test->a = 3;

	struct Test* test2 = malloc (sizeof (struct Test));
	test2 = test;

	ip1 = ip2;
	ipp = &ip1;
	// ipp -> ip1 -> ip2 -> j=6
	j = 7;
	test2->a = 5;
	printf ("ipp:%d ip:%d\n", **ipp, *ip1);
	printf ("test.a:%d, test2.a:%d\n", test->a, test2->a);

	return 0;
}
