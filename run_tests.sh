#!/bin/bash
nodes=$1
trials=$2
filename=$3
echo "file $filename"
if [ -e $filename ]; then
    rm $filename
    echo "yes"
else
    echo "no"
fi
echo "nodes = $nodes" >> $filename
echo "trials = $trials"
protocols=("OLSR" "AODV" "DSDV")
mov=("Grid" "RandomDir" "RandomRect")
for i in $(seq 0 2) 
do
	for j in $(seq 0 2)
	do
	    echo "${protocols[$i]} ${mov[$j]}"
	    echo "${protocols[$i]} ${mov[$j]}" >> $filename
	    for k in $(seq 1 $trials) 
	    do	    
	        rand=$RANDOM
	        echo "rand = $rand"
		    echo $rand
		    str="$(./waf --run "flat --mobilityModel=$j --routing=$i --numNodes=$nodes --seed=$rand")"
		    #echo $str		    
		    ans=$(echo $str | grep -oE "(([0-9]+(\.[0-9]+)?|-nan)[ ]?){6}")
		    echo $ans
		    echo $ans >> $filename
		    echo "out"
		 done
	done
done
