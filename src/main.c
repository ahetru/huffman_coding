#include "huffman_coding.h"
// TODO: replace 0 and 1 with HC_BOOL_TRUE AND HC_BOOL_FALSE
// TODO: replace MAX_SYMBOLS with u8_size ?
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
	t_symbol **arr_symbols = malloc(sizeof(t_symbol*) * data->count);
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

	set_prefix_codes(huffman_tree->left, code | ((u16)1 << code_length), code_length + 1);
	set_prefix_codes(huffman_tree->right, code & ~((u16)1 << code_length), code_length + 1);
}

void	set_dictionnary(t_prefix_code *codes, t_symbol *tree)
{
	if (tree == NULL)
		return ;

	if (tree->value != INTERNAL_NODE)
	{
		codes[tree->value].code = tree->prefix_code;
		codes[tree->value].length = tree->code_length;
	}

	set_dictionnary(codes, tree->left);
	set_dictionnary(codes, tree->right);
}

u8 generate_encoded_file(FILE *infile, FILE *outfile, t_bitstream *bitstream, t_prefix_code *codes)
{
	rewind(infile);
	int c;	
	u64 bit_count = 0;
	while((c = fgetc(infile)) != EOF)
	{
		t_prefix_code code = codes[c];
		if (bitstream->index == bitstream->size)
		{
			u8 *ptr = realloc(bitstream->buffer, bitstream->size + MEMO_BLOCK_SIZE);
			if (ptr == NULL)
			{
				perror("memory realloc failed on bitstream buffer");
				exit(EXIT_FAILURE); //TODO: handle properly
			}
			bitstream->buffer = ptr;
			bitstream->size += MEMO_BLOCK_SIZE;
		}
		u8 byte = 0;
		bit_count += code.length;
		while(code.length)
		{
			byte += (code.code >> (8 * code.length)) &0xff;
			code.length--;
		}
		bitstream->buffer[bitstream->index] = byte;
		bitstream->index++;
	}

	const char header[] = "HC file prefix codes table: \n";
	fwrite(header, sizeof(header[0]),  strlen(header), outfile);
	for (u16 i = 0; i < MAX_SYMBOLS; ++i)
	{
		if (codes[i].length)
		{
			// char buffer[codes[i].length];
			char *buffer = malloc(sizeof(char) * codes[i].length + 1);
			buffer[codes[i].length] = '\0';
			for (u16 j = 0; j < codes[i].length; ++j)
			{
				if (codes[i].code & (1 << j))
					buffer[j] = '1';
				else
					buffer[j] = '0';
			}
			// printf("buffer length for %c: %lu\n", (char)i, strlen(buffer));
			// printf("codes[i].length: %u\n",codes[i].length);
			fprintf(outfile, "%c|%s|%u\n", (char)i, buffer, codes[i].length);
			free(buffer);
		}
	}
	const char end_of_header[] = "end of header\n";
	fwrite(end_of_header, sizeof(header[0]),  strlen(end_of_header), outfile);

	u64 encoded_length = bit_count % 8 ? (bit_count / 8) + 1 : bit_count / 8;
	fwrite(bitstream->buffer, sizeof(bitstream->buffer[0]), encoded_length, outfile);
	return 0;
}

u8	decode_file(FILE *encoded)
{
	rewind(encoded);
	char header_line[256] = {0};
	const char header_title[] = "HC file prefix codes table: \n";
	fgets(header_line, sizeof(header_line), encoded);
	if (strcmp(header_line, header_title))
	{
		fprintf(stderr, "could not decode file, wrong format\n");
		return 1;
	}

	t_prefix_code codes[MAX_SYMBOLS] = {0};
	u8 end_of_header = 0;
	while (fgets(header_line, sizeof(header_line), encoded))
	{
		if (!strcmp(header_line, "end of header\n"))
		{
			end_of_header = 1;
			break;
		}
		char *symbol_str	= strtok(header_line, "|");
		char *code_str 		= strtok(NULL, "|");
		char *code_length_str	= strtok(NULL, "\n");

		if (symbol_str == NULL || code_str == NULL || code_length_str == NULL)
		{
			fprintf(stderr, "could not decode file, wrong format\n");
			return 1;
		}
		u16 symbol = (u16)symbol_str[0];
		u16 code   = str_to_bits(code_str);
		u16 length = atoi(code_length_str);

		t_prefix_code prefix;
		prefix.code = code;
		prefix.length = length;
		codes[symbol] = prefix;
	}
	if (!end_of_header)
	{
		fprintf(stderr, "could not decode file, wrong format\n");
		return 1;
	}
	print_prefix_codes(codes);

	return 0;
}

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}

	FILE *infile = fopen(argv[1], "r");
	if (infile == NULL)
	{
		perror("could not open file");
			return 1;
	}

	t_parser data = {0};
	parse_file_and_set_symbols(infile, &data);
	if (data.arr_symbols == NULL)
	{
		fclose(infile);
		return 1;
	}

	t_symbol *huffman_tree = build_huffman_tree(&data);
	if (huffman_tree == NULL)
		return 1;
	
	set_prefix_codes(huffman_tree, 0, 0);
	t_prefix_code value_to_prefix_code[MAX_SYMBOLS] = {0};
	set_dictionnary(value_to_prefix_code, huffman_tree);

	t_bitstream bitstream = {0};
	if (init_bitstream(&bitstream) == 1)
	{
		printf("memory allocation failed on bitstream buffer\n");
		free_tree(huffman_tree);
		return 1;
	}
	
	char *outfilename = strjoin(argv[1], ".hc");
	if (outfilename == NULL)
	{
		printf("memory allocation failed on strjoin\n");
		free_tree(huffman_tree);
		return 1;
	}
	FILE *outfile = fopen(outfilename, "w+b");
	if (outfile == NULL)
	{
		perror("could not create outfile");
		free_tree(huffman_tree);
		free(bitstream.buffer);
		fclose(infile);
		free(outfilename);
		return 1;
	}
	free(outfilename);
	
	generate_encoded_file(infile, outfile, &bitstream, value_to_prefix_code);
	decode_file(outfile);
	free_tree(huffman_tree);
	free(bitstream.buffer);
	fclose(infile);
	fclose(outfile);

	return 0;
}
