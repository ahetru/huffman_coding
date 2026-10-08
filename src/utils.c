#include <stdio.h>
#include "huffman_coding.h"

int	cmp(const void *a, const void *b)
{
	const t_symbol	*x = *(const t_symbol**)a;
	const t_symbol	*y = *(const t_symbol**)b;
	
	if (x->frequency > y->frequency)
		return 1;
	if (y->frequency > x->frequency)
		return -1;
	return 0;
}

void	free_arr(t_symbol **arr, u16 count)
{
	for (u16 i = 0; i < count; ++i)
		free(arr[i]);
	free(arr);
}

void	free_tree(t_symbol *tree)
{
	if (!tree)
		return;
	free_tree(tree->left);
	free_tree(tree->right);
	free(tree);
}

void	free_on_error(t_parser *data)
{
	for (u16 i = 0; i < data->count; ++i)
		free_tree(data->arr_symbols[i]);
	free(data->arr_symbols);
}


void	print_tree(t_symbol *symbol)
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


void	print_symbol(t_symbol *s)
{
	if (s->value == INTERNAL_NODE)
		printf("value: internal node, ");
	else
		printf("value: %c, ", s->value);
	printf("frequency: %lu\n", s->frequency);
}

void print_code(u16 code, u8 length)
{
	for (u8 i = 0; i < length; ++i)
	{
		if (code & (1 << i))
			write(1, "1", 1);
		else
			write(1, "0", 1);
	}
}
