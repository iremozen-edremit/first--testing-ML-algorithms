/* This code computes the structure functions of the meter readings

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
#include <time.h>
#include "mpi.h"

using namespace std;

const int nMeters = 41163;
const int nIntervals = 14;
const int nMetersPerCore = 258;

const int intervals_arr[nIntervals] = {24,48,72,96,120,144,168,192,216,240,264,288,312,336};

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
    char * outPath;
    char * infoPath;
    
    int procnum;
	int nProcsPerNode;
    int nodenum;
    int startIndex, endIndex;
    int corenum;
    
    char message[1000]; 
 
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &procnum);
    MPI_Comm_size (MPI_COMM_WORLD, &nProcsPerNode);
     
    inPath1 = argv[1];   // meter list (meters_gt_20000.dat)
    inPath2 = argv[2];   // path to meter file
    outPath = argv[3];
    infoPath = argv[4];    // info
    nodenum = atoi(argv[5]);
     
    corenum = (nodenum * nProcsPerNode) + procnum;   //zero based
   
   	sprintf(message, "Started...");
	log_info(infoPath, corenum, message);

    startIndex = corenum * double(nMetersPerCore);
    endIndex = startIndex + nMetersPerCore - 1;
    
    if (endIndex >= nMeters) {
    	endIndex = nMeters - 1;
    }
    
    
    char INfile1[1000];
    char INfile2[1000];
    char OUTfile[1000];
    
    FILE * input_file1 = NULL;
    FILE * input_file2 = NULL;
    FILE * output_file = NULL;
    
    int tmp_idx, tmp_meterId, tmp_num_readings;
    
    int * meter_list; 
    meter_list = (int*)calloc(nMeters, sizeof(int));
    
    sprintf(INfile1, inPath1);
    input_file1 = fopen(INfile1,"r"); 
      
    tmp_idx = 0;
    while (fscanf(input_file1,"%i  %i  \n",&tmp_meterId,&tmp_num_readings) != EOF) {
 
    	meter_list[tmp_idx] = tmp_meterId;
    	tmp_idx++;
 	}
 	     
	struct tm min_datetime = { 0 }; // = new struct tm;
	 
	min_datetime.tm_year = 2004 - 1900;	
	min_datetime.tm_mon = 6 - 1;
	min_datetime.tm_mday = 30;
	min_datetime.tm_hour = 0;
	min_datetime.tm_min = 0;
	
	struct tm max_datetime = { 0 }; // = new struct tm;
	max_datetime.tm_year = 2014 - 1900;
	max_datetime.tm_mon = 4 - 1;
	max_datetime.tm_mday = 20;
	max_datetime.tm_hour = 23;
	max_datetime.tm_min = 30;
	
	char rdate[20];
	char rtime[20];
 	char meterId[50];
   	char customerId[50];
   	double reading;
   	double lead;
   	double lag;
  	double secs_elapsed;
  	
	double max_time_range_secs = difftime(mktime(&max_datetime),mktime(&min_datetime));
			
	long int max_num_intervals = round(max_time_range_secs / 60.0 / 60.0 * 2.0);
	//printf("%s: %i \n", "num intervals", num_intervals);
	
	//sprintf(message, "max num intervals: %li", max_num_intervals);
    //log_info(infoPath, corenum, message);
  
	for (int meterIndex=startIndex; meterIndex<=endIndex; meterIndex++) {
		
		double * reading_list; 
		reading_list = (double*)calloc(max_num_intervals, sizeof(double));
	
		int * has_reading_list; 
		has_reading_list = (int*)calloc(max_num_intervals, sizeof(int));
		
		for (long int i=0; i<max_num_intervals; i++) {
			reading_list[i] = 0.0;
			has_reading_list[i] = 0;
		}
		
		struct tm tm1 = {0}; // = new struct tm;
		long int timeIndex = 0;
		char rdatetime[] = "";
			
		sprintf(INfile2, "%s_%i.dat", inPath2, meter_list[meterIndex]);		
		input_file2 = fopen(INfile2,"r"); 
  	
		int years, months, days, hours, mins, seconds;
		
		while (fscanf(input_file2,"%s %s %s %s %lf %lf %lf \n",&rdate,&rtime,&meterId,&customerId,&reading,&lead,&lag) != EOF) {
			
			sprintf(rdatetime, "%s %s", rdate, rtime);
			sscanf(rdatetime,"%d-%d-%d %d:%d:%d", &years, &months, &days, &hours, &mins, &seconds);
		
			tm1.tm_year = years - 1900;
			tm1.tm_mon = months - 1;
			tm1.tm_mday = days;
			tm1.tm_hour = hours;
			tm1.tm_min = mins;
			tm1.tm_sec = seconds;
			
			secs_elapsed = difftime(mktime(&tm1), mktime(&min_datetime));
			timeIndex = round(difftime(mktime(&tm1), mktime(&min_datetime)) / 60.0 / 60.0 * 2.0);
			
			reading_list[timeIndex] = reading;
			has_reading_list[timeIndex] = 1;
			
		}
		fclose(input_file2);
		
		double * structure_fn_list; 
		structure_fn_list = (double*)calloc(nIntervals, sizeof(double));
		
		for (int intervalIndex=0; intervalIndex<nIntervals; intervalIndex++) {
		
			int currentInterval = intervals_arr[intervalIndex];
			double * offset_list; 
			offset_list = (double*)calloc(max_num_intervals - currentInterval, sizeof(double));
		
			int * has_offset_list; 
			has_offset_list = (int*)calloc(max_num_intervals - currentInterval, sizeof(int));
			
			for (long int i=0; i<(max_num_intervals-currentInterval); i++) {
				offset_list[i] = reading_list[i + currentInterval];
				has_offset_list[i] = has_reading_list[i + currentInterval];
			}
			
			double * result_list; 
			result_list = (double*)calloc(max_num_intervals - currentInterval, sizeof(double));
		
			long int resultIdx = 0;
			double reading_diff;
			double total = 0;
			double mean_diff;
			
			for (int i=0; i<(max_num_intervals - currentInterval); i++) {
				
				if (has_reading_list[i] * has_offset_list[i]) {
					reading_diff = abs(offset_list[i] - reading_list[i]);
					result_list[resultIdx] = reading_diff;
					total = total + reading_diff;
					resultIdx++;
				}	
			}
	
			free(offset_list);
			free(has_offset_list);
			
			mean_diff = total / double(resultIdx);
			//printf("%li  %lf \n", offset_steps, mean_diff);
			
			structure_fn_list[intervalIndex] = mean_diff;
		}
			
		sprintf(OUTfile, "%s_%i.dat", outPath, corenum);
		output_file = fopen(OUTfile,"a");
	 
	 	fprintf(output_file, "%i   %i   ", meterIndex, meter_list[meterIndex]);
	 
		for (int i=0; i<nIntervals; i++) {
					
			fprintf(output_file,"%lf   ", structure_fn_list[i]);
		}
		fprintf(output_file,"\n");
		fclose(output_file);
			
		free(structure_fn_list);
		free(reading_list);
		free(has_reading_list);
		
		printf("Finished!...\n");
	}
	
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


