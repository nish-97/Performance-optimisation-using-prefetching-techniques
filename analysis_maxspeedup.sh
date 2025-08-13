#!/bin/bash

#max_speedup_degree=`expr 12 - $1`
# list=$(cat traces.txt) 
list=sssp
deg=2
dist=10
touch performance_stream_max.csv
echo "Benchmark,Speedup,L1D MPKI,L2C MPKI,Accuracy">>performance_stream_max.csv

for i in $list   #for all the traces
do
cd output/stream_pref
# row_value=$(ls | grep $i | grep "_ipstride_2" | sed 's/.champ.*//g' | sed 's/.trace.*//g')
row_value=$(ls | grep "$i" | grep "_$deg,$dist" | sed 's/.champ.*//g' | sed 's/.trace.*//g') 
#echo $trace0
cd ../..
# speedup=$(cat performance_stream.csv | grep $i | awk 'BEGIN{FS=","; maxspeed=10;} {print $10}') 
speedup=$(cat performance_stream.csv | grep "$row_value" |  grep "$deg,$dist" | awk 'BEGIN{FS=",";} {print $2}') 
echo $speedup
row_value="$row_value,$speedup"
cd output/stream_pref
#echo $trace0

# l1d_mpki_pref=$(cat $trace0 |grep "cpu0_L1D TOTAL     ACCESS" |  sed -E 's/[ ]+/ /g' | awk 'BEGIN {FS=" ";} {print $8}' )
# l2c_mpki_pref=$(cat $trace0 |grep "cpu0_L2C TOTAL     ACCESS" |  sed -E 's/[ ]+/ /g' | awk 'BEGIN {FS=" ";} {print $8}' )
name=$(ls | grep $i | grep "_$deg,$dist")
l1d_mpki_pref=$(cat $name | grep "cpu0_L1D LOAD      ACCESS:" |  sed -E 's/[ ]+/ /g' | awk 'BEGIN {FS=" ";} {print $8}' )
l2c_mpki_pref=$(cat $name | grep "cpu0_L2C LOAD      ACCESS:" |  sed -E 's/[ ]+/ /g' | awk 'BEGIN {FS=" ";} {print $8}' )
#echo $l1d_mpki_pref
#echo $l2c_mpki_pref

cd ../no_pref
file=$(ls | grep $i)
l1d_mpki_nopref=$(cat $file | grep "cpu0_L1D LOAD      ACCESS:" | sed -E 's/[ ]+/ /g' | awk 'BEGIN {FS=" ";} {print $8}' )
l2c_mpki_nopref=$(cat $file | grep "cpu0_L2C LOAD      ACCESS:" | sed -E 's/[ ]+/ /g' | awk 'BEGIN {FS=" ";} {print $8}')

l1d_nor_mpki=$(echo "scale=3; $l1d_mpki_pref / $l1d_mpki_nopref" | bc)
l2c_nor_mpki=$(echo "scale=3; $l2c_mpki_pref / $l2c_mpki_nopref" | bc)

cd ../stream_pref
accuracy_issued=$(cat $name | sed -E 's/[ ]+/ /g' | grep "cpu0_L2C PREFETCH REQUESTED" | awk 'BEGIN {FS=" ";} {print $6}' )
accuracy_useful=$(cat $name | sed -E 's/[ ]+/ /g' | grep "cpu0_L2C PREFETCH REQUESTED" | awk 'BEGIN {FS=" ";} {print $8}' )
accuracy=$(echo "scale=3; $accuracy_useful * 100 / $accuracy_issued" | bc)
cd ../..
row_value="$row_value,$l1d_nor_mpki,$l2c_nor_mpki,$accuracy"
echo $row_value >> performance_stream_max.csv
done

