#define Code_value_bits 16		
typedef long code_value;		
#define Top_value 65536                

#define First_qtr	16384	       
#define Half		32768		
#define Third_qtr   49152		

unsigned char decode_bit(int c0, int c1);
void done_encoding();
void encode_bit(int c0,int c1, unsigned char bit);
void start_encoding();
void start_decoding();

#define Max_frequency	16383	