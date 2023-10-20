#!/bin/bash

#Host discovery based on ping. Only compatible with /24 networks
salida=""
if [ -z $1 ]
then
	echo "Error : not enough arguments"
	echo "Use: ./ping-hostDiscovery [network]"
	exit
fi

netid=`echo $1 | cut -d "." -f 1,2,3` 

for i in $(seq 1 254);
do 
	host=$netid.$i
       	if `ping $host -c 1 >/dev/null 2>/dev/null`
	then 
         	echo "$host is up"
        else	
		echo "$host is down"
	fi
done
