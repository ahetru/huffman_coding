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

/* u32* sort_symbols(u32 symbols[256]) */
/* { */
/* 	//TODO: implement a custom sorting algorithm later */
/* } */

typedef struct 
{
	u8 key;
	u32 value;
} hashtable_item;


int cmp(const void *a, const void *b)
{
	hashtable_item x = *(const hashtable_item*)a;
	hashtable_item y = *(const hashtable_item*)b;
	
	if (x.value< y.value)
		return 1;
	if (y.value < x.value)
		return -1;
	return 0;
}

u8 map_symbol_to_occurrence(const char* filename, hashtable_item table[256])
{
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		perror("Could not open file");
		return 1;
	}

	int c;	
	while((c = fgetc(f)) != EOF)
	{
		table[c].value += 1;
		printf("symbol: %c: %d\n",table[c].key, table[c].value);
	}

	qsort(table, 256, sizeof(hashtable_item), cmp);
	fclose(f);
	return 0;
}

void init_table(hashtable_item table[256])
{
	for (u16 i = 0; i < 256; ++i)
	{
		table[i].key = (u8)i;
		table[i].value = 0;
	}
}

int main(int argc, char **argv)
{
	if (argc !=2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}

	hashtable_item symbols_table[256];
	init_table(symbols_table);
	
	if (map_symbol_to_occurrence(argv[1], symbols_table))
		return 1;

	for(int i = 0; i < 256; ++i)
		if(symbols_table[i].value) 
			printf("occurrence of %u: %d\n", symbols_table[i].key,  symbols_table[i].value);


	return 0;
}
