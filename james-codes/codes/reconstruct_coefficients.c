/* This code computes reconstructs the wavelet coefficients 
from the compressed data and the feature vector. 

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

// TODO: 

// 1.) read in feature vector
// 2.) read in compressed data
// 3.) do matrix multiplication
// 4.) write out result. 


const int nVectors = 10;
const int nDimensions = 511;
const int nMeters = 4;

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

void log_info(char * infoPath, char * message);

int main (int argc, char **argv) {
        
    printf("\n");
    printf("******************************\n");
    printf("Start...\n");
    printf("******************************");
    printf("\n");
    printf("\n");
       
    char * inPath1;   // feature vector
    char * inPath2;   // compressed data
    char * outPath;   // reconstructed coefficients
    char * infoPath;
    
    char message[1000]; 
 
    inPath1 = argv[1]; 
    inPath2 = argv[2];  
    outPath = argv[3];
    infoPath = argv[4];   
        
 	sprintf(message, "starting...");
	log_info(infoPath, message);
 	   
    char INfile1[1000];
    char INfile2[1000];
    char OUTfile[1000];
    
    FILE * input_file1 = NULL;
    FILE * input_file2 = NULL;
    FILE * output_file = NULL;
     
    double * recon_coeff_list; 
	recon_coeff_list = (double*)calloc(nMeters * nDimensions, sizeof(double));	  
	     	 
	double * vector_list; 
	vector_list = (double*)calloc(nVectors * nDimensions, sizeof(double));	  

	char vec_line[10000];  
    string vec_sline, token1; 

	sprintf(message, "Here 1...");
	log_info(infoPath, message);
 	
 	sprintf(INfile1, inPath1);
		
	sprintf(message, "INfile1: %s", INfile1);
	log_info(infoPath, message);
 
	input_file1 = fopen(INfile1,"r"); 

	for (int vectorIdx=0; vectorIdx<nVectors; vectorIdx++) {
	
		fgets (vec_line, 10000, input_file1);
		vec_sline = string(vec_line);
		stringstream vec_ss(vec_sline);
	
		string tokenTmp1;
		int tokenIndex1 = 0;
		
		while (getline(vec_ss,token1, ' ')) {
			tokenTmp1 = trim(token1);
				
			if (strcmp(tokenTmp1.c_str(), "") != 0) {
				
				vector_list[(tokenIndex1) + (vectorIdx*nDimensions)] = atof(tokenTmp1.c_str());
				tokenIndex1++;
			}
		}
	}
	fclose(input_file1);
	
	sprintf(message, "Here 2...");
	log_info(infoPath, message);
 	 
	double * compressed_data_list; 
	compressed_data_list = (double*)calloc(nVectors * nMeters, sizeof(double));	  

	char meter_line[10000];  
    string meter_sline, token2; 
	
	sprintf(INfile2, inPath2);
		
	sprintf(message, "INfile2: %s", INfile2);
	log_info(infoPath, message);
 
	input_file2 = fopen(INfile2,"r"); 

	for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {
	
		fgets (meter_line, 10000, input_file2);
		meter_sline = string(meter_line);
		stringstream meter_ss(meter_sline);
	
		string tokenTmp2;
		int tokenIndex2 = 0;
		
		while (getline(meter_ss,token2, ' ')) {
			tokenTmp2 = trim(token2);
				
			if (strcmp(tokenTmp2.c_str(), "") != 0) {
				
				compressed_data_list[(tokenIndex2) + (meterIdx*nVectors)] = atof(tokenTmp2.c_str());
				tokenIndex2++;
			}
		}
	}
	fclose(input_file2);
	 
	sprintf(message, "Here 3...");
	log_info(infoPath, message);
 	
	
	sprintf(message, "Here 3a...");
	log_info(infoPath, message);
 	
	double elemTmp = 0;
	// do matrix multiplication
	
	for (int dimIdx=0; dimIdx<nDimensions; dimIdx++) {
	
		sprintf(message, "dimension: %i", dimIdx);
		log_info(infoPath, message);
 		
		for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {
			
			elemTmp = 0;
			for (int vectorIdx=0; vectorIdx<nVectors; vectorIdx++) {
			
				elemTmp = elemTmp + vector_list[(vectorIdx*nDimensions) + dimIdx] * compressed_data_list[(meterIdx * nVectors) + vectorIdx];
			}	
			
			//sprintf(message, "meterIdx: %i, vectorIdx: %i, elemTmp: %lf", meterIdx, vectorIdx, elemTmp);
			//log_info(infoPath, message);
 
			recon_coeff_list[(meterIdx * nDimensions) + dimIdx] = elemTmp;
		}
	}
	
	sprintf(message, "Here 4...");
	log_info(infoPath, message);
 	
	FILE * out_file = NULL;
	sprintf(OUTfile, outPath);
	
	sprintf(message, outPath);
	log_info(infoPath, message);
  
	out_file = fopen(OUTfile,"w"); 
	
	for (int i=0; i<nMeters; i++) {
		for (int j=0; j<nDimensions; j++) {
	
			fprintf(out_file, "%lf  ", recon_coeff_list[i*nDimensions + j]);
		}
		fprintf(out_file, "\n");
	}
	fclose(out_file);
	
	printf("Finished!...\n");
	sprintf(message, "finished!");
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
