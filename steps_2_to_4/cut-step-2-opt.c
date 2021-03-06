
//gcc -Wextra -O3 -fomit-frame-pointer -march=native -o step_2_opt cut-step-2-opt.c

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"randombytes.c"
#include"fips202.c"
#include "cpucycles.c"

//-------shake------
void initRandom(uint8_t *x, uint32_t len){

	randombytes(x,len);
}
//-------shake ends-

int main(){

	int64_t i,j,k;
	uint64_t bit[64];
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

	uint64_t disable_update,control;
	clock3=0;
	clock4=0;
	int repeat=1000000;
	for(i=0;i<repeat;i++){	

		memset(out,0,5*sizeof(uint64_t));  

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

		disable_update=0; 
		control=0xffffffffffffffff;

		out[2] = bit[2]&(bit[1] | bit[3]&bit[0]);
		
		out[1] = (~bit[1]&~bit[0]) | ((~bit[3]&~bit[2] | ~bit[4]&bit[3]&bit[2])&bit[0]);
		
		out[0] = (~bit[2]&(bit[3] | ~bit[0])) | (~bit[3]&bit[1]);
		
		disable_update= disable_update | (control & ~(bit[0] & bit[1]));
		control=control & bit[0] & bit[1];
		//-------------1--------------

		out[3] = (~disable_update & (bit[6]&bit[5]&bit[4]&bit[2])) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((bit[4]&bit[3]) | ((~bit[6]&bit[4] | ~bit[5]&~bit[4])&bit[2]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((bit[4]&(bit[3] | ~bit[5]&bit[2])) | (~bit[4]&~bit[3]&~bit[2]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((~bit[3]&~bit[2]) | (bit[4]&(~bit[6]&bit[2] | ~bit[5]&~bit[2])))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[2] & bit[3]));
		control=control & bit[2] & bit[3];
		//-------------2--------------

		out[3] = (~disable_update & ((~bit[6]&bit[5]) | (bit[7]&~bit[6]&bit[4]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((bit[6]&bit[5]) | ((bit[8] | ~bit[7])&bit[6]&bit[4]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & (((bit[8] | ~bit[7])&~bit[6]&bit[4]) | (~bit[8]&bit[6]&bit[4]))) | (disable_update & out[1]); 
		
		out[0] = (~disable_update & ((~bit[7]&bit[5]) | (~bit[6]&~bit[5]&~bit[4]) | ((~bit[7]&bit[6] | ~bit[8]&bit[7])&bit[4]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[4] & bit[5]));
		control=control & bit[4] & bit[5];
		//-------------3--------------

		out[3] = (~disable_update & (~bit[8]&(bit[7] | bit[9]&bit[6]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & (bit[8]&(bit[7] | ~bit[10]&~bit[9]&bit[6]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((~bit[10]&bit[6]) | (~bit[9]&(bit[6] | ~bit[8]&bit[7])))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((~bit[9]&(bit[7] | bit[6])) | (bit[10]&bit[8]&bit[6]) | (~bit[8]&~bit[7]&~bit[6]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[6] & bit[7]));
		control=control & bit[6] & bit[7];
		//-------------4--------------

		out[3] = (~disable_update & ((bit[11]&~bit[10]&bit[8]) | (bit[10]&~bit[9]&~bit[8]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & (bit[9] | (bit[11]&bit[10]&~bit[8]) | ((~bit[12] | ~bit[11])&bit[10]&bit[8]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((bit[11]&bit[9]) | ((~bit[12] | ~bit[11]&~bit[10])&bit[8]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & (bit[10]&(~bit[11] | bit[9]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[8] & bit[9]));
		control=control & bit[8] & bit[9];
		//-------------5--------------
        
		out[3] = (~disable_update & (((~bit[11] | bit[12])&~bit[10]) | ((~bit[14] | bit[13])&~bit[12]&bit[10]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & (((~bit[13] | ~bit[10])&~bit[12]&~bit[11]) | (bit[13]&bit[11]) | (~bit[14]&~bit[13]&bit[10]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & (((~bit[13] | ~bit[14]&bit[12])&bit[11]) | ((~bit[13] | ~bit[12])&bit[14]&bit[10]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & (((~bit[12] | ~bit[11]&~bit[10])&bit[13]) | ((~bit[14] | ~bit[12])&bit[10]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[10] & bit[11]));
		control=control & bit[10] & bit[11];
		//-------------6--------------

		out[3] = (~disable_update & ((bit[15]&bit[14]) | (~bit[14]&~bit[13]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[14]&(bit[15] | ~bit[12])) | (~bit[16]&bit[12]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & (bit[13] | ((~bit[15] | ~bit[17]&bit[14])&bit[12]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((~bit[15]&bit[12]) | (bit[15]&~bit[12]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[12] & bit[13]));
		control=control & bit[12] & bit[13];
		//-------------7--------------

		out[4] = (~disable_update & (~bit[18]&bit[17]&bit[16]&bit[15])) | (disable_update & out[4]);
		
		out[3] = (~disable_update & (~bit[16]&~bit[15]&~bit[14])) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[17] | ~bit[16])&~bit[15])) | (disable_update & out[2]);
		
		out[1] = (~disable_update & (~bit[16]&(~bit[17] | bit[15]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & (bit[17]&(~bit[16] | ~bit[15]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[14] & bit[15]));
		control=control & bit[14] & bit[15];
		//-------------8--------------

		out[4] = (~disable_update & (~bit[20]&~bit[19]&bit[18]&bit[16])) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((~bit[17]&~bit[16]) | (bit[20]&~bit[19]&bit[18]&bit[16])))  | (disable_update & out[3]);
		
		out[2] = (~disable_update & (bit[17] | (~bit[18]&~bit[16]) | ((bit[20] | bit[19])&bit[18]&bit[16])))  | (disable_update & out[2]);
		
		out[1] = (~disable_update & (((~bit[18] | ~bit[16])&~bit[19]) | ((bit[19] | bit[20])&bit[18]&bit[16])))  | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((bit[19]&~bit[18]) | ((~bit[17] | ~bit[18])&~bit[16]) | ((~bit[20] | ~bit[19])&bit[18]&bit[16])))  | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[16] & bit[17]));
		control=control & bit[16] & bit[17];
		//-------------9--------------
        
		out[4] = (~disable_update & ((~bit[21]&~bit[20]&bit[19]) | ((bit[22]&bit[21] | ~bit[22]&~bit[21])&bit[20]&bit[18]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((~bit[19]&~bit[18]) | (~bit[20]&bit[18]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((bit[21]&(~bit[22] | ~bit[20])) | (bit[20]&~bit[18]) | (bit[22]&~bit[21]&bit[18]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & (((~bit[21] | (~bit[23]&bit[20]))&bit[22]&bit[18]) | ((bit[21]&~bit[20] | ~bit[21]&bit[20]&~bit[19])&~bit[18]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & (((bit[22] | ~bit[18])&~bit[21]&bit[20]) | (~bit[22]&bit[21]&bit[18]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[18] & bit[19]));
		control=control & bit[18] & bit[19];
		//-------------10--------------
        
		out[4] = (~disable_update & ((bit[23]&bit[22]&bit[21]) | ((~bit[25] | ~bit[24])&bit[23]&bit[22]&bit[20]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & (((~bit[21] | bit[23]&~bit[22])&~bit[20]) | ((~bit[24] | ~bit[23])&~bit[22]&bit[20]) | (bit[25]&bit[24]&bit[23]&bit[22]&bit[20]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & (~bit[23] | (~bit[22]&bit[21]) | (bit[25]&bit[24]&bit[22]&bit[20]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((~bit[24]&bit[23]&bit[21]) | (bit[24]&~bit[23]&bit[20]) | ((bit[24] | ~bit[23] | ~bit[20])&~bit[22]) | ((~bit[24] | bit[25])&bit[23]&bit[22]&bit[20]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & (((~bit[21] | ~bit[23]&~bit[22])&~bit[20]) | ((bit[24]&bit[22] | ~bit[25]&bit[20])&bit[23]) | ((~bit[24] | bit[23])&~bit[22]&bit[20]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[20] & bit[21]));
		control=control & bit[20] & bit[21];
		//-------------11--------------

		out[4] = (~disable_update & (~bit[26]&bit[25]&bit[24]&bit[22])) | (disable_update & out[4]);
		
		out[3] = (~disable_update & (((~bit[23] | bit[25]&bit[24])&~bit[22]) | ((bit[26]&bit[24] | ~bit[26]&~bit[25]&~bit[24])&bit[22]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & (~bit[24] | (bit[26]&bit[25]&bit[22]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & (((bit[23] | bit[24]&~bit[22])&~bit[25]) | ((~bit[24] | ~bit[25])&bit[26]&bit[22]) | ((~bit[26] | ~bit[27])&bit[25]&bit[24]&bit[22]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & (((~bit[24] | bit[23])&~bit[25]) | ((~bit[26] | ~bit[24])&bit[23]) | ((~bit[27] | bit[26])&bit[25]&bit[24]&bit[22]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[22] & bit[23]));
		control=control & bit[22] & bit[23];
		//-------------12--------------
        
		out[4] = (~disable_update & ((bit[25] | (~bit[29] | ~bit[28])&bit[24])&bit[27]&bit[26])) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((~bit[27]&~bit[26]&(bit[25] | bit[24])) | (bit[28]&bit[24]&(~bit[26] | bit[29]&bit[27])))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((bit[28]&~bit[26]&bit[24]) | ((bit[26]&~bit[24] | bit[27]&~bit[26])&~bit[25]) | ((~bit[29] | ~bit[24])&bit[26]&~bit[28]&bit[27]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & (((~bit[25]&~bit[24] | bit[28]&bit[27]&bit[24])&~bit[26]) | ((~bit[28]&~bit[27] | bit[29]&bit[27])&bit[26]&bit[24]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((~bit[28]&~bit[26]) | (~bit[27]&bit[24]) | ((bit[27]&~bit[25] | ~bit[26])&~bit[24]) | ((bit[28] | bit[29]&~bit[25])&bit[27]&bit[26]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[24] & bit[25]));
		control=control & bit[24] & bit[25];
		//-------------13--------------

		out[4] = (~disable_update & (bit[28]&(bit[27] | bit[30]&bit[26]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((~bit[30]&bit[29]&bit[26]) | (~bit[29]&(~bit[27]&~bit[26] | ~bit[28]&bit[26])))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[29]&bit[28]&bit[27]) | (~bit[31]&~bit[30]&bit[29]&bit[28]&bit[26]) | ((~bit[26] | bit[30]&~bit[28])&bit[29]&~bit[27]) | ((~bit[28] | ~bit[31]&bit[30])&~bit[29]&bit[26]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((~bit[30]&(bit[26] | bit[29]&bit[27])) | (~bit[28]&~bit[26]) | (bit[29]&bit[28]&bit[26]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((bit[29]&(bit[31] | ~bit[28])) | (~bit[30]&(bit[26] | bit[28]&bit[27])) | (bit[29]&~bit[26]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[26] & bit[27]));
		control=control & bit[26] & bit[27];
		//-------------14--------------
        
		out[4] = (~disable_update & ((bit[31]&~bit[30]&bit[29]) | ((bit[32] | bit[33]&~bit[31])&bit[30]&bit[28]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((bit[30]&bit[29]) | ((~bit[32]&bit[31] | ~bit[33]&~bit[32])&bit[30]&bit[28]) | ((~bit[29]&~bit[28] | ~bit[32]&bit[28])&~bit[31]&~bit[30]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[32]&bit[30]&bit[29]) | ((bit[31]&~bit[29]&~bit[28] | ~bit[31]&bit[28])&~bit[30]) | ((bit[31] | ~bit[33]&~bit[32])&bit[30]&bit[28]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & (((bit[31]&~bit[30] | bit[33]&bit[28])&bit[32]) | ((~bit[31] | ~bit[30])&~bit[29]&~bit[28]) | (~bit[33]&~bit[32]&bit[31]&bit[30]&bit[28]) | (~bit[31]&~bit[30]&bit[28]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((bit[31]&bit[29]) | (bit[30]&(~bit[29]&~bit[28] | ~bit[33]&bit[28])))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[28] & bit[29]));
		control=control & bit[28] & bit[29];
		//-------------15--------------
        
		out[4] = (~disable_update & (((bit[34] | ~bit[32])&~bit[33]&bit[31]) | ((bit[35] | (~bit[34]&bit[33]) | (bit[34]&~bit[33]))&~bit[32]&bit[30]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & (((bit[34] | bit[33])&bit[32]&bit[30]) | (~bit[32]&(bit[34]&bit[33]&bit[31] | ~bit[33]&~bit[31]&~bit[30])))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[34]&~bit[33]&(bit[31] | bit[30])) | (bit[33]&(bit[34]&bit[30] | ~bit[32]&~bit[31]&~bit[30])))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((bit[32]&bit[31]) | (bit[33]&~bit[32]&~bit[31]&~bit[30]) | ((bit[35]&bit[34] | ~bit[35]&~bit[34])&bit[30]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & (((bit[34] | ~bit[33]&~bit[32])&bit[31]) | (bit[33]&~bit[31]&~bit[30]) | ((bit[34] | bit[32] | bit[33])&~bit[35]&bit[30]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[30] & bit[31]));
		control=control & bit[30] & bit[31];
		//-------------16--------------
        
		out[4] = (~disable_update & ((bit[34]&~bit[33]&~bit[32]) | ((~bit[37] | ~bit[36])&bit[35]&~bit[34]&bit[32]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & (((~bit[36] | ~bit[35])&bit[33]) | ((bit[37] | ~bit[36])&bit[35]&bit[34]&bit[32]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[36]&bit[34]) | (bit[35]&bit[33]) | ((bit[37] | bit[36])&bit[35]&bit[32]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((bit[35]&bit[34]&~bit[32]) | (bit[36]&bit[34]&(bit[33] | bit[32])) | (bit[35]&bit[32]&(bit[36] | ~bit[37]&~bit[34])) | (~bit[35]&~bit[34]&(~bit[33]&~bit[32] | ~bit[36]&bit[32])))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((~bit[36]&~bit[34]&bit[33]) | ((bit[34] | ~bit[36]&bit[35])&~bit[37]&bit[32]) | ((bit[35]&bit[34] | bit[36]&~bit[35])&~bit[33]&~bit[32]) | ((bit[36] | ~bit[32])&~bit[35]&~bit[34]&~bit[33]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[32] & bit[33]));
		control=control & bit[32] & bit[33];
		//-------------17--------------

		out[4] = (~disable_update & ((bit[37]&(~bit[35]&~bit[34] | ~bit[36]&bit[34])) | (~bit[37]&(~bit[38]&bit[34] | bit[38]&bit[36]&~bit[35])))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & (((bit[38] | bit[37] | ~bit[36])&bit[35]) | ((bit[36] | ~bit[39]&bit[38])&bit[37]&bit[34]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((bit[37]&bit[35]) | ((bit[37] | bit[36]&~bit[35])&~bit[38]) | ((bit[39]&bit[36] | bit[38]&~bit[37]&~bit[36])&bit[34]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((~bit[38]&bit[35]) | ((bit[38]&~bit[37] | ~bit[39]&bit[34])&bit[36]) | ((~bit[38] | ~bit[36]&~bit[35])&bit[37]&~bit[34]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((bit[36]&(~bit[34] | bit[39]&~bit[37])) | (bit[39]&bit[38]&bit[37]&bit[34]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[34] & bit[35]));
		control=control & bit[34] & bit[35];
		//-------------18--------------
        
		out[4] = (~disable_update & (((~bit[40] | bit[39])&~bit[38]&bit[37]) | ((bit[39]&~bit[38] | bit[41]&~bit[40]&~bit[39]&bit[38])&bit[36]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((bit[41]&~bit[40]&bit[39]&~bit[38]&bit[36]) | ((bit[38]&bit[37] | ~bit[38]&~bit[37]&~bit[36])&~bit[39]) | ((~bit[41] | bit[40])&bit[38]&bit[36]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((bit[41]&bit[38]&bit[36]) | ((bit[37] | ~bit[39]&~bit[38]&bit[36])&~bit[40]) | ((~bit[37] | ~bit[38])&bit[39]&~bit[36]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((bit[39]&~bit[38]&~bit[36]) | ((bit[40] | ~bit[39])&bit[37]) | ((bit[39] | bit[40])&~bit[41]&bit[39]&bit[36]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((bit[39]&bit[37]) | ((~bit[39] | ~bit[38])&~bit[37]&~bit[36]) | ((bit[38] | ~bit[41]&bit[39])&~bit[40]&bit[36]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[36] & bit[37]));
		control=control & bit[36] & bit[37];
		//-------------19--------------
        
		out[4] = (~disable_update & ((bit[42]&~bit[41]&bit[40]) | ((~bit[39] | ~bit[41])&bit[40]&~bit[38]) | ((bit[43] | bit[41])&~bit[42]&bit[40]&bit[38]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((bit[42]&bit[41]&bit[39]) | ((~bit[43] | bit[42])&bit[41]&bit[40]&bit[38]) | ((~bit[42]&bit[40]&~bit[38] | ~bit[40]&bit[39])&~bit[41]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((bit[41]&(bit[39] | bit[40]&~bit[38])) | (~bit[42]&bit[38]&(~bit[41] | ~bit[43]&bit[40])))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((bit[41]&~bit[40]&bit[39]) | ((~bit[43] | ~bit[40])&bit[41]&bit[38]) | ((bit[42]&bit[40] | ~bit[41]&~bit[40]&~bit[38])&~bit[39]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((~bit[42]&bit[39]) | (bit[40]&(bit[41]&bit[39] | ~bit[43]&bit[38])) | (~bit[40]&(bit[42]&bit[41]&bit[38] | ~bit[41]&~bit[39]&~bit[38])))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[38] & bit[39]));
		control=control & bit[38] & bit[39];
		//-------------20--------------

		out[4] = (~disable_update & ((~bit[42]&bit[41]) | (bit[45]&bit[42]&bit[40]) | (~bit[45]&bit[44]&bit[43]&~bit[42]&bit[40]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & (((~bit[44] | ~bit[43])&bit[42]&bit[41]) | ((~bit[45] | ~bit[44]&bit[43])&bit[42]&bit[40]) | (~bit[43]&~bit[42]&~bit[41]&~bit[40]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[43]&bit[41]) | (bit[45]&bit[44]&bit[43]&bit[40]) | (bit[44]&bit[42]&(bit[41] | bit[40])))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((~bit[44]&bit[41]) | (bit[43]&(~bit[41]&~bit[40] | bit[45]&bit[44]&bit[40])) | (~bit[44]&(~bit[45]&bit[43]&bit[42] | ~bit[43]&~bit[42]&bit[40])))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((~bit[43]&~bit[41]) | ((bit[45] | ~bit[44])&bit[42]&bit[40]) | ((~bit[41] | ~bit[44]&bit[43])&~bit[42]&~bit[40]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[40] & bit[41]));
		control=control & bit[40] & bit[41];
		//-------------21--------------
        
		out[4] = (~disable_update & ((~bit[46]&~bit[45]&bit[43]) | (bit[45]&~bit[44]&bit[42]) | ((~bit[42] | bit[47]&bit[46]&bit[45])&bit[44]&~bit[43]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((bit[45]&~bit[44]&bit[43]) | (bit[47]&bit[44]&bit[42]) | (bit[44]&~bit[43]&(~bit[46] | ~bit[45]&~bit[42])))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[47]&bit[45]&bit[42]) | (~bit[45]&~bit[44]&~bit[42]) | ((~bit[46] | ~bit[45])&bit[43]) | ((~bit[47]&bit[42] | bit[45]&~bit[43]&~bit[42])&bit[46]&bit[44]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & (((bit[46]&bit[44] | ~bit[46]&~bit[45]&~bit[44])&bit[42]) | ((bit[47] | bit[46])&bit[45]&~bit[43]) | ((~bit[46] | bit[44])&bit[45]&~bit[42]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((bit[44]&bit[43]) | (bit[45]&(bit[46]&bit[43] | bit[47]&~bit[46]&bit[42])) | (bit[44]&(bit[46]&~bit[42] | ~bit[47]&~bit[45]&bit[42])))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[42] & bit[43]));
		control=control & bit[42] & bit[43];
		//-------------22--------------
        
		out[4] = (~disable_update & ((bit[46]&~bit[45]&~bit[44]) | ((bit[49] | ~bit[48] | bit[47])&~bit[46]&bit[44]) | ((bit[45] | bit[49]&bit[44])&bit[48]&bit[47]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((~bit[48]&~bit[47]&(bit[46] | bit[45])) | (bit[49]&bit[44]&(bit[46] | bit[48]&~bit[47])) | (~bit[47]&bit[46]&~bit[44]) | (~bit[48]&bit[47]&~bit[46]&bit[44]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[49]&bit[46]&bit[44]) | ((bit[47]&bit[46] | ~bit[47]&~bit[46])&~bit[45]&~bit[44]) | ((bit[47] | ~bit[46])&~bit[48]&bit[45]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((bit[46]&bit[45]) | (bit[48]&bit[44]) | (~bit[48]&bit[46]&~bit[44]) | (bit[49]&bit[47]&bit[46]&bit[44]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((~bit[48]&bit[46]&bit[44]) | ((bit[48] | ~bit[46])&~bit[44]) | ((~bit[49] | bit[46])&bit[47]&bit[44]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[44] & bit[45]));
		control=control & bit[44] & bit[45];
		//-------------23--------------
        
		out[4] = (~disable_update & (((bit[49] | bit[50]&~bit[48])&bit[47]) | ((bit[49]&~bit[46] | bit[51]&~bit[49]&bit[46])&bit[48]) | ((~bit[51] | ~bit[49])&bit[50]&bit[48]&~bit[47]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((bit[48]&bit[47]) | (bit[49]&bit[48]&~bit[46]) | (~bit[50]&(~bit[49]&bit[47] | bit[48]&bit[46])))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[49]&bit[48]&~bit[46]) | (bit[50]&~bit[48]&(bit[47] | bit[46])) | (bit[49]&bit[46]&(bit[50] | ~bit[51]&bit[48])))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((~bit[50]&(bit[47] | ~bit[49]&bit[48])) | (~bit[49]&~bit[48]&(bit[47] | bit[46])) | (bit[50]&bit[48]&bit[46]&(bit[49] | bit[51])))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & (((bit[51]&bit[49] | ~bit[50]&~bit[48])&bit[46]) | ((bit[50]&bit[48] | ~bit[49]&~bit[47])&~bit[46]) | ((bit[51] | ~bit[46])&~bit[49]&bit[48]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[46] & bit[47]));
		control=control & bit[46] & bit[47];
		//-------------24--------------
        
		out[4] = (~disable_update & (((bit[49] | ~bit[50]&bit[48])&~bit[52]&~bit[51]) | ((~bit[52] | ~bit[51])&~bit[53]&~bit[50]&bit[48]) | ((bit[52]&~bit[49]&~bit[48] | bit[51]&bit[49])&bit[50]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & (((~bit[50]&bit[49] | ~bit[51]&bit[50]&~bit[48])&bit[52]) | ((bit[48] | ~bit[52]&bit[49])&bit[51]&bit[50]) | ((bit[53]&bit[52] | ~bit[53]&~bit[52])&~bit[51]&~bit[50]&bit[48]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & (((~bit[53] | ~bit[51] | ~bit[50])&bit[49]) | ((~bit[51] | bit[48])&~bit[52]&bit[50]) | ((bit[51] | ~bit[52])&bit[53]&bit[48]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((bit[53]&bit[50]&bit[49]) | (~bit[51]&(bit[50]&~bit[48] | bit[53]&bit[48])) | (bit[51]&bit[48]&(~bit[52] | ~bit[53]&bit[50])))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((~bit[51]&(bit[49] | ~bit[52]&bit[48])) | (~bit[51]&~bit[48]&(bit[52] | ~bit[50])) | (~bit[53]&bit[51]&bit[48]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[48] & bit[49]));
		control=control & bit[48] & bit[49];
		//-------------25--------------
        
		out[4] = (~disable_update & ((bit[54]&~bit[52]&bit[50]) | ((~bit[54]&~bit[50] | bit[53]&~bit[52])&~bit[51]) | ((bit[54] | bit[52])&bit[53]&~bit[50]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((~bit[52]&~bit[50]) | (bit[54]&~bit[53]&bit[50]) | (~bit[54]&(bit[53]&bit[51] | bit[52]&bit[50])))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[53]&bit[51]) | (~bit[54]&~bit[53]&~bit[52]&~bit[50]) | ((bit[52] | bit[54])&~bit[55]&bit[50]) | ((bit[51] | bit[53]&bit[50])&bit[54]&bit[52]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & (((bit[55] | ~bit[50])&~bit[53]&bit[52]) | ((~bit[52]&~bit[50] | bit[55]&bit[50])&bit[54]) | ((~bit[55]&bit[53] | bit[54])&bit[52]&bit[50]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((~bit[54]&bit[52]&(bit[53] | ~bit[50])) | (bit[55]&bit[50]&(bit[53] | bit[54])) | (bit[54]&bit[53]&~bit[52]&~bit[51]&~bit[50]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[50] & bit[51]));
		control=control & bit[50] & bit[51];
		//-------------26--------------
        
		out[4] = (~disable_update & (((~bit[56] | ~bit[55] | ~bit[54])&bit[53]) | ((~bit[57] | ~bit[56] | ~bit[55])&bit[54]&bit[52]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((~bit[54]&bit[53]) | (bit[56]&bit[55]&(bit[53] | bit[52])) | (bit[55]&bit[52]&(bit[57] | ~bit[54])))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((bit[56]&bit[53]) | (bit[55]&~bit[54]&(bit[53] | bit[56]&bit[52])) | (~bit[55]&(bit[54]&bit[53] | (~bit[56]&bit[52]) | (~bit[54]&~bit[53]&~bit[52]))))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((~bit[56]&bit[53]) | ((bit[56]&bit[54] | ~bit[55]&~bit[54])&bit[52]) | ((~bit[52] | bit[57]&bit[56])&bit[55]&~bit[54]&~bit[53]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((bit[55]&~bit[52]) | (~bit[56]&bit[54]&bit[53]) | ((bit[57]&bit[56]&bit[55] | ~bit[57]&~bit[55]&bit[52])&bit[54]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[52] & bit[53]));
		control=control & bit[52] & bit[53];
		//-------------27--------------
        
		out[4] = (~disable_update & ((~bit[57] | ~bit[56])&bit[55])) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((bit[56]&bit[55]) | ((~bit[56] | ~bit[57])&~bit[55]))) | (disable_update & out[3]);  
		
		out[2] = (~disable_update & ((bit[57]&bit[56]&~bit[55]) | (~bit[58]&bit[55]&(~bit[57]&bit[56] | bit[57]&~bit[56])))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((bit[57]&bit[55]) | (bit[56]&~bit[55]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((bit[57]&~bit[55]) | (~bit[58]&(bit[57]&bit[56] | ~bit[57]&~bit[56]&bit[55])))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[54]);
		control=control & bit[54];
		//-------------28---------------
        
		out[4] = (~disable_update & ((~bit[57]&bit[55]) | ((bit[59] | bit[58])&bit[57]&~bit[55]) | ((~bit[60] | ~bit[55])&bit[59]&bit[58]&~bit[56]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((~bit[59]&bit[56]) | (bit[58]&bit[57]&~bit[56]&~bit[55]) | ((bit[60] | bit[59])&~bit[58]&bit[55]) | ((~bit[58] | (~bit[60]&bit[59]) | (bit[60]&~bit[59]))&~bit[57]&bit[55]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & (((~bit[59] | ~bit[58] | ~bit[57])&bit[56]) | ((~bit[60] | ~bit[59])&bit[58]&bit[55]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((~bit[59]&(~bit[58] | bit[55])) | (~bit[58]&(bit[56] | ~bit[57]&~bit[55])) | (~bit[60]&bit[58]&bit[57]&bit[55]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & (((bit[60] | bit[58]&bit[57])&bit[59]&bit[55]) | ((~bit[55] | ~bit[59])&~bit[58]&bit[57]) | ((bit[58]&~bit[56] | bit[57])&~bit[59]&~bit[55]) | (~bit[60]&~bit[59]&bit[55]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[55] & bit[56]));
		control=control & bit[55] & bit[56];
		//-------------29--------------
        
		out[4] = (~disable_update & (((~bit[62] | bit[61])&~bit[59]&bit[57]) | ((bit[59]&~bit[58] | bit[60]&~bit[59])&~bit[61]&~bit[57]) | ((~bit[57] | bit[62])&bit[61]&bit[60]&bit[59]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((~bit[59]&bit[58]) | ((~bit[60] | bit[62]&bit[61])&bit[57]) | ((bit[61] | ~bit[59])&bit[60]&~bit[57]))) | (disable_update & out[3]);
		
		out[2] = (~disable_update & ((~bit[61]&(bit[58] | bit[57])) | (~bit[60]&(~bit[61]&bit[59] | ~bit[59]&bit[58])) | (bit[59]&(bit[60]&bit[58] | ~bit[62]&bit[57])))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((~bit[60]&bit[58]) | ((~bit[62] | ~bit[59])&bit[61]&bit[58]) | ((~bit[61]&bit[57] | ~bit[60]&~bit[57])&bit[59]) | ((bit[61]&bit[60] | bit[59])&bit[62]&bit[57]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & ((bit[61]&(bit[60]&~bit[59] | ~bit[60]&bit[58])) | (bit[62]&bit[57]&(~bit[61] | ~bit[59])) | (~bit[61]&(bit[60]&~bit[57] | ~bit[60]&~bit[59]&bit[57])))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~(bit[57] & bit[58]));
		control=control & bit[57] & bit[58];
		//-------------30--------------

		out[4] = (~disable_update & ((bit[61]&(bit[63] | ~bit[60])) | (~bit[63]&bit[62]&~bit[61]&bit[60]))) | (disable_update & out[4]);
		
		out[3] = (~disable_update & ((~bit[61] | ~bit[63]&bit[62])&bit[60])) | (disable_update & out[3]);
		
		out[2] = (~disable_update & (bit[62]&(bit[61] | bit[63]&bit[60]))) | (disable_update & out[2]);
		
		out[1] = (~disable_update & ((bit[63]&(~bit[62] | bit[60])) | (~bit[61]&~bit[60]))) | (disable_update & out[1]);
		
		out[0] = (~disable_update & (((bit[63]&~bit[62] | bit[62]&~bit[61])&bit[60]) | ((~bit[62]&~bit[61] | ~bit[63]&bit[62]&bit[61])&~bit[60]))) | (disable_update & out[0]);
		//-------------31--------------

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
		if(disable_update!=0xffffffffffffffff)
			break;
	}

	printf("%lu\n",clock3/repeat);
    printf("%lu\n",clock4/repeat);

	return 1;
}








