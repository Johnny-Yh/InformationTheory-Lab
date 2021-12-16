#define Code_value_bits 16		
typedef long code_value;		
#define Top_value 65536                

#define First_qtr	16384	      
#define Half		32768	
#define Third_qtr   49152		

#define No_of_chars	256			
#define EOF_symbol	257
#define No_of_symbols	257
#define Max_frequency	16383	

int decode_symbol();
void done_encoding();
void encode_symbol(int symbol);
void start_encoding();
void start_decoding();
void start_model();
void update_model(int symbol);