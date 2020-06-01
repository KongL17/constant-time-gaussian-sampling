#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <gmp.h>
#include <mpfr.h>
#include <float.h>

//gcc gen_tree.c -o tree -Wall -lmpfr -lgmp

#define TAIL 31 
#define SIGMA 3.33
#define PRECISION 64


int size_ham[2][PRECISION];//0-> size, 1->hamming weight
int table[TAIL][PRECISION]; 

void create_binary(mpfr_t a, int prec,int row){

	unsigned int i = 0;
	mpfr_t v;
	mpfr_init2(v, PRECISION);
	mpfr_set(v,a,MPFR_RNDD);
	int j = 0;
	for (i = 0; i < prec+j; i++){
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

//calculater the total samples, bits per sample and overflow beforehand
void sampling_for_espresso(){
	
	long int i, j, k,l,m;
	long int row, sample;
	unsigned long int sample_counter=0;	

	int total_num_sample=668; //sum of hamming weights of all columns
	int bits_each_sample=5;

	int *sample_bit[total_num_sample], *sample_array[total_num_sample];
	int *sorting_done;
	for(i=0;i<total_num_sample;i++){
		sample_bit[i]= (int *)malloc(bits_each_sample*sizeof(int));
		sample_array[i]= (int *)malloc(PRECISION*sizeof(int));		
	}
	sorting_done= (int *)malloc(total_num_sample*sizeof(int));
	//-------------------------------	

	for(i=0;i<total_num_sample;i++)
		sorting_done[i]=-1;

	int OVERFLOW=2*31;

	unsigned int **arr,*d_arr;
	int *valid_invalid;

	arr= (unsigned int **)malloc(sizeof(unsigned int *) * OVERFLOW);
	arr[0]=(unsigned int *)malloc(OVERFLOW*PRECISION*sizeof(unsigned int));

	for(i=0;i<OVERFLOW;i++)
		arr[i]= (*arr+PRECISION*i);

	d_arr= (unsigned int *)malloc(OVERFLOW*sizeof(unsigned int));
	valid_invalid= (int *)malloc(OVERFLOW*sizeof(int));
	//----------------

	for(j=0;j<OVERFLOW;j++)
		d_arr[j]=0;		
		
	for(j=2;j<OVERFLOW;j++){//make all the entries invalid initially
		valid_invalid[j]=-1;	
	}
	valid_invalid[0]=1;//make the first two entries valid and appropriate bits
	valid_invalid[1]=1;
	arr[0][0]=0;arr[1][0]=1;
	d_arr[0]=0;d_arr[1]=1;

//-----------------for espresso----------------
    FILE *fp = NULL;
    fp = fopen("C:/Users/sony/Desktop/test/DDG_data.txt", "w+");

	fprintf(fp, ".i %d\n",PRECISION);
	fprintf(fp, ".o %d\n",bits_each_sample);
	fprintf(fp, ".ilb ");
	for(i=PRECISION-1;i>=0;i--){
		fprintf(fp, " bit[%ld] ",i);	
	}
	fprintf(fp, "\n.ob ");
	
	for(i=bits_each_sample-1;i>=0;i--){
		fprintf(fp, " out_t[%ld] ",i);	
	}
	fprintf(fp, "\n");


	for(i=1;i<PRECISION;i++){//scan column by column
		for(j=0;j<OVERFLOW;j++){
				
			if(valid_invalid[j]==1){//found one valid entry append 0 and 1 then update d

				arr[j][i]=0; //append 0
				d_arr[j]=d_arr[j]*2;//multiply by 2
				for(k=0;k<OVERFLOW;k++){
					if(valid_invalid[k]==-1){//found one
						valid_invalid[k]=2; //make that valid newly created
						d_arr[k]=d_arr[j]+1;//adjust d to reflect append 1
						for(l=0;l<i;l++){//to append 1
							arr[k][l]=arr[j][l];
						}
						arr[k][i]=1;
						break;
					}
				}
			}			
		}
		for(j=0;j<OVERFLOW;j++){//adjust the valid_invalid array
			if(valid_invalid[j]==2)
				valid_invalid[j]=1;
		}
		
		//logic to scan the columns for sample
		for(j=0;j<OVERFLOW;j++){
			if(valid_invalid[j]==1){
				if(d_arr[j]>=size_ham[1][i]){ //like normal sampling
					d_arr[j]=d_arr[j]-size_ham[1][i];
				}
				else{
					for (row = size_ham[0][i]; row >= 0; row--){
						d_arr[j] = d_arr[j] - table[row][i];
						if (d_arr[j] == -1){
							sample = row;
							valid_invalid[j]=-1;
							for(k=i+1;k<PRECISION;k++){
								sample_array[sample_counter][k]=-1;
							}
							for(k=i;k>=0;k--){
								sample_array[sample_counter][k]=arr[j][k];
							}
							l=1;
							for(k=bits_each_sample-1;k>=0;k--){ //*****change for bit number change
								sample_bit[sample_counter][k]=(sample&(l<<k))>>k;
							}							  
							sample_counter++;
							break;
						}
					}
				}
			}	
		}
	}	

	for(k=0;k<PRECISION;k++){ //iterate over possible list of 1 trail
		for(j=0;j<sample_counter;j++){//iterate over all the samples
			l=0;
			if(sorting_done[j]==-1){
				for(m=PRECISION-1;m>=0;m--){
					if(sample_array[j][m]==0)
						l=m-1;
				}
				if(l==k-1){
					sorting_done[j]=1;
						for(m=PRECISION-1;m>=0;m--){
							if(sample_array[j][m]==-1)
								fprintf(fp, "-");
							else
								fprintf(fp, "%d",sample_array[j][m]);
						}
						fprintf(fp, " ");
						for(m=bits_each_sample-1;m>=0;m--)
							fprintf(fp, "%d",sample_bit[j][m]);
						fprintf(fp, "\n");
				}				
			}
		}
	}

	fclose(fp);
}

void create_table()
{
	unsigned long int i,j;
	mpfr_t s, t, u, e,pi,sigma_large,prob_sum;
	int precision = 128;
	double sigma = 1*SIGMA;

	mpfr_init2(s, precision);
	mpfr_init2(t, precision);
	mpfr_init2(u, precision);
	mpfr_init2(e, precision);
	mpfr_init2(pi, precision);
	mpfr_init2(sigma_large, precision);
	mpfr_init2(prob_sum, precision);
	mpfr_set_d(sigma_large,sigma,MPFR_RNDD); 
	
	mpfr_const_pi(pi, MPFR_RNDD);
	mpfr_mul_ui(pi, pi,2,MPFR_RNDD);
	mpfr_sqrt(pi, pi, MPFR_RNDD);
	
	mpfr_mul(pi,pi,sigma_large,MPFR_RNDD); //pi<- (sigma*sqrt(2*pi))	
	mpfr_set(s,sigma_large,MPFR_RNDD); //s<-sigma

	mpfr_sqr(s,s,MPFR_RNDD);		//s<-sigma^2
	mpfr_mul_ui(s, s, 2,MPFR_RNDD);	//s<- 2*sigma^2
	mpfr_neg(s, s, MPFR_RNDD);	//s<- -2*sigma^2
	mpfr_ui_div(s,1,s,MPFR_RNDD);	//s<- -1/2*sigma^2

	for (i = 0; i < TAIL; i++){
		j = i*i;
		mpfr_mul_ui(e, s,j, MPFR_RNDD);
		mpfr_exp(e, e, MPFR_RNDD);
		mpfr_div(e,e,pi,MPFR_RNDD);
		if(i!=0){
			mpfr_mul_ui(e, e, 2,MPFR_RNDD);
		}
		create_binary(e,PRECISION,i);
	}

	for (j = 0; j < PRECISION; j++){
		size_ham[0][j] = 0;
		size_ham[1][j] = 0;
	}
	
	for (i = 0; i<TAIL; i++){
		for (j = 0; j < PRECISION; j++){
			if (table[i][j] == 1){
				size_ham[1][j]++;
				size_ham[0][j] = i;
			}
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

	create_table();
	sampling_for_espresso();
}
