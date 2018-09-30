from pyspark import SparkContext

#logFile = "/Users/jamesetherington/Documents/SSE_working/results/meters/part-*"  # Should be some file on your system
#logFile = "lustre/sse/anonfull/part-*"

logFile = "lustre/sse/results/meters/part-*"

sc = SparkContext("local", "sse App")
lines = sc.textFile(logFile)

print('*************************')
print('*************************')
print('*************************')


#pathOut = "/Users/jamesetherington/Documents/SSE_working/results/"
pathOut = "lustre/sse/results/"

def write_meters_single_file():    
    ln = lines.map(lambda line: line.strip()).coalesce(1)
    ln.saveAsTextFile(pathOut + 'meters_single') 
    
write_meters_single_file()
