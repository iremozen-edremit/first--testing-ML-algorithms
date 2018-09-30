"""SSE App.py"""

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

def compute_mean_usage():
    ln = lines.map(lambda line: line.strip('(').strip(')').split(',')).map(lambda line: float(line[3].strip().strip('\'')))
    print('min usage: ' + str(ln.min()))
    print('max usage: ' + str(ln.max()))    
    print('mean usage: ' + str(ln.mean()))

def count_records():
    ln = lines.map(lambda line: line.strip('(').strip(')').split(',')).map(lambda line: float(line[3].strip().strip('\'')))
    print('number records: ' + str(ln.count()))
    
def count_unique_customers():
    ln = lines.map(lambda line: line.strip('(').strip(')').split(',')).map(lambda line: str(line[2].strip().strip('\''))).distinct()
    ln.saveAsTextFile(pathOut + 'customers')
    print('number unique customers: ' + str(ln.count()))
    
def count_unique_meters():
    ln = lines.map(lambda line: line.strip('(').strip(')').split(',')).map(lambda line: str(line[1].strip().strip('\''))).distinct()
    ln.saveAsTextFile(pathOut + 'meters') 
    print('number unique meters: ' + str(ln.count()))

def count_unique_timestamps():
    ln = lines.map(lambda line: line.strip('(').strip(')').split(',')).map(lambda line: (str(line[0].strip().strip('\'')), str(line[0].strip().strip('\'')))).distinct().sortBy(lambda line: line[0])
    ln2 = ln.map(lambda x: str(x[0]))
    ln2.saveAsTextFile(pathOut + 'timestamps') 
    print('number unique timestamps: ' + str(ln2.count()))
    print('min timestamp: ' + str(ln2.min()))
    print('max timestamp: ' + str(ln2.max()))

compute_mean_usage()    
count_records()
count_unique_customers()
count_unique_meters()
count_unique_timestamps()
