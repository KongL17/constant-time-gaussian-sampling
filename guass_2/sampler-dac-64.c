
// The improved constant-time sampler by Karmakar et al. after manual post-processing
// in the paper "Pushing the speed limit of constant-time discreteGaussian sampling." accepted by DAC 2019.


//gcc -Wextra -O3 -fomit-frame-pointer -march=native -o sample_dac64 sampler-dac-64.c

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"randombytes.c"
#include"fips202.c"
#include "cpucycles.c"

#define ZERO 0x0000000000000000
#define ONE 0xffffffffffffffff
#define BITS_PER_SAMPLE 5

//-------shake------
void initRandom(uint8_t *x, uint32_t len){

	randombytes(x,len);
}
//-------shake ends-


int main(){

	int64_t i,j,k;
	uint64_t bit[64];// to hold the bits
    uint64_t sample[64];
	volatile uint64_t out[5];
	
	volatile uint64_t clock1, clock2,clock3,clock4, clock11;
	clock1=0;clock2=0;clock3=0;clock4=0;clock11=0;

	//----------------extract-------------
	uint8_t	 sample_o[64];
 	uint64_t *sample_n = (uint64_t*) ((void*) sample_o);
	uint64_t sample_t;
	uint64_t mask=(1UL | (1UL<<8) | (1UL<<16) | (1UL<<24) | (1UL<<32) | (1UL<<40)| (1UL<<48)| (1UL<<56));
	//----------------extract ends---------
	
	//-------------------shake--------------------

	uint8_t seed[32];
	uint32_t SEEDBYTES=32;
	uint64_t s[25];
	uint8_t temp_byte[SHAKE128_RATE];
	uint32_t nrounds=3;
 	unsigned char* byte_bank = (unsigned char*) ((void*) bit);

	for (i = 0; i < 25; ++i)
	    s[i] = 0;
	initRandom(seed,SEEDBYTES);
	keccak_absorb(s, SHAKE128_RATE, seed, SEEDBYTES, 0x1F);

	//-------------------shake ends---------------


	uint64_t control;

	clock3=0;
	clock4=0;
	int repeat=1000000;
	for(i=0;i<repeat;i++){	

		memset(out,0,BITS_PER_SAMPLE*sizeof(uint64_t));  
		clock1=0;
		clock2=0;

		clock11 = cpucycles();

//-------------------shake----------------------
	keccak_squeezeblocks(byte_bank, nrounds, s, SHAKE128_RATE);
		
	keccak_squeezeblocks(temp_byte, 1, s, SHAKE128_RATE);

	for(j=0;j<(64*8-SHAKE128_RATE*nrounds);j++){
		byte_bank[nrounds*SHAKE128_RATE+j]=temp_byte[j];			
	}

//-------------------shake ends-----------------

		clock1 = cpucycles();

//-----------------------------------------------------------------------------------------------------------

		control=0xffffffffffffffff; 

		//------------------0-3-----------------

		out[2]= ( ~(~bit[0] | (bit[0]&~bit[1]))&( (~bit[2]&(~bit[4]&bit[3])) | ( bit[2]&((~bit[3]&(bit[4])) /*| ( bit[Y]&(expr2))*/))  ) );
		out[1]= ( (~bit[0]&(bit[1])) | ( bit[0]&((~bit[1]&(~bit[3]&bit[2])) | ( bit[1]&((~bit[2]&(bit[4]&bit[3])) | ( bit[2]&((~bit[3]&(ZERO)) /*| ( bit[3]&(expr2))*/))))))  );
		out[0]= ( (~bit[0]&((~bit[2]) | (~bit[1]))) | ( bit[0]&((~bit[1]&(bit[3]&bit[2])) | ( bit[1]&((~bit[2]&(ZERO)) | ( bit[2]&((~bit[3]&(~(bit[5] & bit[4]))) /*| ( bit[3]&(expr2))*/))))))  );
		control=control&bit[0]&bit[1]&bit[2]&bit[3];

		//------------------0-3 ends------------combine 0 ~ 3-th expressions

		//---------------4-7--------------

		out[2]= (control&( (~bit[4]&(ZERO)) | ( bit[4]&((~bit[5]&(~bit[6])) | ( bit[5]&((~bit[6]&(~bit[8]&bit[7])) | ( bit[6]&((~bit[7]&((bit[9]) | (~bit[8]))) /*| ( bit[7]&(expr2))*/))))))  )) | (~control&out[2]);
		out[1]= (control&( (~bit[4]&(~bit[5])) | ( bit[4]&((~bit[5]&((~bit[7]) | (bit[6]))) | ( bit[5]&((~bit[6]&(bit[8]&bit[7])) | ( bit[6]&((~bit[7]&(bit[8])) /*| ( bit[7]&(expr2))*/))))))  )) | (~control&out[1]);
		out[0]= (control&( (~bit[4]&(~bit[6]&bit[5])) | ( bit[4]&((~bit[5]&((~bit[7]&bit[6]))) | ( bit[5]&((~bit[6]&(ZERO)) | ( bit[6]&((~bit[7]&(~(bit[9] | bit[8]) | (~bit[10]&bit[9]&bit[8]))) /*| ( bit[Y]&(expr2))*/))))))  )) | (~control&out[0]);
		control=control&bit[4]&bit[5]&bit[6]&bit[7]; //remember to update control while merging
		
		//---------------4-7 ends--------------combine 4 ~ 7-th expressions

		//-----------------8-11-------------
		
		out[3]= ((control&bit[8]&bit[9])&( (~bit[10]&(~bit[12]&bit[11])) | ( bit[10]&(~bit[11]&(ZERO)/*| ( bit[11]&(expr2))*/))  )); 		
		out[2]= (control&( (~bit[8]&(~(bit[10] | bit[9]))) | ( bit[8]&((~bit[9]&(~(bit[11] & bit[10]))) | ( bit[9]&((~bit[10]&(bit[12]&bit[11])) | ( bit[10]&(~bit[11]&(~bit[12])/*| ( bit[11]&(expr2))*/))))))  ) ) | (~control&out[2]);
		out[1]= (control&( (~bit[8]&(bit[10]^bit[9])) | ( bit[8]&((~bit[9]&(~(bit[11]^bit[10]))) | ( bit[9]&((~bit[10]&(bit[12]&bit[11])) | ( bit[10]&(~bit[11]&(bit[12])/*| ( bit[11]&(expr2))*/))))))  ) ) | (~control&out[1]);
		out[0]= (control&( (~bit[8]&((bit[10]) | (~bit[9]))) | ( bit[8]&((~bit[9]&(~bit[10])) | ( bit[9]&((~bit[10]&(bit[12]&bit[11])) | ( bit[10]&(~bit[11]&(ONE)/*| ( bit[11]&(expr2))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[8]&bit[9]&bit[10]&bit[11]; //remember to update control while merging
		
		//-----------------8-11 ends-------------combine 8 ~ 11-th expressions

		//-----------------12-15-------------

		out[3]=( control & ( (~bit[12]&(ZERO)) | ( bit[12]&((~bit[13]&(~(bit[15] | bit[14]) | (~bit[16]&bit[15]&bit[14]))) | ( bit[13]&((~bit[14]&(~bit[17]&bit[16]&bit[15])) | ( bit[14]&((~bit[15]&(ZERO)) /*| ( bit[15]&(expr2))*/  ))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[12]&(ONE)) | ( bit[12]&((~bit[13]&((bit[16]&bit[15]) | (bit[15]&~bit[14]))) | ( bit[13]&((~bit[14]&(bit[17]&bit[16]&bit[15])) | ( bit[14]&((~bit[15]&(~(bit[17]^bit[16]))) /*| ( bit[15]&(expr2))*/  ))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[12]&(~bit[14]&bit[13])) | ( bit[12]&((~bit[13]&(bit[15]^bit[14])) | ( bit[13]&((~bit[14]&(~(bit[16] | bit[15]) | (bit[17]&bit[16]&bit[15]))) | ( bit[14]&((~bit[15]&(bit[17])) /*| ( bit[15]&(expr2))*/  ))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[12]&(~bit[13])) | ( bit[12]&((~bit[13]&((~bit[14]) | (bit[16]&bit[15]))) | ( bit[13]&((~bit[14]&(bit[16]&~bit[15])) | ( bit[14]&((~bit[15]&(bit[16]&(~(bit[18]&bit[17]) ))) /*| ( bit[15]&(expr2))*/  ))))))  ) ) | (~control&out[0]);
		control=control&bit[12]&bit[13]&bit[14]&bit[15]; //remember to update control while merging
		
		//-----------------12-15 ends-------------

		//-----------------16-19-------------

		out[3]=( control & ( (~bit[16]&(~(bit[19] | bit[18])&bit[17])) | ( bit[16]&((~bit[17]&(bit[19]&bit[18])) | ( bit[17]&((~bit[18]&(ZERO)) | ( bit[18]&((~bit[19]&(~bit[20])) /*| ( bit[19]&(expr2))*/))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[16]&((~(bit[18]^bit[17])) | (bit[19]&bit[17]))) | ( bit[16]&((~bit[17]&(~bit[19]&~bit[18])) | ( bit[17]&((~bit[18]&(~bit[19])) | ( bit[18]&((~bit[19]&(~bit[21]&bit[20])) /*| ( bit[19]&(expr2))*/))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[16]&(~(bit[19]&bit[18])&bit[17])) | ( bit[16]&((~bit[17]&(bit[19]&( ~( bit[20]&bit[18] ) ))) | ( bit[17]&((~bit[18]&(~bit[20]&bit[19])) | ( bit[18]&((~bit[19]&(ZERO)) /*| ( bit[19]&(expr2))*/))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[16]&((bit[19]) | (~bit[17]))) | ( bit[16]&((~bit[17]&((bit[19]^bit[18]) | (bit[20]&bit[18]))) | ( bit[17]&((~bit[18]&(~(bit[20] | bit[19]))) | ( bit[18]&((~bit[19]&((~bit[21]) | (bit[20]))) /*| ( bit[19]&(expr2))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[16]&bit[17]&bit[18]&bit[19]; //remember to update control while merging
		
		//-----------------16-19-------------

		//------------------20-23-----------------

		out[3]=( control & ( (~bit[20]&(ZERO)) | ( bit[20]&((~bit[21]&(~bit[22])) | ( bit[21]&((~bit[22]&((bit[24]) | (~bit[23]))) | ( bit[22]&((~bit[23]&(~bit[25])) /*| ( bit[23]&(expr2))*/))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[20]&(~bit[21])) | ( bit[20]&((~bit[21]&(~bit[23]&bit[22])) | ( bit[21]&((~bit[22]&(ZERO)) | ( bit[22]&((~bit[23]&(~bit[26]&bit[25]&bit[24])) /*| ( bit[23]&(expr2))*/))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[20]&(~bit[22])) | ( bit[20]&((~bit[21]&((~bit[23]) | (bit[22]))) | ( bit[21]&((~bit[22]&(ONE)) | ( bit[22]&((~bit[23]&(bit[24]&(~bit[26] | bit[25]))) /*| ( bit[23]&(expr2))*/))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[20]&(ZERO)) | ( bit[20]&((~bit[21]&(~bit[23])) | ( bit[21]&((~bit[22]&((~bit[24]) | (~bit[25]&bit[23]))) | ( bit[22]&((~bit[23]&((bit[26]|bit[25])&bit[24])) /*| ( bit[23]&(expr2))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[20]&bit[21]&bit[22]&bit[23]; //remember to update control while merging
		
		//------------------20-23 ends-----------------

		//------------------24-27----------------------

		out[3]=( control & ( (~bit[24]&(bit[26]&~bit[25])) | ( bit[24]&((~bit[25]&(~bit[26])) | ( bit[25]&((~bit[26]&(bit[28]&~bit[27])) | ( bit[26]&((~bit[27]&(bit[29]&~bit[28])) /*| ( bit[27]&(expr2))*/))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[24]&((~bit[27]&bit[26]) | (~bit[26]&bit[25]))) | ( bit[24]&((~bit[25]&(~bit[27]&(~bit[28] | bit[26]))) | ( bit[25]&((~bit[26]&(~bit[28]&bit[27])) | ( bit[26]&((~bit[27]&((~bit[30]&bit[29]) | (~bit[29]&bit[28]))) /*| ( bit[27]&(expr2))*/))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[24]&((bit[27]&bit[26]) | (~bit[27]&(~bit[26]&bit[25])))) | ( bit[24]&((~bit[25]&(bit[26]^(bit[28]&~bit[27]))) | ( bit[25]&((~bit[26]&(((~bit[29])&( bit[28] | bit[27] )) | (bit[28]&bit[27]))) | ( bit[26]&((~bit[27]&((~bit[29]&bit[28]) | (bit[30]&bit[29]&~bit[28]))) /*| ( bit[27]&(expr2))*/))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[24]&(bit[27]&(~bit[26]&bit[25]))) | ( bit[24]&((~bit[25]&(~( bit[28] ^ (~(bit[27]|bit[26])) ))) | ( bit[25]&((~bit[26]&(~(bit[29]&bit[28]))) | ( bit[26]&((~bit[27]&((~bit[30]&~bit[29]&bit[28]) | (bit[30]&bit[29]&~bit[28]))) /*| ( bit[27]&(expr2))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[24]&bit[25]&bit[26]&bit[27]; //remember to update control while merging
		
		//------------------24-27 ends----------------

		//------------------28-31---------------------

		out[3]=( control & ( (~bit[28]&(~bit[29]&(~(bit[31]&bit[30])))) | ( bit[28]&((~bit[29]&(bit[31]&~bit[30])) | ( bit[29]&((~bit[30]&(bit[32]&~bit[31])) | ( bit[30]&((~bit[31]&(~bit[32])) /*| ( bit[31]&(expr2))*/))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[28]&(~( bit[31]^(bit[30]&~bit[29]) ) | (~bit[30]&bit[29]))) | ( bit[28]&((~bit[29]&(~bit[31]&bit[30])) | ( bit[29]&((~bit[30]&(~(bit[33]&bit[32])&bit[31])) | ( bit[30]&((~bit[31]&(~(bit[34] | bit[33]) | (bit[32]& ( ~(bit[34]&bit[33]) ) ))) /*| ( bit[31]&(expr2))*/))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[28]&(bit[31]^(~bit[30]&bit[29]))) | ( bit[28]&((~bit[29]&((~bit[32]&bit[31]) | (~bit[31]&bit[30]))) | ( bit[29]&((~bit[30]&((bit[33]&bit[32]&bit[31]) | ( ~bit[33]&(bit[32]^bit[31]) ))) | ( bit[30]&((~bit[31]&(( ~bit[33]&(bit[34] | bit[32]) ) | (bit[34]&bit[32]))) /*| ( bit[31]&(expr2))*/))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[28]&(~(bit[31]^bit[29]) | (bit[30]&~bit[29]))) | ( bit[28]&((~bit[29]&((bit[31]) | (~bit[32]&bit[30]))) | ( bit[29]&((~bit[30]&(bit[33]&~bit[32]&bit[31])) | ( bit[30]&((~bit[31]&(~( bit[34]^(bit[33]&bit[32]) ))) /*| ( bit[31]&(expr2))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[28]&bit[29]&bit[30]&bit[31]; //remember to update control while merging

		//------------------28-31 ends------------------

		//------------------32-35-----------------------

		out[3]=( control & ( (~bit[32]&(~bit[34]&bit[33])) | ( bit[32]&((~bit[33]&((~bit[35]&bit[34]) | (bit[35]&~bit[34]))) | ( bit[33]&((~bit[34]&((bit[36]&~bit[35]) | (~(bit[37]|bit[36])&bit[35]))) | ( bit[34]&((~bit[35]&(~bit[37]&bit[36])) /*| ( bit[35]&(expr2))*/))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[32]&(bit[33]&(~bit[35] | bit[34]))) | ( bit[32]&((~bit[33]&((bit[35]) | (~bit[34]))) | ( bit[33]&((~bit[34]&((~bit[37]&bit[36]) | (bit[37]&bit[35]))) | ( bit[34]&((~bit[35]&((~bit[38]&bit[36]) | (bit[37]&bit[36]))) /*| ( bit[35]&(expr2))*/))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[32]&(~(bit[34] | bit[33]) | (~bit[35]&bit[34]&bit[33]))) | ( bit[32]&((~bit[33]&((~bit[36]&bit[35]) | (~bit[35]&bit[34]))) | ( bit[33]&((~bit[34]&(bit[37]&(bit[36]^bit[35]))) | ( bit[34]&((~bit[35]&(~(bit[38] & bit[37] & bit[36]))) /*| ( bit[3X]&(expr_4))*/))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[32]&(bit[35]&bit[34]&bit[33])) | ( bit[32]&((~bit[33]&((bit[36]&bit[35]&bit[34]) | (~bit[36]&~bit[35]&bit[34]))) | ( bit[33]&((~bit[34]&((~bit[37]&bit[35]) | (bit[36]&~bit[35]))) | ( bit[34]&((~bit[35]&(~(bit[37]^bit[36]) | (bit[38]&bit[36]))) /*| ( bit[35]&(expr2))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[32]&bit[33]&bit[34]&bit[35]; //remember to update control while merging
		
		//------------------32-35 ends------------------

		//------------------36-39-----------------------

		out[3]=( control & ( (~bit[36]&(bit[37])) | ( bit[36]&((~bit[37]&(bit[38]&( ~(bit[40]&bit[39]) ))) | ( bit[37]&((~bit[38]&(bit[40]^bit[39])) | ( bit[38]&((~bit[39]&((bit[41]&~bit[40]) | (~(bit[42] | bit[41])&bit[40]))) /*| ( bit[39]&(expr_4))*/))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[36]&(~(bit[39] | bit[38])&bit[37])) | ( bit[36]&((~bit[37]&((bit[40]) | ~(bit[39]&bit[38]))) | ( bit[37]&((~bit[38]&(~(bit[40]^bit[39]) | ~(bit[41] | bit[39]))) | ( bit[38]&((~bit[39]&(bit[42]&~bit[41]&bit[40])) /*| ( bit[39]&(expr_4))*/))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[36]&((bit[39]&~bit[38]) | ~(bit[38] | bit[37]))) | ( bit[36]&((~bit[37]&(~(bit[39]^bit[38]))) | ( bit[37]&((~bit[38]&((~bit[41]&bit[39]) | (bit[40]&~bit[39]))) | ( bit[38]&((~bit[39]&((~bit[42]&bit[41]) | (bit[42]&bit[40]))) /*| ( bit[39]&(expr_4))*/))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[36]&((~bit[39]&bit[37]) | (bit[38]&~bit[37]))) | ( bit[36]&((~bit[37]&((bit[39]) | (~bit[40]&bit[38]))) | ( bit[37]&((~bit[38]&((~bit[39]) | (bit[41]&bit[40]))) | ( bit[38]&((~bit[39]&(bit[41]&(bit[42]^bit[40]))) /*| ( bit[39]&(expr_4))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[36]&bit[37]&bit[38]&bit[39]; //remember to update control while merging
		
		//------------------36-39 ends-------------------		

		//------------------40-43-----------------------

		out[4]=(control&bit[40]&bit[41]&bit[42]&((~bit[47]&bit[46]&bit[45]&bit[44])));
		out[3]=( control & ( (~bit[40]&(~(bit[43] | bit[41])&bit[42])) | ( bit[40]&((~bit[41]&((bit[44]&bit[43]&bit[42]) | ~(bit[44] | bit[43] | bit[42]))) | ( bit[41]&((~bit[42]&(~(bit[45] | bit[44] | bit[43]))) | ( bit[42]&((~bit[43]&(~( bit[44] | (bit[46]&bit[45]) ) | (bit[47]&bit[46]&bit[45]&bit[44]))) /*| ( bit[43]&(expr_4))*/))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[40]&((~bit[42]&bit[41]) | (bit[43]&bit[42]&~bit[41]))) | ( bit[40]&((~bit[41]&(~((bit[44]&bit[43])^bit[42]))) | ( bit[41]&((~bit[42]&(~(bit[43]^( ~(bit[44]|bit[45]) ) ))) | ( bit[42]&((~bit[43]&(~(bit[46]|bit[45]) | ((bit[47] | ~bit[44])&bit[46]&bit[45]))) /*| ( bit[43]&(expr_4))*/))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[40]&(bit[42]&(bit[43]^bit[41]))) | ( bit[40]&((~bit[41]&(~(bit[44] | bit[43]) | ( bit[44] & (~bit[42] | (~bit[45]&bit[43]) ) ))) | ( bit[41]&((~bit[42]&((bit[45]^bit[44]) | ~(bit[44]|bit[43]))) | ( bit[42]&((~bit[43]&((~bit[45]) | ((bit[47] | ~bit[44])&bit[46]))) /*| ( bit[43]&(expr_4))*/))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[40]&((~bit[43]& (bit[42] | bit[41]) ) | (bit[42]&bit[41]))) | ( bit[40]&((~bit[41]&(bit[44])) | ( bit[41]&((~bit[42]&(bit[45]&(~(bit[44]&bit[43]) ))) | ( bit[42]&((~bit[43]&((~bit[46]&bit[45]) | (bit[46]&~bit[44]))) /*| ( bit[43]&(expr_4))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[40]&bit[41]&bit[42]&bit[43]; //remember to update control while merging
		
		//------------------40-43 ends------------------

		//------------------44-47-----------------------

		out[4]=( control & ( (~bit[44]&(ZERO)) | ( bit[44]&((~bit[45]&(~bit[49]&bit[48]&bit[47]&bit[46])) | ( bit[45]&((~bit[46]&(ZERO)) | ( bit[46]&((~bit[47]&(~(bit[50] | bit[49] | bit[48]))) /*| ( bit[47]&(expr_4))*/))))))  ) ) | (~control&out[4]);
		out[3]=( control & ( (~bit[44]&(~((bit[47]&bit[46])^bit[45]))) | ( bit[44]&((~bit[45]&(~( bit[46] | (bit[48]&bit[47]) ) | (bit[49]&bit[48]&bit[47]&bit[46]))) | ( bit[45]&((~bit[46]&(~bit[47])) | ( bit[46]&((~bit[47]&(~bit[48]&(bit[50]^bit[49]))) /*| ( bit[47]&(expr_4))*/))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[44]&(~(bit[47]^bit[46]) | (~bit[46]&bit[45]))) | ( bit[44]&((~bit[45]&((bit[49]&bit[48]&bit[47]) | (~(bit[48] | bit[47])&bit[46]) | (bit[48]&bit[47]&~bit[46]))) | ( bit[45]&((~bit[46]&(~(bit[49] | bit[48]) | ( bit[47]&(~(bit[49]&bit[48])) ))) | ( bit[46]&((~bit[47]&(~(~(bit[50] | bit[49])^bit[48]))) /*| ( bit[47]&(expr_4))*/))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[44]&((bit[47]^bit[45]) | (~bit[48]&bit[46]&bit[45]))) | ( bit[44]&((~bit[45]&((bit[48]&~bit[46]) | ~(bit[47] | bit[46]))) | ( bit[45]&((~bit[46]&(( bit[49]&(~bit[48] | bit[47]) ) | (~bit[48]&bit[47]))) | ( bit[46]&((~bit[47]&(bit[50]&( ~(bit[49]&bit[48]) ))) /*| ( bit[47]&(expr_4))*/))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[44]&((~bit[45]) | ( (~(bit[48]&bit[47]))&bit[46] ))) | ( bit[44]&((~bit[45]&((bit[47]^bit[46]) | ~(bit[48] | bit[46]) | (bit[49]&bit[48]&bit[46]))) | ( bit[45]&((~bit[46]&(~bit[49]&(bit[48]^bit[47]))) | ( bit[46]&((~bit[47]&(~bit[50]&bit[49]&bit[48])) /*| ( bit[47]&(expr_4))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[44]&bit[45]&bit[46]&bit[47]; //remember to update control while merging
		
		//------------------44-47 ends------------------

		//------------------48-51-----------------------

		out[4]=( control & ( (~bit[48]&(ZERO)) | ( bit[48]&((~bit[49]&(~(bit[52] | bit[51] | bit[50]))) | ( bit[49]&((~bit[50]&(bit[52]&~bit[51])) | ( bit[50]&((~bit[51]&(ZERO)) /*| ( bit[51]&(expr_4))*/))))))  ) ) | (~control&out[4]);
		out[3]=( control & ( (~bit[48]&(~(bit[50] & bit[49]))) | ( bit[48]&((~bit[49]&(~bit[50]&(bit[52] | bit[51]))) | ( bit[49]&((~bit[50]&(~bit[52]&bit[51])) | ( bit[50]&((~bit[51]&(bit[53]^bit[52])) /*| ( bit[51]&(expr_4))*/))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[48]&((~bit[51]&bit[50]) | ~(bit[50] | bit[49]))) | ( bit[48]&((~bit[49]&((bit[52]&~bit[51]) | (~bit[52]&bit[50]))) | ( bit[49]&((~bit[50]&(bit[51])) | ( bit[50]&((~bit[51]&(bit[53]&( ~(bit[54]&bit[52]) ))) /*| ( bit[51]&(expr_4))*/))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[48]&(~(bit[49] | (bit[51]^bit[50])))) | ( bit[48]&((~bit[49]&((~bit[52] & bit[51])^bit[50])) | ( bit[49]&((~bit[50]&((~bit[53]&bit[51]) | ~(bit[52] | bit[51]))) | ( bit[50]&((~bit[51]&(~( bit[53]^(bit[54]&bit[52]) ))) /*| ( bit[51]&(expr_4))*/))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[48]&((bit[51]^bit[49]) | (bit[50]&bit[49]))) | ( bit[48]&((~bit[49]&(~(bit[52]^bit[51])&bit[50])) | ( bit[49]&((~bit[50]&((bit[51]) | (~bit[53]&bit[52]))) | ( bit[50]&((~bit[51]&(~bit[54]&(bit[53]^bit[52]))) /*| ( bit[51]&(expr_4))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[48]&bit[49]&bit[50]&bit[51]; //remember to update control while merging
		
		//------------------48-51 ends-------------------

		//------------------52-55------------------------

		out[4]=( control & ( (~bit[52]&(~(bit[55] | bit[54] | bit[53]) | (~(bit[56] | bit[55])&bit[54]&bit[53]))) | ( bit[52]&((~bit[53]&(~bit[57]&bit[56]&bit[55]&bit[54])) | ( bit[53]&((~bit[54]&(~(bit[58] |bit[57])&bit[56]&bit[55])) | ( bit[54]&((~bit[55]&(~(bit[59] | bit[58])&bit[57]&bit[56])) /*| ( bit[55]&(expr_4))*/))))))  ) ) | (~control&out[4]);
		out[3]=( control & ( (~bit[52]&((bit[54]&(bit[56] | bit[55])) | (~bit[53]& (bit[55] | bit[54])))) | ( bit[52]&((~bit[53]&(~(bit[55] | bit[54]) | (bit[57]&bit[56]&bit[55]&bit[54]))) | ( bit[53]&((~bit[54]&((~bit[55]) | (bit[56]&(bit[58] | bit[57])))) | ( bit[54]&((~bit[55]&(~(bit[57] | bit[56]) | (bit[57]&bit[56]&(bit[59] | bit[58])))) /*| ( bit[55]&(expr_4))*/))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[52]&((bit[55]&bit[54]) | ~(bit[53]^(~(bit[55]|bit[54]))) | (bit[56]&~bit[55]&bit[53]))) | ( bit[52]&((~bit[53]&((bit[55]&~bit[54]) | (bit[57]&bit[56]&bit[55]))) | ( bit[53]&((~bit[54]&(((bit[58] | bit[57])&bit[56]&bit[55]) | ~(bit[57] | bit[56] | bit[55]))) | ( bit[54]&((~bit[55]&(bit[57]&(~bit[56] | (bit[59]^bit[58]) ))) /*| ( bit[55]&(expr_4))*/))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[52]&((bit[56]&~bit[55]&bit[54]) | (~bit[53]& (bit[55]^bit[54])))) | ( bit[52]&((~bit[53]&(((~bit[55] | (bit[57]&bit[56]) )&bit[54]) | ~(bit[56] | bit[54]))) | ( bit[53]&((~bit[54]&(~((~(bit[57] | bit[55]))^bit[56]) | ((bit[58]^bit[57])&bit[55]))) | ( bit[54]&((~bit[55]&((~bit[58]) | (bit[59]&bit[57]&bit[56]))) /*| ( bit[55]&(expr_4))*/))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[52]&(~(bit[54] | (bit[55]&bit[53])) | (~bit[56]&bit[54]&bit[53]))) | ( bit[52]&((~bit[53]&((bit[56]^bit[54]) | (~bit[57]&bit[55]&bit[54]))) | ( bit[53]&((~bit[54]&(( bit[55]&( ~bit[57] | (bit[58]&bit[56])) ) | (bit[57]&~(bit[56] | bit[55])))) | ( bit[54]&((~bit[55]&((bit[58]&~bit[57]) | (bit[59]&bit[58]&bit[56]) | (~(bit[58]|bit[56])&bit[57]))) /*| ( bit[55]&(expr_4))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[52]&bit[53]&bit[54]&bit[55]; //remember to update control while merging
		
		//------------------52-55 ends-------------------

		//------------------56-59------------------------

		out[4]=( control & ( (~bit[56]&(~bit[60]&bit[59]&bit[58]&bit[57])) | ( bit[56]&((~bit[57]&(ZERO)) | ( bit[57]&((~bit[58]&(~(bit[61] | bit[60] | bit[59]))) | ( bit[58]&((~bit[59]&(~(bit[61]|bit[60]) | (~bit[63]&bit[62]&bit[61]&bit[60]))) /*| ( bit[59]&(expr_4))*/))))))  ) ) | (~control&out[4]);
		out[3]=( control & ( (~bit[56]&(~(bit[57] | (bit[59]&bit[58])) | (bit[60]&bit[59]&bit[58]&bit[57]))) | ( bit[56]&((~bit[57]&(~bit[58])) | ( bit[57]&((~bit[58]&(~bit[59]&(bit[61] | bit[60]))) | ( bit[58]&((~bit[59]&((bit[61]&~bit[60]) | (bit[63]&bit[62]&bit[61]))) /*| ( bit[59]&(expr_4))*/))))))  ) ) | (~control&out[3]);
		out[2]=( control & ( (~bit[56]&((bit[59]&bit[58]&(bit[60] | ~bit[57])) | (~bit[59]&~bit[58]&bit[57]))) | ( bit[56]&((~bit[57]&(~(bit[60] | bit[59]) | (bit[58]&~(bit[60]&bit[59])))) | ( bit[57]&((~bit[58]&(~bit[60]&(bit[61] | bit[59]))) | ( bit[58]&((~bit[59]&((bit[60]&(~bit[61] | (bit[63]&bit[62]))) | (~(bit[62] | bit[60])&bit[61]))) /*| ( bit[59]&(expr_4))*/))))))  ) ) | (~control&out[2]);
		out[1]=( control & ( (~bit[56]&((bit[59]&( (bit[58]^bit[57]) | (bit[60]&bit[57]) )) | ~(bit[59] | bit[58] | bit[57]))) | ( bit[56]&((~bit[57]&(~bit[59]&(bit[60] | bit[58]))) | ( bit[57]&((~bit[58]&(~bit[61]&(bit[60] | bit[59]))) | ( bit[58]&((~bit[59]&(~(bit[62] | bit[61]) | (bit[63]&bit[62]&bit[61]&bit[60]))) /*| ( bit[59]&(expr_4))*/))))))  ) ) | (~control&out[1]);
		out[0]=( control & ( (~bit[56]&(bit[59]&~(bit[58]&bit[57]))) | ( bit[56]&((~bit[57]&(~bit[60])) | ( bit[57]&((~bit[58]&((bit[61]^bit[59]) | (bit[60]&bit[59]))) | ( bit[58]&((~bit[59]&((~bit[62]&(bit[61]^bit[60])) | (bit[62]&~(bit[61] | bit[60])))) /*| ( bit[59]&(expr_4))*/))))))  ) ) | (~control&out[0]);
		control=control&bit[56]&bit[57]&bit[58]&bit[59]; //remember to update control while merging
		
		//------------------56-59------------------------

		out[4] = (control & (ZERO))  | (~control & out[4]);
		out[3] = (control & (~bit[63]&~bit[62]&~bit[61]))  | (~control & out[3]);
		out[2] = (control & ((bit[63]&~bit[61]) | (bit[62]&~bit[61])))  | (~control & out[2]);
		out[1] = (control & (~bit[62]))  | (~control & out[1]);
		out[0] = (control & ((~bit[63]&bit[62]) | (~bit[63]&bit[61])))  | (~control & out[0]);
		control=control&bit[60];
		//-------------60--------------
		
		for(k=0;k<8;k++){
			
				sample_t=out[4]&mask;
				sample_t=(sample_t<<1) | (out[3]&mask);
				sample_t=(sample_t<<1) | (out[2]&mask);
				sample_t=(sample_t<<1) | (out[1]&mask);
				sample_t=(sample_t<<1) | (out[0]&mask);
				sample_n[k]=sample_t;
				
				out[4]=out[4]>>1;
				out[3]=out[3]>>1;
				out[2]=out[2]>>1;
				out[1]=out[1]>>1;
				out[0]=out[0]>>1;

		}

		for(k=0;k<64;k++){
			sample[k]=(int)sample_o[k];
		}
		clock2 = cpucycles();

		clock3=clock3+(clock2-clock1);
		clock4=clock4+(clock2-clock11);

//-----------------------------------------------------------------------------------------------------

		if(control==0xffffffffffffffff)//even if the sample is not found within depth 64
			break;
	}

	printf("%lu\n",clock3/repeat);
    printf("%lu\n",clock4/repeat);

	return 1;
}


		
		

		






