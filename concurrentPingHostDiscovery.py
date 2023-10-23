import sys
import subprocess
from multiprocessing import Pool

def pingScan(hostid):
    if(hostid<255):
        #salida=""
        process = subprocess.Popen(['ping', '-c 1','sys.argv[1][0:-1]+str(hostid)'],stdout=subprocess.PIPE,universal_newlines=True)
        print(process.stdout.readline())
        if( "bytes from" in process.stdout.readline()):
            print(f"Host {sys.argv[1][0:-1]+str(hostid)} is up")

if __name__ == '__main__':
    nThreads=5
    #Error control, we make sure there is only one argument and it es a net address not a host one
    if(len(sys.argv)<2):
        print("Usage: concurrentPingHostDiscovery.py [Net address] [nThreads]")
        exit(1)
    if(len(sys.argv)>4):
        print("Usage: concurrentPingHostDiscovery.py [Net address] [nThreads]")
        exit(1)
    if(sys.argv[1][-2::] !=".0"):
        print("The net address must end in *.*.*.0")
        exit(1)
        
    if(len(sys.argv)==3):
        nThreads=int(sys.argv[2])
    hostid=0
    while(hostid<255):
        p=Pool(nThreads)
        p.map(pingScan,range(hostid,hostid+nThreads))
        print(";")
        hostid+=nThreads
        
        
    #        
    