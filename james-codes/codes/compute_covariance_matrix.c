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

const int nChunks = 160;
const int nDimensions = 511;

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
       
    char * inPath;
    char * outPath;
    char * infoPath;
    
    char message[1000]; 
 
    inPath = argv[1];   
    outPath = argv[2];
    infoPath = argv[3];   
        
 	sprintf(message, "starting...");
	log_info(infoPath, message);
 	   
    char INfile[1000];
    char OUTfile[1000];
    
    FILE * input_file = NULL;
    FILE * output_file = NULL;
        
	char meter_line[10000];  
    string meter_sline, token; 

	int nMetersInChunk = 0;
	int ch = 0;
	
	int nMeters = 0;
	for (int chunkIdx=0; chunkIdx<nChunks; chunkIdx++) {
	
		nMetersInChunk = 0;
		sprintf(INfile, "%s_%i.dat", inPath, chunkIdx);	
		input_file = fopen(INfile,"r"); 
		
		while(!feof(input_file)) {
			
			ch = fgetc(input_file);
			
			if(ch == '\n') {
				nMeters++;
			}
		}
	}
	fclose(input_file);

	sprintf(message, "Total number of meters: %i", nMeters);
	log_info(infoPath, message);
 
	double * data_list; 
	data_list = (double*)calloc(nMeters * nDimensions, sizeof(double));	  
	
	for (long int i=0; i<(nMeters*nDimensions); i++) {
	
		data_list[i] = 0.0;
	}
	 
	int meterIdx = 0;  
	  
	for (int chunkIdx=0; chunkIdx<nChunks; chunkIdx++) {
 
		nMetersInChunk = 0;
		sprintf(INfile, "%s_%i.dat", inPath, chunkIdx);
	
		input_file = fopen(INfile,"r"); 
		
		while(!feof(input_file)) {
			
			ch = fgetc(input_file);
			
			if(ch == '\n') {
				nMetersInChunk++;
			}
		}
		fclose(input_file);
		
		sprintf(INfile, "%s_%i.dat", inPath, chunkIdx);
		input_file = fopen(INfile,"r"); 
		
		for (long int meterInChunkIdx=0; meterInChunkIdx<nMetersInChunk; meterInChunkIdx++) {
	
			fgets (meter_line, 10000, input_file);
			meter_sline = string(meter_line);
			stringstream meter_ss(meter_sline);
	
			string tokenTmp;
			long int tokenIdx = 0;
		
			while (getline(meter_ss,token, ' ')) {
				tokenTmp = trim(token);
				
				if (strcmp(tokenTmp.c_str(), "") != 0) {
				
					if (tokenIdx > 2) {
					
						int num = (tokenIdx - 3) + (meterIdx*nDimensions);
						data_list[num] = atof(tokenTmp.c_str());
					}		
					tokenIdx++;
				}
			}
			meterIdx++;
		}
		fclose(input_file);
	}
	
	double * covariance_list;
	covariance_list = (double*)calloc(nDimensions * nDimensions, sizeof(double));
	
	for (int i=0; i<nDimensions; i++) {

		double * first_list; 
		first_list = (double*)calloc(nMeters, sizeof(double));
	
		sprintf(message, "computing dimension: %i", i);
		log_info(infoPath, message);
 
		double first_sum = 0;
		double first_sqrd_sum = 0;
		double first_mean = 0;
		double first_stddev = 0;
		double readingTmp = 0;
		for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {
		
			readingTmp = data_list[meterIdx * nDimensions + i];
			first_list[meterIdx] = readingTmp;
			first_sum = first_sum + readingTmp;		
			first_sqrd_sum = first_sqrd_sum + readingTmp * readingTmp;
		}
		
		first_mean = first_sum / double(nMeters);
		first_stddev = sqrt((first_sqrd_sum / double(nMeters) ) - first_mean * first_mean );

	//	sprintf(message, "first mean: %lf, first stddev: %lf", first_mean, first_stddev);
	//	log_info(infoPath, message);
	
		for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {
		
			first_list[meterIdx] = (first_list[meterIdx] - first_mean) / first_stddev;
		}
		
		for (int j=0; j<nDimensions; j++) {
	
			double * second_list; 
			second_list = (double*)calloc(nMeters, sizeof(double));

			double second_sum = 0;
			double second_sqrd_sum = 0;
			double second_mean = 0;
			double second_stddev = 0;
			double readingTmp2 = 0;
			for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {
				readingTmp2 = data_list[meterIdx * nDimensions + j];
				second_list[meterIdx] = readingTmp2;
				second_sum = second_sum + readingTmp2;
				second_sqrd_sum = second_sqrd_sum + readingTmp2 * readingTmp2;
			}
			second_mean = second_sum / double(nMeters);
			second_stddev = sqrt((second_sqrd_sum / double(nMeters) ) - second_mean * second_mean);
		
			for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {
		
				second_list[meterIdx] = (second_list[meterIdx] - second_mean) / second_stddev;
			}
		
			// compute covariance
			double covar = 0; 
			
			for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {
				covar = covar + (first_list[meterIdx] * second_list[meterIdx]);
				
				//sprintf(message, "first: %lf, second: %lf, covar: %lf", first_list[meterIdx], second_list[meterIdx], covar);
				//log_info(infoPath, message);
	
			}
			covar = covar / double(nMeters - 1);
			covariance_list[i*nDimensions + j] = covar;
			
			free(second_list);
		}
		
		free(first_list);
	}
	
	FILE * out_file = NULL;
	sprintf(OUTfile, outPath);  
	out_file = fopen(OUTfile,"w"); 
	
	for (int i=0; i<nDimensions; i++) {
		for (int j=0; j<nDimensions; j++) {
	
			fprintf(out_file, "%i  %i  %lf  \n", i, j, covariance_list[i*nDimensions + j]);
		}
	}
	fclose(out_file);
	
	free(covariance_list);
	free(data_list);
	
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
