/* This code combines results files together.

James Etherington                                              */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_histogram.h>
#include <string>

// Constants

const long int nRows_max = 100000;
const int nChunks = 159;

struct coldata {
	int id;
	int meterId;
	double mean_signal;
	double rms_amplitude;
};

int main (int argc, char **argv) {
        
    printf("\n");
    printf("******************************\n");
    printf("Start...\n");
    printf("******************************");
    printf("\n");
    printf("\n");
    
    char * inPath; 
    char * outPath;  
           
	printf("HERE 1. \n");
    inPath = argv[1];
    outPath = argv[2];
   
    char INfile[1000];
    char OUTfile[1000];
                
    FILE * input_file = NULL;
    FILE * output_file = NULL;     
    
    printf("HERE 2. \n");
    
    struct coldata * cdata;
    cdata = (struct coldata*)malloc(nRows_max*sizeof(struct coldata));
    
    printf("HERE 3. \n");
       
    struct coldata * cdata1;     
    cdata1 = cdata;           
     
	int id;
	int meterId;
	double mean_signal;
	double rms_amplitude;
	
	
	long int nFound = 0; 
	 
    for (int i=0; i<nChunks; i++) {
        
        sprintf(INfile,"%s_%i.dat", inPath, i);
        printf("INfile: %s \n", INfile);
        input_file = fopen(INfile,"r"); 
    
    	while (fscanf(input_file,"%i  %i  %lf  %lf  \n",&id,&meterId,&mean_signal,&rms_amplitude) != EOF) {
    
  		  	cdata1->id = id;
			cdata1->meterId = meterId;
			cdata1->mean_signal = mean_signal;
			cdata1->rms_amplitude = rms_amplitude;
			
			nFound++;
			cdata1++;
    	}
    	fclose(input_file);
	}
   
    sprintf(OUTfile,outPath);
    output_file = fopen(OUTfile,"w");
    
    struct coldata * cdata2;     
    cdata2 = cdata;           
       
    for (long int i=0; i<nFound; i++) {
    
		fprintf(output_file, "%i  %i  %lf  %lf  \n", cdata2[i].id, cdata2[i].meterId, cdata2[i].mean_signal, cdata2[i].rms_amplitude);
    } 
      
    fclose(output_file);

  	free(cdata);
    
    return 0;
}
