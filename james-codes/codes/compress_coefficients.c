/* This code computes the covariance matrix

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

// 1.) read in in wavelet coefficients
// 2.) read in eigenvectors
// 3.) do matrix multiplication
// 4.) write out compressed vector


const int nChunks = 160;
const int nDimensions = 511;
const int nMeters = 893828;
const int nVectors = 10;

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
       
    char * inPath1;   // wavelet coefficients
    char * inPath2;   // eigenvectors
    char * outPath;
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
        
	char meter_line[10000];  
    string meter_sline, token; 

	int nMetersInChunk = 0;
	int ch = 0;
	  
	double * data_list; 
	data_list = (double*)calloc(nDimensions * nMeters, sizeof(double));	  
	 
	int * row_list;
	row_list = (int*)calloc(nMeters, sizeof(int));	  
	
	 
	int meterIdx = 0;  
	for (int chunkIdx=0; chunkIdx<nChunks; chunkIdx++) {
	
		sprintf(message, "reading chunk: %i", chunkIdx);
		log_info(infoPath, message);
 
		nMetersInChunk = 0;
		sprintf(INfile1, "%s_%i.dat", inPath1, chunkIdx);
		
		sprintf(message, "INfile1: %s", INfile1);
		log_info(infoPath, message);
 
		input_file1 = fopen(INfile1,"r"); 
		
		while(!feof(input_file1)) {
			
			ch = fgetc(input_file1);
			
			if(ch == '\n') {
				nMetersInChunk++;
			}
		}
		fclose(input_file1);
		
		printf("chunk: %i, Number of objects: %li \n", chunkIdx, nMetersInChunk);
	  
		sprintf(INfile1, "%s_%i.dat", inPath1, chunkIdx);
		input_file1 = fopen(INfile1,"r"); 
		
		for (long int meterInChunkIdx=0; meterInChunkIdx<nMetersInChunk; meterInChunkIdx++) {
	
			fgets (meter_line, 10000, input_file1);
			meter_sline = string(meter_line);
			stringstream meter_ss(meter_sline);
	
			string tokenTmp;
			long int tokenIdx = 0;
			
			while (getline(meter_ss,token, ' ')) {
				tokenTmp = trim(token);
				
				if (strcmp(tokenTmp.c_str(), "") != 0) {
				
					if (tokenIdx == 0) {
						row_list[meterIdx] = atoi(tokenTmp.c_str());
					} else if (tokenIdx > 2) {
						data_list[(tokenIdx - 3) + (meterIdx*nDimensions)] = atof(tokenTmp.c_str());
					}

					tokenIdx++;
				}
			}
			meterIdx++;
		}
		fclose(input_file1);
	}
	
	int total_meters = meterIdx;
	
	sprintf(INfile2,inPath2);
	input_file2 = fopen(INfile2,"r"); 
		
	char vec_line[10000];  
    string vec_sline, token2; 
	
	double * vector_list; 
	vector_list = (double*)calloc(nVectors * nDimensions, sizeof(double));	  
	
	sprintf(message, "Here 1. ");
	log_info(infoPath, message);
 
	for (int vectorIdx=0; vectorIdx<nVectors; vectorIdx++) {
	
		fgets (vec_line, 10000, input_file2);
		vec_sline = string(vec_line);
		stringstream vec_ss(vec_sline);
	
		string tokenTmp1;
		int tokenIndex = 0;
		
		while (getline(vec_ss,token2, ' ')) {
			tokenTmp1 = trim(token2);
				
			if (strcmp(tokenTmp1.c_str(), "") != 0) {
				
				vector_list[(tokenIndex) + (vectorIdx*nDimensions)] = atof(tokenTmp1.c_str());
				tokenIndex++;
			}
		}
	}
	
	sprintf(message, "Here 2. ");
	log_info(infoPath, message);

	int compressedIdx = 0;
	double * compressed_list; 
	compressed_list = (double*)calloc(nVectors * nMeters, sizeof(double));	  
	
	double elemTmp = 0;
	// do matrix multiplication
	
	for (int vectorIdx=0; vectorIdx<nVectors; vectorIdx++) {
	
		for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {
			
			elemTmp = 0;
			for (int dimIdx=0; dimIdx<nDimensions; dimIdx++) {
		
				elemTmp = elemTmp + vector_list[(vectorIdx*nDimensions) + dimIdx] * data_list[(meterIdx * nDimensions) + dimIdx];
			}	
			
			//sprintf(message, "meterIdx: %i, vectorIdx: %i, elemTmp: %lf", meterIdx, vectorIdx, elemTmp);
			//log_info(infoPath, message);
 
			compressed_list[(meterIdx * nVectors) + vectorIdx] = elemTmp;
		}
	}
	
	sprintf(message, "Here 3. ");
	log_info(infoPath, message);

	FILE * out_file = NULL;
	sprintf(OUTfile, outPath);  
	out_file = fopen(OUTfile,"w"); 
	
	for (int i=0; i<total_meters; i++) {
		
		fprintf(out_file, "%i  ", row_list[i]);
		
		for (int j=0; j<nVectors; j++) {
	
			fprintf(out_file, "%lf  ", compressed_list[i*nVectors + j]);
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
