#!/bin/bash

#qsub kickOff_count_readings.pbs
#qsub kickOff_count_customers.pbs
#qsub kickOff_count_meters.pbs
#qsub kickOff_count_timestamps.pbs
#qsub kickOff_get_min_reading.pbs
#qsub kickOff_get_max_reading.pbs
#qsub kickOff_get_mean_reading.pbs
#qsub kickOff_get_min_timestamp.pbs
#qsub kickOff_get_max_timestamp.pbs
#qsub kickOff_write_customers.pbs
#qsub kickOff_write_meters.pbs
#qsub kickOff_write_timestamps.pbs
#qsub kickOff_write_readings_for_meter.pbs
#qsub kickOff_write_meters_single_file.pbs    
#qsub kickOff_write_meters_by_file.pbs     
qsub kickOff_checkMeterFiles.pbs
