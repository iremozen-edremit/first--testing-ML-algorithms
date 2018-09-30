from pyspark import SparkContext

#inFile = "/Users/jamesetherington/Documents/SSE_working/"  
inFile = "lustre/sse/anonfull/"

#meterPath = "/Users/jamesetherington/Documents/SSE_working/meter_list.dat"
meterPath = "lustre/sse/results/meters_single/meter_list.dat"

#pathOut = "/Users/jamesetherington/Documents/SSE_working/"
pathOut = "lustre/sse/results/meters_by_file/"
  
sc = SparkContext("local", "sse App")

print('*************************')
print('*************************')
print('*************************')


def get_meter_dict():
 	
  
    with open(meterPath) as f:
        meter_list = f.readlines()
    
    idx =0
    meter_dict = dict()
    for meter in meter_list:
        meter_dict.update({meter.replace("\n", ""):idx})
        idx = idx + 1
        
    return meter_dict
  
def write_meters_by_file():    
    
    meter_dict = get_meter_dict()    
    
    numfiles = 5801
    for fileIdx in range(0, numfiles):
        
        if (fileIdx < 10):
            filename = "part-0000" + str(fileIdx)
        elif ((fileIdx >= 10) and (fileIdx < 100)):
            filename = "part-000" + str(fileIdx)
        elif ((fileIdx >= 100) and (fileIdx < 1000)):
            filename = "part-00" + str(fileIdx)
        elif ((fileIdx >= 1000) and (fileIdx<10000)):
            filename = "part-0" + str(fileIdx)
        elif ((fileIdx >= 10000)) and (fileIdx<1000000):
            filename = "part-" + str(fileIdx)

        lines = sc.textFile(inFile + filename)

        meter_list = lines.map(lambda line: line.strip('(').strip(')').split(',')).map(lambda line: str(line[1].strip().strip('\''))).distinct().collect()
    
        f = open(pathOut + 'meters_by_file.dat', 'a')  

        for meter in meter_list:     
            f.write(str(fileIdx) + '    ' + meter + '    ' + str(meter_dict[meter]) + ' \n')
        f.close()
     
write_meters_by_file()
