#include "arithmetic_coding.h"

extern unsigned short int *char_to_index;
extern unsigned char *index_to_char;
extern unsigned short int *freq;
extern unsigned short int *cum_freq;

void start_model()
{
	int i;
	for(i=0;i<No_of_chars; i++) {		
		char_to_index[i]=i+1;	
		index_to_char[i+1]=i;		
	}
	for(i=0;i<=No_of_chars;i++) {		
		freq[i]=1; 		
		cum_freq[i]=No_of_chars-i;	
	}
	freq[0]=0;			
}

void update_model(int symbol)
{
	int i;	
	if(cum_freq[0]==Max_frequency)	{
		int cum=0;
		for(i=No_of_chars; i>=0;i--){   
			freq[i]=(freq[i]+1)>>1;	      
			cum_freq[i]=cum;
			cum += freq[i];
		}
	}
	
	for(i=symbol;freq[i]==freq[i-1];i--);	
	if(i<symbol)	{
		int ch_i=index_to_char[i];		   
		int ch_symbol = index_to_char[symbol];  
		index_to_char[i]=ch_symbol;
		index_to_char[symbol]=ch_i;
		char_to_index[ch_i]=symbol;
		char_to_index[ch_symbol]=i;
	}
	freq[i] += 1;		
	while(i>0)	{
		i -= 1;
		cum_freq[i] += 1;
	}
}