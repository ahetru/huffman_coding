#include "huffman_coding.h"
#include <stdlib.h>
#include <stdio.h>



/* void free_dynamic_array(t_dynamic_array da) */
/* { */
/* 		for (u16 i = 0; i < da.count; ++i) */
/* 			free(da.symbols[i]); */
/* } */

void append_dynamic_array(t_dynamic_array *da, t_symbol s)
{
	if (da->count >= da->capacity)
	{
		if (da->capacity == 0)
			da->capacity = 256;
		else
			da->capacity *= 2;
		
		t_symbol *tmp = realloc(da->symbols, da->capacity * sizeof(*da->symbols));	
		if (tmp == NULL)
		{
			free(da->symbols);
			exit(1);
		}
		da->symbols = tmp;
	}
	t_symbol	symbol;
	symbol.value = s.value;
	symbol.frequency = s.frequency;
	symbol.prefix_code = s.prefix_code;
	symbol.code_length = s.code_length;
	symbol.left= s.left;
	symbol.right= s.right;
	da->symbols[da->count] = symbol;
	++da->count;
}
