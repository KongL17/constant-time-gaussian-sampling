//gcc -Wextra -O3 -fomit-frame-pointer -march=native -o sample_128_opt sampler-128-opt.c

// the optimized sampler by DTC with further optimizations

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"randombytes.c"
#include"fips202.c"
#include "cpucycles.c"

#define ZERO 0x0000000000000000
#define ONE 0xffffffffffffffff

//-------shake------
void initRandom(uint8_t *x, uint32_t len){

	randombytes(x,len);
}
//-------shake ends-

int main(){

	int64_t i,j,k;
	uint64_t bit[128];
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
	uint32_t nrounds=6;
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

		memset(out,0,5*sizeof(uint64_t));  

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

		control=0xffffffffffffffff; 

		out[2] = (bit[2]&bit[1]&bit[0]&(~bit[3] | bit[4]&(bit[6] | bit[8]&bit[7]&bit[5])));
		out[1] = (bit[1]&~bit[0]) | (bit[4]&bit[3]&~bit[2]&bit[1]) | (bit[3]&bit[2]&bit[0]&(~bit[1] | (~bit[5]&~bit[4]) | bit[4]&(~bit[7] | ~bit[8]&bit[5])));
		out[0] = (~bit[2]&~bit[0]) | (~bit[1]&~bit[0]) | (~bit[2]&bit[1]&(~bit[4] | ~bit[3])) | (bit[5]&bit[1]&bit[0]&(bit[4]&~bit[3] | bit[2]&(~bit[7]&bit[4] | bit[6]&bit[3])));
		control=control&bit[0]&bit[1]&bit[2]&bit[3]&bit[4]&bit[5]&bit[6];
		//-------------1---------------for the first obtained subtree in DTC after subsequent optimizations 
		
		out[3] = (control & (~bit[12]&bit[11]&~bit[10]&bit[9]&bit[8]&bit[7]))  | (~control & out[3]);
		out[2] = (control & ((bit[10]&bit[9]) | (bit[10]&bit[7]) | (~bit[8]&~bit[7]) | (~bit[11]&bit[9]&bit[8]&bit[7])))  | (~control & out[2]);
		out[1] = (control & ((~bit[8]&bit[7]) | (~bit[9]&bit[8]&~bit[7]) | (~bit[11]&~bit[10]&bit[7]) | ((bit[12] | bit[14]&bit[13]&bit[11]&bit[10])&bit[9]&bit[7])))  | (~control & out[1]);
		out[0] = (control & ((bit[12]&bit[9]) | (~bit[11]&bit[9]) | (bit[9]&~bit[8]) | (bit[9]&~bit[7]) | (~bit[13]&bit[10]&bit[9]) | (~bit[11]&bit[10]&bit[8]&bit[7])))  | (~control & out[0]);
		control=control&bit[7]&bit[8]&bit[9]&bit[10]&bit[11]&bit[12];
		//-------------2---------------for the second obtained subtree in DTC after subsequent optimizations

		out[3] = (control & ((bit[15]&~bit[14]&~bit[13]) | (bit[19]&~bit[18]&bit[17]&bit[15]&bit[14]&bit[13]) | (bit[16]&bit[13]&(~bit[15]&~bit[14] | ~bit[17]&~bit[14]))))  | (~control & out[3]);
		out[2] = (control & ((bit[16]&bit[14]&(bit[18] | ~bit[13])) | (~bit[15]&bit[14]&(~bit[17] | ~bit[13])) | ((~bit[19]&~bit[16]&bit[15] | bit[19]&~bit[17])&bit[14]&bit[13])))  | (~control & out[2]);
		out[1] = (control & ((~bit[15]&~bit[13]) | (~bit[16]&bit[15]&bit[13]) | ((bit[20]&bit[17]&bit[15] | ~bit[16])&bit[14]&bit[13])))  | (~control & out[1]);
		out[0] = (control & ((bit[15]&~bit[13]) | (bit[14]&~bit[13]) | (bit[18]&bit[15]&bit[14]&(bit[20] | ~bit[16])) | (bit[17]&bit[16]&(~bit[15]&bit[14] | bit[15]&~bit[14])) | ((~bit[19]&~bit[17]&bit[15] | ~bit[20]&bit[19]&bit[17])&bit[16]&bit[14])))  | (~control & out[0]);
		control=control&bit[13]&bit[14]&bit[15]&bit[16]&bit[17]&bit[18];
		//-------------3---------------for the third obtained subtree in DTC after subsequent optimizations

		out[3] = (control & ((bit[20]&~bit[19]) | (~bit[22]&~bit[21]&bit[20]) | ((~bit[24] | ~bit[26]&bit[22] | bit[25]&bit[23])&bit[21]&bit[20])))  | (~control & out[3]);
		out[2] = (control & ((bit[21]&~bit[20]) | (bit[22]&bit[19]&(bit[23]&bit[20] | bit[26]&bit[25]&bit[24]&bit[21]))))  | (~control & out[2]);
		out[1] = (control & (((~bit[23]&~bit[22] | ~bit[24]&bit[21])&bit[20]&bit[19]) | ((~bit[20] | bit[26] | ~bit[25] | ~bit[21])&bit[22]&bit[19])))  | (~control & out[1]);
		out[0] = (control & ((bit[21]&~bit[19]) | (~bit[20]&~bit[19]) | (bit[20]&bit[19]&(bit[23]&bit[22] | ~bit[23]&~bit[22])) | (bit[22]&bit[21]&bit[20]&(bit[25] | bit[26]&bit[24]))))  | (~control & out[0]);
		control=control&bit[19]&bit[20]&bit[21]&bit[22]&bit[23];
		//-------------4--------------

		out[3] = (control & ((~bit[27]&bit[26]&~bit[25]) | (bit[29]&bit[28]&bit[27]&bit[25]&bit[24]) | (~bit[27]&bit[26]&(~bit[25] | ~bit[24]))))  | (~control & out[3]);
		out[2] = (control & ((bit[25]&~bit[24]) | (~bit[29]&bit[27]&bit[25]) | (~bit[30]&bit[29]&bit[26]&bit[25]) | (bit[28]&bit[24]&(~bit[27]&~bit[25] | bit[26]&(bit[30] | ~bit[25])))))  | (~control & out[2]);
		out[1] = (control & ((bit[27]&bit[26]) | (bit[26]&~bit[25]&~bit[24]) | (~bit[28]&bit[24]&(bit[27] | ~bit[26]&~bit[25])) | (bit[29]&bit[25]&bit[24]&(bit[28] | bit[30]&bit[26]))))  | (~control & out[1]);
		out[0] = (control & ((bit[27]&~bit[26]&bit[25]) | (bit[28]&bit[27]&bit[24]) | (~bit[28]&~bit[26]&bit[24]) | (bit[30]&bit[29]&bit[26]&bit[25]&bit[24])))  | (~control & out[0]);
		control=control&bit[24]&bit[25]&bit[26]&bit[27];
		//-------------5--------------

		out[3] = (control & ((bit[34]&~bit[32]&bit[30]&bit[29]) | ((~bit[31] | bit[30])&bit[29]&~bit[28]) | ((~bit[33]&bit[29] | bit[30]&~bit[29])&bit[32]&bit[28])))  | (~control & out[3]);
		out[2] = (control & ((~bit[29]&~bit[28]) | (~bit[32]&bit[31]&bit[28]) | (bit[31]&bit[30]&~bit[28]) | ((bit[33]&bit[31] | ~bit[34]&bit[30])&bit[29]&bit[28])))  | (~control & out[2]);
		out[1] = (control & ((bit[31]&~bit[29]) | (~bit[30]&bit[29]&~bit[28]) | (bit[32]&bit[31]&bit[28]) | (bit[33]&bit[29]&bit[28]&(bit[30] | bit[32]))))  | (~control & out[1]);
		out[0] = (control & ((bit[32]&bit[30]) | (bit[30]&~bit[29]) | (bit[30]&~bit[28]) | (bit[32]&bit[31]&~bit[29]&bit[28]) | (bit[31]&bit[29]&(~bit[28] | bit[33]&~bit[32]))))  | (~control & out[0]);
		control=control&bit[28]&bit[29]&bit[30]&bit[31];
		//-------------6--------------

		out[3] = (control & ((bit[34]&~bit[33]&bit[32]) | (~bit[35]&bit[33]&~bit[32]) | (bit[36]&bit[33]&bit[32]&(bit[35] | bit[37]))))  | (~control & out[3]);
		out[2] = (control & ((~bit[37]&bit[34]&bit[33]) | (~bit[37]&bit[35]&bit[32]) | ((~bit[34] | bit[35])&~bit[33]&bit[32]) | ((bit[35] | bit[34])&bit[33]&~bit[32]) | ((bit[38]&bit[34] | ~bit[37]&bit[32])&bit[36]&bit[33])))  | (~control & out[2]);
		out[1] = (control & ((~bit[35]&~bit[33]&bit[32]) | (bit[34]&~bit[33]&~bit[32]) | ((~bit[36] | bit[37]&bit[33])&bit[35]&bit[32]) | ((bit[37] | bit[38]&bit[36])&bit[34]&bit[33]&bit[32])))  | (~control & out[1]);
		out[0] = (control & ((~bit[36]&~bit[35]&bit[34]&bit[32]) | (bit[34]&bit[33]&(bit[35] | ~bit[37]&bit[32])) | (bit[36]&bit[32]&(bit[35]&~bit[34] | (bit[37]&bit[33]&(~bit[34] | ~bit[38]))))))  | (~control & out[0]);
		control=control&bit[32]&bit[33]&bit[34]&bit[35];
		//-------------7--------------

		out[3] = (control & ((bit[38]&bit[37]&~bit[36]) | (bit[41]&bit[39]&bit[37]&bit[36]) | (bit[40]&bit[36]&(~bit[37] | ~bit[41] | bit[42]&bit[38])) | ((~bit[37] | ~bit[40]&~bit[39])&~bit[38]&bit[36])))  | (~control & out[3]);
		out[2] = (control & ((~bit[38]&bit[37]&~bit[36]) | (~bit[39]&bit[36]&(~bit[37] | ~bit[40]&~bit[38])) | (~bit[41]&bit[37]&bit[36]&(~bit[40] | bit[39])) | (~bit[40]&bit[38]&bit[36]&(bit[42] | ~bit[37]))))  | (~control & out[2]);
		out[1] = (control & ((bit[39]&bit[38]) | (bit[39]&bit[37]) | (bit[38]&~bit[37]&~bit[36]) | (bit[41]&bit[38]&bit[37]&bit[36])))  | (~control & out[1]);
		out[0] = (control & ((~bit[38]&~bit[37]) | (~bit[37]&bit[36]) | (bit[40]&bit[39]&bit[36]) | (~bit[39]&~bit[38]&~bit[36]) | ((~bit[42]&bit[40] | ~bit[41]&~bit[40])&bit[38]&bit[36])))  | (~control & out[0]);
		control=control&bit[36]&bit[37]&bit[38]&bit[39];
		//-------------8--------------

		out[4] = (control & (~bit[47]&bit[46]&bit[45]&bit[44]&bit[42]&bit[41]&bit[40]))  | (~control & out[4]);
		out[3] = (control & ((~bit[44]&bit[43]&~bit[42]&bit[40]) | (~bit[43]&bit[41]&(bit[42]&~bit[40] | ~bit[45]&bit[44]&bit[40])) | ((bit[47] | ~bit[46])&bit[44]&bit[42]&bit[41]&bit[40])))  | (~control & out[3]);
		out[2] = (control & ((bit[42]&~bit[41]&~bit[40]) | (bit[45]&bit[44]&bit[43]&bit[40]) | (~bit[44]&bit[40]&(~bit[42] | ~bit[45]&bit[41])) | (bit[43]&~bit[42]&(~bit[41]&bit[40] | bit[41]&~bit[40])) | (bit[46]&bit[42]&bit[41]&bit[40]&(~bit[45] | bit[47]&bit[44]))))  | (~control & out[2]);
		out[1] = (control & ((bit[43]&bit[42]) | (~bit[45]&bit[41]&bit[40]) | (~bit[44]&~bit[41]&bit[40]) | (bit[46]&bit[42]&bit[40]&(~bit[44] | bit[47]&bit[41]))))  | (~control & out[1]);
		out[0] = (control & ((bit[43]&bit[41]) | (bit[41]&~bit[40]) | (~bit[46]&bit[42]&bit[41]) | (~bit[43]&~bit[42]&~bit[41]&bit[40]) | ((bit[45] | ~bit[44])&bit[43]&bit[42]&bit[40])))  | (~control & out[0]);
		control=control&bit[40]&bit[41]&bit[42]&bit[43];
		//-------------9--------------

		out[4] = (control & (((~bit[49]&bit[47] | ~bit[50]&bit[49]&bit[45])&bit[48]&bit[46]&bit[44])))  | (~control & out[4]);
		out[3] = (control & ((~bit[48]&bit[47]&bit[46]) | (~bit[48]&~bit[45]&bit[44]) | ((~bit[47] | bit[46])&~bit[45]&~bit[44]) | ((bit[49]&~bit[46] | ~bit[49]&bit[48]&~bit[47])&bit[45]&bit[44])))  | (~control & out[3]);
		out[2] = (control & ((bit[48]&bit[47]&(~bit[46] | bit[45])) | (~bit[46]&~bit[44]) | (~bit[49]&bit[45]&bit[44]) | (~bit[48]&bit[46]&~bit[45]&bit[44])))  | (~control & out[2]);
		out[1] = (control & ((bit[45]&~bit[44]) | (~bit[48]&bit[47]&~bit[45]&bit[44]) | (bit[47]&bit[46]&(bit[49] | ~bit[44])) | (~bit[49]&bit[45]&(bit[47] | bit[50]&bit[46])) | (bit[49]&bit[48]&~bit[47]&bit[45]&(~bit[46] | bit[50]))))  | (~control & out[1]);
		out[0] = (control & ((~bit[47]&~bit[44]) | (~bit[48]&bit[47]&bit[44]) | (~bit[46]&~bit[45]&bit[44]) | (bit[46]&~bit[44]&(bit[48] | ~bit[45])) | (bit[49]&bit[45]&bit[44]&(bit[47] | bit[50]&~bit[48]&bit[46]))))  | (~control & out[0]);
		control=control&bit[44]&bit[45]&bit[46]&bit[47];
		//-------------10--------------

		out[4] = (control & (bit[52]&~bit[51]&~bit[50]&bit[48]))  | (~control & out[4]);
		out[3] = (control & ((~bit[51]&~bit[48]) | (bit[50]&~bit[48]) | (~bit[52]&~bit[51]&~bit[50]) | (bit[52]&bit[50]&bit[49]) | (bit[52]&bit[51]&~bit[50]&bit[48])))  | (~control & out[3]);
		out[2] = (control & ((bit[51]&bit[49]) | (~bit[51]&bit[50]&~bit[48]) | (bit[54]&bit[52]&bit[50]&bit[49]) | (bit[51]&bit[48]&(~bit[52] | bit[50])) | (~bit[52]&bit[49]&bit[48]&(~bit[50] | bit[54]&bit[53]))))  | (~control & out[2]);
		out[1] = (control & ((bit[51]&bit[50]) | ((~bit[49] | ~bit[54]&bit[53] | ~bit[53]&~bit[52])&bit[50]&bit[48]) | ((bit[53]&bit[49] | bit[52]&~bit[49])&bit[51]&bit[48])))  | (~control & out[1]);
		out[0] = (control & ((~bit[49]&~bit[48]) | (bit[52]&bit[50]&~bit[49]) | (bit[53]&bit[52]&bit[49]&bit[48]) | (bit[51]&~bit[50]&bit[49])))  | (~control & out[0]);
		control=control&bit[48]&bit[49]&bit[50]&bit[51];
		//-------------11--------------

		out[4] = (control & ((~bit[55]&bit[53]&~bit[52]&bit[51]&(~bit[54] | ~bit[56])) | (bit[56]&bit[55]&bit[52]&bit[51]&(bit[57]&bit[54] | ~bit[58]&~bit[57]&bit[53]))))  | (~control & out[4]);
		out[3] = (control & ((bit[55]&~bit[52]&bit[51]) | (~bit[55]&~bit[54]&bit[52]&bit[51]) | ((bit[56]&bit[54] | (bit[57] | bit[58]&bit[56])&bit[52])&bit[53]&bit[51])))  | (~control & out[3]);
		out[2] = (control & ((bit[55]&bit[51]&(~bit[52] | bit[58]&bit[56]&bit[53])) | (bit[57]&bit[53]&bit[52]&bit[51]&(~bit[55] | bit[56])) | (bit[54]&bit[51]&(~bit[56]&~bit[53] | bit[56]&~bit[52]))))  | (~control & out[2]);
		out[1] = (control & ((bit[54]&bit[52]&(bit[53] | ~bit[55]&bit[51] | ~bit[57]&bit[56]&bit[51])) | (~bit[54]&bit[51]&(bit[55]&~bit[52] | ~bit[56]&bit[52])) | (bit[56]&bit[55]&bit[53]&bit[51]&(bit[58] | ~bit[52]))))  | (~control & out[1]);
		out[0] = (control & ((bit[52]&bit[51]&(bit[55]&(bit[57] | ~bit[56] | ~bit[58]&bit[53]) | bit[56]&~bit[54]&~bit[53])) | ((~bit[55]&bit[54] | ~bit[54]&bit[53])&~bit[52]&bit[51])))  | (~control & out[0]);
		control=control&bit[51]&bit[52]&bit[53]&bit[54];
		//-------------12--------------

		out[4] = (control & ((bit[59]&bit[57]&(~bit[58]&bit[56] | ~bit[60]&bit[58]&bit[55]))))  | (~control & out[4]);
		out[3] = (control & ((bit[58]&~bit[57]&bit[55]) | (~bit[58]&bit[57]&bit[55]) | (~bit[59]&~bit[56]&bit[55]) | (bit[57]&~bit[56]&~bit[55]) | (bit[60]&bit[59]&bit[56]&bit[55])))  | (~control & out[3]);
		out[2] = (control & ((~bit[59]&bit[56]) | (~bit[57]&bit[56]&~bit[55]) | ((~bit[58] | bit[60]&bit[59])&bit[57]&bit[55])))  | (~control & out[2]);
		out[1] = (control & ((~bit[59]&bit[55]&(~bit[58] | bit[57])) | (bit[58]&~bit[55]) | (bit[60]&bit[58]&bit[56]) | (bit[59]&bit[57]&bit[56])))  | (~control & out[1]);
		out[0] = (control & ((bit[58]&bit[56]&~bit[55]) | (~bit[58]&~bit[56]&~bit[55]) | (bit[60]&bit[55]&(bit[58]&bit[57] | ~bit[58]&bit[56])) | (~bit[59]&bit[58]&bit[55]&(~bit[56] | ~bit[60]))))  | (~control & out[0]);
		control=control&bit[55]&bit[56]&bit[57];
		//-------------13--------------

		out[4] = (control & ((bit[62]&bit[58]&((bit[63]&(bit[61] | bit[64]&bit[60]&bit[59])) | ~bit[60]&~bit[59])) | (bit[61]&bit[60]&~bit[59]&~bit[58])))  | (~control & out[4]);
		out[3] = (control & (((~bit[64]&bit[63] | bit[64]&~bit[62] | ~bit[58])&bit[60]&bit[59]) | ((~bit[62] | ~bit[60]&~bit[59])&bit[61]&bit[58])))  | (~control & out[3]);
		out[2] = (control & ((bit[61]&bit[59]&(bit[60] | ~bit[63]&&bit[58])) | (~bit[61]&~bit[59]&(~bit[58] | bit[62]&bit[60])) | ((~bit[62]&~bit[59] | bit[63]&~bit[61]&bit[59])&~bit[60]&bit[58]) | ((~bit[64]&bit[62] | bit[63]&~bit[62])&bit[60]&bit[59]&bit[58])))  | (~control & out[2]);
		out[1] = (control & (((~bit[58] | ~bit[63])&~bit[61]&bit[59]) | ((bit[64]&(bit[63]&bit[61] | ~bit[65]&bit[60]) | ~bit[62])&bit[59]&bit[58]) | (~bit[61]&~bit[60]&~bit[59])))  | (~control & out[1]);
		out[0] = (control & ((~bit[60]&~bit[59]&(~bit[62] | bit[61])) | (~bit[63]&bit[62]&(~bit[60]&bit[59] | bit[61]&bit[58])) | (bit[64]&bit[60]&bit[59]&bit[58]&(bit[65] | ~bit[62] | ~bit[63])) | (~bit[60]&~bit[58]) | (bit[61]&bit[59]&~bit[58])))  | (~control & out[0]);
		control=control&bit[58]&bit[59]&bit[60]&bit[61];
		//-------------14--------------

		out[4] = (control & (((bit[66] | ~bit[62])&bit[65]&bit[64]) | ((bit[64] | bit[69]&(bit[68]&~bit[66] | bit[67]&~bit[66]))&bit[64]&bit[63]) | (~bit[68]&bit[67]&bit[66]&bit[65]&bit[63]&bit[62])))  | (~control & out[4]);
		out[3] = (control & (((bit[65] | bit[63])&~bit[64]&~bit[62]) | ((~bit[69]&bit[67] | ~bit[68]&bit[66])&bit[64]&bit[63]&bit[62]) | (~bit[66]&~bit[64]&bit[62]&(~bit[65] | ~bit[63])) | ((bit[68]&bit[63] | bit[67]&~bit[66]&~bit[63])&bit[65]&bit[62])))  | (~control & out[3]);
		out[2] = (control & (((bit[66] | ~bit[62])&~bit[65]&~bit[64]) | (~bit[64]&bit[63]&(~bit[62] | bit[66]&bit[62])) | (((bit[69]&bit[68] | ~bit[67]&bit[65])&bit[66] | (~bit[69]&~bit[68]&bit[64] | bit[68]&bit[66])&bit[67])&bit[63]&bit[62])))  | (~control & out[2]);
		out[1] = (control & ((~bit[66]&~bit[65]&(~bit[63] | ~bit[64])) | (bit[66]&bit[65]&bit[62]&(bit[67]&bit[64] | bit[63])) | (bit[66]&~bit[62]&(~bit[65] | bit[64]&bit[63])) | ((bit[67]&(~bit[66] | bit[69]&bit[64]) | ~bit[67]&(bit[66]&~bit[64] | ~bit[69]&bit[68]&bit[64]))&bit[63]&bit[62])))  | (~control & out[1]);
		out[0] = (control & (((~bit[68] | ~bit[62])&bit[65]&bit[63]) | ((bit[66] | bit[64])&~bit[65]&~bit[63]) | ((bit[68]&(bit[69] | ~bit[66]) | ~bit[69]&~bit[68]&bit[66])&bit[64]&bit[63]&bit[62])))  | (~control & out[0]);
		control=control&bit[62]&bit[63]&bit[64]&bit[65];
		//-------------15--------------

		out[4] = (control & ((bit[69]&bit[66]&(~bit[68]&~bit[67] | bit[70]&bit[67])) | ((bit[69]&&~bit[66] | bit[70]&~bit[69]&bit[66])&bit[68]&~bit[67]) | ((~bit[73]&bit[71] | bit[72]&~bit[71])&bit[70]&bit[68]&bit[67]&bit[66])))  | (~control & out[4]);
		out[3] = (control & (((bit[69] | bit[71]&~bit[70])&bit[68]&bit[66]) | ((bit[70] | ~bit[68])&bit[67]&~bit[66]) | (((~bit[72]&~bit[70] | bit[73]&bit[71])&bit[68] | bit[71]&~bit[70])&bit[67]&bit[66])))  | (~control & out[3]);
		out[2] = (control & ((~bit[70]&bit[67]&(~bit[71] | ~bit[66])) | ((~(bit[70]^bit[69]) | (bit[73] | ~bit[72])&bit[71]&bit[67])&bit[68]&bit[66]) | (bit[69]&bit[68]&bit[67])))  | (~control & out[2]);
		out[1] = (control & (((bit[73]&bit[71]&bit[70] | bit[72])&bit[68]&bit[67]) | ((bit[67] | ~bit[70])&bit[68]&~bit[66]) | (bit[70]&bit[67]&(bit[69]&~bit[66] | ~bit[71]&~bit[68]&bit[66])) | (~bit[70]&~bit[67]&bit[66])))  | (~control & out[1]);
		out[0] = (control & (((~bit[67] | ~bit[72]&~bit[70]&bit[68])&bit[71]&bit[66]) | ((~bit[71] | bit[73]&bit[70])&bit[72]&bit[68]&bit[67]&bit[66]) | ((~bit[71]&~bit[68] | bit[72]&bit[67])&bit[69]&bit[66]) | (~bit[70]&bit[69]&bit[67]) | (~bit[69]&~bit[68]&~bit[67]) | (bit[70]&~bit[69]&bit[67]&~bit[66])))  | (~control & out[0]);
		control=control&bit[66]&bit[67]&bit[68]&bit[69];
		//-------------16--------------

		out[4] = (control & ((bit[75]&bit[70]&(bit[74]&bit[72] | ~bit[73]&bit[71])) | ((~bit[72]&~bit[70] | ~bit[76]&bit[74]&bit[70])&bit[73]&bit[71])))  | (~control & out[4]);
		out[3] = (control & ((bit[75]&bit[72]) | (bit[72]&~bit[70]) | (~bit[73]&bit[71]&~bit[70]) | ((bit[76]&bit[74] | ~bit[75]&~bit[74])&bit[73]&bit[71]&bit[70])))  | (~control & out[3]);
		out[2] = (control & ((bit[73]&~bit[71]) | (~bit[71]&~bit[70]) | ((bit[75] | bit[76]&bit[74])&bit[73]&bit[70]) | (bit[72]&(~bit[74]^bit[70]))))  | (~control & out[2]);
		out[1] = (control & ((~bit[74]&bit[70]&(bit[72] | bit[73]&bit[71])) | (bit[74]&bit[70]&(~bit[72]&~bit[71] | ~bit[73] | (bit[76] | ~bit[75])&bit[71])) | (~bit[73]&~bit[72]&~bit[70])))  | (~control & out[1]);
		out[0] = (control & ((bit[75]&~bit[73]&(bit[74] | bit[71])) | (~bit[72]&~bit[71]&(~bit[73] | ~bit[70])) | (~bit[74]&bit[70]&(bit[73]&bit[72] | bit[76]&bit[75]&bit[71])) | (~bit[73]&~bit[70])))  | (~control & out[0]);
		control=control&bit[70]&bit[71]&bit[72];
		//-------------17--------------

		out[4] = (control & (((bit[77] | bit[76])&~bit[75]&bit[74]&~bit[73]) | ((~bit[79]&bit[77]&bit[75] | bit[79]&~bit[78]&bit[76])&bit[74]&bit[73]) | (~bit[77]&bit[76]&bit[75]&bit[73])))  | (~control & out[4]);
		out[3] = (control & ((bit[75]&~bit[73]) | (bit[78]&bit[76]&bit[74]&bit[73]) | (bit[77]&bit[75]&(bit[79] | ~bit[74]))))  | (~control & out[3]);
		out[2] = (control & ((~bit[78]&~bit[77]&bit[74]&(bit[76] | ~bit[79]&bit[75])) | ((bit[77] | bit[78]&~bit[76])&~bit[75]&~bit[74]&bit[73]) | (~bit[78]&bit[77]&bit[73]&(~bit[74] | ~bit[79]&bit[76])) | ((bit[79]&bit[78] | (bit[79] | bit[80]&bit[78])&bit[75])&bit[77]&bit[74]&bit[73]) | (bit[76]&bit[75]&bit[74]) | (~bit[77]&bit[74]&~bit[73]) | (~bit[76]&~bit[74]&~bit[73])))  | (~control & out[2]);
		out[1] = (control & (((bit[77] | bit[78]&bit[75])&bit[76]&bit[73]) | ((~bit[78] | ~bit[73])&bit[77]&~bit[75]&bit[74]) | ((bit[76]&~bit[73] | ~bit[77]&~bit[76]&bit[73])&~bit[75]&~bit[74]) | ((bit[78] | ~bit[77])&bit[79]&bit[75]&bit[74]&bit[73]) | (~bit[76]&bit[74]&~bit[73])))  | (~control & out[1]);
		out[0] = (control & ((bit[77]&bit[76]&(~bit[78]&bit[73] | bit[74]&~bit[73])) | (~bit[77]&~bit[76]&bit[74]&(bit[75]&~bit[73] | bit[78]&bit[73])) | ((~bit[79]&bit[78] | bit[79]&bit[77])&bit[76]&bit[73]) | ((bit[80]&bit[77] | ~bit[79]&~bit[77])&bit[75]&bit[74]&bit[73]) | (bit[76]&bit[75]&~bit[74]) | (~bit[75]&~bit[74]&bit[73])))  | (~control & out[0]);
		control=control&bit[73]&bit[74]&bit[75]&bit[76];
		//-------------18--------------

		out[4] = (control & ((bit[80]&bit[78]&(bit[79] | bit[83]&bit[81]&bit[77])) | (~bit[82]&bit[79]&bit[77]&(bit[81]&~bit[80] | ~bit[83]&bit[78]))))  | (~control & out[4]);
		out[3] = (control & (((bit[80] | bit[78])&~bit[79]&~bit[77]) | (bit[82]&bit[79]&bit[77]&(bit[81] | bit[78])) | (bit[80]&bit[79]&bit[77])))  | (~control & out[3]);
		out[2] = (control & (((~bit[81] | bit[79])&~bit[80]&~bit[77]) | (bit[81]&bit[77]&(~(bit[80]^bit[79]))) | ((bit[84]&bit[81]&(bit[83] | bit[82]) | bit[83]&~bit[82]&~bit[81])&bit[79]&bit[78]&bit[77]) | (~bit[79]&~bit[78]&~bit[77])))  | (~control & out[2]);
		out[1] = (control & ((bit[80]&bit[78]&(~bit[81] | ~bit[77])) | (bit[81]&bit[80]&bit[77]&(~bit[78] | bit[82])) | (~bit[83]&bit[79]&(~bit[84]&bit[81] | ~bit[82]&bit[78])) | (bit[79]&~bit[77]) | (~bit[80]&bit[79]&~bit[78]) | (bit[83]&bit[82]&bit[81]&bit[79])))  | (~control & out[1]);
		out[0] = (control & ((bit[82]&bit[77]&(bit[80] | ~bit[79]&bit[78])) | (~bit[80]&bit[79]&~bit[78]&(bit[81] | ~bit[77])) | ((bit[83]&bit[77] | bit[84]&bit[81])&bit[79]&bit[78]) | (bit[80]&~bit[79]&~bit[78]) | (bit[81]&bit[78]&~bit[77])))  | (~control & out[0]);
		control=control&bit[77]&bit[78]&bit[79]&bit[80];
		//-------------19--------------

		out[4] = (control & (((bit[86]&~bit[85] | ~bit[86]&bit[83])&bit[82]&bit[81]) | ((bit[84]^bit[83])&bit[82]&~bit[81])))  | (~control & out[4]);
		out[3] = (control & (((bit[85] | ~bit[83])&~bit[84]&~bit[82]&bit[81]) | ((bit[86]&bit[85] | ~bit[86]&~bit[83])&bit[82]&bit[81]) | (bit[84]&bit[83]&bit[82])))  | (~control & out[3]);
		out[2] = (control & (((~bit[84] | ~bit[87]&bit[81])&bit[83]&~bit[82]) | (bit[85]&bit[81]&(~bit[83] | ~bit[86]&(~bit[88] | ~bit[82]))) | (bit[84]&~bit[81])))  | (~control & out[2]);
		out[1] = (control & (((~bit[87]&bit[86]&bit[82] | ~bit[85])&bit[84]&bit[81]) | ((bit[88]&bit[86] | bit[87] | ~bit[81])&bit[85]&bit[83]&bit[82]) | ((bit[87]&bit[83] | bit[85]&~bit[84]&~bit[83])&~bit[86]&bit[82]&bit[81]) | (bit[84]&bit[83]&~bit[81])))  | (~control & out[1]);
		out[0] = (control & (((~bit[86]&~bit[85] | bit[85]&bit[84])&bit[82]&bit[81]) | ((bit[87] | ~bit[82])&bit[86]&bit[85]&bit[84]&bit[81]) | ((bit[88]&bit[87] | ~bit[87]&bit[85]&(~bit[88] | bit[86]))&bit[83]&bit[82]&bit[81]) | (~bit[83]&~bit[82]) | (~bit[84]&~bit[83]&bit[81])))  | (~control & out[0]);
		control=control&bit[81]&bit[82]&bit[83]&bit[84];
		//-------------20--------------

		out[4] = (control & (((~bit[92] | ~bit[91]&~bit[90])&bit[89]&bit[87]&bit[85]) | ((bit[90]&bit[88] | bit[89]&~bit[86])&bit[87]&bit[85]) | ((~bit[87] | ~bit[90]&bit[89])&bit[88]&bit[86]&~bit[85])))  | (~control & out[4]);
		out[3] = (control & (((~bit[85] | bit[92]&bit[91]&bit[89])&bit[87]&bit[86]) | ((~bit[88] | ~bit[90])&~bit[89]&bit[87]&~bit[86]&bit[85]) | (bit[90]&~bit[87]&bit[86]&bit[85])))  | (~control & out[3]);
		out[2] = (control & (((~bit[86] | bit[89])&bit[87]&~bit[85]) | (~bit[90]&bit[85]&(bit[88]&bit[87] | bit[89]&~bit[86])) | ((bit[91] | bit[90])&bit[89]&~bit[87]&bit[85]) | ((~bit[91]&bit[90] | bit[91]&(~bit[89] | bit[92]&~bit[90]))&bit[87]&bit[86]&bit[85]) | (~bit[88]&~bit[87]&bit[86])))  | (~control & out[2]);
		out[1] = (control & ((bit[88]&~bit[86]) | (~bit[87]&bit[86]&~bit[85]) | (bit[91]&bit[90]&~bit[89]&bit[88]) | (bit[88]&~bit[85]) | (((bit[91] | bit[92]&bit[89])&bit[87] | ~bit[90]&~bit[88])&bit[86]&bit[85])))  | (~control & out[1]);
		out[0] = (control & ((~bit[88]&~bit[85]&(bit[87] | bit[86])) | (~bit[89]&bit[86]&(~bit[85] | bit[90]&~bit[88])) | ((bit[90]&bit[87] | bit[91]&bit[88]&bit[86])&bit[89]&bit[85]) | ((~bit[89]&~bit[86] | ~bit[90]&bit[89]&bit[86])&~bit[87]&bit[85])))  | (~control & out[0]);
		control=control&bit[85]&bit[86]&bit[87]&bit[88];
		//-------------21--------------

		out[4] = (control & (((bit[93] | bit[91])&~bit[92]&~bit[90]&bit[89]) | (~bit[93]&bit[91]&(bit[90]&~bit[89] | ~bit[94]&~bit[92]&bit[89])) | ((~bit[94]&bit[92]&(~bit[95] | bit[93]) | ~bit[96]&bit[91]&(bit[95] | bit[93]))&bit[90]&bit[89])))  | (~control & out[4]);
		out[3] = (control & ((bit[92]&bit[89]&(bit[91] |~bit[91]&bit[90])) | ((bit[96]&bit[93]&bit[91]&(bit[95] | bit[94]) | (bit[94] | bit[95]&~bit[93])&bit[92])&bit[90]&bit[89])))  | (~control & out[3]);
		out[2] = (control & (((bit[92] | ~bit[89])&~bit[91]&~bit[90]) | ((bit[94]&~bit[93] | bit[96]&~bit[94]&bit[90])&bit[91]&bit[89]) | (bit[95]&bit[90]&bit[89]&(bit[91] | bit[94]&bit[92]&bit[89])) | ((~bit[89] | ~bit[95]&~bit[94])&bit[93]&bit[92]&bit[90]) | (~bit[93]&~bit[92]&bit[90])))  | (~control & out[2]);
		out[1] = (control & ((bit[94]&~bit[92]&(bit[91] | bit[90])) | (bit[93]&bit[89]&(~bit[91] | ~bit[90])) | (~bit[92]&~bit[89])))  | (~control & out[1]);
		out[0] = (control & (((bit[94] | ~bit[93])&~bit[92]&~bit[91]&bit[89]) | ((bit[94] | ~bit[91])&bit[93]&~bit[90]&bit[89]) | ((~bit[89] | bit[96] | bit[95])&bit[93]&bit[91]&bit[90]) | ((bit[95]&bit[92] | ~bit[95]&~bit[94]&bit[91])&bit[90]&bit[89]) | (~bit[93]&bit[92]&bit[91]) | (bit[92]&~bit[90]&~bit[89])))  | (~control & out[0]);
		control=control&bit[89]&bit[90]&bit[91]&bit[92];
		//-------------22--------------

		out[4] = (control & ((~bit[98]&bit[95]&bit[93]&(bit[96] | bit[94])) | (((bit[100] | ~bit[99])&bit[97]&bit[95] | bit[98]&bit[96])&bit[94]&bit[93]) | (~bit[95]&bit[94]&~bit[93])))  | (~control & out[4]);
		out[3] = (control & ((bit[96]&bit[95]&(bit[98] | ~bit[93])) | ((bit[99]&bit[96] | bit[97]&~bit[96]&~bit[95])&~bit[98]&bit[94]&bit[93]) | ((~bit[96]&~bit[94] | ~bit[100]&bit[99]&bit[98])&bit[97]&bit[95]&bit[93]) | (bit[95]&bit[94]&~bit[93])))  | (~control & out[3]);
		out[2] = (control & (((bit[96] | ~bit[97]&bit[93])&~bit[95]&~bit[94]) | ((bit[95] | bit[96])&bit[94]&~bit[93]) | ((bit[100]&bit[97]&bit[95] | bit[96])&bit[99]&bit[94]) | (bit[98]&bit[94]&bit[93])))  | (~control & out[2]);
		out[1] = (control & ((bit[93]&(bit[97]^bit[96])) | ((~bit[99]&bit[95] | ~bit[97]&bit[96])&bit[94]&bit[93]) | (bit[96]&bit[95]&(bit[98] | ~bit[93])) | (~bit[96]&~bit[94]&~bit[93])))  | (~control & out[1]);
		out[0] = (control & (((~bit[100] | ~bit[93])&bit[97]&bit[95]) | (bit[97]&(bit[94]^bit[93])) | ((~bit[97] | bit[98])&bit[99]&bit[95]&bit[94]&bit[93]) | ((~bit[98]&bit[95] | ~bit[99]&(bit[98]&bit[94] | bit[97]))&bit[96]&bit[93]) | (~bit[96]&bit[95]&~bit[94]&~bit[93])))  | (~control & out[0]);
		control=control&bit[93]&bit[94]&bit[95]&bit[96];
		//-------------23--------------

		out[4] = (control & (((~bit[102]&bit[99] | ~bit[103]&bit[102]&bit[100])&bit[101]&bit[97]) | ((bit[101]&bit[97] | bit[99]&~bit[97])&bit[100]&~bit[98]) | ((bit[102] | ~bit[99])&~bit[101]&~bit[100]&~bit[98]&bit[97])))  | (~control & out[4]);
		out[3] = (control & (((~bit[100] | bit[98])&bit[99]&~bit[97]) | ((bit[102]&~bit[101]&bit[100] | bit[101]&~bit[100]&~bit[99])&~bit[98]&bit[97]) | ((bit[103]&bit[100]&bit[98]&bit[97] | ~bit[100]&bit[99])&bit[102]&bit[101]) | ((bit[103]&bit[98] | ~bit[102]&bit[97])&~bit[101]&bit[99])))  | (~control & out[3]);
		out[2] = (control & (((~bit[101] | bit[103]&bit[98])&~bit[102]&bit[100]) | ((bit[102] | ~bit[100]&~bit[99])&bit[101]&~bit[98]&bit[97]) | (((~bit[103] | ~bit[104]&bit[101])&bit[99] | bit[102]&~bit[100]&~bit[99])&bit[98]&bit[97]) | (bit[100]&bit[99]) | (bit[100]&~bit[97])))  | (~control & out[2]);
		out[1] = (control & (((~bit[101] | ~bit[100]&~bit[97])&bit[99]&~bit[98]) | ((bit[99] | ~bit[102]&~bit[98])&bit[100]&bit[97]) | ((bit[101]&~bit[100] | bit[100]&bit[98])&~bit[99]&bit[97]) | ((bit[102]&(~bit[101]&bit[100] | bit[104]&bit[103]&bit[101]&bit[99]) | (~bit[104] | ~bit[102])&~bit[103]&bit[99])&bit[98]&bit[97])))  | (~control & out[1]);
		out[0] = (control & ((bit[101]&~bit[97]&(bit[98] | bit[100]&bit[99])) | (~bit[102]&~bit[98]&bit[97]&(bit[100] | ~bit[101]&bit[99])) | (bit[104]&bit[99]&bit[98]&(~bit[103]&bit[101] | bit[102]&bit[97])) | ((bit[103] | bit[102] | ~bit[101])&bit[100]&bit[98]&bit[97])))  | (~control & out[0]);
		control=control&bit[97]&bit[98]&bit[99]&bit[100];
		//-------------24--------------

		out[4] = (control & (((~bit[103] | bit[106]&bit[105]&bit[104])&~bit[102]&bit[101]) | ((bit[104] | (bit[105]^bit[102]))&bit[103]&~bit[101]) | (~bit[105]&~bit[104]&bit[101]&(~bit[103] | ~bit[106]&bit[102])) | (bit[108]&~bit[107]&bit[102]&bit[101]&(~bit[106]&bit[103] | ~bit[105]&~bit[104]))))  | (~control & out[4]);
		out[3] = (control & (((~bit[107]&bit[105] | (bit[105] | bit[104])&~bit[101])&~bit[103]&bit[102]) | ((~bit[106]&(bit[104] | bit[103]) | bit[104]&bit[103])&bit[105]&~bit[102]&bit[101]) | ((~bit[108]&bit[103]&(~bit[107] | bit[105]) | ~(bit[107]^bit[106])&bit[104] | bit[107]&bit[106]&~bit[105]&bit[103])&bit[102]&bit[101])))  | (~control & out[3]);
		out[2] = (control & ((bit[104]&~bit[103]&(bit[106]&bit[101] | bit[105]&bit[102]&~bit[101])) | ((~bit[107]&bit[106] | bit[107]&bit[104])&bit[102]&bit[101]) | ((~bit[105] | ~bit[104])&bit[103]&bit[102]&~bit[101]) | ((bit[106]&~bit[105] | bit[107]&(bit[108]&bit[102] | bit[106]&~bit[102]))&bit[103]&bit[101]) | (bit[105]&~bit[104]&bit[103]&~bit[102])))  | (~control & out[2]);
		out[1] = (control & (((~bit[105] | bit[104])&bit[102]&~bit[101]) | ((bit[104]&bit[101] | bit[107]&~bit[106]&bit[102])&~bit[105]&~bit[103]) | ((bit[106]&(bit[105] | ~bit[108]&bit[107]&bit[102]) | ~bit[104]&(~bit[102] | bit[108]&~bit[107]&~bit[105]))&bit[103]&bit[101])))  | (~control & out[1]);
		out[0] = (control & ((bit[104]&~bit[101]&(~bit[103] | bit[102])) | (bit[107]&bit[101]&(bit[106]&bit[105] | bit[103]&bit[102])) | ((~bit[106]&~bit[103] | bit[108]&bit[106]&bit[103])&~bit[105]&bit[102]&bit[101]) | (bit[106]&~bit[102]&(bit[105]&~bit[104]&bit[101] | bit[104]&~bit[103]))))  | (~control & out[0]);
		control=control&bit[101]&bit[102]&bit[103]&bit[104];
		//-------------25--------------

		out[4] = (control & ((bit[110]&bit[109]&(~bit[108]&bit[106] | bit[107]&bit[105])) | (bit[109]&bit[106]&(~bit[108]&~bit[105] | bit[111]&bit[108]&bit[105])) | ((~bit[110]&bit[108] | ~bit[111]&bit[106])&~bit[109]&bit[107]&bit[105]) | (~bit[107]&bit[106]&~bit[105])))  | (~control & out[4]);
		out[3] = (control & (((~bit[110]&bit[105] | bit[109]&~bit[105])&bit[108]&bit[107]) | ((~bit[109] | ~bit[106])&~bit[108]&~bit[107]&bit[105]) | ((~bit[109] | ~bit[108])&~bit[110]&bit[106]&bit[105]) | (bit[107]&bit[106]&~bit[105])))  | (~control & out[3]);
		out[2] = (control & ((~bit[109]&~bit[107]&(bit[106] | bit[105])) | (bit[107]&~bit[106]&(bit[109] | ~bit[105])) | ((bit[111]&(bit[110] | bit[108]) | bit[110]&~bit[108]&~bit[107])&bit[106]&bit[105]) | ((bit[108] | (~bit[112]&~bit[111] | bit[112]&~bit[110])&bit[105])&bit[109]&bit[107])))  | (~control & out[2]);
		out[1] = (control & (((~bit[106] | bit[110]&~bit[109])&~bit[107]&bit[105]) | (((~bit[111] | bit[112]&bit[110])&bit[109]&bit[107] | bit[110]&(bit[108] | ~bit[111]&bit[107]))&bit[106]&bit[105]) | (~bit[108]&~bit[107]&~bit[105]) | (bit[109]&bit[108]&bit[107]&~bit[106])))  | (~control & out[1]);
		out[0] = (control & ((~bit[108]&~bit[107]&(bit[106] | ~bit[109]&bit[105])) | ((bit[110] | ~bit[105])&~bit[109]&bit[108]&bit[106]) | ((bit[111]&bit[110] | (bit[111] | bit[112]&bit[110]&bit[109])&bit[107])&bit[106]&bit[105]) | (bit[108]&~bit[107]&~bit[106]&~bit[105])))  | (~control & out[0]);
		control=control&bit[105]&bit[106]&bit[107]&bit[108];
		//-------------26--------------

		out[4] = (control & ((bit[113]&bit[112]&bit[110]) | (bit[113]&bit[111]&~bit[110]&bit[109]) | ((~bit[112]&~bit[111] | ~bit[114]&bit[111])&~bit[113]&bit[110]&bit[109])))  | (~control & out[4]);
		out[3] = (control & ((~bit[113]&bit[112]&bit[110]) | (bit[114]&bit[113]&~bit[112]&bit[110]) | ((~bit[112] | ~bit[111])&bit[110]&~bit[109]) | ((bit[113]&bit[111] | ~bit[114]&bit[110])&bit[112]&bit[109])))  | (~control & out[3]);
		out[2] = (control & ((~bit[113]&bit[112]&bit[111]) | (bit[112]&~bit[110]&~bit[109]) | (bit[114]&bit[112]&bit[110]&bit[109]) | (bit[113]&~bit[112]&bit[111]&bit[109]) | ((bit[114] | ~bit[110])&~bit[113]&bit[109])))  | (~control & out[2]);
		out[1] = (control & ((~bit[111]&~bit[110]&(~bit[112] | bit[109])) | ((~bit[109] | ~bit[114]&bit[113])&bit[111]&bit[110]) | (bit[115]&bit[110]&bit[109]&(~bit[114]&~bit[113] | bit[114]&bit[112])) | (~bit[113]&~bit[111]&bit[109])))  | (~control & out[1]);
		out[0] = (control & (((bit[114] | ~bit[113])&bit[112]&bit[111]) | ((bit[113] | ~bit[110])&bit[111]&~bit[109]) | (bit[115]&bit[110]&bit[109]&(bit[113] | bit[114]&bit[111])) | (~bit[112]&~bit[111]&bit[109])))  | (~control & out[0]);
		control=control&bit[109]&bit[110]&bit[111]&bit[112];
		//-------------27--------------

		out[4] = (control & ((bit[116]&~bit[114]&bit[113]) | ((~bit[116] | ~bit[114])&bit[115]&~bit[113])))  | (~control & out[4]);
		out[3] = (control & ((bit[116]&bit[115]&bit[114]&~bit[113]) | ((bit[116] | bit[114])&~bit[115]&bit[113])))  | (~control & out[3]);
		out[2] = (control & ((bit[116]&bit[115]) | (~bit[116]&bit[113])))  | (~control & out[2]);
		out[1] = (control & ((bit[114]) | (bit[116]&bit[115])))  | (~control & out[1]);
		out[0] = (control & ((bit[116]&bit[114]) | (bit[116]&bit[113]) | (bit[115]&~bit[114]&bit[113])))  | (~control & out[0]);
		//-------------28--------------

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
	}

	printf("%lu\n",clock3/repeat);
	printf("%lu\n",clock4/repeat);

	return 1;
}








