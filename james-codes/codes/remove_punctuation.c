/* This code removes the punctuation from the text files.

James Etherington                                              */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_histogram.h>
#include <string>
#include <fstream>
#include <iostream> 
#include <algorithm>

using namespace std;

const long int nRows_max = 1000000;

struct coldata {
	long int id;
	char line[2000];
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
    int fileIdx; 
             
    inPath = argv[1];
    outPath = argv[2];
    fileIdx = atoi(argv[3]);
   
    char INfile[1000];
    char OUTfile[1000];
    
    char pathToFile[2000];   
                
    FILE * input_file = NULL;
    FILE * output_file = NULL;     
    
    printf("Here.. 1 \n");
    
    struct coldata * cdata;
    cdata = (struct coldata*)malloc(nRows_max*sizeof(struct coldata));
    
    printf("Here.. 2 \n");
    
    struct coldata * cdata1;     
    cdata1 = cdata;           
    
    struct coldata * cdata2;     
    cdata2 = cdata;           
    		 
	printf("Here.. 3 \n");
   
	char filenum[5];
	
	if ((fileIdx >= 0) && (fileIdx < 10)) {
		sprintf(filenum, "%s%i", "0000", fileIdx);
	} else if ((fileIdx >= 10) && (fileIdx < 100)) {
		sprintf(filenum, "%s%i", "000", fileIdx);
	} else if ((fileIdx >= 100) && (fileIdx < 1000)) {
		sprintf(filenum, "%s%i", "00", fileIdx);
	} else if ((fileIdx >= 1000) && (fileIdx < 10000)) {
		sprintf(filenum, "%s%i", "0", fileIdx);
	} else if (fileIdx >= 10000) {
		sprintf(filenum, "%i", fileIdx);
	}
	
    printf("Here.. 4 \n");
   
	sprintf(pathToFile, "%s-%s", inPath, filenum);
	sprintf(INfile, pathToFile);
	
	printf(INfile);
    input_file = fopen(INfile,"r"); 
    printf("\nHere.. 5 \n");
 
 	long int idx = 0;
	char rline[2000];
  
 	while (fgets(rline, 2000, input_file)) {
    	
		string tmpStr = string(rline);
		
		replace( tmpStr.begin(), tmpStr.end(), ',', ' ');
		replace( tmpStr.begin(), tmpStr.end(), '\'', ' ');
		replace( tmpStr.begin(), tmpStr.end(), '(', ' ');
		replace( tmpStr.begin(), tmpStr.end(), ')', ' ');
		
		cdata1[idx].id = idx;    
	    sprintf(cdata1[idx].line,tmpStr.c_str());

	    printf("%i  %s \n", idx, tmpStr.c_str());	
		idx++;
	} 
	fclose(input_file);

    printf("Here.. 7 \n");
    
    char output_line[4000];
    char tmpline[4000];
     
    sprintf(OUTfile, "%s%i.dat", outPath, fileIdx);
    output_file = fopen(OUTfile,"w");  
       
    for (long int i=0; i<idx; i++) {
   
    	fprintf(output_file, "%s", cdata2[i].line);    
    	printf("out: %li / %li \n", i, idx);
    } 
      
    fclose(output_file);

  	free(cdata);
    
    return 0;
}
