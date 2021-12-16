#include <iostream>
#include <time.h> 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "arithmetic_coding.h" 

extern unsigned char *CODEBUF;
extern int codebuf_pt; 
extern unsigned char *TABLE1a;
extern unsigned char *TABLE2a;
extern unsigned short int *cum_freq;

static code_value L, R;	 
static long bits_to_follow;	
static int bits_to_fill;		 

// static int durclock;

void start_encoding() 
{ 
	L=0; 
	R=Half;			
	bits_to_follow=0;	
	codebuf_pt=0;
	bits_to_fill=8; 

	// durclock=0;
} 

void encode_symbol(int symbol) 
{ 
	code_value T=(R*cum_freq[symbol])/cum_freq[0];
	L=L+T;
	R=(R*cum_freq[symbol-1])/cum_freq[0]-T; 
			
	// clock_t start = clock();

	while (R<=First_qtr) {								
		if(L+R<=Half) {			 
			// output_bit(0);						 
			CODEBUF[codebuf_pt] >>= 1;
			bits_to_fill -= 1; if(bits_to_fill == 0) { codebuf_pt += 1; bits_to_fill=8; }
			while (bits_to_follow>0) { 
				// output_bit(1);						 
				CODEBUF[codebuf_pt] >>= 1;	CODEBUF[codebuf_pt] |= 0x80;
				bits_to_fill -= 1; if(bits_to_fill == 0) { codebuf_pt += 1; bits_to_fill=8; } 
				bits_to_follow -= 1;				 
			}
		} 
		else if(L>=Half) {	 
			// output_bit(1);						 
			CODEBUF[codebuf_pt] >>= 1;	CODEBUF[codebuf_pt] |= 0x80;
			bits_to_fill -= 1; if(bits_to_fill == 0) { codebuf_pt += 1; bits_to_fill=8; } 
			while (bits_to_follow>0) { 
				// output_bit(0);	
				CODEBUF[codebuf_pt] >>= 1;
				bits_to_fill -= 1; if(bits_to_fill == 0) { codebuf_pt += 1; bits_to_fill=8; }
				bits_to_follow -= 1;				 
			}
			L  -= Half;		
		} 
		else {		 
			bits_to_follow += 1;
			L -= First_qtr;		
		} 
		L = L<<1;
		R = R<<1; 
	} 

	// clock_t finish = clock();
	// durclock += (finish-start);
} 
 

void done_encoding()							  
{	
	int bit=L>>15;
	// output_bit(bit);					
	CODEBUF[codebuf_pt] >>= 1;	if(bit) CODEBUF[codebuf_pt] |= 0x80; 
	bits_to_fill -= 1; if(bits_to_fill == 0) { codebuf_pt += 1; bits_to_fill=8; } 
	while (bits_to_follow>0) { 
		// output_bit(!bit);						 
		CODEBUF[codebuf_pt] >>= 1;	if(!bit) CODEBUF[codebuf_pt] |= 0x80; 
		bits_to_fill -= 1; if(bits_to_fill == 0) { codebuf_pt += 1; bits_to_fill=8; } 
		bits_to_follow -= 1;				 
	}
	for(int i=14; i>=0; i--) {
		bit = (L>>i) & 0x01;
		// output_bit(bit);						 
		CODEBUF[codebuf_pt] >>= 1;	if(bit) CODEBUF[codebuf_pt] |= 0x80; 
		bits_to_fill -= 1; if(bits_to_fill == 0) { codebuf_pt += 1; bits_to_fill=8; } 
	}

	CODEBUF[codebuf_pt] >>= bits_to_fill;
	codebuf_pt += 1;

	// printf("number of ticks in re-normalization: %d\n", durclock);
}
