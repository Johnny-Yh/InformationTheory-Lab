#include <iostream> 
#include <fstream> 
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <memory.h>
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
	memset(CODEBUF,0,1100000);

	if(argc != 3) { 
		cout <<"usage: encode input_file output_file\n"; 
		exit(0); 
	} 
 
	if( (ich = fopen( argv[1], "rb" )) == NULL ) {
		cout << "failed to open input file\n";
		exit(0);
	}
	fseek(ich,0L,SEEK_END);
	int datalen=ftell(ich);
	fseek(ich,0L,SEEK_SET);
	fread(DATABUF,sizeof(unsigned char),8000000,ich);
	fclose(ich); 

	if( (och = fopen( argv[2], "wb" )) == NULL ) {
		cout << "failed to open output file\n";
		exit(0);
	}

	clock_t start = clock();

	int c0=1; int c1=1;	 
	start_encoding(); 

	for(int i=0;i<datalen;i++) {					
		unsigned char bit=DATABUF[i];
		encode_bit(c0,c1,bit);
		if(c0+c1==Max_frequency) {c0=(c0+1)>>1; c1=(c1+1)>>1;}
		if(bit) c1 ++;
		else c0 ++;
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
