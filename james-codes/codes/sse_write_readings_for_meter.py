from pyspark import SparkContext
from datetime import datetime


#meterFiles = "/Users/jamesetherington/Documents/SSE_working/results/meters/part-*"
meterFiles = "lustre/sse/results/meters/part-*"

#logFile = "/Users/jamesetherington/Documents/SSE_working/part-*.txt"  # Should be some file on your system
logFile = "lustre/sse/anonfull/part-*"
sc = SparkContext("local", "sse App")
lines = sc.textFile(logFile)
meters = sc.textFile(meterFiles)
 
print('*************************')
print('*************************')
print('*************************')

# write out timestamp, reading data in order for one customer.

#pathOut = "/Users/jamesetherington/Documents/SSE_working/results/"
pathOut = "lustre/sse/results/"

min_datetime = datetime(2004, 6, 30, 0, 0)

print('**1')

ln = lines.map(lambda line: line.strip('(').strip(')').split(',')).map(lambda line: (str(line[0].strip().strip('\'')),  str(line[1].strip().strip('\'')), float(line[3].strip().strip('\''))))

print('**2')
  
def get_unique_meters():
   
    unique_list = meters.take(5)
    return unique_list

def write_readings_for_meter(meterId, index):
    ln2 = ln.filter(lambda line: str(line[1])==meterId).map(lambda line: (line[0], line[2]))
    
    print('**4....' + str(index))
    time_reading_list = ln2.map(lambda x: (str((datetime.strptime(x[0], '%Y-%m-%d %H:%M:%S') - min_datetime).total_seconds() / 60.0 / 60.0), str(x[1]))).collect()
    
    f = open(pathOut + 'meter.dat' + str(index), 'w')
    
    for treading in time_reading_list:
        f.write(treading[0] + '   ' + treading[1] + '\n')
    
    f.close()
        
def write_readings_for_all_meters():
   
    print('**3')
   
    meter_list = get_unique_meters()
    
    index = 0
    for met in meter_list:        
        write_readings_for_meter(met, index)
        fmap = open(pathOut + 'meter_mapping.dat', 'a')            
        fmap.write(str(index) + '   ' + met + '\n') 
        fmap.close()
        index = index + 1
        
write_readings_for_all_meters()
     
     