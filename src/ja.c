#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

int main()
{
	FILE *fr, *fw;
	char buffer[10000];
	fr = fopen("nej.txt","rt");
	if (fr == NULL) {
		printf("Error opening input file\n");
		return 1;
	}
	fw = fopen("ja.txt","wt");
	if (fw==NULL) {
		printf("Error opening output file\n");
		fclose (fr);
		return 1;
	}
	srand((unsigned)time(NULL));
	while (fgets(buffer, 10000, fr) != NULL) {
		int a = rand() % 1000 + 1;
		printf ("%d\n", a);
		buffer [strcspn(buffer, "\r\n")] = 0;
		fprintf (fw, "%s %d\n", buffer, a);
		// sleep (1);
	}
	fclose(fw);
	fclose(fr);
	return 0;
}
