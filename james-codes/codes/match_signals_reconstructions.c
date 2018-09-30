/* This code matches the signals to their reconstructions 

James Etherington                                              */


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <numeric>
#include <vector>

using namespace std;

const int nMeters = 25;
const int windowSize = 512;

void log_info(char * infoPath, char * message);

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

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
    char * infoPath;
 	
    char message[1000]; 
 
	inPath1 = argv[1]; 
	inPath2 = argv[2];
	outPath = argv[3];
	infoPath = argv[4];   

	char INfile1[1000];
	char INfile2[1000];
	char OUTfile[1000];
	
	int * meter_list; 
    meter_list = (int*)calloc(nMeters, sizeof(int));
    
    FILE * input_file1 = NULL;
    FILE * input_file2 = NULL;
    FILE * output_file = NULL;
 
    sprintf(INfile1, inPath1);
    input_file1 = fopen(INfile1,"r"); 
      
    double * orig_list; 
	orig_list = (double*)calloc(windowSize * nMeters, sizeof(double));
		  		
	sprintf(INfile1, inPath1);		
	input_file1 = fopen(INfile1,"r"); 
  	
  	char meter_line[10000];  
    string meter_sline, token1; 

  	for (int meterIndex=0; meterIndex<nMeters; meterIndex++) {
	
 		fgets (meter_line, 10000, input_file1);
		meter_sline = string(meter_line);
		stringstream meter_ss(meter_sline);
	
		string tokenTmp;
		long int tokenIdx = 0;
		
		while (getline(meter_ss,token1, ' ')) {
			tokenTmp = trim(token1);
				
			if (strcmp(tokenTmp.c_str(), "") != 0) {
				
				if (tokenIdx > 2) {
					
					orig_list[(tokenIdx - 3) + (meterIndex*windowSize)] = atof(tokenTmp.c_str());
				}

				tokenIdx++;
			}
 		}
 	}
	fclose(input_file1); 		
 
	double * recon_list; 
	recon_list = (double*)calloc(windowSize * nMeters, sizeof(double));

 	sprintf(INfile2,inPath2);		
	input_file2 = fopen(INfile2,"r"); 
  	
  	char meter_line2[10000];  
    string meter_sline2, token2; 

 	for (int meterIndex=0; meterIndex<nMeters; meterIndex++) {
	
 		fgets (meter_line2, 10000, input_file2);
		meter_sline2 = string(meter_line2);
		stringstream meter_ss2(meter_sline2);
	
		string tokenTmp2;
		long int tokenIdx2 = 0;
		
		while (getline(meter_ss2,token2, ' ')) {
			tokenTmp2 = trim(token2);
				
			if (strcmp(tokenTmp2.c_str(), "") != 0) {
				
				if (tokenIdx2 > 1) {
					
						recon_list[(tokenIdx2 - 2) + (meterIndex*windowSize)] = atof(tokenTmp2.c_str());
				}

				tokenIdx2++;
			}
 		}
 	}
	fclose(input_file2); 		
 			
	for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {

		sprintf(OUTfile, "%s_%i.dat", outPath, meterIdx);
		output_file = fopen(OUTfile,"w");
		
		for (int readingIdx=0; readingIdx<windowSize; readingIdx++) {
		
			fprintf(output_file,"%lf  %lf  \n", orig_list[meterIdx*windowSize + readingIdx], recon_list[meterIdx*windowSize + readingIdx]);
		}
	}
	fclose(output_file);
		
	printf("Finished!...\n");
	sprintf(message, "Finished...");
	log_info(infoPath, message);
 	 
	return 0;
}

void log_info(char * infoPath, char * message) {

    char INFOname[1000];
    FILE * info_file = NULL;                      
  
    time_t now = time(0);
   	char* dt = ctime(&now);
  	dt[strlen(dt) - 1] = '\0';    //remove carriage return
  
	sprintf(INFOname,infoPath);
	info_file = fopen(INFOname,"a");
	fprintf(info_file,"%s - %s \n", dt, message); 
	fclose(info_file);
}

