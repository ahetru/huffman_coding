#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"


/* hasmap pour identifier les elements de l alphabet a leur occurence dans le texte. */

// ordonner le hashmap en fonction du nombre d'occurences
//
// construire l'arbre binaire en fonction de l'occurence pour etablir les prefix codes
//
// encoder le texte

int cmp(const void *a, const void *b)
{
	int x = *(const int *)a;
	int y = *(const int *)b;
	
	if (x < y)
		return 1;
	if (y < x)
		return -1;
	return 0;
}

/* u32* sort_symbols(u32 symbols[256]) */
/* { */
/* 	//TODO: implement a custom sorting algorithm later */
/* } */

u8 map_symbol_to_occurrence(const char* filename, u32 symbol_to_occurrence_mapping[256])
{
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		perror("Could not open file");
		return 1;
	}

	int c;	
	while((c = fgetc(f)) != EOF)
		symbol_to_occurrence_mapping[c] += 1;

	qsort(symbol_to_occurrence_mapping, 256, sizeof(u32), cmp);
	fclose(f);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc !=2)
	{
		printf("Usage: %s <file_to_compress>\n", argv[0]);
		return 0;
	}

	u32 symbol_to_occurrence_mapping[256];
	memset(&symbol_to_occurrence_mapping, 0, 256 * sizeof(u32));
	
	if (map_symbol_to_occurrence(argv[1], symbol_to_occurrence_mapping))
		return 1;

	for(int i = 0; i < 256; ++i)
		if(symbol_to_occurrence_mapping[i]) 
			printf("occurrence of %c: %d\n", (char)i,  symbol_to_occurrence_mapping[i]);


	return 0;
}
