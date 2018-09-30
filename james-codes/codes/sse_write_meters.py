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

def write_meters():    
    ln = lines.map(lambda line: line.strip('(').strip(')').split(',')).map(lambda line: str(line[1].strip().strip('\''))).distinct()
    ln.saveAsTextFile(pathOut + 'meters') 
    
write_meters()

