#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "arithmetic_coding.h"

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
		if (bits_to_go==0) { codebuf_pt ++; bits_to_go=8; } 
		V=(V<<1)+input_bit();
		CODEBUF[codebuf_pt] >>= 1;
		bits_to_go -= 1; 
	}
	L=0;	
	R=Half;
}

unsigned char decode_bit(int c0, int c1)
{
	unsigned char LPS; int cLPS;
	if(c0<c1) {LPS=0; cLPS=c0;}
	else	  {LPS=1; cLPS=c1;}
	
	unsigned char bit;
	code_value T=(R*cLPS)/(c0+c1);
	if(V>=L+R-T) {bit=LPS;   L=L+R-T; R=T;}
	else         {bit=1-LPS; R=R-T;}

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

		if (bits_to_go==0) { codebuf_pt ++; bits_to_go=8; } 
		V = (V<<1) + input_bit();
		CODEBUF[codebuf_pt] >>= 1;
		bits_to_go -= 1; 
	}

	return bit;
}