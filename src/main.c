#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "huffman_coding.h"

u8	parse_file_and_set_symbols(const char* filename, t_parser *data)
{
	u64 frequency[MAX_SYMBOLS] = {0};
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		perror("Could not open file");
		return 1;
	}

	int c;	
	while((c = fgetc(f)) != EOF)
	{
		if (frequency[c] == 0)
			data->count++;
		frequency[c]++;
	}
	t_symbol ** arr_symbols = malloc(sizeof(t_symbol*) * data->count);
	if (arr_symbols == NULL)
	{
		fclose(f);
		return 1;
	}

	u16 j = 0;
	for (u16 i = 0; i < MAX_SYMBOLS; ++i)
		if (frequency[i])
		{
			t_symbol *s = malloc(sizeof(t_symbol));
			if (!s)
			{
				free_arr(arr_symbols, j);
				exit(1);
			}
			s->value = i;
			s->frequency = frequency[i];
			s->prefix_code = 0;
			s->code_length = 0;
			s->left = NULL;
			s->right = NULL;
			arr_symbols[j] = s;
			j++;
		}
	fclose(f);
	data->arr_symbols = arr_symbols;

	return 0;
}


t_symbol	*build_huffman_tree(t_parser *data)
{
	if (data == NULL || data->count == 0)
		return NULL;
	while(data->count > 1)
	{
		qsort(data->arr_symbols,data->count, sizeof(*data->arr_symbols), cmp);

		t_symbol *root = malloc(sizeof(t_symbol));
		if (root == NULL)
		{
			free_on_error(data);
			exit(1);
		}
		t_symbol *left = data->arr_symbols[0];
		t_symbol *right = data->arr_symbols[1]; 
		
		root->value = INTERNAL_NODE;
		root->frequency = left->frequency + right->frequency;
		root->prefix_code = 0;
		root->code_length = 0;
		root->left = left;
		root->right = right;

		data->arr_symbols[0] = root;

		for (u16 i = 1; i < data->count - 1; ++i)
			data->arr_symbols[i] = data->arr_symbols[i + 1];
		data->count--;
	}
	t_symbol *root = data->arr_symbols[0];
	free(data->arr_symbols);

	return root;
}

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}
	t_parser data = {0};
	parse_file_and_set_symbols(argv[1], &data);
	if (data.arr_symbols == NULL)
		return 1;

	/* for (u16 i = 0; i < data.count; ++i) */
	/* 	print_symbol(data.arr_symbols[i]); */
	t_symbol *huffman_tree = build_huffman_tree(&data);
	if (!huffman_tree)
		return 1;

	print_tree(huffman_tree);
	free_tree(huffman_tree);

	return 0;
}
