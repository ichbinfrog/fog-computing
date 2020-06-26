#!/bin/bash


N=8
(
for file in $(find scratch/ -mindepth 1 -maxdepth 1 -name "*100*"); do
	((i=i%N)); ((i++==0)) && wait
	sudo ./waf --run=`basename $file` &
done
)
