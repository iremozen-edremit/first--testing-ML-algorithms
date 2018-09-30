/* This code checks the SSE meter files.

James Etherington                                              */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <vector>

const int nFiles = 59387; 

int main (int argc, char **argv) {
    
    printf("\n");
    printf("******************************\n");
    printf("Start...\n");
    printf("******************************");
    printf("\n");
    printf("\n");
    
    char * inPath; 
    char * outPath;  
           
    inPath = argv[1];
    outPath = argv[2];
    
    char INfile[1000];
    char OUTfile[1000];
    
    FILE * file_out;
	
	sprintf(OUTfile, outPath);  
			
	char rdate[20];
	char rtime[20];
 	char meterId[50];
   	char customerId[50];
   	double reading;
   	double lead;
   	double lag;
  	
  	long int count;
  	  	
	for (int fileIdx=0; fileIdx<nFiles; fileIdx++) {

		FILE * file_in = NULL;
		
		sprintf(INfile, "%s_%i.dat", inPath, fileIdx);  
		file_in = fopen (INfile, "r");
		
		if (file_in == NULL) {
			//printf("missing: %i \n", fileIdx);
			
		} else {	
		
			count = 0;
    		while (fscanf(file_in,"%s %s %s %s %lf %lf %lf \n",&rdate,&rtime,&meterId,&customerId,&reading,&lead,&lag) != EOF) {
    		
  				count++;
			}
			file_out = fopen(OUTfile, "a");
			fprintf(file_out,"%i: %li \n", fileIdx, count);
  			fclose(file_out);
  			fclose (file_in);
		}
		
	}
  	
    return 0;
}
