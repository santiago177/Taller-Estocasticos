#!/bin/bash
nodes=$1
trials=$2
if [ -e "results.txt" ]; then
    rm results.txt
    echo "yes"
else
    echo "no"
fi
echo "nodes = $nodes" >> results.txt
echo "trials = $trials"
protocols=("OLSR" "AODV" "DSDV")
mov=("Grid" "RandomDir" "RandomRect")
for i in $(seq 0 3) 
do
	for j in $(seq 0 3)
	do
	    echo "${protocols[$i]} ${mov[$j]}" >> results.txt
	    for k in $(seq 1 $trials) 
	    do	    
	        rand=$RANDOM
	        echo "rand = $rand"
		    echo $rand
		    str="$(./waf --run "flat --mobilityModel=$i --routing=$j --numNodes=$nodes --seed=$rand")"
		    #echo $str		    
		    ans=$(echo $str | grep -oE "(([0-9]+(\.[0-9]+)?|-nan)[ ]?){6}")
		    echo $ans
		    echo $ans >> results.txt
		    echo "out"
		 done
	done
done
