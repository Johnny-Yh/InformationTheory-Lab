#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "arithmetic_coding.h"

extern unsigned short int *char_to_index;
extern unsigned char *index_to_char;
extern unsigned short int *cum_freq;
extern unsigned char *CODEBUF;
extern int codebuf_pt;

static code_value V;			
static code_value L,R;		
static int bits_to_go;				

#define input_bit() (CODEBUF[codebuf_pt] & 0x01)

void start_decoding()
{
	codebuf_pt=0;			
	bits_to_go=8;

	V=0;	
	for(int i=1;i<=Code_value_bits;i++) {
		if (bits_to_go==0) { codebuf_pt ++; bits_to_go=8;}
		V=(V<<1)+input_bit();
		CODEBUF[codebuf_pt] >>= 1;
		bits_to_go -= 1;
	}
	L=0;				
	R=Half;
}

int decode_symbol()		
{
	int symbol;				
	int cum=((V-L+1)*cum_freq[0]-1)/R; 	
	for(symbol=1; cum_freq[symbol]>cum;symbol++);	 

	code_value T=(R*cum_freq[symbol])/cum_freq[0];
	L=L+T;
	R=(R*cum_freq[symbol-1])/cum_freq[0]-T; 
						
	while (R<=First_qtr) {								
		if(L+R<=Half) {					 
			// nothing
		} 
		else if(L>=Half) {			 
			L  -= Half;		
			V  -= Half;
		} 
		else {		 
			L -= First_qtr;	
			V -= First_qtr;
		} 
		L = L<<1;	
		R = R<<1; 
		if (bits_to_go==0) { codebuf_pt ++; bits_to_go=8;}
		V = (V<<1) + input_bit();
		CODEBUF[codebuf_pt] >>= 1;
		bits_to_go -= 1;
	}

	return symbol;
}
