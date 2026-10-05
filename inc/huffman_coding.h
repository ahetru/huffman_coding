#ifndef HUFFMAN_CODING
# define HUFFMAN_CODING
# include <stdint.h>

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef int8_t		i8;
typedef int16_t		i16;
typedef int32_t		i32;
typedef int64_t		i64;

#define SYMBOL_UNSET	65535
#define MAX_SYMBOLS	65535

// TODO: Change s_symbol to s_node
typedef struct s_symbol
{
	u16		value;
	u16		prefix_code;
	u8		code_length;
	u64		frequency;
	struct s_symbol	*left;
	struct s_symbol	*right;
} t_symbol;

typedef struct s_dynamic_array
{
	t_symbol	*symbols;
	u16		count;
	u16		capacity;
} t_dynamic_array;

void append_dynamic_array(t_dynamic_array *da, t_symbol symbol);

#endif
