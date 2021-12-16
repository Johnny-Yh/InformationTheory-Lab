#include <time.h> 
#include <stdio.h>
#include "arithmetic_coding.h" 

extern unsigned char *CODEBUF;
extern int codebuf_pt; 

static code_value L, R;	 
static long bits_to_follow;	
static int bits_to_fill;	
 
void start_encoding() 
{ 
	L=0; 
	R=Half;			
	bits_to_follow=0;
	
	codebuf_pt=0;
	bits_to_fill=8; 
}  

void encode_bit(int c0, int c1, unsigned char bit) 
{ 
	unsigned char LPS; int cLPS;
	if(c0<c1) {LPS=0; cLPS=c0;}
	else	  {LPS=1; cLPS=c1;}
	
	code_value T=(R*cLPS)/(c0+c1);
	if(bit==LPS) {L=L+R-T; R=T;}
	else R=R-T;

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
} 
 

void done_encoding()							  
{	
	for(int i=Code_value_bits-1; i>=0; i--) {
		int b = (L>>i) & 0x01;
		// output_bit(b);						 
		CODEBUF[codebuf_pt] >>= 1;	if(b) CODEBUF[codebuf_pt] |= 0x80; 
		bits_to_fill -= 1; if(bits_to_fill == 0) { codebuf_pt += 1; bits_to_fill=8; } 
		while (bits_to_follow>0) { 
			// output_bit(!b);			
			CODEBUF[codebuf_pt] >>= 1;	if(!b) CODEBUF[codebuf_pt] |= 0x80; 
			bits_to_fill -= 1; if(bits_to_fill == 0) { codebuf_pt += 1; bits_to_fill=8; } 
			bits_to_follow -= 1;				 
		}
	}

	CODEBUF[codebuf_pt] >>= bits_to_fill;
	codebuf_pt += 1;
}