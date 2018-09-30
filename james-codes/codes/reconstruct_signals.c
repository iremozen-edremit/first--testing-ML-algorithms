/* This code reconstructs the signal from the reconstructed coefficients 

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
#include <gsl/gsl_sort.h>
#include <gsl/gsl_wavelet.h>


using namespace std;

// TODO: 

// 1.) read in reconstructed coefficients
// 2.) read in mean and RMS amplitude for each signal
// 3.) do inverse wavelet transform.
// 4.) multiply by RMS amplitude, then add mean signal
// 5.) write out reconstructed signals

const int nDimensions = 511;
const int nMeters = 25;

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
    
    gsl_wavelet *w;
	gsl_wavelet_workspace *work;
       
    char * inPath1;   // reconstructed coefficients
    char * inPath2;   // mean and rms amplitudes
    char * outPath;   // 
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
	recon_coeff_list = (double*)calloc((nDimensions + 1) * nMeters, sizeof(double));	  

	char meter_line[10000];  
    string meter_sline, token1; 
	
	sprintf(INfile1, inPath1);		
	sprintf(message, "INfile1: %s", INfile1);
	log_info(infoPath, message);
	input_file1 = fopen(INfile1,"r"); 

	// set first coefficient to zero
	for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {
		recon_coeff_list[meterIdx*(nDimensions + 1)] = 0;
	}
		
	for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {
	
		fgets (meter_line, 10000, input_file1);
		meter_sline = string(meter_line);
		stringstream meter_ss(meter_sline);
	
		string tokenTmp1;
		int tokenIndex1 = 0;
		
		while (getline(meter_ss,token1, ' ')) {
			tokenTmp1 = trim(token1);
				
			if (strcmp(tokenTmp1.c_str(), "") != 0) {
				
				recon_coeff_list[(tokenIndex1) + (meterIdx*(nDimensions + 1)) + 1] = atof(tokenTmp1.c_str());
				tokenIndex1++;
			}
		}
	}
	fclose(input_file1);
	
	sprintf(message, "Here 1.");
	log_info(infoPath, message);
	 
	int id, meterId;
	double mean_signal, rms_amplitude;
	
	int * meterId_list; 
	meterId_list = (int*)calloc(nMeters, sizeof(int));	  

	double * mean_signal_list; 
	mean_signal_list = (double*)calloc(nMeters, sizeof(double));	  

	double * rms_amplitude_list; 
	rms_amplitude_list = (double*)calloc(nMeters, sizeof(double));	  

	sprintf(INfile2, inPath2);		
	input_file2 = fopen(INfile2,"r"); 
  	
  	sprintf(message, "Here 2.");
	log_info(infoPath, message);
	
	int idx = 0;
	while (fscanf(input_file2,"%i  %i  %lf  %lf  \n",&id,&meterId,&mean_signal,&rms_amplitude) != EOF) {
			
		meterId_list[idx] = meterId;
		mean_signal_list[idx] = mean_signal;
		rms_amplitude_list[idx] = rms_amplitude;
		
		idx++;
		
		if (idx > (nMeters - 1)) {
			break;
		}
	}
	fclose(input_file2);
	
	sprintf(message, "Here 3.");
	log_info(infoPath, message);

	w = gsl_wavelet_alloc (gsl_wavelet_daubechies, 4);
	work = gsl_wavelet_workspace_alloc (nDimensions+1);
	
	double coeffTmp =0;
	  
  	for (int meterIdx=0; meterIdx<nMeters; meterIdx++) {
	
		sprintf(message, "meterIdx: %i", meterIdx);
		log_info(infoPath, message);
	
		double * current_list; 
		current_list = (double*)calloc(nDimensions+1, sizeof(double));	  

		for (int dimIdx=0; dimIdx<(nDimensions+1); dimIdx++) {
		
			coeffTmp = recon_coeff_list[meterIdx * (nDimensions + 1) + dimIdx];
			current_list[dimIdx] = coeffTmp;
		
			//sprintf(message, "%lf", coeffTmp);
			//log_info(infoPath, message);
		
		}
		gsl_wavelet_transform_inverse (w, current_list, 1, nDimensions+1, work);
	

		for (int dimIdx=0; dimIdx<(nDimensions+1); dimIdx++) {
		
			//sprintf(message, "%lf", current_list[dimIdx]);
			//log_info(infoPath, message);
		
			current_list[dimIdx] = current_list[dimIdx] * rms_amplitude_list[meterIdx] + mean_signal_list[meterIdx];
		}

	
		//sprintf(message, "Here 4.");
		//log_info(infoPath, message);

		FILE * out_file = NULL;
		sprintf(OUTfile, outPath);
		out_file = fopen(OUTfile,"a"); 
	
		fprintf(out_file, "%i  ", meterId_list[meterIdx]);
		
		for (int i=0; i<(nDimensions+1); i++) {
	
			fprintf(out_file, "%lf  ", current_list[i]);
		}
		fprintf(out_file, "\n");
		
		fclose(out_file);
	
  	}
  
  	gsl_wavelet_free (w);
  	gsl_wavelet_workspace_free (work);
	
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
