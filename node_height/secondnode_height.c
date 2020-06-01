
//gcc secondnode_height.c -o height -lmpfr -lgmp

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <gmp.h>
#include <mpfr.h>
#include <float.h>

#define SIGMA 3.33
#define PRECISION 64
#define TAIL 30

int hamming[PRECISION];
int table[TAIL+1][PRECISION];
int node_internal[PRECISION];


void create_binary(mpfr_t a, int row){

	unsigned int i = 0;
	mpfr_t v;
	mpfr_init2(v, PRECISION);
	mpfr_set(v,a,MPFR_RNDD);
	int j = 0;
	for (i = 0; i < PRECISION+j; i++){
		mpfr_mul_ui(v,v,2,MPFR_RNDD);
		if (mpfr_cmp_ui(v, 1) >= 0){
			if (i>=j)
				table[row][i-j] = 1;
			mpfr_sub_ui(v,v,1,MPFR_RNDD);
		}
		else{
			if (i >= j)
				table[row][i-j] = 0;
		}
	}
	mpfr_clear(v);
}


void create_table()
{
	unsigned long int i,j;
	int k;
	mpfr_t s, t, u, e,pi,sigma_large;
	int precision = 250;
	double sigma = SIGMA*1;

	mpfr_init2(s, precision);
	mpfr_init2(t, precision);
	mpfr_init2(u, precision);
	mpfr_init2(e, precision);
	mpfr_init2(pi, precision);

	mpfr_init2(sigma_large, precision);
	mpfr_set_d(sigma_large,sigma,MPFR_RNDD);
	
	mpfr_const_pi(pi, MPFR_RNDD);
	mpfr_mul_ui(pi, pi,2,MPFR_RNDD);
	mpfr_sqrt(pi, pi, MPFR_RNDD);
	
	mpfr_mul(pi,pi,sigma_large,MPFR_RNDD); 	
	mpfr_set(s,sigma_large,MPFR_RNDD); 

	mpfr_sqr(s,s,MPFR_RNDD);		
	mpfr_mul_ui(s, s, 2,MPFR_RNDD);	
	mpfr_neg(s, s, MPFR_RNDD);	
	mpfr_ui_div(s,1,s,MPFR_RNDD);	

	for (i = 0; i <= TAIL; i++){
		j = i*i;
		mpfr_mul_ui(e, s,j, MPFR_RNDD);
		mpfr_exp(e, e, MPFR_RNDD);
		mpfr_div(e,e,pi,MPFR_RNDD);
		if(i!=0){
			mpfr_mul_ui(e, e, 2,MPFR_RNDD);
		}
		create_binary(e,i);
	}		
	
	for (j = 0; j < PRECISION; j++)
		hamming[j] = 0;
	
	for (i = 0; i<TAIL+1 ; i++){
		for (j = 0; j < PRECISION; j++){
			if (table[i][j] == 1)
				hamming[j]++;
		}
	}

	mpfr_clear(s);
	mpfr_clear(t);
	mpfr_clear(u);
	mpfr_clear(e);
	mpfr_clear(pi);
	mpfr_clear(sigma_large);
	return;
}

void main(){

	unsigned long int i,j;
	create_table();

	int sec_height[PRECISION-4];
	int sub_value;


	FILE *fp = NULL;
    fp = fopen("C:/Users/sony/Desktop/node_height/probtable_matrix.txt", "w+");
    
	for(i=0;i<=TAIL;i++) {
		for(j=0;j<PRECISION;j++) {
			fprintf(fp, "%1d ", table[i][j]);
		}
		fprintf(fp, "\n");
	}
    
	fclose(fp);


	FILE *fp1 = NULL;
    fp1 = fopen("C:/Users/sony/Desktop/node_height/hamming.txt", "w+");

    for(j=0;j<PRECISION;j++) {
		fprintf(fp1, "%2d ", j);
	}

    fprintf(fp1, "\n");

    for(j=0;j<PRECISION;j++) {
		fprintf(fp1, "%2d ", hamming[j]);
	}

    fprintf(fp1, "\n");
    
	node_internal[0] = 2-hamming[0];
	for(i=1;i<PRECISION;i++) {
		node_internal[i] = 2*node_internal[i-1]-hamming[i];
	}

	for(j=0;j<PRECISION;j++) {
		fprintf(fp1, "%2d ", node_internal[j]);
	}
    fprintf(fp1, "\n");
    
	fclose(fp1);
    
	/*
	int t;
	int sub_value;
    int num = PRECISION/3 -2;
	int num_subtree_satis[num];
	for(i=2;i+5<=PRECISION;i=i+3) {
		sub_value = 32*size_internal[i]-16*size_ham[1][i+1]-8*size_ham[1][i+2]-4*size_ham[1][i+3]-2*size_ham[1][i+4]-32;
		t = (i+1)/3 -1;
		if(sub_value<=size_ham[1][i+5])
	        num_subtree_satis[t] = 1;
		else
		{
			num_subtree_satis[t] = 0;
		}
		
	}
	*/

	for(i=0;i+4<PRECISION;i++)
		sec_height[i] = 0;
	for(i=0;i+4<PRECISION;i++){
		sub_value = 4*node_internal[i]-2*hamming[i+1]-4;
		if(sub_value<=hamming[i+2]){
			sec_height[i] = 2;
		}
		else {
			sub_value = 2*(sub_value-hamming[i+2]);
			if(sub_value<=hamming[i+3]){
				sec_height[i] = 3;
			}
			else {
				sub_value = 2*(sub_value-hamming[i+3]);
				if(sub_value<=hamming[i+4]){
					sec_height[i] = 4;
				}
				else {
					sub_value = 2*(sub_value-hamming[i+4]);
					if(sub_value<=hamming[i+5]){
						sec_height[i] = 5;
					}
					else {
						sec_height[i] = 6;
					}
				}
			}
		}
	}

/*
    int t=-1;
	int sub_value;
    //int num = PRECISION/3 -2;
	int num_subtree_satis[60];
	for(i=0;i<60;i++) {
		num_subtree_satis[i] = 0;
	}
	for(i=2;i+4<=PRECISION;i=i+3) {
		t = t + 1;
		sub_value = 16*size_internal[i]-8*hamming[i+1]-4*hamming[i+2]-2*hamming[i+3]-16;
		if(sub_value<=hamming[i+4]) {
			num_subtree_satis[t] = 1;
		}
		else
		    num_subtree_satis[t] = 0;
	}
*/
	
    FILE *fp2 = NULL;
    fp2 = fopen("C:/Users/sony/Desktop/node_height/sec_height.txt", "w+");
    
	for(j=0;j+4<PRECISION;j++) 
		fprintf(fp1, "%2d ", j);
	fprintf(fp2, "\n");

	for(j=0;j+4<PRECISION;j++) 
		fprintf(fp2, "%2d ", sec_height[j]);
    
	fclose(fp2);

}
