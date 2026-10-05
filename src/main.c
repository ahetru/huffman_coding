#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "huffman_coding.h"

//TODO: all of this needs a proper error handling especially on syscall fails.

int cmp(const void *a, const void *b)
{
	t_symbol	x = *(const t_symbol*)a;
	t_symbol	y = *(const t_symbol*)b;
	
	if (x.frequency > y.frequency)
		return 1;
	if (y.frequency > x.frequency)
		return -1;
	return 0;
}


t_dynamic_array	*set_symbols_frequency(const char* filename)
{

	u64 frequency[MAX_SYMBOLS];
	memset(frequency, 0, sizeof(frequency));
	u16 symbols_count = 0;
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		perror("Could not open file");
		return NULL;
	}

	int c;	
	while((c = fgetc(f)) != EOF)
		frequency[c]++;

	/* qsort(symbols, 256, sizeof(t_symbol), cmp); */
	t_dynamic_array *arr_symbols = calloc(1, sizeof(t_dynamic_array));
	if (arr_symbols == NULL)
	{
		fclose(f);
		return NULL;
	}
	for (u16 i = 0; i < MAX_SYMBOLS; ++i)
		if (frequency[i])
		{
			t_symbol s;
			s.value = i;
			s.frequency = frequency[i];
			s.prefix_code = 0;
			s.code_length = 0;
			s.left = NULL;
			s.right = NULL;
			append_dynamic_array(arr_symbols, s);
			++symbols_count;
		}
	//TODO:does not close file if malloc or realloc fail
	fclose(f);
	return arr_symbols;
}

t_symbol cpy_symbol(t_symbol src)
{
	t_symbol cpy;
	cpy.value = src.value;
	cpy.frequency = src.frequency;
	cpy.prefix_code =src.prefix_code;
	cpy.code_length = src.code_length;
	cpy.left = src.left;
	cpy.right = src.right;
	return cpy;
}

void print_tree(t_symbol *symbol)
{
	if (!symbol)
		return;
	
    printf("Node: %c | ", symbol->value);
    if (symbol->left)
        printf("Left child: %c | ", symbol->left->value);
    else
        printf("Left child: NULL | ");

    if (symbol->right)
        printf("Right child: %c", symbol->right->value);
    else
        printf("Right child: NULL");

    printf("\n");

    print_tree(symbol->left);
    print_tree(symbol->right);	
}

void print_symbol(t_symbol *s)
{
	u8 is_leaf = !(s->left) && !(s->right);
	printf("value: %c, frequency: %lu, prefix_code: %u, code_length: %u me: %p left: %p, right: %p, is_leaf: %u\n", s->value, s->frequency, s->prefix_code, s->code_length, s, s->left, s->right, is_leaf);
}

void print_dynamic_array(t_dynamic_array *da)
{
	printf("******************************DELIMETER******************************\n");
	for(u16 i = 0; i < da->count; ++i)
		print_symbol(&da->symbols[i]);
	printf("******************************DELIMETER******************************\n");
}



/* t_symbol *build_huffman_tree(t_dynamic_array *da) */
/* { */
/* 	t_dynamic_array *result = calloc(1, sizeof(t_dynamic_array)); */

/* 	u16 i = 0; */
/* 	t_symbol *s; */
/* 	while(da->count - i >= 2) */
/* 	{ */
/* 		qsort(da->symbols + i, da->count - i, sizeof(t_symbol), cmp); */
/* 		/1* printf("current i value: %u\n", i); *1/ */
/* 		/1* print_dynamic_array(da); *1/ */
/* 		/1* printf("first element: %c\n", da->symbols[i].value); *1/ */
/* 		t_symbol lowest_frequency_symbol 	= cpy_symbol(da->symbols[i]); */
/* 		t_symbol second_lowest_frequency_symbol	= cpy_symbol(da->symbols[i + 1]); */
/* 		append_dynamic_array(result, lowest_frequency_symbol); */
/* 		append_dynamic_array(result, second_lowest_frequency_symbol); */
	
		
/* 		t_symbol s; */
/* 		s.value = 6400;// TODO:find a value to define internal node or path node */
/* 		s.frequency = lowest_frequency_symbol.frequency + second_lowest_frequency_symbol.frequency; */
/* 		s.prefix_code = 0; */
/* 		s.code_length = 0; */
/* 		s.left = &result->symbols[i]; */
/* 		s.right = &result->symbols[i+1]; */
/* 		da->symbols[i + 1] = s; */
/* 		/1* append_dynamic_array(result, s); *1/ */
/* 		++i; */ 
/* 	} */
/* 	return result; */
/* } */

t_symbol *build_huffman_tree(t_dynamic_array *da)
{
	t_dynamic_array *result = calloc(1, sizeof(t_dynamic_array));

	u16 i = 0;
	t_symbol *root;
	while(da->count - i >= 2)
	{
		qsort(da->symbols + i, da->count - i, sizeof(t_symbol), cmp);
		t_symbol lowest_frequency_symbol 	= cpy_symbol(da->symbols[i]);
		t_symbol second_lowest_frequency_symbol	= cpy_symbol(da->symbols[i + 1]);
		
		
		++i; 
	}
	return root;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}
	
	t_dynamic_array *da_symbols = set_symbols_frequency(argv[1]);
	if (da_symbols == NULL)
		return 1;

	/* printf("da count: %u\n", da_symbols->count); */
	/* qsort(da_symbols->symbols, da_symbols->count, sizeof(t_symbol), cmp); */
	/* for(int i = 0; i < da_symbols->count; ++i) */
	/* 	printf("frequency of %c: %d\n", da_symbols->symbols[i].value,  da_symbols->symbols[i].frequency); */

	t_dynamic_array *huffman_tree = build_huffman_tree(da_symbols);
	/* print_dynamic_array(da_symbols); */
	print_dynamic_array(huffman_tree);
	/* print_tree(&huffman_tree->symbols[huffman_tree->count - 1]); */
	free(da_symbols->symbols);
	free(da_symbols);
	free(huffman_tree->symbols);
	free(huffman_tree);
	return 0;
}
