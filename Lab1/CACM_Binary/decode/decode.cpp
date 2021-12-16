#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include "arithmetic_coding.h"

using namespace std;

FILE *ich; 
FILE *och; 
unsigned char *DATABUF;
int databuf_pt;
unsigned char *CODEBUF;
int codebuf_pt;

int main(int argc, char *argv[])
{
	unsigned char *MEMBANK=new unsigned char[10485760];
	DATABUF = (unsigned char *) (MEMBANK);
	CODEBUF = (unsigned char *) (MEMBANK + 8000000); 

	if(argc != 3) {
		cout << "usage: decode input_file output_file\n";
		exit(0);
	}

	if( (ich = fopen( argv[1], "rb" )) == NULL ) {
		cout << "failed to open input file\n";
		exit(0);
	}
	fseek(ich,0L,SEEK_END);
	int codelen=ftell(ich)-4; // datalen occupies 4 bytes
	fseek(ich,0L,SEEK_SET);
	int datalen;
	fread(&datalen,sizeof(int),1,ich);
	fread(CODEBUF, sizeof(unsigned char),codelen,ich);
	fclose(ich); 

	if( (och = fopen( argv[2], "wb" )) == NULL ) {
		cout << "failed to open output file\n";
		exit(0);
	}
	
	clock_t start = clock();

	int c0=1; int c1=1;
	start_decoding();

	for(int i=0;i<datalen;i++) {
		unsigned char bit=decode_bit(c0,c1);
		DATABUF[i]=bit;
		if(c0+c1==Max_frequency) {c0=(c0+1)>>1; c1=(c1+1)>>1;}
		if(bit) c1 ++;
		else c0 ++;
	}

	clock_t finish = clock();
	printf("number of ticks: %d\n", finish-start);
	
	fwrite(DATABUF,sizeof(unsigned char),datalen,och);
	fclose(och);		

	delete []MEMBANK;
	return(0);
}
