#define msg_length  168
#define Byte_length 21
#include <stdio.h>
#include <fstream>
#include <iostream>
using namespace std;

unsigned short crc_table[256];

unsigned short Bit_set[16]={0x8000,0x4000,0x2000,0x1000,
                            0x0800,0x0400,0x0200,0x0100,
                            0x0080,0x0040,0x0020,0x0010,
                            0x0008,0x0004,0x0002,0x0001};

void init_crc_table(void)
      {
            int i, j;
            unsigned short k;
            for (i = 0; i < 256; i++)
            {
                  k = 0xC0C1;
                  for (j = 1; j < 256; j <<= 1)
                  {
                        if (i & j)
                              crc_table[i] ^= k;
                        k = (k << 1) ^ 0x4003;
                  }
            }
      }

/* crc_calc() -- calculate cumulative crc-16 for buffer */

unsigned short crc_calc(unsigned short crc, unsigned char *buf, unsigned int nbytes)
{
      unsigned char *p, *lim;
      p = buf;
      lim = p + nbytes;
      while (p < lim)
      {
            crc = (crc >> 8 ) ^ crc_table[(crc & 0xFF) ^ *p++];
      }
      return crc;
}



int main( int argc, char *argv[ ])
{ 

      unsigned char p[Byte_length];
      unsigned char msg[msg_length];
      unsigned char Msg_checksum[16];
      unsigned short crc;
      int i,j;
      FILE *f;

      //f = fopen(argv[1], "rb");
      /*f = fopen("info.dat", "rb");
      if (f == NULL)
      {
            printf("%s: can't open file\n", "info");
            return 1;
      }*/

      if( (f = fopen(argv[1], "rb" )) == NULL ) {
            cout << "failed to open input file\n";
		exit(0);
	}
      fread(msg, 1, msg_length, f); 
      fclose(f);
      for (i = 0; i < Byte_length; i++)
      {
            for(j = 0; j < 8; j++)
            p[i] = (msg[i*8+j]<<7) + (msg[i*8+j+1]<<6) + (msg[i*8+j+2]<<5) + (msg[i*8+j+3]<<4)
                  + (msg[i*8+j+4]<<3) + (msg[i*8+j+5]<<2) + (msg[i*8+j+6]<<1) + (msg[i*8+j+7]);
            //printf("%d,%d",i,p[i]);
            //printf("\n");
      }
      /*CRC computing*/

      init_crc_table();
      crc = 0;

      /*printf("%d",p[1]);
      printf("\n");*/

      crc = crc_calc(crc, p , Byte_length);
      printf("%d", crc);
      printf("\n");
      for(i = 0; i < 16; i++)
      {
            Msg_checksum[i]=(crc & Bit_set[i])>>(15-i);
            //printf("%c",Msg_checksum[i]);
      }

      FILE *crcoutput;
  
      int numwritten;  
      /* Open file in text mode: */
      if( (crcoutput = fopen( argv[2], "w+t" )) != NULL )
      {
      numwritten = fwrite( Msg_checksum, sizeof( char ), 16, crcoutput );
      //printf( "Wrote %d items\n", numwritten );
      fclose( crcoutput );

      }
      else printf( "Problem opening the file\n" );


      FILE *f1;
  
      unsigned char msg1[16];
      f1= fopen("crc.out", "rb");
      if (f1 == NULL)
      {
            printf("%s: can't open file\n", "info");
            return 1;
      }

      fread(msg1, 1,16, f1); 

	//used for testing

      unsigned short crc1;
      p[1]=8;
      crc1=0;
      crc1 = crc_calc(crc1, p , 16);	 
      printf("%d",crc1);
      printf("\n");
      
      return 0;
}