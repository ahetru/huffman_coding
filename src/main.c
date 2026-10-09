#include "huffman_coding.h"

/* static u32 count = 0; */
/* void	count_nodes(t_symbol *s) */
/* { */
/* 	if ( s == NULL) */
/* 		return ; */
/* 	count++; */
/* 	count_nodes(s->left); */	
/* 	count_nodes(s->right); */	

/* } */

u8	parse_file_and_set_symbols(FILE *f, t_parser *data)
{
	u64 frequency[MAX_SYMBOLS] = {0};

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
			if ( s == NULL)
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

void	set_prefix_codes(t_symbol *huffman_tree, u16 code, u8 code_length)
{
	if (huffman_tree == NULL)
		return ;

	if (huffman_tree->value != INTERNAL_NODE)
	{
		huffman_tree->prefix_code = code;
		huffman_tree->code_length = code_length ? code_length : 1;
	}

	/* if (huffman_tree->left == NULL && huffman_tree->right == NULL) */
	/* { */
	/* 	dprintf(1, "code for %c: ", huffman_tree->value); */
	/* 	print_code(code, code_length); */
	/* 	dprintf(1, "\n"); */
	/* } */

	set_prefix_codes(huffman_tree->left, code | (1 << code_length), code_length + 1);
	set_prefix_codes(huffman_tree->right, code & ~ (1 << code_length), code_length + 1);
}

void	set_dictionnary(t_prefix_code dictionnary[MAX_SYMBOLS], t_symbol *tree)
{
	if (tree == NULL)
		return ;

	if (tree->value != INTERNAL_NODE)
	{
		dictionnary[tree->value].code = tree->prefix_code;
		dictionnary[tree->value].length = tree->code_length;
	}
	/* printf("value: %c code: %u, length: %u\n",tree->value, dictionnary[tree->value].code, dictionnary[tree->value].length); */
	/* printf("Hello World!\n"); */

	set_dictionnary(dictionnary, tree->left);
	set_dictionnary(dictionnary, tree->right);
}
/* u8 encode_file(FILE *f, *t_bitstream bitstream, ) */

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}

	FILE *f = fopen(argv[1], "r");
	if (f == NULL)
	{
		perror("Could not open file");
		return 1;
	}

	t_parser data = {0};
	parse_file_and_set_symbols(f, &data);
	if (data.arr_symbols == NULL)
		return 1;

	t_symbol *huffman_tree = build_huffman_tree(&data);
	if (huffman_tree == NULL)
		return 1;
	
	set_prefix_codes(huffman_tree, 0, 0);
	t_prefix_code value_to_prefix_code[MAX_SYMBOLS] = {0};
	set_dictionnary(value_to_prefix_code, huffman_tree);

	for (u16 i = 0; i < MAX_SYMBOLS; ++i)
		if (value_to_prefix_code[i].length)
		{
			dprintf(1, "value %c: ", (char)i);
			print_code(value_to_prefix_code[i].code, value_to_prefix_code[i].length);
			dprintf(1, "\n");
		}

	t_bitstream bitstream = {0};
	if (init_bitstream(&bitstream) == 1)
	{
		printf("memory allocation failed on bitstream buffer\n");
		free_tree(huffman_tree);
		return 1;
	}
	
	/* encode_file(f, &bitstream, huffman_tree); */
	free_tree(huffman_tree);

	return 0;
}


