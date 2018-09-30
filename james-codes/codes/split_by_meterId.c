/* This code uses STILTS to match two dataset together

James Etherington                                              */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

using namespace std;

int main (int argc, char **argv) {
        
    printf("\n");
    printf("******************************\n");
    printf("Start...\n");
    printf("******************************");
    printf("\n");
    printf("\n");
       
    char * inPath1;
    char * inPath2;
    char * outPath;
    int procnum;
    
    inPath1 = argv[1];   // unique meters
    inPath2 = argv[2];   // raw#.dat
    outPath = argv[3];
    procnum = atoi(argv[4]);
    
    printf("Here 1. \n");
    
    char rdate[50];
	char rtime[50];
 	char meterId[50];
 	char meterId2[50];
   	char customerId[50];
   	double reading;
   	double lead;
   	double lag;
  
    char INfile[1000];
    char INfile2[1000];
    char OUTfile[1000];
    
    FILE * input_file = NULL;
    FILE * input_file2 = NULL;
  	FILE * output_file = NULL;
  
    std::map<std::string,long int> my_map;
    std::map<std::string,long int>::iterator it = my_map.begin();
    long int idx = 0;
    
    sprintf(INfile, inPath1);
    input_file = fopen(INfile,"r"); 
    
    printf("Here 2. \n");
    
    while (fscanf(input_file,"%s \n",&meterId) != EOF) {
    			
		std::string ssmeterId = string(meterId);
   	    my_map.insert (it, std::pair<std::string,long int>(ssmeterId,idx));  
		
		printf("%s %s %li \n", "meterId: ", meterId, idx);
		idx = idx + 1;
    }
	fclose(input_file);

	printf("%i \n", idx);

	long int filenum; 

	printf("Here 3. \n");
    
    sprintf(INfile2, "%s%i.dat", inPath2, procnum);
  
    printf(INfile2);
    printf("\n");
   
    printf("Here 3a. \n");
    
    input_file2 = fopen(INfile2,"r"); 
  
    printf("Here 3b. \n");
  
  	while (fscanf(input_file2,"%s %s %s %s %lf %lf %lf \n",&rdate,&rtime,&meterId2,&customerId,&reading,&lead,&lag) != EOF) {
  
    	//printf("Here 3c. \n");
     	
     	map<string,long int>::iterator iter;
 		
 		std::string ssmeterId2 = string(meterId2);
   	
 		iter = my_map.find(meterId2);
 		filenum = iter->second;
   		
  		sprintf(OUTfile, "%s_%i.dat", outPath, filenum);
    	output_file = fopen(OUTfile,"a");  
   
    	fprintf(output_file, "%s %s   %s   %s   %lf   %lf   %lf \n", rdate, rtime, meterId, customerId, reading, lead, lag);    
    	fclose(output_file);
	}
	fclose(input_file2);
		
	printf("Here 4. \n");
    
	printf("\n Finished!...\n");
	 
	return 0;
}


