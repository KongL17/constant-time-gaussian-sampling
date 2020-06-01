//gcc -Wextra -O3 -fomit-frame-pointer -march=native -o sample_128_opt sampler-128-opt.c

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"randombytes.c"
#include"fips202.c"
#include "cpucycles.c"

#define ZERO 0x0000000000000000

//-------shake------
void initRandom(uint8_t *x, uint32_t len){

	randombytes(x,len);
}
//-------shake ends-

int main(){

	int64_t i,j,k;
	uint64_t bit[128];
	uint64_t sample[64];
	volatile uint64_t out[6];
	
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
	uint32_t nrounds=6;
 	unsigned char* byte_bank = (unsigned char*) ((void*) bit);

	for (i = 0; i < 25; ++i)
	    s[i] = 0;
	initRandom(seed,SEEDBYTES);
	keccak_absorb(s, SHAKE128_RATE, seed, SEEDBYTES, 0x1F);
	
	//-------------------shake ends---------------

	uint64_t disable_update,control,vart;

	clock3=0;
	clock4=0;
	int repeat=1000000;
	for(i=0;i<repeat;i++){	

		for(k=0;k<6;k++)
		out[k]=out[k]^out[k];

		clock1=0;
		clock2=0;

		clock11 = cpucycles();

		//-------------------shake----------------------
		keccak_squeezeblocks(byte_bank, nrounds, s, SHAKE128_RATE);
		
		keccak_squeezeblocks(temp_byte, 1, s, SHAKE128_RATE);

		for(j=0;j<(128*8-SHAKE128_RATE*nrounds);j++){
			byte_bank[nrounds*SHAKE128_RATE+j]=temp_byte[j];			
		}

		//-------------------shake ends-----------------

		clock1 = cpucycles();

		//-----------------------------------------------------------------------------------------------------------

		disable_update=0; 
		control=0xffffffffffffffff; 

		out[2] = (bit[2]&bit[1]) | ((bit[4] | ~bit[3])&bit[1]&bit[0]);
		out[1] = (~bit[1]&~bit[0]) | (bit[3]&bit[1]&bit[0]) | ((~bit[1] | ~bit[4]&bit[0])&~bit[3]&~bit[2]);
		out[0] = ((~bit[0] | bit[3]&~bit[1])&~bit[2]) | ((~bit[4] | ~bit[5]&bit[1])&bit[3]&bit[0]) | (~bit[3]&bit[1]&~bit[0]);
		vart = bit[0] & bit[1] & bit[2];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------1--------------

		out[3] = (~disable_update & ((bit[7] | ~bit[4])&~bit[6]&bit[3])) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[5] | (bit[8] | ~bit[7])&bit[4])&bit[6]&bit[3]) | ((~bit[5]&~bit[4] | ~bit[6]&bit[4])&~bit[3]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[5]&bit[4]&~bit[3]) | ((~bit[8]&(~bit[7] | bit[6]) | bit[8]&~bit[6])&bit[4]&bit[3]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[6]&bit[4]&~bit[3]) | ((bit[4] | ~bit[7]&bit[3])&bit[5]) | ((~bit[7]&bit[6] | ~bit[8]&bit[7])&bit[4]&bit[3]))) | (disable_update & out[0]);

		vart = bit[3] & bit[4] & bit[5];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------2--------------

		out[3] = (~disable_update & (((bit[7]&~bit[6] | bit[9]&~bit[7]&bit[6])&~bit[8]) | (bit[10]&~bit[9]&bit[7]&bit[6]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[7] | ~bit[10]&~bit[9]&bit[6])&bit[8]) | ((bit[9] | bit[11]&bit[10])&bit[7]&bit[6]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[10] | ~bit[9])&~bit[7]&bit[6]) | ((bit[11]&bit[9]&bit[6] | ~bit[9]&~bit[8]&~bit[6])&bit[7]))) | (disable_update & out[1]); 
		out[0] = (~disable_update & (((bit[8] | (bit[9] | ~bit[11])&bit[7])&bit[10]&bit[6]) | (~bit[9]&(bit[7]^bit[6])) | (~bit[8]&~bit[7]&~bit[6]))) | (disable_update & out[0]);

		vart = bit[6] & bit[7] & bit[8];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------3--------------

		out[3] = (~disable_update & (((~bit[11] | bit[12])&~bit[10]&bit[9]) | ((~bit[14] | bit[13])&~bit[12]&bit[10]&bit[9]) | (bit[11]&~bit[10]&~bit[9]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[13]&bit[11] | ~bit[14]&~bit[13]&bit[10])&bit[9]) | ((~bit[11] | ~bit[12])&bit[10]&~bit[9]) | ((~bit[13] | ~bit[10])&~bit[12]&~bit[11]&bit[9]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[12] | ~bit[11]&~bit[10])&~bit[9]) | ((~bit[12] | ~bit[13]&bit[9])&bit[14]&bit[10]) | ((~bit[13] | ~bit[14]&bit[12])&bit[11]&bit[9]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[12] | ~bit[11]&~bit[10])&bit[13]&bit[9]) | ((~bit[14] | ~bit[12])&bit[10]&bit[9]) | (~bit[11]&bit[10]&~bit[9]))) | (disable_update & out[0]);

		vart = bit[9] & bit[10] & bit[11];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------4--------------

		out[4] = (~disable_update & (~bit[18]&bit[17]&bit[16]&bit[15]&bit[13]&bit[12])) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[15]&bit[14]) | (~bit[14]&~bit[13]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[12] | bit[15]&~bit[13])&~bit[14]) | ((~bit[16] | ~bit[17]&bit[13])&~bit[15]&bit[12]) | (~bit[16]&~bit[13]&bit[12]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[13]&~bit[12]) | (~bit[16]&bit[15]&bit[13]) | (~bit[17]&bit[14]&bit[12]) | ((~bit[13] | ~bit[17]&~bit[16])&~bit[15]&bit[12]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[17] | ~bit[13])&~bit[15]&bit[12]) | (bit[15]&~bit[12]) | (bit[17]&~bit[16]&bit[13]&bit[12]))) | (disable_update & out[0]);

		vart = bit[12] & bit[13] & bit[14];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------5--------------

		out[4] = (~disable_update & (~bit[20]&~bit[19]&bit[18]&bit[16]&bit[15])) | (disable_update & out[4]);
		out[3] = (~disable_update & (((~bit[17] | ~bit[15])&~bit[16]) | (bit[20]&~bit[19]&bit[18]&bit[16]&bit[15]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[18]&(bit[17] | bit[16]&~bit[15])) | ((bit[20] | bit[19])&bit[18]&bit[16]) | (~bit[18]&~bit[16]&bit[15]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[16] | ~bit[18])&~bit[19]&bit[15]) | ((~bit[18] | bit[16])&~bit[17]&~bit[15]) | ((bit[20] | bit[19])&bit[18]&bit[16]&bit[15]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[19] | ~bit[16])&bit[18]&~bit[17]) | ((bit[19] | ~bit[16])&~bit[18]&bit[15]) | (bit[16]&~bit[15]) | (~bit[20]&bit[19]&~bit[17]&bit[15]))) | (disable_update & out[0]);

		vart = bit[15] & bit[16] & bit[17];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------6--------------

		out[4] = (~disable_update & (((bit[20] | bit[23]&bit[19])&bit[22]&bit[21]&bit[18]) | ((~bit[22]&bit[20]&bit[18] | ~bit[20]&bit[19]&~bit[18])&~bit[21]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((~bit[20] | ~bit[18])&~bit[19]) | ((~bit[21] | bit[23]&~bit[22])&bit[19]&bit[18]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[21] | bit[20])&~bit[18]) | ((~bit[21] | ~bit[20])&bit[22]&~bit[19]&bit[18]) | (~bit[22]&bit[21]) | (~bit[23]&bit[19]&bit[18]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[22] | ~bit[24]&bit[23]&bit[21])&bit[19]&bit[18]) | ((bit[22]&bit[18] | bit[20]&~bit[18])&~bit[21]&~bit[19]) | (bit[21]&~bit[20]&~bit[18]) | (~bit[23]&bit[22]&bit[20]&bit[18]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[22] | ~bit[18])&~bit[21]&bit[20]) | ((~bit[21] | bit[24]&bit[23]&bit[22])&bit[19]&bit[18]) | ((bit[21]&~bit[19] | ~bit[23]&bit[19])&~bit[22]&bit[18]))) | (disable_update & out[0]);

		vart = bit[18] & bit[19] & bit[20];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------7--------------

		out[4] = (~disable_update & (((~bit[25] | ~bit[24])&bit[23]&bit[22]) | (~bit[26]&bit[25]&bit[24]&bit[22]&bit[21]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((~bit[23] | ~bit[24]&~bit[21])&~bit[22]) | ((bit[26] | ~bit[22])&bit[24]&bit[21]) | (~bit[26]&~bit[25]&~bit[24]&bit[22]&bit[21]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[24]&bit[21]) | (~bit[23]&~bit[21]) | ((~bit[25]&bit[23] | bit[26]&bit[25]&bit[22])&bit[21]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[23]&bit[22] | bit[25]&~bit[22]&bit[21])&~bit[24]) | ((bit[25] | ~bit[21])&~bit[23]&~bit[22]) | ((~bit[23] | ~bit[22])&bit[24]&~bit[21]) | ((~bit[21] | ~bit[26]&bit[22])&bit[25]&bit[24]) | ((~bit[27] | ~bit[25] | ~bit[24])&bit[26]&bit[22]&bit[21]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[22] | ~bit[25]&bit[21])&~bit[24]) | ((~bit[27] | bit[26])&bit[25]&bit[24]&bit[22]&bit[21]) | ((~bit[25] | bit[24])&bit[23]&~bit[21]) | (~bit[26]&bit[25]&bit[23]&bit[21]))) | (disable_update & out[0]);

		vart = bit[21] & bit[22] & bit[23];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------8--------------

		out[4] = (~disable_update & (((bit[26] | bit[28]&bit[24])&bit[27]&bit[25]) | ((~bit[29] | ~bit[28])&bit[27]&bit[26]&bit[24]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((~bit[28] | ~bit[27])&~bit[29]&bit[25]&bit[24]) | ((bit[28] | ~bit[27])&~bit[26]&~bit[25]&bit[24]) | (~bit[27]&~bit[26]&bit[25]&~bit[24])))  | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[27] | bit[28]&bit[24])&~bit[26]&~bit[25]) | ((~bit[25] | ~bit[28]&bit[27])&bit[26]&~bit[24]) | ((~bit[29]&bit[28]&bit[27] | bit[29]&~bit[27]&bit[25])&bit[24])))  | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[25] | ~bit[27]&bit[26])&~bit[28]&bit[24]) | ((~bit[24] | bit[28]&bit[27])&~bit[26]&~bit[25]) | ((bit[26] | ~bit[30]&bit[25])&bit[29]&bit[27]&bit[24])))  | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[26]&~bit[24]) | ((~bit[28] | ~bit[30]&bit[25])&bit[27]&bit[24]) | ((bit[29]&bit[25] | ~bit[27]&~bit[25])&bit[24]) | ((bit[28] | ~bit[25])&bit[27]&~bit[24])))  | (disable_update & out[0]);

		vart = bit[24] & bit[25] & bit[26];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------9--------------
        
		out[4] = (~disable_update & (((bit[32] | ~bit[27] | bit[33]&~bit[31])&bit[30]&bit[28]) | (bit[31]&~bit[30]&bit[29]&bit[27]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[30]&bit[29]&bit[27]) | ((~bit[30] | ~bit[27])&~bit[29]&~bit[28]) | ((~bit[28] | bit[31]&bit[29])&~bit[30]&~bit[27]) | (~(bit[31]^bit[30])&~bit[32]&bit[28]&bit[27]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[31]&~bit[30]&bit[29]&bit[28]) | ((bit[30] | ~bit[29])&~bit[28]&~bit[27]) | ((bit[31]&bit[28] | ~bit[32]&bit[29])&bit[30]&bit[27]) | ((bit[30]^bit[27])&~bit[31]&~bit[29]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[29]&bit[28]) | (bit[33]&bit[32]&bit[28]&bit[27]) | ((~bit[27] | bit[32]&bit[31] | ~bit[31]&bit[28])&~bit[30]) | ((~bit[30] | ~bit[31]&bit[27])&~bit[29]&~bit[28]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[31]&bit[29] | ~bit[33]&bit[28]&bit[27])&bit[30]) | ((~bit[29] | bit[31])&~bit[28]&bit[27]) | ((~bit[30]&~bit[29] | bit[29]&~bit[28])&~bit[27]))) | (disable_update & out[0]);

		vart = bit[27] & bit[28] & bit[29];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------10--------------
        
		out[4] = (~disable_update & (((bit[34] | ~bit[32]&~bit[30])&~bit[33]&bit[31]) | ((bit[34]&~bit[33] | (bit[35] | ~bit[34]&bit[33])&~bit[31])&~bit[32]&bit[30]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[32]&~bit[31]&~bit[30]) | (((~bit[36] | ~bit[35])&bit[30] | bit[34]&~bit[32]&~bit[30])&bit[33]&bit[31]) | ((bit[34] | bit[33])&bit[32]&bit[30]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[34] | ~bit[30])&bit[33]&~bit[31]) | ((~bit[36]&bit[34] | bit[35]&bit[33])&bit[31]&bit[30]) | ((bit[31]^bit[30])&~bit[34]&~bit[33]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[32]&bit[31]) | (bit[33]&~bit[31]&~bit[30]) | ((bit[35] | bit[36]&bit[33]&bit[31])&bit[34]&bit[30]) | ((~bit[33] | ~bit[31])&~bit[35]&~bit[34]&bit[30]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[33] | bit[34]&bit[30])&~bit[35]&~bit[31]) | ((~bit[33]&~bit[32] | bit[34])&bit[31]&~bit[30]) | ((~bit[36]&~bit[34]&bit[33] | (bit[36] | ~bit[34])&~bit[35]&~bit[33])&bit[31]&bit[30]) | (~bit[35]&bit[32]&bit[30]) | (bit[33]&~bit[31]&~bit[30]) | (bit[35]&bit[34]&~bit[33]&bit[31]))) | (disable_update & out[0]);

		vart = bit[30] & bit[31] & bit[32];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------11--------------

		out[4] = (~disable_update & (((bit[37]&~bit[34] | bit[38]&~bit[37]&bit[36])&~bit[35]&bit[33]) | ((~bit[37] | ~bit[36])&bit[35]&~bit[34]&~bit[33]) | ((~bit[38]&~bit[37] | bit[37]&~bit[36])&bit[34]&bit[33]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[37] | ~bit[36])&bit[35]&bit[34]) | ((bit[38] | bit[37] | ~bit[36])&bit[35]&bit[33]) | ((bit[36] | ~bit[39]&bit[38])&bit[37]&bit[34]&bit[33]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[37] | bit[36]&~bit[33])&bit[35]) | ((bit[37] | bit[36]&~bit[35])&~bit[38]&bit[33]) | ((~bit[33] | bit[38]&~bit[37])&~bit[36]&bit[34]) | (bit[39]&bit[36]&bit[34]&bit[33]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[35] | bit[34])&bit[36]&~bit[33]) | ((bit[37]&~bit[35] | ~bit[37]&~bit[33])&~bit[36]&~bit[34]) | ((bit[37]&bit[33] | ~bit[37]&bit[35])&~bit[38]&~bit[34]) | ((~bit[39]&bit[34] | bit[38]&~bit[37]&bit[33])&bit[36]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[35] | bit[33])&bit[36]&~bit[34]) | ((bit[34] | ~bit[36]&bit[35])&~bit[37]&~bit[33]) | ((bit[38]&bit[37]&bit[34] | ~bit[37]&bit[36])&bit[39]&bit[33]))) | (disable_update & out[0]);

		vart = bit[33] & bit[34] & bit[35];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------12--------------
        
		out[4] = (~disable_update & (((bit[40]&bit[37] | bit[41]&~bit[40]&bit[38])&~bit[39]&bit[36]) | ((~bit[37] | ~bit[41]&bit[40])&bit[39]&~bit[38]&bit[36]) | ((~bit[40] | bit[39])&~bit[38]&bit[37]&~bit[36]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((~bit[41] | bit[40])&bit[38]&bit[36]) | ((~bit[42]&bit[40] | ~bit[40]&bit[39])&~bit[41]&bit[37]&bit[36]) | ((bit[42]&bit[37] | ~bit[40]&~bit[38]&~bit[37])&bit[41]&bit[39]&bit[36]) | (~bit[39]&(bit[38]&bit[37] | ~bit[38]&~bit[37]&~bit[36])))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[38] | (bit[40] | bit[39])&bit[37])&bit[41]&bit[36]) | ((~bit[38] | ~bit[37])&bit[39]&~bit[36]) | (~bit[40]&(bit[37]&~bit[36] | ~bit[39]&~bit[38]&~bit[37]&bit[36])))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[39]&~bit[38] | ~bit[39]&bit[37])&~bit[36]) | ((~bit[41]&~bit[39] | bit[41]&bit[39]&bit[36])&~bit[40]&bit[37]) | ((bit[42]&~bit[39] | ~bit[36])&bit[40]&bit[37]) | ((bit[38] | bit[40]&~bit[37])&~bit[41]&bit[39]&bit[36]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[42] | bit[41]&bit[40])&bit[39]&bit[37]) | ((~bit[38] | bit[37])&bit[39]&~bit[36]) | ((~bit[39]^bit[37])&~bit[41]&~bit[40]&bit[36]) | (~bit[40]&bit[38]&bit[36]) | (~bit[39]&~bit[37]&~bit[36]))) | (disable_update & out[0]);

		vart = bit[36] & bit[37] & bit[38];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------13--------------

		out[4] = (~disable_update & (((bit[41] | ~bit[45]&bit[44]&bit[43])&~bit[42]&bit[40]) | ((bit[42] | bit[43])&~bit[41]&bit[40]&~bit[39]) | ((~bit[42]&bit[41] | bit[45]&bit[42]&bit[40])&bit[39]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[43]&bit[41]&bit[40]) | ((~bit[44]&bit[42] | ~bit[41]&~bit[40])&bit[43]&bit[39]) | ((~bit[43]&bit[41] | ~bit[45]&bit[40])&bit[42]&bit[39]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[43]&(bit[41]&bit[39] | ~bit[42]&bit[40]&~bit[39])) | ((~bit[39] | bit[45]&bit[44]&bit[43]&bit[40])&~bit[42]&~bit[41]) | ((bit[41] | bit[40])&bit[44]&bit[42]&bit[39]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[42]&bit[40]&~bit[39]) | (~bit[42]&~bit[41]&~bit[40]&bit[39]) | ((~bit[40] | ~bit[43])&bit[41]&~bit[39]) | ((~bit[43]&~bit[42] | bit[41])&~bit[44]&bit[39]) | (~(bit[45]^bit[44])&bit[43]&bit[42]&bit[40]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[44]&bit[43]&~bit[42]&bit[41]&bit[39]) | (~bit[43]&(bit[40] | ~bit[41]&bit[39])) | ((~bit[41]^bit[39])&bit[42]&~bit[40]) | ((bit[45] | ~bit[44])&bit[42]&bit[40]&bit[39]))) | (disable_update & out[0]);

		vart = bit[39] & bit[40] & bit[41];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------14--------------
        
		out[4] = (~disable_update & (((bit[44]&~bit[42] | bit[45]&~bit[44]&bit[42])&~bit[43]) | (~(bit[46]^bit[42])&~bit[45]&bit[43]) | ((bit[48]&bit[43] | bit[46]&~bit[43])&bit[47]&bit[45]&bit[42]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((~bit[46] | ~bit[45]&~bit[42])&bit[44]&~bit[43]) | ((bit[46] | ~bit[48]&bit[45])&~bit[47]&bit[43]&bit[42]) | (bit[47]&bit[44]&bit[42]) | (bit[45]&~bit[44]&bit[43]&~bit[42]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[44] | bit[43])&~bit[45]&~bit[42]) | ((bit[47]&bit[46] | ~bit[47]&~bit[46])&~bit[45]&bit[43]) | ((~bit[47]&bit[42] | bit[45]&~bit[43]&~bit[42])&bit[46]&bit[44]) | ((~bit[46] | bit[47]&bit[42])&~bit[48]&bit[45]&bit[43]) | (~bit[46]&bit[43]&~bit[42]) | (~bit[47]&bit[45]&~bit[43]&bit[42]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[45] | bit[44] | ~bit[48]&bit[43])&bit[46]&bit[42]) | ((~bit[46] | bit[44])&bit[45]&~bit[42]) | ((~bit[42] | ~bit[46]&~bit[45])&~bit[44]&~bit[43]) | (bit[47]&bit[45]&~bit[43]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[44] | bit[48]&bit[42])&bit[43]) | ((bit[43] | bit[47]&bit[45])&~bit[46]&bit[42]) | ((bit[44] | bit[45]&bit[43])&bit[46]&~bit[42]) | (~bit[47]&~bit[45]&bit[44]&bit[42]))) | (disable_update & out[0]);

		vart = bit[42] & bit[43] & bit[44];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------15--------------
        
		out[4] = (~disable_update & ((bit[49]&bit[48]&(bit[47] | ~bit[46])) | ((~bit[48] | bit[47])&~bit[46]&~bit[45]) | ((bit[49] | bit[50]&~bit[48])&bit[47]&bit[45]) | ((~bit[51] | ~bit[49])&bit[50]&bit[48]&~bit[47]&bit[45]) | (bit[51]&~bit[49]&bit[48]&bit[46]&bit[45]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[47] | ~bit[50]&bit[46])&bit[48]&bit[45]) | ((~bit[48]&~bit[47] | bit[49])&bit[46]&~bit[45]) | ((~bit[45] | ~bit[50]&~bit[49])&~bit[48]&bit[47]&~bit[46]) | (bit[49]&bit[48]&~bit[47]&~bit[46]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[45] | bit[48]&bit[45])&~bit[49]&~bit[46]) | ((bit[47] | bit[46])&bit[50]&~bit[48]&bit[45]) | ((bit[50] | ~bit[51]&bit[48])&bit[49]&bit[46]&bit[45]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[45] | ~bit[50]&~bit[49])&bit[48]) | ((bit[47] | bit[50]&bit[48])&bit[49]&bit[46]) | ((~bit[50] | ~bit[49]&~bit[48])&bit[47]&bit[45]) | ((~bit[48] | bit[51])&~bit[49]&bit[46]&bit[45]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[47]&bit[46]) | ((~bit[50] | ~bit[45])&~bit[48]&bit[46]) | ((bit[47] | ~bit[47]&~bit[46])&~bit[49]&~bit[45]) | ((bit[50] | ~bit[49])&bit[48]&~bit[46]&bit[45]) | ((bit[49] | bit[48])&bit[51]&bit[46]&bit[45]))) | (disable_update & out[0]);

		vart = bit[45] & bit[46] & bit[47];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------16--------------
        
		out[4] = (~disable_update & (((bit[54] | bit[52])&bit[53]&bit[49]&bit[48]) | ((~bit[52] | ~bit[54])&~bit[51]&bit[49]&~bit[48]) | ((bit[52]&~bit[48] | bit[51])&bit[50]&bit[49]) | (((~bit[52] | ~bit[51])&~bit[53]&~bit[49] | bit[53]&~bit[52]&~bit[51])&~bit[50]&bit[48]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[51]&bit[48] | bit[52]&~bit[51]&~bit[48])&bit[50]) | ((bit[48] | bit[51]&bit[50])&~bit[52]&bit[49]) | ((~bit[48] | ~bit[54]&bit[53]&bit[51])&bit[52]&~bit[50]&bit[49]) | ((~bit[53]&~bit[52] | bit[53]&bit[52]&~bit[49])&~bit[51]&~bit[50]&bit[48]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[51] | bit[48])&~bit[52]&bit[50]) | ((bit[51] | ~bit[54]&~bit[52])&~bit[53]&bit[49]) | ((~bit[50] | ~bit[51])&bit[49]&~bit[48]) | ((~bit[52] | bit[51])&bit[53]&~bit[49]&bit[48]) | (bit[54]&bit[52]&bit[51]&bit[49]&bit[48]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[53]&bit[49] | ~bit[51]&~bit[48] | ~bit[53]&bit[51]&bit[48])&bit[50]) | ((~bit[53]&bit[52] | bit[54]&~bit[52])&bit[49]&bit[48]) | ((~bit[52]&bit[51] | bit[53]&~bit[51])&~bit[49]&bit[48]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[52] | bit[49])&~bit[51]&~bit[48]) | ((~bit[50] | bit[48])&~bit[52]&~bit[51]&~bit[49]) | ((~bit[54]&bit[52]&bit[49] | ~bit[53]&bit[51]&~bit[49] | bit[54]&bit[53]&~bit[52]&~bit[51])&bit[48]))) | (disable_update & out[0]);

		vart = bit[48] & bit[49] & bit[50];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------17--------------

		out[4] = (~disable_update & ((bit[54]&~bit[52]&~bit[51]) | ((~bit[57] | ~bit[56] | ~bit[55])&bit[54]&bit[52]&bit[51]) | ((~bit[56] | ~bit[55] | ~bit[54])&bit[53]&~bit[52]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[55] | ~bit[51])&~bit[54]&bit[52]) | ((~bit[54] | bit[56]&bit[55])&bit[53]&bit[51]) | ((bit[57] | bit[56])&bit[55]&bit[52]&bit[51]) | (bit[54]&~bit[53]&~bit[51]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[56]&bit[53]&bit[51]) | ((bit[53] | ~bit[51])&~bit[55]&bit[54]) | ((~bit[56] | ~bit[51])&~bit[55]&bit[52]) | ((bit[55] | ~bit[53])&~bit[54]&~bit[52]&bit[51]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[56] | ~bit[51])&bit[54]&bit[52]) | ((bit[53] | ~bit[54]&bit[51])&~bit[55]&bit[52]) | ((bit[54] | ~bit[53]&bit[52])&bit[55]&~bit[51]) | ((~bit[52] | bit[57]&bit[56])&bit[55]&~bit[54]&~bit[53]&bit[51]) | (~bit[56]&bit[53]&bit[51]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[55] | ~bit[54]&bit[52])&bit[53]) | ((~bit[51] | bit[57]&bit[56])&bit[55]&bit[54]) | ((~bit[56]&bit[53] | ~bit[57]&~bit[55]&bit[52])&bit[54]&bit[51]) | (bit[55]&~bit[52]&bit[51]))) | (disable_update & out[0]);

		vart = bit[51] & bit[52] & bit[53];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------18--------------
        
		out[4] = (~disable_update & (((~bit[57] | ~bit[56]&~bit[54])&bit[55]) | ((bit[59] | bit[58])&bit[57]&~bit[55]&bit[54]) | ((~bit[60] | ~bit[55])&bit[59]&bit[58]&~bit[56]&bit[54]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[55] | ~bit[59]&bit[54])&bit[56]) | ((~bit[54] | bit[58]&bit[57])&~bit[56]&~bit[55]) | (((bit[60] | bit[59])&~bit[58] | (~bit[58] | (~bit[60]^bit[59]))&~bit[57])&bit[55]&bit[54]) | (~bit[57]&~bit[55]&~bit[54]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[57] | bit[54])&~bit[58]&bit[56]) | ((~bit[57] | bit[56])&~bit[55]&~bit[54]) | ((~bit[60] | ~bit[59])&bit[58]&bit[55]&bit[54]) | ((~bit[59] | ~bit[57])&bit[56]&bit[54]) | (~bit[58]&bit[57]&~bit[56]&bit[55]&~bit[54]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[58] | bit[55])&~bit[59]&bit[54]) | ((bit[57]&bit[55] | bit[56]&~bit[55])&~bit[54]) | ((bit[59]&bit[56] | bit[58]&bit[55])&~bit[60]&bit[57]) | ((~bit[57]&~bit[55] | bit[56])&~bit[58]&bit[54]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[58]&bit[57]&(bit[56] | ~bit[55])) | ((bit[60]&bit[59] | ~bit[60]&(~bit[59] | bit[58]&bit[57]))&bit[55]&bit[54]) | ((bit[58]&~bit[56]&~bit[55] | (~bit[58] | bit[56])&bit[57])&~bit[59]&bit[54]) | ((bit[57]&~bit[55] | ~bit[58]&~bit[57]&~bit[56]&bit[55])&~bit[54]))) | (disable_update & out[0]);

		vart = bit[54] & bit[55] & bit[56];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------19--------------
        
		out[4] = (~disable_update & (((~bit[62] | bit[61])&~bit[59]&bit[57]) | ((bit[59]&~bit[58] | bit[60]&~bit[59])&~bit[61]&~bit[57]) | ((bit[62] | ~bit[57])&bit[61]&bit[60]&bit[59]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[59]&bit[58]) | ((~bit[60] | bit[62]&bit[61])&bit[57]) | ((bit[61] | ~bit[59])&bit[60]&~bit[57]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[61]&(bit[58] | bit[57])) | ((~bit[61]&~bit[60] | ~bit[62]&bit[57])&bit[59]) | (~(bit[60]^bit[59])&bit[58]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[60]&bit[58]) | ((~bit[62] | ~bit[59])&bit[61]&bit[58]) | ((~bit[61] | ~bit[60])&bit[59]&bit[57]) | ((bit[61]&bit[60] | bit[59])&bit[62]&bit[57]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[60]&~bit[59] | ~bit[60]&bit[58])&bit[61]) | ((bit[62]&bit[57] | bit[60]&~bit[57])&~bit[61]) | ((bit[62] | ~bit[61]&~bit[60])&~bit[59]&bit[57]))) | (disable_update & out[0]);

		vart = bit[58] & bit[57];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//------------- 19-1 --------------

		out[4] = (~disable_update & (((~bit[61] | ~bit[64]&~bit[63]&~bit[62])&~bit[60]&bit[59]) | ((~bit[60] | bit[63])&bit[61]&~bit[59]) | ((~bit[59] | bit[65]&~bit[64])&~bit[63]&bit[62]&~bit[61]&bit[60]) | ((bit[65]^bit[64])&bit[63]&bit[60]&bit[59]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[62]&~bit[60]&bit[59]) | ((~bit[61] | ~bit[63]&bit[62])&bit[60]&~bit[59]) | ((~bit[62]&~bit[61] | bit[63]&bit[59])&bit[64]&bit[60]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[61] | bit[63]&bit[60])&bit[62]&~bit[59]) | (bit[64]&~bit[63]&bit[59]&(~bit[60] | bit[65]&~bit[62])) | ((~bit[63] | bit[64])&~bit[65]&bit[62]&bit[60]&bit[59]) | ((~bit[60] | bit[65]&bit[62])&~bit[64]&bit[63]&~bit[61]&bit[59]) | ((~bit[63] | ~bit[64]&~bit[62])&bit[61]&bit[59]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[61]&~bit[60]&~bit[59]) | ((~bit[62] | ~bit[66]&bit[59])&bit[64]&bit[63]) | ((~bit[62] | bit[60])&bit[63]&~bit[59]) | ((~bit[62]&bit[61] | bit[65]&bit[62]&bit[60])&~bit[63]&bit[59]) | ((bit[64] | bit[63]&bit[62])&~bit[60]&bit[59]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[63] | bit[62])&~bit[64]&bit[59]) | ((bit[61] | bit[65]&bit[64]&bit[60])&~bit[62]&bit[59]) | ((bit[62]&~bit[61] | bit[63]&~bit[62])&bit[60]&~bit[59]) | ((~bit[62]&~bit[61] | ~bit[63]&bit[62]&bit[61])&~bit[60]&~bit[59]) | ((bit[66] | ~bit[65])&bit[63]&bit[62]&bit[60]&bit[59]))) | (disable_update & out[0]);

		vart = bit[61] & bit[60] & bit[59];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------20--------------

		out[5] = (~disable_update & (~bit[68]&bit[67]&bit[66]&bit[65]&bit[64]&bit[62])) | (disable_update & out[5]);
		out[4] = (~disable_update & (((~bit[64] | ~bit[63])&~bit[65]&~bit[62]) | ((bit[65] | bit[66])&~bit[64]&~bit[63]&bit[62]) | ((bit[64]&~bit[62] | ~bit[65]&bit[62])&~bit[66]&~bit[63]) | ((~(bit[68]^bit[67]) | ~bit[67]&~bit[63])&bit[66]&bit[65]&bit[62]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[64]&bit[63]) | ((bit[67] | ~bit[65])&~bit[66]&bit[64]&bit[62]) | (((~bit[65]&bit[63]) | (bit[68]&(bit[67]&bit[63] | bit[65]&bit[64])))&bit[66]&bit[62]) | ((bit[65] | ~bit[64])&~bit[67]&bit[66]&~bit[63]&bit[62]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[64]&~bit[62]) | ((bit[64] | ~bit[66])&~bit[67]&bit[65]&bit[62]) | ((bit[67]&bit[66]&bit[64] | ~bit[68]&~bit[67]&bit[63])&~bit[65]&bit[62]) | ((~bit[66] | bit[68]&bit[65])&bit[67]&bit[63]&bit[62]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[66] | bit[64]&~bit[63])&~bit[65]&~bit[62]) | (((~bit[68]&bit[67]) | (bit[68]&~bit[67]&(~bit[66] | bit[65])))&bit[63]&bit[62]) | (((~bit[66]&~bit[63]) | (bit[68]&bit[64]))&bit[65]&bit[62]) | ((bit[66]&bit[62] | bit[64])&~bit[67]&~bit[65]&~bit[63]) | (bit[66]&bit[65]&~bit[62]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[66]&bit[63]&~bit[62]) | ((~bit[63] | ~bit[67]&bit[64])&~bit[66]&~bit[63]) | ((bit[68]&bit[64] | ~bit[67]&~bit[63])&bit[66]&bit[62]) | ((~bit[66] | (~bit[69]&bit[65]))&bit[68]&bit[63]&bit[62]) | ((bit[66] | ~bit[64])&~bit[65]&~bit[63]&bit[62]))) | (disable_update & out[0]);

		vart = bit[64] & bit[63] & bit[62];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------21--------------

		out[5] = (~disable_update & (((~bit[69]&~bit[68]&bit[67]) | (~bit[72]&bit[71]&bit[69]&bit[68]&bit[65]))&~bit[70]&bit[66])) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[69]&~bit[65]) | (bit[70]&bit[67]&(bit[66] | (bit[69]&~bit[68]))) | ((~bit[67] | bit[66])&bit[68]&~bit[65]) | ((bit[70] | (bit[72]&bit[71]))&bit[69]&bit[68]&bit[66]) | ((~bit[69] | ~bit[71] | ~bit[70])&bit[68]&bit[67]&bit[65]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((((bit[66]&bit[65]) | (bit[71]&~bit[70]&~bit[67]&bit[66]))&~bit[69]&~bit[68]) | ((~bit[70]&bit[65] | (bit[69]&bit[67]&~bit[65]))&~bit[68]&~bit[66]) | ((((bit[69] | bit[68])&~bit[67]) | (bit[70]&bit[69]))&bit[66]&~bit[65]) | (~(bit[68]^bit[67])&~bit[69]&~bit[66]&~bit[65]) | ((bit[70]&~bit[67] | bit[72]&bit[71]&bit[66])&bit[69]&bit[68]&bit[65]) | ((bit[69] | bit[68])&bit[71]&bit[70]&bit[67]&bit[65]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[69]&bit[67]&bit[66]) | ((bit[68] | ~bit[69]&bit[67])&~bit[66]&~bit[65]) | ((~bit[66]&bit[65] | bit[70]&bit[66])&~bit[69]&~bit[68]) | ((bit[71] | bit[70])&bit[72]&bit[68]&bit[66]&bit[65]) | (((bit[70]&bit[69]) | (bit[71]&bit[68]))&bit[67]&bit[65]) | (((~bit[70]&~bit[67]) | (~bit[71]&bit[66]))&~bit[69]&bit[65]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[68] | ~bit[70]&~bit[67]&~bit[66])&~bit[69]&~bit[65]) | ((bit[67] | bit[72]&bit[71]&bit[69]&bit[65])&~bit[70]&bit[68]&bit[66]) | ((bit[69] | ~bit[71]&bit[68])&bit[70]&bit[67]&bit[65]) | ((~bit[72] | ~bit[69] | ~bit[68])&bit[70]&bit[66]&bit[65]) | ((bit[68]^bit[65])&~bit[67]&~bit[66]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[71]&bit[66] | bit[69]&bit[68]&bit[67])&bit[65]) | ((~bit[69] | ~bit[68])&~bit[66]&~bit[65]) | ((bit[68]&bit[66] | ~bit[69]&bit[67])&bit[70]&~bit[65]) | ((bit[69]&bit[66]&bit[65] | ~bit[69]&~bit[66])&~bit[70]&~bit[68]) | ((bit[71]&bit[69] | ~bit[67]&~bit[66])&bit[70]&bit[65]))) | (disable_update & out[0]);

		vart = bit[67] & bit[66] & bit[65];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------22--------------
        
		out[5] = (~disable_update & ZERO) | (disable_update & out[5]);
		out[4] = (~disable_update & (((bit[69] | bit[72]&~bit[70])&bit[71]&~bit[68]) | ((~bit[71] | bit[72])&bit[73]&bit[69]&bit[68]) | (~(bit[71]^bit[68])&~bit[72]&~bit[70]&~bit[69]) | ((~bit[69] | ~bit[74]&~bit[72]&bit[71])&~bit[73]&~bit[70]&bit[68]) | ((~bit[74] | ~bit[69])&~bit[73]&bit[72]&~bit[71]&bit[68]) | (bit[73]&~bit[72]&~bit[71]&bit[70]&bit[68]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[71]&~bit[70]&~bit[68]) | ((bit[72] | bit[68])&~bit[71]&~bit[69]) | ((bit[72]&bit[69] | ~bit[72]&~bit[69]&~bit[68])&bit[71]&bit[70]) | ((~bit[73]&bit[70] | ~bit[75]&bit[74]&bit[73]&bit[69])&bit[72]&bit[68]) | (((bit[74]&bit[72]) | (bit[73]^bit[72]))&~bit[71]&bit[68]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[69] | bit[73]&bit[71])&~bit[72]&bit[68]) | ((bit[70] | bit[72]&bit[71])&~bit[69]&~bit[68]) | ((~bit[74] | bit[75]&bit[71])&bit[73]&bit[69]&bit[68]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[71]&bit[70]&~bit[69]&~bit[68]) | ((~bit[68] | bit[73]&bit[72])&bit[71]&~bit[70]) | ((~bit[73]&bit[72] | bit[73]&~bit[72]&bit[68])&~bit[70]&~bit[69]) | ((bit[72] | bit[73]&bit[68])&~bit[74]&bit[69]) | ((~bit[70] | bit[69])&bit[72]&~bit[68]) | ((~bit[74] | ~bit[69])&~bit[73]&bit[71]&bit[68]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[72]&bit[71]&(bit[69] | ~bit[68])) | ((bit[72] | bit[73])&bit[70]&bit[68]) | ((bit[70] | bit[69])&~bit[72]&~bit[68]) | ((bit[70]&bit[69] | ~bit[69]&bit[68])&~bit[73]&bit[71]) | ((bit[72]&~bit[71] | bit[75]&bit[74]&bit[73]&bit[71])&bit[69]&bit[68]))) | (disable_update & out[0]);

		vart = bit[70] & bit[69] & bit[68];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------23--------------

		out[5] = (~disable_update & (((~bit[76]&bit[75]&bit[73]) | ((~bit[76] | ~bit[75])&bit[77]&bit[71]))&bit[74]&bit[72])) | (disable_update & out[5]);
		out[4] = (~disable_update & (((~bit[75] | bit[71])&~bit[73]&~bit[72]) | (((bit[76]&bit[75]) | (~bit[77]&~bit[75]) | ((bit[76] | bit[75])&~bit[74]))&bit[72]&bit[71]) | (~bit[75]&~bit[74]&~bit[73]&~bit[71]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[75] | bit[73])&~bit[72]&~bit[71]) | ((bit[76]&~bit[72] | ~bit[73]&bit[72])&bit[75]&bit[74]) | ((~bit[75]&~bit[72] | ~bit[76]&bit[75])&~bit[74]&bit[71]) | ((~bit[78]&bit[75] | ~bit[77]&~bit[74])&bit[76]&bit[72]&bit[71]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[74]&~bit[73]&~bit[71]) | (bit[75]&~bit[74]&~bit[72]&~bit[71]) | ((~bit[76]&bit[71] | bit[74]&~bit[72])&~bit[75]&bit[73]) | ((bit[77] | bit[76]&~bit[75])&~bit[74]&bit[72]&bit[71]) | ((~bit[72] | ~bit[77]&bit[74])&bit[76]&bit[75]&bit[71]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[73] | (bit[76] | ~bit[78]&bit[72])&bit[74])&bit[75]&bit[71]) | ((~bit[74] | ~bit[72])&~bit[73]&~bit[71]) | ((~bit[75] | ~bit[77])&~bit[76]&bit[72]&bit[71]) | ((bit[77]&bit[71] | ~bit[75]&~bit[71])&~bit[74]&bit[72]) | (bit[76]&~bit[75]&~bit[72]&bit[71]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[74]&bit[73]&~bit[71]) | ((bit[76]&bit[75] | ~bit[77]&~bit[76]&bit[71])&bit[74]&bit[72]) | ((bit[76]&~bit[74] | ~bit[75]&~bit[73])&~bit[72]&bit[71]) | ((~bit[73] | bit[72])&bit[75]&bit[74]&~bit[71]) | ((~bit[75]&~bit[71] | ~bit[76]&bit[75]&bit[71])&~bit[74]&bit[72]))) | (disable_update & out[0]);

		vart = bit[73] & bit[72] & bit[71];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------24--------------

		out[5] = (~disable_update & (((~bit[79]&~bit[77]&bit[76] | ~bit[80]&bit[79]&bit[77]&bit[74])&bit[78]&bit[75]) | ((bit[79]&bit[77] | bit[78])&~bit[80]&bit[77]&bit[76]&bit[74]))) | (disable_update & out[5]);
		out[4] = (~disable_update & (((bit[79] | bit[77])&bit[76]&bit[75]) | ((~bit[79] | ~bit[80]&bit[75])&~bit[77]&bit[74]) | ((~bit[76]&~bit[75] | bit[76]&~bit[74])&~bit[78]&~bit[77]) | ((~bit[77]&~bit[74] | ~bit[79]&bit[78]&bit[74])&~bit[76]&~bit[75]) | ((bit[77]&bit[76] | bit[78]&bit[75])&bit[80]&bit[79]&bit[74]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[77]&bit[76]&bit[75]) | ((~bit[76] | bit[79]&bit[75])&bit[78]&~bit[74]) | ((~bit[76] | ~bit[78]&~bit[77])&bit[79]&~bit[75]&bit[74]) | ((~bit[78]&bit[77] | bit[80]&bit[78])&~bit[79]&bit[75]&bit[74]) | ((bit[78] | ~bit[75])&bit[80]&bit[79]&bit[77]&bit[74]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[77] | ~bit[74])&bit[78]&~bit[75]) | ((~bit[74] | ~bit[79]&~bit[78])&~bit[77]&~bit[76]) | ((~bit[78]&bit[75] | bit[79]&bit[78]&~bit[74])&bit[77]&bit[76]) | (~(bit[80]^bit[78])&bit[75]&bit[74]) | ((~bit[79] | ~bit[77])&~bit[78]&bit[75]&bit[74]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[74] | ~bit[81]&bit[75] | bit[79]&~bit[75])&bit[78]&bit[77]) | ((~bit[76] | ~bit[79]&bit[75])&~bit[78]&~bit[74]) | ((bit[78]&~bit[75] | (~bit[79]&(~bit[77] | ~bit[75])))&~bit[76]&bit[74]) | ((~bit[79]&bit[78] | bit[80]&~bit[78])&bit[75]&bit[74]) | (bit[79]&~bit[77]&bit[76]&bit[75]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[80] | ~bit[79]&bit[78])&bit[75]&bit[74]) | ((bit[78] | bit[77])&~bit[75]&~bit[74]) | ((bit[77]&~bit[76] | ~bit[79]&bit[74])&~bit[78]&~bit[75]) | ((bit[80]&bit[76] | ~bit[79]&~bit[76])&bit[78]&bit[77]&bit[74]) | (~bit[79]&bit[77]&bit[76]&~bit[74]))) | (disable_update & out[0]);

		vart = bit[76] & bit[75] & bit[74];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------25--------------

		out[5] = (~disable_update & ((bit[80]&bit[79] | bit[82]&~bit[80]&bit[77])&~bit[81]&bit[78])) | (disable_update & out[5]);
		out[4] = (~disable_update & (((~bit[80] | ~bit[82]&~bit[81]&bit[77])&~bit[79]&~bit[78]) | ((~bit[82]&bit[80] | bit[82]&bit[81]&~bit[80])&bit[78]&bit[77]) | ((~(bit[80]^bit[79]) | bit[80]&~bit[78])&bit[81]&~bit[77]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((~bit[81] | ~bit[77])&~bit[80]&~bit[78]) | ((bit[81] | ~bit[83]&bit[82]&bit[80])&bit[78]&bit[77]) | ((bit[79]&bit[78] | ~bit[79]&bit[77])&bit[81]&bit[80]) | ((~bit[81]&~bit[77] | bit[82]&bit[80]&bit[77])&~bit[79]&~bit[78]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[82]&~bit[78] | ~bit[81]&bit[79]&bit[77])&bit[80]) | ((bit[80] | ~bit[78])&bit[79]&~bit[77]) | ((~bit[80]&~bit[78] | bit[81]&bit[78])&~bit[82]&bit[77]) | ((bit[81] | bit[83]&bit[77])&bit[80]&bit[78]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[79] | bit[82]&bit[78])&bit[81]&bit[77]) | ((~bit[79] | ~bit[78])&~bit[81]&~bit[77]) | ((bit[81]&bit[79] | ~bit[83]&~bit[81]&bit[77])&bit[80]&bit[78]) | ((bit[82]&bit[78] | (~bit[82] | bit[80])&~bit[78])&bit[81]&bit[77]) | ((~bit[82] | ~bit[80])&~bit[81]&bit[79]&bit[78]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[79] | bit[80]&~bit[78])&~bit[82]&bit[77]) | ((bit[81] | bit[80])&~bit[79]&~bit[77]) | ((~bit[79] | bit[82]&bit[80])&bit[78]&~bit[77]) | ((~bit[80]&bit[79] | (bit[83]&bit[82] | ~bit[82]&~bit[80])&bit[78])&~bit[81]&bit[77]) | ((~bit[82] | bit[81])&~bit[83]&bit[78]&bit[77]))) | (disable_update & out[0]);

		vart = bit[79] & bit[78] & bit[77];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------26--------------

		out[5] = (~disable_update & (((bit[81] | bit[85]&~bit[84]&bit[80])&bit[83]&bit[82]) | ((~bit[87] | ~bit[86])&bit[85]&bit[84]&bit[83]&bit[81]&bit[80]) | (bit[83]&~bit[82]&~bit[81]&~bit[80]))) | (disable_update & out[5]);
		out[4] = (~disable_update & (((~bit[84] | ~bit[83])&~bit[82]&~bit[81]&bit[80]) | (~(bit[84]^bit[82])&~bit[81]&~bit[80]) | (((bit[86] | bit[84])&~bit[85] | bit[86]&bit[84] | ~bit[86]&bit[85]&~bit[84])&~bit[83]&bit[81]&bit[80]) | (bit[84]&bit[83]&bit[82]&bit[80]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((~bit[83] | bit[81])&~bit[84]&bit[80]) | ((~bit[82] | ~bit[86])&bit[84]&bit[83]&~bit[81]&bit[80]) | (bit[82]&~bit[81]&~bit[80]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[82] | ~bit[84]&bit[81])&~bit[83]&~bit[80]) | ((~bit[82] | ~bit[83])&~bit[85]&~bit[81]&bit[80]) | ((~bit[86]&bit[85]&bit[83] | bit[86]&bit[84])&bit[82]&bit[80]) | ((bit[86]&~bit[84] | ~bit[83])&bit[85]&bit[81]&bit[80]) | ((~bit[80] | ~bit[87]&~bit[86]&bit[85])&bit[84]&bit[83]&~bit[82]&bit[81]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[81] | ~bit[80])&bit[83]&~bit[82]) | ((~bit[86]&~bit[85] | (bit[85]&bit[84]&~bit[83] | ~bit[84]&bit[83])&bit[86])&bit[81]&bit[80]) | ((~bit[81]&~bit[80] | bit[85]&~bit[84]&bit[80])&~bit[83]&bit[82]) | ((~bit[80] | bit[87]&bit[85])&bit[84]&bit[83]&bit[81]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[86] | ~bit[80])&~bit[84]&bit[81]) | ((bit[83] | bit[82])&~bit[84]&~bit[80]) | ((bit[82] | ~bit[86]&bit[80])&~bit[85]&bit[83]&bit[81]) | ((~bit[85]&~bit[83] | ~bit[86]&bit[84]&bit[83])&bit[82]&bit[80]) | ((~bit[82]&~bit[81] | ~bit[86]&~bit[83]&bit[81])&bit[85]&bit[80]) | ((bit[84] | ~bit[81])&bit[86]&bit[85]&bit[83]&bit[80]))) | (disable_update & out[0]);

		vart = bit[82] & bit[81] & bit[80];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------27--------------

		out[5] = (~disable_update & (bit[87]&~bit[86]&~bit[85]&~bit[84]&bit[83])) | (disable_update & out[5]);
		out[4] = (~disable_update & (((~bit[87] | ~bit[88]&~bit[85])&bit[86]&~bit[84]) | ((~bit[84] | bit[85])&bit[86]&~bit[83]) | (((bit[88] | ~bit[86])&bit[87] | bit[89]&~bit[88]&~bit[87])&bit[84]&bit[83]) | (~bit[87]&~bit[86]&~bit[83]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[85]&~bit[83]) | ((~bit[87] | ~bit[86])&bit[85]&bit[83]) | (((bit[88] | bit[89]&~bit[87])&~bit[86] | (bit[86] | bit[90]&bit[88])&bit[87])&bit[84]&bit[83]))) | (disable_update & out[3]);  
		out[2] = (~disable_update & (((bit[87] | ~bit[84])&~bit[86]&~bit[83]) | ((bit[88]&bit[85] | (~bit[88] | bit[86])&bit[84])&~bit[87]&bit[83]) | (((bit[87] | ~bit[88])&~bit[84] | bit[90]&bit[88]&bit[87])&bit[86]&~bit[85]&bit[83]) | (bit[86]&bit[85]&bit[84]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[88]&bit[85]&bit[83]) | (bit[86]&~bit[84]&~bit[83]) | ((~bit[88] | bit[87])&~bit[89]&bit[84]&bit[83]) | ((~bit[83] | bit[88]&~bit[86]&bit[83])&bit[87]&~bit[85]&bit[84]) | ((~bit[88]&~bit[87]&bit[83] | bit[87]&bit[85])&bit[86]&bit[84]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[90] | ~bit[83])&bit[87]&bit[86]) | ((~bit[83] | ~bit[85]&~bit[84])&~bit[87]&~bit[86]) | ((~bit[88]&bit[86] | bit[89]&~bit[87])&bit[84]&bit[83]) | ((~bit[88]&bit[87] | bit[88]&bit[86])&~bit[84]&bit[83]) | (bit[87]&bit[85]&~bit[84]))) | (disable_update & out[0]);

		vart = bit[85] & bit[84] & bit[83];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------28---------------

		out[5] = (~disable_update & ((~bit[90]&~bit[89]&bit[88]&bit[87]) | ((bit[91] | ~bit[92]&bit[90])&bit[89]&bit[88]&bit[86]) | ((~bit[88]&~bit[86] | bit[92]&bit[91]&bit[86])&bit[90]&bit[89]&bit[87]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[89]&bit[88]&bit[87]) | ((~bit[91] | ~bit[86])&~bit[89]&~bit[87]) | ((~bit[92]&bit[91] | (bit[90] | ~bit[92])&~bit[89])&bit[87]&bit[86]) | ((~bit[88]&~bit[87] | ~bit[89]&bit[88])&bit[90]&~bit[86]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[90] | ~bit[88])&~bit[89]&bit[87]&~bit[86]) | (((~bit[92] | bit[90])&~bit[91] | (bit[91] | bit[91])&~bit[90])&bit[87]&bit[86]) | ((~bit[90]&~bit[86] | bit[91]&~bit[88]&bit[86])&~bit[87]) | (bit[89]&bit[88]&bit[87]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[87] | ~bit[89])&bit[88]&~bit[86]) | ((bit[92]&~bit[91] | ~bit[92]&bit[91]&bit[89])&bit[87]&bit[86]) | ((~bit[86] | ~bit[91])&~bit[90]&~bit[88]&bit[87]) | (~bit[90]&~bit[87]&bit[86]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[86] | ~bit[91])&~bit[90]&~bit[88]) | ((~bit[88] | ~bit[92]&~bit[91]&bit[89])&bit[90]&~bit[87]&bit[86]) | ((~bit[89]&bit[88] | bit[89]&bit[87])&bit[90]&~bit[86]) | ((bit[93] | ~bit[92])&bit[91]&bit[89]&bit[87]&bit[86]) | (~bit[90]&bit[87]&bit[86]) | (bit[89]&~bit[88]&~bit[86]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[86] | ~bit[92]&bit[91]&bit[89]&bit[86])&~bit[90]&~bit[87]) | ((bit[92]&bit[91] | ~bit[89])&bit[88]&bit[86]) | ((~bit[91] | ~bit[89])&bit[92]&bit[87]&bit[86]) | ((bit[91]&~bit[89] | ~bit[91]&bit[90]&~bit[88])&~bit[87]&bit[86]) | (~bit[91]&bit[88]&bit[87]) | (bit[89]&~bit[88]&~bit[86]))) | (disable_update & out[0]);

		vart = bit[88] & bit[87] & bit[86];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------29--------------

		out[5] = (~disable_update & (((bit[93]&bit[91] | (bit[96]&~bit[95]&~bit[94]&bit[93] | bit[95]&bit[94])&bit[89])&bit[92]&bit[90]) | ((~bit[94] | ~bit[92])&bit[93]&bit[91]&bit[89]) | ((bit[93]^bit[91])&~bit[92]&~bit[90]&~bit[89]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((((~bit[93]&bit[91]) | (~bit[91]&~bit[89]) | bit[94]&~bit[90])&bit[92]) | ((~bit[92]&~bit[91] | bit[93]&bit[91])&~bit[90]&~bit[89]) | (((~bit[95]&bit[92] | bit[95]&~bit[94])&~bit[93] | bit[94]&~bit[93]&~bit[92])&bit[90]&bit[89]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[93]^bit[92])&bit[91]&bit[90]) | ((~bit[91] | ~bit[95]&bit[93]&bit[92])&~bit[94]&~bit[90]&bit[89]) | ((~bit[89] | ~bit[95]&bit[94]&bit[90])&bit[93]&bit[92]&~bit[91]) | ((bit[90]^bit[89])&~bit[93]&~bit[92]&~bit[91]) | (((bit[94] | ~bit[96]&~bit[94])&bit[93]&~bit[92] | (~bit[94] | ~bit[92])&~bit[95]&~bit[93])&bit[90]&bit[89]) | (bit[94]&bit[92]&bit[91]&bit[89]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[95] | ~bit[93])&~bit[92]&bit[90]) | ((~bit[89] | bit[95]&bit[94])&~bit[93]&bit[90]) | ((bit[92] | bit[93])&bit[95]&bit[91]&bit[89]) | ((~bit[94] | bit[93])&bit[92]&~bit[91]&~bit[90]&bit[89]) | ((bit[96]&bit[95] | ~bit[96]&~bit[95]&bit[92])&~bit[94]&bit[93]&bit[90]&bit[89]) | (bit[93]&~bit[92]&~bit[89]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[91]&bit[90]) | ((~bit[93] | bit[93]&bit[92]&~bit[90])&bit[91]&~bit[89]) | ((~bit[89] | ~bit[95]&bit[94]&bit[93])&~bit[92]&bit[91]) | ((bit[94] | ~bit[92])&~bit[95]&bit[90]&bit[89]) | ((bit[95]&bit[91] | bit[96]&bit[90] | (~bit[95] | ~bit[90])&~bit[94])&bit[93]&bit[92]&bit[89]) | ((~bit[93]&~bit[92]&~bit[90] | bit[92]&bit[91])&~bit[94]&bit[89]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[92] | ~bit[94]&bit[91])&bit[93]&bit[90]) | ((~bit[90] | bit[94]&~bit[92]&bit[89])&~bit[93]&~bit[91]) | ((~bit[93] | ~bit[92])&~bit[90]&~bit[89]) | ((bit[94]&bit[91] | ~bit[96]&bit[90])&bit[93]&bit[89]) | ((bit[92]&bit[91] | bit[96]&bit[90])&bit[95]&bit[94]&bit[89]))) | (disable_update & out[0]);

		vart = bit[91] & bit[90] & bit[89];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------30--------------

		out[5] = (~disable_update & (((~bit[98] | ~bit[97])&bit[96]&~bit[95]&bit[94]&bit[92]) | ((bit[98]&~bit[96]&bit[92] | (~bit[92] | ~bit[99]&bit[98])&bit[96]&~bit[94])&~bit[97]&bit[95]&bit[93]))) | (disable_update & out[5]);
		out[4] = (~disable_update & (((bit[94] | bit[97]&bit[95])&bit[93]) | ((bit[95] | ~bit[97]&~bit[96])&bit[94]&bit[92]) | ((~bit[94]&~bit[92] | bit[97]&bit[92])&~bit[96]&bit[95]) | ((bit[98]&bit[97]&bit[94] | ~bit[98]&bit[95]&bit[93])&bit[96]&bit[92]) | ((~bit[95] | bit[99]&bit[96])&bit[98]&~bit[97]&bit[93]&bit[92]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[98] | ~bit[97]&bit[96])&bit[95]&bit[94]) | (((bit[99] | ~bit[95])&bit[97] | (bit[97] | ~bit[99]&bit[96]&bit[95])&~bit[98])&bit[93]&bit[92]) | ((~bit[93]&bit[92] | bit[96]&~bit[92])&~bit[95]&~bit[94]) | (bit[94]&bit[93]) | (bit[95]&~bit[93]&~bit[92]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[94] | ~bit[99]&bit[96]&bit[92])&bit[95]&bit[93]) | ((~bit[95] | bit[97]&bit[96]&bit[93])&~bit[94]&~bit[92]) | ((bit[96] | bit[99]&bit[97]&bit[93])&~bit[98]&bit[95]&bit[92]) | ((~bit[97]&bit[94] | (~bit[98] | bit[97])&~bit[95]&bit[93])&~bit[96]&bit[92]) | ((~bit[94] | ~bit[97]&bit[95])&bit[96]&~bit[93]&bit[92]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[94] | bit[93]&bit[92] | bit[95]&~bit[92])&bit[97]&bit[96]) | ((~bit[95] | bit[96])&~bit[93]&~bit[92]) | ((~bit[95]&bit[93] | bit[97]&bit[95]&bit[94])&~bit[98]&bit[92]) | ((~bit[94]&~bit[93] | bit[99]&bit[98]&bit[95]&bit[93])&~bit[97]&bit[92]) | ((bit[98] | ~bit[92])&bit[96]&bit[94]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[94] | bit[96]&bit[95]&~bit[92])&bit[97]&bit[93]) | ((~bit[93]&~bit[92] | bit[95]&bit[92])&bit[96]&bit[94]) | (~(bit[97]^bit[93])&~bit[96]&~bit[95]&bit[92]) | (((~bit[98]&~bit[96] | ~bit[99]&bit[95])&bit[97] | (bit[96]&~bit[95] | bit[99]&~bit[97]&~bit[96]&bit[95])&bit[98])&bit[93]&bit[92]) | ((bit[97]&~bit[96] | bit[96]&bit[95])&bit[94]&bit[92]))) | (disable_update & out[0]);

		vart = bit[94] & bit[93] & bit[92];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------31--------------

		out[5] = (~disable_update & ((~bit[99]&~bit[98]&bit[97]&bit[96]) | ((bit[99] | ~bit[101]&bit[98])&bit[100]&bit[97]&bit[95]))) | (disable_update & out[5]);
		out[4] = (~disable_update & (((bit[99] | bit[98])&bit[97]&bit[96]) | (((bit[101] | ~bit[98])&~bit[99] | ~bit[100]&bit[98])&bit[97]&bit[95]) | ((bit[102] | ~bit[101] | ~bit[98])&bit[100]&bit[96]&bit[95]) | ((bit[98]&bit[96] | ~bit[98]&~bit[97]&~bit[96])&bit[99]&bit[95]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~(bit[97]^bit[96])&bit[98]) | ((~bit[97] | bit[101]&bit[98]&bit[95])&~bit[99]&~bit[96]) | ((~bit[96] | bit[99]&bit[97])&~bit[98]&~bit[95]) | ((bit[99]&~bit[98]&bit[96] | bit[98]&~bit[96])&~bit[100]&bit[95]) | (((bit[99] | bit[101]&bit[100])&~bit[102] | ~bit[101]&bit[99])&bit[98]&bit[96]&bit[95]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[97] | ~bit[99]&bit[96])&~bit[98]&~bit[95]) | ((~bit[97] | ~bit[101]&bit[99])&bit[100]&~bit[96]&bit[95]) | ((bit[98] | ~bit[96])&bit[99]&bit[97]&~bit[95]) | (((~bit[101]&bit[97] | bit[101]&bit[96])&~bit[100] | (~bit[102]&bit[101]&~bit[97] | bit[102]&~bit[101]&bit[96])&bit[100])&bit[98]&bit[95]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[100] | ~bit[101]&bit[95] | (~bit[96] | ~bit[99])&~bit[95])&bit[98]&bit[97]) | ((~bit[99] | ~bit[98]&~bit[97])&~bit[96]&~bit[95]) | ((bit[96]^bit[95])&bit[99]&~bit[97]) | (((~bit[102]&bit[100] | bit[102]&bit[99])&bit[98] | (bit[101]&~bit[100] | ~bit[101]&~bit[99])&~bit[98])&bit[96]&bit[95]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[95] | bit[98]&~bit[96])&~bit[97]) | ((bit[99]&~bit[98] | ~bit[95])&~bit[100]&bit[96]) | ((bit[100]&bit[99] | ~bit[100]&~bit[99])&bit[97]&bit[95]) | (((bit[101] | bit[100]&bit[98])&~bit[102] | (~bit[98] | bit[100]&~bit[99])&bit[101])&bit[96]&bit[95]))) | (disable_update & out[0]);

		vart = bit[97] & bit[96] & bit[95];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------32--------------

		out[5] = (~disable_update & (((~bit[101] | bit[99])&~bit[100]&~bit[98]) | ((~bit[103] | ~bit[102])&bit[101]&bit[100]&bit[98]) | ((~bit[104]&bit[102]&bit[98] | (~bit[103]&~(bit[102]^bit[98])))&bit[101]&bit[99]))) | (disable_update & out[5]);
		out[4] = (~disable_update & (((bit[103] | bit[102] | ~bit[101])&bit[100]&bit[99]) | (((bit[100] | bit[103]&bit[99])&~bit[102] | ~bit[104]&bit[99])&~bit[101]&bit[98]) | ((bit[100] | bit[104]&bit[99])&bit[103]&bit[102]&bit[101]&bit[98]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[102] | bit[101])&~bit[100]&~bit[99]&bit[98]) | ((~bit[101] | bit[103]&~bit[102])&bit[100]&~bit[98]) | (((bit[104] | ~bit[102])&bit[99] | bit[102]&~bit[99])&bit[103]&bit[101]&bit[98]) | (bit[100]&~bit[99]&~bit[98]) | (~bit[103]&~bit[101]&bit[99]&bit[98]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[100] | bit[102]&bit[99])&bit[103]&bit[101]) | ((~bit[100] | bit[104])&~bit[102]&bit[101]&~bit[99]) | ((~bit[101] | bit[103]&~bit[100])&~bit[102]&bit[99]&bit[98]) | ((~bit[100]&~bit[98] | ~bit[103]&~bit[99]&bit[98])&bit[102]&~bit[101]) | ((~bit[105]&bit[104]&bit[101]&bit[98] | bit[103]&~bit[98])&bit[102]&bit[99]) | ((bit[100] | ~bit[99])&bit[101]&~bit[98]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[101] | bit[100])&~bit[102]&~bit[98]) | ((bit[103]&~bit[102] | ~bit[104]&bit[98])&bit[101]&bit[100]) | ((~bit[103]&~bit[100] | ~bit[101]&bit[98])&bit[102]&~bit[99]) | (((~bit[101] | ~bit[103]&~bit[102])&bit[104] | (bit[103] | bit[105]&bit[101])&~bit[104]&bit[102])&bit[99]&bit[98]) | (~bit[100]&~bit[99]&~bit[98]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((~bit[102] | ~bit[98])&~bit[103]&~bit[101]&bit[100]) | ((~bit[103]&~bit[98] | ~bit[105]&bit[102]&bit[98])&bit[101]&bit[99]) | ((bit[103] | ~bit[98])&bit[102]&~bit[99]) | ((bit[102]&bit[100] | bit[104]&bit[103]&bit[98])&bit[101]&~bit[99]) | ((~bit[104]&~bit[102] | bit[104]&~bit[101])&bit[103]&~bit[100]&bit[98]) | (~bit[101]&~bit[99]&~bit[98]))) | (disable_update & out[0]);

		vart = bit[100] & bit[99] & bit[98];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------33--------------

		out[5] = (~disable_update & ((~bit[104]&bit[103]&bit[102]) | (((~bit[107] | ~bit[106] | ~bit[105])&bit[103] | (bit[107] | ~bit[108]&~bit[106])&bit[105]&bit[102])&bit[104]&bit[101]))) | (disable_update & out[5]);
		out[4] = (~disable_update & (((bit[107] | bit[106])&~bit[104]&bit[102]&bit[101]) | ((~bit[102]&~bit[101] | ~bit[103]&bit[101])&~bit[105]&~bit[104]) | ((bit[107]&bit[106]&bit[103] | bit[108]&~bit[107]&bit[102])&bit[105]&bit[104]&bit[101]) | (bit[104]&bit[103]&bit[102]) | (~bit[103]&~bit[102]&~bit[101]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((~bit[103] | ~bit[101])&bit[105]&~bit[102]) | ((bit[106]&~bit[105] | ~bit[105]&bit[104])&bit[103]&bit[102]) | ((~bit[105]&~bit[104] | ~bit[107]&bit[104])&bit[102]&bit[101]) | ((bit[107]&bit[104]&bit[103] | ~bit[108]&~bit[105]&bit[102])&bit[106]&bit[101]) | (bit[105]&bit[104]&~bit[103]&~bit[101]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((((~bit[106] | ~bit[105])&~bit[103] | (~bit[107]&bit[104] | ~bit[105]&~bit[104])&~bit[106])&~bit[102]&bit[101]) | ((bit[103]&~bit[101] | ~bit[107]&bit[106]&bit[102])&bit[105]&~bit[104]) | (((~bit[106] | bit[106]&bit[102])&bit[108]&bit[107] | bit[107]&bit[105]&bit[103])&bit[104]&bit[101]) | ((bit[104]&~bit[103] | ~bit[106]&bit[101])&~bit[107]&~bit[105]) | ((~bit[105]&bit[104] | ~bit[104]&bit[102])&~bit[103]&~bit[101]) | (~bit[106]&bit[103]&bit[102]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[102] | ~bit[106]&bit[102])&~bit[105]&~bit[103]) | ((~bit[104]&~bit[103] | bit[104]&~bit[102])&~bit[101]) | ((bit[108]&bit[101] | ~bit[102])&bit[105]&bit[104]) | ((~bit[105]&~bit[104] | bit[102])&bit[106]&bit[101]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[102] | ~bit[105])&~bit[106]&bit[103]) | ((~bit[105] | bit[104])&~bit[103]&~bit[101]) | ((~bit[103]&bit[101] | bit[104]&~bit[101])&bit[105]&~bit[102]) | ((bit[106]&bit[102] | ~bit[106]&bit[105]&~bit[104])&bit[107]&bit[101]) | ((bit[107]&bit[104] | bit[105]&~bit[104])&~bit[106]&~bit[102]&bit[101]))) | (disable_update & out[0]);

		vart = bit[103] & bit[102] & bit[101];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------34--------------

		out[5] = (~disable_update & (((bit[110] | ~bit[104])&bit[108]&bit[107]&bit[105]) | ((bit[107]&bit[105] | bit[106]&bit[104])&bit[109]&bit[108]))) | (disable_update & out[5]);
		out[4] = (~disable_update & (((~bit[108] | ~bit[109]&bit[104])&bit[107]&bit[106]) | ((~bit[104] | ~bit[109]&~bit[106])&~bit[108]&~bit[105]) | ((bit[109] | bit[108])&~bit[107]&~bit[106]&bit[104]) | (~bit[107]&bit[106]&bit[105]) | (bit[107]&~bit[105]&~bit[104]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((~bit[108] | ~bit[109]&bit[104])&bit[106]) | (((~bit[106] | bit[105])&~bit[104] | bit[110]&bit[109]&bit[105])&bit[108]&~bit[107]) | ((bit[110] | ~bit[108])&~bit[109]&bit[107]&bit[105]&bit[104]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[110] | bit[109] | ~bit[104])&~bit[108]&bit[105]) | ((~bit[108] | ~bit[107]&bit[105])&~bit[106]&~bit[104]) | ((~bit[110]&bit[108] | bit[109]&bit[106])&bit[107]&bit[104]) | ((bit[108]&~bit[106] | (bit[108] | ~bit[107])&~bit[109])&~bit[105]&bit[104]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[109]&bit[106]&bit[105]) | ((~bit[105] | ~bit[108]&~bit[106])&bit[107]&~bit[104]) | ((~bit[107] | bit[110]&bit[105])&~bit[109]&~bit[108]&bit[104]) | ((bit[110] | ~bit[109])&~bit[107]&bit[105]&bit[104]) | ((~bit[110]&bit[106] | (bit[110]&~bit[106] | ~bit[111]&bit[105])&bit[107])&bit[109]&bit[108]&bit[104]) | ((~bit[107]&~bit[106] | ~bit[109])&bit[108]&bit[105]&~bit[104]) | ((~bit[109] | ~bit[106])&bit[108]&bit[107]&~bit[105]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[108]&bit[106]&bit[105]) | ((~bit[109] | ~bit[104])&~bit[106]&~bit[105]) | ((~bit[105] | ~bit[107]&bit[106])&~bit[108]&~bit[104]) | ((~bit[110]&~bit[107] | bit[110]&~bit[105])&~bit[109]&bit[104]) | ((bit[108]&~bit[107] | bit[110]&bit[107])&bit[106]&bit[104]) | ((~bit[110] | bit[109]&~bit[107])&~bit[108]&bit[105]&bit[104]) | ((bit[110] | bit[109])&~bit[111]&bit[108]&bit[107]&bit[105]&bit[104]))) | (disable_update & out[0]);

		vart = bit[106] & bit[105] & bit[104];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------35--------------

		out[5] = (~disable_update & (((bit[112] | ~bit[110])&bit[109]&bit[108]) | ((~bit[111]&bit[110] | bit[112]&bit[111]&~bit[110])&bit[109]&bit[107]) | ((~bit[110]&~bit[107] | bit[113]&bit[110]&bit[107])&~bit[111]&bit[108]))) | (disable_update & out[5]);
		out[4] = (~disable_update & (((bit[111] | bit[110]&~bit[108])&~bit[109]&~bit[107]) | ((bit[109]&bit[108] | bit[111]&bit[107])&~bit[112]&bit[110]) | ((bit[111]&bit[110] | (~bit[111] | ~bit[109])&~bit[110])&~bit[108]&bit[107]) | ((~bit[113]&(~bit[111] | bit[110]) | bit[113]&~bit[110])&bit[112]&bit[108]&bit[107]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[110]&~bit[108]&~bit[107]) | ((bit[109] | ~bit[113]&bit[110]&bit[107])&bit[112]&~bit[111]&bit[108]) | (((~bit[112] | bit[109])&~bit[110] | ~bit[109]&~bit[108])&~bit[111]&bit[107]) | ((bit[111]&bit[109] | ~bit[114]&~bit[112]&bit[108])&bit[113]&bit[110]&bit[107]) | ((~bit[110]&bit[108] | bit[110]&bit[109])&~bit[113]&~bit[112]&bit[107]) | ((~bit[107] | bit[114]&~bit[113]&bit[112])&bit[111]&bit[110]&~bit[109]&bit[108]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[108] | bit[109])&bit[111]&~bit[107]) | ((bit[110]&bit[109] | bit[113]&~bit[111]&bit[108])&bit[112]&bit[107]) | (((~bit[114] | bit[111])&bit[112]&bit[110] | ~bit[113]&~bit[112]&~bit[110])&bit[108]&bit[107]) | ((~bit[111]&~bit[110] | ~bit[112]&bit[110])&~bit[109]&~bit[108]&bit[107]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((~bit[107] | ~bit[112]&~bit[110])&~bit[108]) | (((~bit[114] | ~bit[110])&bit[111] | (~bit[111]&bit[110] | bit[113]&~bit[110])&bit[112])&bit[108]&bit[107]) | ((~bit[112] | bit[111]&~bit[110])&bit[109]&~bit[107]) | ((bit[113]&bit[110] | ~bit[113]&bit[112]&bit[111])&bit[109]&bit[107]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[108] | bit[111]&bit[107])&~bit[112]&bit[109]) | (bit[110]&~bit[109]&(~bit[108] | ~bit[107])) | ((~bit[108]&~bit[107] | bit[112]&bit[110])&~bit[111]&bit[109]) | ((bit[114]&bit[113] | ~bit[114]&bit[111])&bit[110]&bit[107]) | ((bit[113] | bit[112])&bit[110]&~bit[108]&bit[107]) | (~bit[113]&~bit[110]&bit[108]&bit[107]))) | (disable_update & out[0]);

		vart = bit[109] & bit[108] & bit[107];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------36---------------

		out[5] = (~disable_update & (((bit[111] | (~bit[116] | ~bit[115])&bit[114]&bit[110])&~bit[113]&bit[112]) | ((bit[114]&~bit[112]&~bit[110] | (~bit[114]&bit[110] | bit[116]&bit[114]&~bit[112])&~bit[117]&bit[115])&bit[113]&bit[111]))) | (disable_update & out[5]);
		out[4] = (~disable_update & (((bit[111] | bit[110])&bit[113]&bit[112]) | ((bit[117]&bit[115] | bit[116]&~bit[115] | ~bit[116]&bit[114])&bit[113]&bit[111]&bit[110]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[114] | ~bit[113] | ~bit[112])&~bit[111]&~bit[110]) | ((~bit[112] | bit[116]&bit[110])&bit[115]&bit[113]&~bit[111]) | ((~bit[113]&~bit[111] | ~bit[115]&bit[113]&~bit[110])&bit[114]&~bit[112]) | ((~bit[115]&~bit[114] | bit[114]&~bit[113])&~bit[116]&bit[111]&bit[110]) | ((bit[116] | bit[115])&bit[117]&bit[113]&bit[111]&bit[110]) | ((~bit[114]&bit[113]&bit[111] | bit[115]&bit[114]&bit[110])&bit[112]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[111] | ~bit[114]&~bit[113])&bit[112]&~bit[110]) | ((bit[113]&~bit[111] | ~bit[115]&~bit[114]&~bit[113])&~bit[112]&bit[110]) | (((~bit[117]&bit[114] | bit[115])&bit[113] | bit[114]&bit[112])&bit[116]&bit[110]) | ((bit[113] | ~bit[110])&~bit[115]&bit[112]) | ((bit[116]&~bit[114] | (bit[117] | bit[114])&bit[113])&bit[115]&bit[111]&bit[110]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[114] | ~bit[113])&bit[116]&bit[111]&bit[110]) | ((bit[115]&~bit[113] | bit[113]&~bit[112])&~bit[111]&bit[110]) | ((bit[115] | ~bit[111])&bit[113]&bit[112]&~bit[110]) | ((bit[117]&bit[116] | ~bit[115]&~bit[114])&bit[113]&~bit[112]&bit[110]) | ((bit[110] | ~bit[115]&bit[111])&bit[114]&bit[112]) | (~bit[113]&~bit[112]&~bit[110]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[114] | ~bit[114]&~bit[112])&~bit[111]&~bit[110]) | ((bit[115]&bit[110] | ~bit[114]&~bit[110])&~bit[113]&bit[111]) | ((bit[116]&bit[114]&~bit[113] | bit[111])&bit[115]&bit[112]) | ((bit[117] | bit[116]&~bit[115])&bit[114]&bit[113]&bit[111]&bit[110]) | ((bit[114]&bit[112] | ~bit[114]&bit[113]&~bit[111])&~bit[116]&~bit[115]&bit[110]))) | (disable_update & out[0]);

		vart = bit[112] & bit[111] & bit[110];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------37--------------

		out[5] = (~disable_update & ((((~bit[115] | ~bit[118]&bit[117])&~bit[113] | (~bit[118] | ~bit[120]&~bit[119])&~bit[117]&bit[113])&bit[116]&bit[114]) | ((~bit[115] | ~bit[119]&~bit[118])&bit[117]&bit[116]&~bit[114]&bit[113]))) | (disable_update & out[5]);
		out[4] = (~disable_update & (((bit[115] | bit[117]&~bit[113])&~bit[116]&bit[114]) | ((bit[117]^bit[116])&bit[115]&bit[113]) | ((bit[117] | ~bit[120]&bit[119]&bit[118])&bit[116]&bit[114]&bit[113]) | ((bit[119] | bit[118])&~bit[116]&bit[115]&bit[113]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((bit[118] | bit[117])&bit[115]&bit[114]) | ((bit[117] | ~bit[119]&bit[118])&bit[115]&bit[113]) | ((~bit[114] | bit[115])&bit[116]&~bit[113]) | ((~bit[119] | ~bit[115]&~bit[114])&~bit[118]&~bit[117]&~bit[116]&bit[113]) | (((bit[119] | bit[120]&~bit[117])&bit[118] | ~bit[120]&~bit[119]&~bit[118])&bit[116]&bit[114]&bit[113]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[117]&~bit[116]&~bit[113]) | (((bit[118] | ~bit[115])&bit[119] | (~bit[115]&~bit[114] | bit[120]&bit[114])&bit[118])&bit[116]&bit[113]) | ((bit[119]&bit[114] | (~bit[116]&bit[114] | ~bit[119]&bit[115])&bit[118])&~bit[117]&bit[113]) | ((bit[116]&bit[114] | bit[115])&~bit[118]&~bit[117]&~bit[113]) | ((bit[116]&bit[113] | bit[115]&~bit[113])&~bit[117]&~bit[114]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[117]&bit[115]&bit[114]) | ((~bit[116] | bit[117]&~bit[114])&bit[115]&~bit[113]) | ((bit[116] | bit[117])&bit[118]&bit[114]&~bit[113]) | ((bit[118]&~bit[116] | ~bit[120]&bit[119]&bit[116] | (bit[118] | bit[117]&bit[116])&bit[120]&~bit[119])&bit[114]&bit[113]) | ((~bit[118] | ~bit[116])&bit[117]&~bit[115]&~bit[114]&bit[113]) | ((~(bit[119]^bit[118]) | bit[118]&bit[116])&bit[115]&bit[113]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[119]&bit[115]&bit[113]) | ((bit[118] | ~bit[113])&~bit[117]&~bit[114]) | ((bit[116] | bit[115])&bit[118]&bit[117]&bit[114]) | ((bit[116]&bit[114] | bit[115]&~bit[113])&~bit[118]&~bit[117]) | ((bit[117]&bit[114] | bit[118]&~bit[115])&~bit[119]&~bit[116]&bit[113]) | ((bit[120]&~bit[119]&bit[113] | bit[117]&~bit[115]&~bit[113])&bit[116]&bit[114]) | ((~bit[119]&bit[116] | ~bit[116]&~bit[115])&bit[118]&~bit[114]&bit[113]))) | (disable_update & out[0]);

		vart = bit[115] & bit[114] & bit[113];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------38--------------

		out[5] = (~disable_update & (((~bit[120] | ~bit[122]&~bit[121])&~bit[119]&bit[118]&bit[116]) | ((~bit[119] | ~bit[121]&~bit[120])&~bit[118]&bit[117]&~bit[116]) | ((~bit[120]&bit[117] | bit[120]&~bit[118]&~bit[117])&bit[121]&bit[119]&bit[116]) | ((~bit[123]&~bit[122]&bit[120]&bit[116] | bit[122]&~bit[120]&~bit[118])&~bit[121]&bit[119]&bit[117]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[118]&bit[117]) | (((bit[120] | ~bit[116])&bit[121] | (bit[123] | bit[122])&bit[120]&bit[116])&bit[119]&bit[117]) | (((bit[122] | bit[121])&bit[120] | ~bit[120]&bit[119])&bit[118]&bit[116]))) | (disable_update & out[4]);
		out[3] = (~disable_update & (((~bit[119] | ~(bit[121]^bit[120]))&bit[118]&bit[117]) | ((bit[121] | bit[117])&~bit[120]&~bit[119]&bit[116]) | ((bit[118] | ~bit[122]&bit[117])&bit[120]&bit[119]&bit[116]) | ((~bit[120]&bit[116] | ~bit[119]&~bit[116])&~bit[118]&~bit[117]) | ((bit[122]&bit[117] | ~bit[120]&~bit[117])&~bit[121]&bit[119]&bit[116]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((bit[121] | ~bit[120]&~bit[119])&bit[117]&~bit[116]) | ((bit[121]&bit[117] | bit[118]&bit[116])&bit[120]&bit[119]) | ((bit[117] | ~bit[122]&~bit[120]&bit[118])&~bit[121]&~bit[119]&bit[116]) | ((bit[120]&~bit[118] | bit[118]&~bit[117])&bit[119]&~bit[116]) | ((~bit[120] | bit[120]&~bit[118])&bit[121]&bit[119]&~bit[117]&bit[116]) | ((~bit[120]&bit[119] | bit[120]&~bit[119]&~bit[117])&bit[122]&bit[121]&bit[116]))) | (disable_update & out[2]);
		out[1] = (~disable_update & (((bit[121] | ~bit[117])&~bit[120]&~bit[116]) | ((~bit[120]&bit[119] | ~bit[121]&~bit[119]&bit[116])&~bit[118]&~bit[117]) | ((~bit[121]&bit[116] | ~bit[119]&bit[117])&bit[120]&bit[118]) | ((~bit[119]&bit[117] | ~bit[120]&bit[118])&~bit[122]&bit[116]) | ((bit[120]&~bit[116] | bit[122]&bit[119]&bit[116])&~bit[121]&bit[117]) | (((bit[123] | ~bit[121])&~bit[120] | (~bit[117] | bit[123])&~bit[122]&bit[121]&bit[116])&bit[119]&~bit[118]))) | (disable_update & out[1]);
		out[0] = (~disable_update & (((bit[118] | (bit[122] | bit[121] | bit[120])&bit[123]&bit[116])&bit[119]&bit[117]) | ((~bit[122]&bit[121] | ~bit[121]&bit[120])&bit[118]&bit[116]) | ((bit[119]&~bit[116] | (~bit[120] | bit[116])&~bit[119])&~bit[118]&~bit[117]) | ((~bit[119]&~bit[118] | bit[121])&bit[120]&bit[117]&~bit[116]) | (((bit[122] | bit[121])&bit[120] | bit[122]&~bit[117])&~bit[119]&bit[116]))) | (disable_update & out[0]);

		vart = bit[118] & bit[117] & bit[116];
		disable_update= disable_update | (control & ~vart);
		control=control & vart;
		//-------------39--------------

		out[5] = (~disable_update & ((((~bit[125] | ~bit[123])&bit[121] | (bit[121] | bit[123]&bit[120])&bit[124])&bit[122]&bit[119]) | ((bit[123]^bit[122])&~bit[121]&bit[120]&~bit[119]) | ((~bit[119] | ~bit[126]&bit[125]&bit[123])&~bit[124]&bit[122]&~bit[121]&bit[120]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[121]&bit[120]) | ((~bit[123]&bit[119] | bit[121]&~bit[119])&~bit[122]&bit[120]) | ((bit[124]&~bit[122]&bit[121] | bit[126]&bit[125]&~bit[124]&bit[122]&bit[120])&bit[123]&bit[119]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((((bit[125] | ~bit[120])&~bit[122] | ~bit[121]&~bit[120])&bit[124]&bit[119]) | ((bit[125] | ~bit[119])&bit[124]&bit[123]&bit[120]) | ((~bit[123]&~bit[119] | ~bit[124]&bit[123]&bit[119])&bit[122]&bit[121]) | (bit[123]&~bit[122]&bit[120]) | (bit[122]&~bit[120]&~bit[119]))) | (disable_update & out[3]);
		out[2] = (~disable_update & (((~bit[119] | bit[125]&bit[121])&bit[122]) | ((bit[124]&bit[123]&bit[120] | ~bit[123]&~bit[120]&~bit[119])&bit[121]) | ((~bit[122] | ~bit[124]&bit[120])&~bit[123]&bit[119]) | ((~bit[122] | ~bit[125]&bit[123])&bit[124]&bit[120]&bit[119]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[123]&~bit[120]&~bit[119]) | ((~bit[123]&bit[121] | bit[122]&~bit[119])&bit[124]&bit[120]) | ((~bit[121] | ~bit[124]&~bit[123])&~bit[122]&~bit[120]&bit[119]) | ((~bit[119] | bit[125])&~bit[124]&bit[123]&~bit[121]) | ((~bit[126]&bit[122] | bit[125]&~bit[122]&bit[119])&bit[124]&bit[123]) | ((~bit[125] | bit[124])&bit[123]&bit[122]&~bit[120]) | ((~bit[124] | bit[122])&~bit[125]&~bit[123]&bit[120]&bit[119]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[124]&bit[123]&bit[121]) | ((bit[121] | bit[123]&~bit[122])&~bit[120]&~bit[119]) | ((~bit[121]&bit[119] | ~bit[122]&bit[120])&~bit[124]&~bit[123]) | ((~bit[121]&~bit[120] | bit[124]&~bit[123])&bit[122]&bit[119]) | ((bit[123]&bit[122] | bit[124]&~bit[122]&~bit[121])&bit[120]&~bit[119]) | ((bit[125]&bit[122] | bit[123]&~bit[122])&bit[120]&bit[119]))) | (disable_update & out[0]);
		//-------------40--------------

		for(k=0;k<8;k++){
			
				sample_t=out[5]&mask;
				sample_t=(sample_t<<1) | (out[4]&mask);
				sample_t=(sample_t<<1) | (out[3]&mask);
				sample_t=(sample_t<<1) | (out[2]&mask);
				sample_t=(sample_t<<1) | (out[1]&mask);
				sample_t=(sample_t<<1) | (out[0]&mask);
				sample_n[k]=sample_t;

			    out[5]=out[5]>>1;			
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








