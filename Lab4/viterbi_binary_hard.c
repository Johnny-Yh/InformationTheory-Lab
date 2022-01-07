// ------------------------------------------------------------------------
// File: viterbi_binary_hard.c
// Date: April 1, 2002
// Description: Viterbi decoder, hard decision decoding. For a
//              rate-1/n convolutional code. Hamming metric.
//
// 4 states; max. truncation length = 1024
//
// ------------------------------------------------------------------------
////
// You may use this program for academic and personal purposes only. 
// ------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>

#define MAX_RANDOM LONG_MAX 

// Use the compiling directive -DSHOW_PROGRESS to see how the decoder
// converges to the decoded sequence by monitoring the survivor memory
#ifdef SHOW_PROGRESS
#define DELTA 1
#endif

#define NUM_STATES 4

int k2=1, n2, m2;
int OUT_SYM, NUM_TRANS;
long TRUNC_LENGTH;

double RATE;
double INIT_SNR, FINAL_SNR, SNR_INC;
long NUMSIM;

FILE  *fp_ber;

int g2[10][10];
unsigned int memory2, output;            /* Memory and output */
unsigned int data2;                      /* Data */

unsigned long seed;                      /* Seed for random generator */
unsigned int data_symbol[1024];          /* 1-bit data sequence */
unsigned int dec_data_symbol[1024];
long index;                              /* Index for memory management*/
double transmitted[1024];                  /* Transmitted signals/branch */
double snr, amp;
double received[1024];                     /* Received signals/branch */

int fflush();

// Data structures used for trellis sections and survivors
struct trel {
	int init;                /* current state */
	int data;                /* data symbol */
	int final;               /* next state */
	double output[10];       /* output coded symbols (branch label) */
}; 
struct surv {
	double metric;           /* metric */
	//int data[1024];        /* estimated data symbols */
	//int state[1024];       /* state sequence */
	int prev_state;
	int datainput;
};

// A trellis section is an array of branches, indexed by an initial
//   state and  input data. The values read
//   are the final state and the output symbols 
struct trel trellis[1024][100];

/* A survivor is a sequence of states and estimated data, of length
equal to TRUNC_LENGTH, together with its corresponding metric.
A total of NUM_STATES survivors are needed */
struct surv survivor[1024][4];

/* Function prototypes */
void encoder2(void);           /* Encoder for C_{O2} */
int random_data(void);         /* Random data generator */
void transmit(void);           /* Encoder & BPSK modulator */
void awgn(void);               /* Add AWGN */
void viterbi(void);            /* Viterbi decoder */
double comp_metric(double rec, double ref); /* Metric calculator */
void open_files(void);         /* Open files for output */
void close_files(void);        /* Close files */


main(int argc, char *argv[])
{

	int i, j, k, st_no, min_st_no;
	int init, data, final, output;
	register int error;
	unsigned long error_count;
	char name2[40];
	double min_metric;
	double aux_metric, surv_metric[4];
	int trans_no,data_bit_no;
	int offset=0;

	// Command line processing
	if (argc != 7)
	{
		printf("Usage %s file_input file_output truncation snr_init snr_final snr_inc num_sim\n", argv[0]);
		exit(0);
	}

	sscanf(argv[1],"%s",  name2);
	sscanf(argv[2],"%ld", &TRUNC_LENGTH);
	sscanf(argv[3],"%lf", &INIT_SNR);
	sscanf(argv[4],"%lf", &FINAL_SNR);
	sscanf(argv[5],"%lf", &SNR_INC);
	sscanf(argv[6],"%ld", &NUMSIM);

	printf("\nSimulation of Viterbi decoding over an AWGN channel with hard decisions\n");
	printf("%ld simulations per Eb/No (dB) point\n", NUMSIM);

	fp_ber = fopen(name2,"w");

	n2 = 2; // number of coding output bits
	m2 = 2; // pow(2,2) number of states

	RATE = 1.0 / (double) n2;

	OUT_SYM = 2; 
	NUM_TRANS = 2; // each state has two tansition branch

	// generator polynomial
	g2[0][0] = 5;   // 101
	g2[1][0] = 7;   // 111 

	printf("\n%d-state rate-1/%d binary convolutional encoder\n",NUM_STATES, n2);
	printf("with generator polynomials ");
	for (j=0; j<n2; j++) printf("%x ", g2[j][0]); printf("\n");
	printf("\n\nDecoding depth = %ld\n\n", TRUNC_LENGTH);

	// Initializing trellis
	trellis[0][0].init  = 0;      // current state
	trellis[0][0].data  = 0;      // input data
	trellis[0][0].final = 0;      // next state
	trellis[0][0].output[0] = 1;  // bit 0 output
	trellis[0][0].output[1] = 1;  // bit 0 output

	trellis[0][1].init  = 0;      // current state
	trellis[0][1].data  = 1;      // input data
	trellis[0][1].final = 1;      // next state
	trellis[0][1].output[0] = -1; // bit 1 output
	trellis[0][1].output[1] = -1; // bit 1 output

	trellis[1][0].init  = 1;      // current state
	trellis[1][0].data  = 0;      // input data
	trellis[1][0].final = 2;      // nextstate
	trellis[1][0].output[0] =  1; // bit 0 output
	trellis[1][0].output[1] = -1; // bit 1 output

	trellis[1][1].init  = 1;      // current state
	trellis[1][1].data  = 1;      // input data
	trellis[1][1].final = 3;      // next state
	trellis[1][1].output[0] = -1; // bit 1 output
	trellis[1][1].output[1] =  1; // bit 0 output

	trellis[2][0].init  = 2;      // current state
	trellis[2][0].data  = 0;      // input data
	trellis[2][0].final = 0;      // next state
	trellis[2][0].output[0] = -1; // bit 1 output
	trellis[2][0].output[1] = -1; // bit 1 output

	trellis[2][1].init  = 2;      // current state
	trellis[2][1].data  = 1;      // input data
	trellis[2][1].final = 1;      // next state
	trellis[2][1].output[0] = 1;  // bit 0 output
	trellis[2][1].output[1] = 1;  // bit 0 output

	trellis[3][0].init  = 3;      // current state
	trellis[3][0].data  = 0;      // input data
	trellis[3][0].final = 2;      // next state
	trellis[3][0].output[0] = -1; // bit 1 output
	trellis[3][0].output[1] = 1;  // bit 0 output

	trellis[3][1].init  = 3;       // current state
	trellis[3][1].data  = 1;       // input data
	trellis[3][1].final = 3;       // next state
	trellis[3][1].output[0] = 1;   // bit 0 output
	trellis[3][1].output[1] = -1;  // bit 1 output

	snr = INIT_SNR;

	/* ======================== SNR LOOP ============================= */

	while ( snr < (FINAL_SNR+1.0e-6) )
	{
		amp = sqrt( 2.0 * RATE * pow(10.0, (snr/10.0)) );

		/* Random seed from current time */
		//time(&seed);
		srand(seed);

		/* Index used in simulation loop */
		index = 0;

		/* Initialize encoder memory */
		memory2 = 0;

		/* Error counter */
		error_count = 0;

		/* ===================== SIMULATION LOOP ========================= */ 
		while (index < NUMSIM) 
		{ 

			/* Initialize transmitted data sequence */

			for (i=0; i<TRUNC_LENGTH+1; i++)
			{
				data_symbol[i]=0;

				/* Initialize survivor sequences and metrics */

				for (k=0; k<NUM_STATES; k++)
				{
					survivor[i][k].metric = 0.0;             /* Metric = 0 */

				}
			}  

			for (i = 1; i< TRUNC_LENGTH+1; i++)
			{ 
				/* GENERATE a random bit */
				data_symbol[index % TRUNC_LENGTH] = random_data(); /* */

				/* ENCODE AND MODULATE (BPSK) data bit */
				transmit();

				/* ADD ADDITIVE WHITE GAUSSIAN NOISE */
				awgn(); 

				/* VITERBI computing */
				{

					for (st_no=0; st_no<NUM_STATES; st_no++) // Initialize survivor branch metric 
					{
						surv_metric[st_no] =1e+8;
					}

					for (st_no=0; st_no<NUM_STATES; st_no++) // Loop over inital states 
					{
						for (trans_no=0; trans_no<NUM_TRANS; trans_no++) // Loop over data 
						{
							// Compute CORRELATION between received seq. and coded branch 
							aux_metric = 0.0;
							for (data_bit_no=(OUT_SYM-1); data_bit_no>=0; data_bit_no--)
								aux_metric += comp_metric(received[data_bit_no],trellis[st_no][trans_no].output[data_bit_no]);

							aux_metric += survivor[i-1][st_no].metric;

							// compare with survivor metric at final state 
							// We compare HAMMING DISTANCE
							if ( aux_metric < surv_metric[trellis[st_no][trans_no].final] )
							{ 
								// Good candidate found 
								surv_metric[trellis[st_no][trans_no].final] = aux_metric;
								survivor[i][trellis[st_no][trans_no].final].metric = aux_metric;

								survivor[i][trellis[st_no][trans_no].final].prev_state = st_no;
								survivor[i][trellis[st_no][trans_no].final].datainput  = trans_no;
							}
						}
					}

				}  // Viterbi 

				index++;

			} // end of loop i
		 

			min_metric  = survivor[TRUNC_LENGTH][0].metric;
			min_st_no   = 0;

			for( j = 1; j < 4; j++)
		    {
			   if (survivor[TRUNC_LENGTH][j].metric < min_metric)
			   {
				 min_metric = survivor[TRUNC_LENGTH][j].metric;
				 min_st_no = j;
			   }
		    }

		    st_no = min_st_no;
		    dec_data_symbol[TRUNC_LENGTH-1] = survivor[TRUNC_LENGTH][st_no].datainput;

			// back_track to find the trellis path
			for( i = TRUNC_LENGTH-1; i > 0; i--)
			{ 
				dec_data_symbol[i-1] = survivor[i][survivor[i+1][st_no].prev_state].datainput;
				st_no = survivor[i+1][st_no].prev_state;
		    }

			for ( i = 0; i< TRUNC_LENGTH;i++)
		    {
			    if ( dec_data_symbol[i] != data_symbol[i])
				{
			 	   error_count++;
				}
		    }

#ifdef SHOW_PROGRESS
			if ( (index % DELTA) == 0 )
			{
				for (j=0; j<NUM_STATES; j++) 
				{
					printf("%3d %2d   ", (index % TRUNC_LENGTH), j);
					for (i=0; i<TRUNC_LENGTH; i++)
						printf("%x", survivor[j].data[i]);
					printf(" %ld\n", error_count);
				}
			}
#endif
		}

		printf("%f  %10.4e\n",snr, ((double) error_count/(double) index) );

		fprintf(fp_ber, "%f %10.4e\n", snr, ((double)error_count /(double)index));

		fflush(stdout);
		fflush(fp_ber);

		snr += SNR_INC;
	}

	fclose(fp_ber);
}


int random_data()
{
	/* Random bit generator */
	return( (rand() >> 5) & 1 );
}


void transmit()
{
	/* Encode and modulate a 1-bit data sequence */
	int ii;

	encoder2(); /* */

	/* Modulate: {0,1} --> {+1,-1} */
	for (ii=(OUT_SYM-1); ii >=0; ii--)
		if ( (output >> ii) & 0x01 )
			transmitted[OUT_SYM-1-ii] = -1.0;  /* if bit = 1 */
		else
			transmitted[OUT_SYM-1-ii] =  1.0;  /* if bit = 0 */
}


void encoder2()
{
	// Binary convolutional encoder, rate 1/n2 
	register int iii, jj, result, temp;

	temp = memory2;
	output = 0;

	temp = (temp<<1) ^ data_symbol[index % TRUNC_LENGTH];

	for (iii=0; iii<n2; iii++)
	{
		result = 0;
		for (jj=m2; jj>=0; jj--)
			result ^= ( ( temp & g2[iii][0] ) >> jj ) & 1;
		output = ( output<<1 ) ^ result;
	}

	memory2 = temp ;
}

void awgn()
{
	/* Add AWGN to transmitted sequence */
	double u1,u2,s,noise,randmum;
	int iiii;

	for (iiii=0; iiii<OUT_SYM; iiii++)
	{
		do
		{	
			randmum = (double)(rand())/RAND_MAX;
			u1 = randmum*2.0 - 1.0;
			randmum = (double)(rand())/RAND_MAX;
			u2 = randmum*2.0 - 1.0;
			s = u1*u1 + u2*u2;
		} while( s >= 1);

		noise = u1 * sqrt( (-2.0*log(s))/s );
		received[iiii] = transmitted[iiii] + noise/amp;
	}
}


double comp_metric(double rec, double ref)
{ // HAMMING DISTANCE between received and reference values
	// This is equivalent to hard decision + Hamming distance computation:
	if ( (rec<=0.0)  && (ref<0.0) ) return(0.0);
	if ( (rec<0.0)  && (ref>0.0) ) return(1.0);
	if ( (rec>0.0)  && (ref<0.0) ) return(1.0);
	if ( (rec>=0.0)  && (ref>0.0) ) return(0.0);
}