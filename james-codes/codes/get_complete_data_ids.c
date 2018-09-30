/* This code gets the ids of all the meters with complete data 
between two sets of dates. 

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

const int nMeters = 59387;
const int nMetersPerCore = 372;

void log_info(char * infoPath, int corenum, char * message);

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
    
    int procnum;
	int nProcsPerNode;
    int nodenum;
    int startIndex, endIndex;
    int corenum;
    
    char message[1000]; 
 
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &procnum);
    MPI_Comm_size (MPI_COMM_WORLD, &nProcsPerNode);
     
    inPath = argv[1];   
    outPath = argv[2];
    infoPath = argv[3];   
    nodenum = atoi(argv[4]);
     
    corenum = (nodenum * nProcsPerNode) + procnum;   //zero based
   
   	sprintf(message, "Started...");
	log_info(infoPath, corenum, message);

    startIndex = corenum * nMetersPerCore;
    endIndex = startIndex + nMetersPerCore - 1;
    
    if (endIndex >= nMeters) {
    	endIndex = nMeters - 1;
    }
    
    char INfile[1000];
    char OUTfile[1000];
    
    FILE * input_file = NULL;
    FILE * output_file = NULL;
    
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
	
	time_t mintime = mktime(&min_datetime);
	time_t maxtime = mktime(&max_datetime);
	
	struct tm start_window_datetime = { 0 }; 
	
	start_window_datetime.tm_year = 2012 - 1900;	
	start_window_datetime.tm_mon = 1 - 1;
	start_window_datetime.tm_mday = 2;
	start_window_datetime.tm_hour = 0;
	start_window_datetime.tm_min = 0;
	
	struct tm end_window_datetime = { 0 }; 
	
	end_window_datetime.tm_year = 2012 - 1900;	
	end_window_datetime.tm_mon = 12 - 1;
	end_window_datetime.tm_mday = 30;
	end_window_datetime.tm_hour = 23;
	end_window_datetime.tm_min = 30;

	time_t end_win_time = mktime(&end_window_datetime);
	time_t start_win_time = mktime(&start_window_datetime);
	
	long int startWinIndex = round(difftime(start_win_time, mintime) / 60.0 / 60.0 * 2.0);
	long int endWinIndex = round(difftime(end_win_time, mintime) / 60.0 / 60.0 * 2.0);
	
	long int winIndexSize = endWinIndex - startWinIndex + 1;

	sprintf(message, "startWinIndex: %li, endWinIndex: %li", startWinIndex, endWinIndex);
	log_info(infoPath, corenum, message);
	
	sprintf(message, "winIndexSize: %li", winIndexSize);
	log_info(infoPath, corenum, message);
		
	double max_time_range_secs = difftime(maxtime,mintime);
			
	long int max_num_intervals = round(max_time_range_secs / 60.0 / 60.0 * 2.0);
	
	sprintf(message, "max time range secs: %lf", max_time_range_secs);
	log_info(infoPath, corenum, message);

	sprintf(message, "max num intervals: %li", max_num_intervals);
	log_info(infoPath, corenum, message);

	int * window_list; 
	window_list = (int*)calloc(max_num_intervals, sizeof(int));
	
	for (long int i=0; i<max_num_intervals; i++) {
		
		window_list[i] = 0;
		if ((i >= startWinIndex) && (i <= endWinIndex)) {
			window_list[i] = 1;
		}	
	}
		
	char rdate[20];
	char rtime[20];
 	char meterId[50];
   	char customerId[50];
   	double reading;
   	double lead;
   	double lag;
  	double secs_elapsed;
  	
	for (int meterIndex=startIndex; meterIndex<=endIndex; meterIndex++) {
		
		sprintf(message, "meterIndex: %i", meterIndex);
     	log_info(infoPath, corenum, message);

		double * reading_list; 
		reading_list = (double*)calloc(max_num_intervals, sizeof(double));
		
		int * has_reading_list; 
		has_reading_list = (int*)calloc(max_num_intervals, sizeof(int));
		
		for (long int i=0; i<max_num_intervals; i++) {
			has_reading_list[i] = 0;
		}
		
		struct tm tm1 = {0}; // = new struct tm;
		long int timeIndex = 0;
		char rdatetime[] = "";
			
		sprintf(INfile, "%s_%i.dat", inPath, meterIndex);		
		input_file = fopen(INfile,"r"); 
  	
		int years, months, days, hours, mins, seconds;
		
		if (input_file == NULL) {
			continue;
		} else {
		
			while (fscanf(input_file,"%s %s %s %s %lf %lf %lf \n",&rdate,&rtime,&meterId,&customerId,&reading,&lead,&lag) != EOF) {
		
				sprintf(rdatetime, "%s %s", rdate, rtime);
				sscanf(rdatetime,"%d-%d-%d %d:%d:%d", &years, &months, &days, &hours, &mins, &seconds);
			
				tm1.tm_year = years - 1900;
				tm1.tm_mon = months - 1;
				
				tm1.tm_mday = days;
				tm1.tm_hour = hours;
				tm1.tm_min = mins;
				//tm1.tm_sec = seconds;
				
		        time_t tm1_time = mktime(&tm1);
				secs_elapsed = difftime(tm1_time, mintime);
				
				timeIndex = round(difftime(tm1_time, mintime) / 60.0 / 60.0 * 2.0);
				
				//sprintf(message, "secs elapsed: %lf", secs_elapsed);
			    //log_info(infoPath, corenum, message);
		     	//sprintf(message, "timeIndex: %li", timeIndex);
			    //log_info(infoPath, corenum, message);
				
				if (reading != 0.0) {
					reading_list[timeIndex] = reading;
					has_reading_list[timeIndex] = 1;	
				}
				
			}
			fclose(input_file);	

		}
		long int totalInWin = 0;
		double totalWinReading = 0;
		for (long int i=0; i<max_num_intervals; i++) {
		
			totalInWin = totalInWin + (has_reading_list[i] * window_list[i]);
			totalWinReading = totalWinReading + (has_reading_list[i] * window_list[i]) * reading_list[i];
		}
		
		sprintf(OUTfile, "%s_%i.dat", outPath, corenum);
		output_file = fopen(OUTfile,"a");
	
		if (totalWinReading!=0.0) {
	 		fprintf(output_file, "%i   %i   \n", meterIndex, totalInWin);
		}
		else {
			fprintf(output_file, "%i   %i   \n", meterIndex, totalInWin);
		}
	  
		fclose(output_file);	
		
	}
	
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


