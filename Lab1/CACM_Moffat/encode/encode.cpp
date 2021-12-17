#include <time.h>
#include <iostream>
#include <fstream>
#include <stdio.h> 
#include <stdlib.h> 
#include <memory.h>
#include "arithmetic_coding.h" 

using   namespace   std;


FILE *ich; 
FILE *och; 
unsigned short int *char_to_index;		
unsigned char *index_to_char;	
unsigned short int *freq;
unsigned short int *cum_freq;	
unsigned char *DATABUF;
int databuf_pt;
unsigned char *CODEBUF;
int codebuf_pt;

int main(int argc, char *argv[]) 
{ 
	unsigned char *MEMBANK=new unsigned char[17825792];
	char_to_index = (unsigned short int *)  (MEMBANK );
	index_to_char = (unsigned char *)		(MEMBANK + 512);
	freq		  = (unsigned short int *)  (MEMBANK + 769);
	cum_freq	  = (unsigned short int *)  (MEMBANK + 1283);
	DATABUF		  = (unsigned char *)       (MEMBANK + 1797);
	CODEBUF       = (unsigned char *)       (MEMBANK + 8001797);

	if(argc != 3) { 
		cout <<"usage: encode input_file output_file\n"; 
		exit(0); 
	} 
 
	if( (ich = fopen( argv[1], "rb" )) == NULL ) {
		cout << "failed to open input file\n";
		exit(0);
	}
	int datalen=8000000;
	fread(DATABUF,sizeof(unsigned char),datalen,ich);
	fclose(ich); 

	if( (och = fopen( argv[2], "wb" )) == NULL ) {
		cout << "failed to open output file\n";
		exit(0);
	}

	clock_t start=clock();

	start_model();			 
	start_encoding(); 
		
	for(int i=0;i<datalen;i++) {					  
		int symbol=char_to_index[DATABUF[i]]; 
		encode_symbol(symbol); 

		//if(i<=13*512)
		update_model(symbol);			  
	} 
	  
	done_encoding();					 
 
	clock_t finish = clock();
	printf("number of ticks: %d\n", finish-start);

	fwrite(&datalen,sizeof(int),1,och);
	fwrite(CODEBUF,sizeof(unsigned char),codebuf_pt,och);
	fclose(och);

	delete []MEMBANK;
	return(0); 
}