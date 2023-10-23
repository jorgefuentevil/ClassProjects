#Host discovery script with concurrency, it just works in linux for /24 networks  
# by Jorge de la Fuente Villanueva

import sys
import subprocess
from threading import Thread

#Tread function, it pings the ip sent and prints if host is up
def pingScan(hostid):
    process = subprocess.Popen(['ping', '-c 1',sys.argv[1][0:-1]+str(hostid)],stdout=subprocess.PIPE,universal_newlines=True)
    output = process.stdout.readlines()
    if( "bytes from" in output[1]):
        print(f"Host {sys.argv[1][0:-1]+str(hostid)} is up")

if __name__ == '__main__':
    
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

    hostid=0
    while(hostid<255):
        #Create the thread and start it
        p=Thread(target=pingScan,args=(hostid,))
        p.start()
        hostid+=1