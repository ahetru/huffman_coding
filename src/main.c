#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "huffman_coding.h"

int cmp(const void *a, const void *b)
{
	const t_symbol	*x = *(const t_symbol**)a;
	const t_symbol	*y = *(const t_symbol**)b;
	
	if (x->frequency > y->frequency)
		return 1;
	if (y->frequency > x->frequency)
		return -1;
	return 0;
}

void free_arr(t_symbol **arr, u16 count)
{
	for (u16 i = 0; i < count; ++i)
		free(arr[i]);
	free(arr);
}

void free_tree(t_symbol *tree)
{
	if (!tree)
		return;
	free_tree(tree->left);
	free_tree(tree->right);
	free(tree);
}

void free_on_error(t_parser *data)
{
	for (u16 i = 0; i < data->count; ++i)
		free_tree(data->arr_symbols[i]);
	free(data->arr_symbols);
}

u8 parse_file_and_set_symbols(const char* filename, t_parser *data)
{

	u64 frequency[MAX_SYMBOLS];
	memset(frequency, 0, sizeof(frequency));
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


void print_tree(t_symbol *symbol)
{
	if (!symbol)
		return;

	if (symbol->value == INTERNAL_NODE)
			printf("Node: internal node with p: %p | ", symbol);
	else
		printf("Node: %c | ", symbol->value);
	if (symbol->left)
		if (symbol->left->value == INTERNAL_NODE)
			printf("Left child: internal node with p: %p | ", symbol->left);
		else
			printf("Left child: %c | ", symbol->left->value);
	else
		printf("Left child: NULL | ");

	if (symbol->right)
		if (symbol->right->value == INTERNAL_NODE)
			printf("Right child: internal node with p: %p | ", symbol->right);
		else
			printf("Right child: %c", symbol->right->value);
	else
		printf("Right child: NULL");

	printf("\n");
	print_tree(symbol->left);
	print_tree(symbol->right);	
}

t_symbol *build_huffman_tree(t_parser *data)
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

void print_symbol(t_symbol *s)
{
	if (s->value == INTERNAL_NODE)
		printf("value: internal node, ");
	else
		printf("value: %c, ", s->value);
	printf("frequency: %lu\n", s->frequency);
}

int main(int argc, char **argv)
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

	for (u16 i = 0; i < data.count; ++i)
		print_symbol(data.arr_symbols[i]);
	t_symbol *huffman_tree = build_huffman_tree(&data);
	if (!huffman_tree)
		return 1;

	print_tree(huffman_tree);
	free_tree(huffman_tree);

	return 0;
}
