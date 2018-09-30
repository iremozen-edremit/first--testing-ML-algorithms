#include <stdio.h>
#include <math.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_wavelet.h>
#include "mpi.h"


// TODO: 
// 1.) Read in list of meters (done)
// 2.) Read meter file, set up timesteps (done)
// 3.) extract 512 step signal (done)
// 4.) compute mean of signal, and center signal (done)
// 5.) compute RMS amplitude and divide signal by RMS amplitude (done)
// 6.) compute coefficients (done)
// 7.) write out coefficients (done)
// 8.) MPI (done)
// 9.) Info logging (done)

const int nRows = 893828;
const int windowSize = 512;
const int nRowsPerCore = 5587;

void log_info(char * infoPath, int corenum, char * message);

int main (int argc, char **argv) {
  
    printf("\n");
    printf("******************************\n");
    printf("Start...\n");
    printf("******************************");
    printf("\n");
    printf("\n");
 
	char * inPath1;
    char * inPath2;
    char * outPath1;
    char * outPath2;
    char * outPath3;
    char * infoPath;
 	int procnum, nProcsPerNode;
 	int nodenum;
    int startIndex, endIndex;
    int corenum;
    
    char message[1000]; 
 
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &procnum);
    MPI_Comm_size (MPI_COMM_WORLD, &nProcsPerNode);
 
	inPath1 = argv[1]; 
	inPath2 = argv[2];
	outPath1 = argv[3];
	outPath2 = argv[4];
	outPath3 = argv[5];
	infoPath = argv[6];   
	nodenum = atoi(argv[7]);
     
    corenum = (nodenum * nProcsPerNode) + procnum;   //zero based
   
    startIndex = corenum * nRowsPerCore;
    endIndex = startIndex + nRowsPerCore - 1;
  	
  	if (endIndex > nRows) {
  		endIndex = nRows - 1;
  	}
  
	char INfile1[1000];
	char INfile2[1000];
	char OUTfile[1000];
	
	int rowId, tmp_meterId, slotIndex;
	
	gsl_wavelet *w;
	gsl_wavelet_workspace *work;

	w = gsl_wavelet_alloc (gsl_wavelet_daubechies, 4);
	work = gsl_wavelet_workspace_alloc (windowSize);

	int * meter_list; 
    meter_list = (int*)calloc(nRows, sizeof(int));
    
    int * slot_list;
    slot_list = (int*)calloc(nRows, sizeof(int));
    
    FILE * input_file1 = NULL;
    FILE * input_file2 = NULL;
    FILE * output_file = NULL;
 
    sprintf(INfile1, inPath1);
    input_file1 = fopen(INfile1,"r"); 
      
    int tmp_idx = 0;
    while (fscanf(input_file1,"%i  %i  %i  \n",&rowId,&tmp_meterId,&slotIndex) != EOF) {
 
    	meter_list[tmp_idx] = tmp_meterId;
		slot_list[tmp_idx] = slotIndex;
    	tmp_idx++;
 	}
 	
 	struct tm min_datetime = { 0 }; 
	 
	min_datetime.tm_year = 2004 - 1900;	
	min_datetime.tm_mon = 6 - 1;
	min_datetime.tm_mday = 30;
	min_datetime.tm_hour = 0;
	min_datetime.tm_min = 0;
		
	struct tm start_window_datetime = { 0 }; 
	
	start_window_datetime.tm_year = 2012 - 1900;	
	start_window_datetime.tm_mon = 1 - 1;
	start_window_datetime.tm_mday = 2;
	start_window_datetime.tm_hour = 0;
	start_window_datetime.tm_min = 0;
		
	time_t mintime = mktime(&min_datetime);
	time_t start_win_time = mktime(&start_window_datetime);
				
	int firstWinIndex = round(abs(difftime(start_win_time, mintime)) / 60.0 / 60.0* 2.0);
	
	//int endWinIndex = startWinIndex + 512 - 1;
	
	struct tm tm1 = {0}; 
	char rdatetime[] = "";
	
	char rdate[20];
	char rtime[20];
 	char meterId[50];
   	char customerId[50];
   	double reading;
   	double lead;
   	double lag;
  	double secs_elapsed;
	long int timeIndex = 0;
  	
  	int years, months, days, hours, mins, seconds;
	
	sprintf(message, "startIndex: %i, endIndex: %i", startIndex, endIndex);
	log_info(infoPath, corenum, message);
 	
 	int num_of_meters = endIndex - startIndex + 1;
 	sprintf(message, "num_meters: %i", num_of_meters);
	log_info(infoPath, corenum, message);
 
 	int startWinIndex = 0;
 	int endWinIndex = 0;
 			
 	for (int rowIndex=startIndex; rowIndex<=endIndex; rowIndex++) {
		
		sprintf(message, "processing rowIndex: %i", rowIndex);
		log_info(infoPath, corenum, message);
 	
 		startWinIndex = firstWinIndex + (slot_list[rowIndex] * 48 * 7);
		endWinIndex = startWinIndex + windowSize - 1;
		
		double * reading_list; 
		reading_list = (double*)calloc(windowSize, sizeof(double));
		
		double * out_reading_list;
		out_reading_list = (double*)calloc(windowSize, sizeof(double));
		
		sprintf(INfile2, "%s_%i.dat", inPath2, meter_list[rowIndex]);		
		input_file2 = fopen(INfile2,"r"); 
  	
 		while (fscanf(input_file2,"%s %s %s %s %lf %lf %lf \n",&rdate,&rtime,&meterId,&customerId,&reading,&lead,&lag) != EOF) {
			
			sprintf(rdatetime, "%s %s", rdate, rtime);
			sscanf(rdatetime,"%d-%d-%d %d:%d:%d", &years, &months, &days, &hours, &mins, &seconds);
		
			tm1.tm_year = years - 1900;
			tm1.tm_mon = months - 1;
			tm1.tm_mday = days;
			tm1.tm_hour = hours;
			tm1.tm_min = mins;
			tm1.tm_sec = seconds;
			
			time_t tm1_time = mktime(&tm1);
			secs_elapsed = abs(difftime(tm1_time, mintime));
			timeIndex = round(secs_elapsed / 60.0 / 60.0 * 2.0);
			
			if ((timeIndex >= startWinIndex) && (timeIndex <= endWinIndex)) {
				reading_list[timeIndex - startWinIndex] = reading;	
				out_reading_list[timeIndex - startWinIndex] = reading;
			}
		}
		fclose(input_file2);
		
		double sum_signal = 0;
		double mean_signal = 0;
		
		for (int readingIndex=0; readingIndex<windowSize; readingIndex++) {
		
			sum_signal = sum_signal + reading_list[readingIndex];
		}
		mean_signal = sum_signal / double(windowSize);
		
		for (int readingIndex=0; readingIndex<windowSize; readingIndex++) {
		
			reading_list[readingIndex] = reading_list[readingIndex] - mean_signal;
		}
	
		double sum_sqrd_signal = 0;
		double rms_amplitude = 0;
		
		for (int readingIndex=0; readingIndex<windowSize; readingIndex++) {
		
			sum_sqrd_signal = sum_sqrd_signal + (reading_list[readingIndex] * reading_list[readingIndex]);
		}
	
		rms_amplitude = sqrt( sum_sqrd_signal / double(windowSize));
		
		for (int readingIndex=0; readingIndex<windowSize; readingIndex++) {
		
			reading_list[readingIndex] = reading_list[readingIndex] / rms_amplitude;
		}
		
		// compute the wavelet transform
		gsl_wavelet_transform_forward (w, reading_list, 1, windowSize, work);
	
		int success = 1;
		for (int readingIndex=0; readingIndex<windowSize; readingIndex++) {
			
			if (isnan(reading_list[readingIndex])) {
				success = 0;
				break;
			}
		}
	
		if (success==1) {
			sprintf(OUTfile, "%s_%i.dat", outPath1, corenum);
			output_file = fopen(OUTfile,"a");
		    
		    // output wavelet coefficients
			fprintf(output_file, "%i   %i   ", rowIndex, meter_list[rowIndex]);
			
			for (int readingIndex=0; readingIndex<windowSize; readingIndex++) {
			
				fprintf(output_file,"%lf   ", reading_list[readingIndex]);
			
			}
			fprintf(output_file,"\n");
			fclose(output_file);
			
			// output mean signal and rms error
			sprintf(OUTfile, "%s_%i.dat", outPath2, corenum);
			output_file = fopen(OUTfile,"a");
			fprintf(output_file, "%i   %i   %lf   %lf \n", rowIndex, meter_list[rowIndex], mean_signal, rms_amplitude);
			fclose(output_file);
			
			// output original signals
			sprintf(OUTfile, "%s_%i.dat", outPath3, corenum);
			output_file = fopen(OUTfile,"a");
		 
			fprintf(output_file, "%i   %i   ", rowIndex, meter_list[rowIndex]);
			
			for (int readingIndex=0; readingIndex<windowSize; readingIndex++) {
			
				fprintf(output_file,"%lf   ", out_reading_list[readingIndex]);
			}
			fprintf(output_file,"\n");
			fclose(output_file);
		}
		
		free(reading_list);
 	}	
 	
 	free(meter_list);
 	
	gsl_wavelet_free (w);
	gsl_wavelet_workspace_free (work);

	printf("Finished!...\n");
	sprintf(message, "Finished...");
	log_info(infoPath, corenum, message);
 	
	MPI_Finalize ();	
 
	return 0;
}

void log_info(char * infoPath, int corenum, char * message) {

    char INFOname[1000];
    FILE * info_file = NULL;                      
  
    time_t now = time(0);
   	char* dt = ctime(&now);
  	dt[strlen(dt) - 1] = '\0';    //remove carriage return
  
	sprintf(INFOname,"%s_%i.dat",infoPath,corenum);
	info_file = fopen(INFOname,"a");
	fprintf(info_file,"%s - %s \n", dt, message); 
	fclose(info_file);
}

