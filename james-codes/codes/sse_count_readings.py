from pyspark import SparkContext

#logFile = "/Users/jamesetherington/Documents/SSE_working/part-0000*.txt"  # Should be some file on your system
logFile = "lustre/sse/anonfull/part-*"
sc = SparkContext("local", "sse App")
lines = sc.textFile(logFile)

print('*************************')
print('*************************')
print('*************************')


#pathOut = "/Users/jamesetherington/Documents/SSE_working/results/"
pathOut = "lustre/sse/results/"

def count_readings():
    ln = lines.map(lambda line: line.strip('(').strip(')').split(',')).map(lambda line: float(line[3].strip().strip('\'')))
    print('number records: ' + str(ln.count()))
    
count_readings()    
