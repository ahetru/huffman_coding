#ifndef HUFFMAN_CODING
# define HUFFMAN_CODING
# include <stdint.h>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef int8_t		i8;
typedef int16_t		i16;
typedef int32_t		i32;
typedef int64_t		i64;

#define MAX_SYMBOLS	65535
#define INTERNAL_NODE	65534

typedef struct s_symbol
{
	u16		value;
	u16		prefix_code;
	u8		code_length;
	u64		frequency;
	struct s_symbol	*left;
	struct s_symbol	*right;
} t_symbol;

typedef struct s_parse
{
	t_symbol 	**arr_symbols;
	u16		count;
} t_parser;

typedef struct s_bitstream
{
	u8	*buffer;
	u8	bit_pos;
	u8	bytes_written;
	u64	size;
} t_bitstream;

typedef struct s_prefix_code
{
	u16	code;
	u8	length;
} t_prefix_code;


int	cmp(const void *a, const void *b);
void	free_arr(t_symbol **arr, u16 count);
void	free_tree(t_symbol *tree);
void	free_on_error(t_parser *data);
void	print_tree(t_symbol *symbol);
void	print_symbol(t_symbol *s);
void	print_code(u16 code, u8 length);
u8	init_bitstream(t_bitstream *bitstream);

#endif
