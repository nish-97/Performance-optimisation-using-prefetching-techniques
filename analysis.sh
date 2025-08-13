list=$(cat traces.txt)  
#touch performance.csv
#echo "Benchmark,Speedup_10,Speedup_9,Speedup_8,Speedup_7,Speedup_6,Speedup_5,Speedup_4,Speedup_3,Speedup_2,Speedup_1">>performance.csv
touch performance_stream_stride.csv
echo "Benchmark,Speedup,Degree,Distance">>performance_stream_stride.csv

for i in $list   #for all the traces
do
#i=602
k=1
cd output/no_pref
trace0=$(ls | grep $i)
#echo $trace0
row_value=$(echo $trace0 | sed 's/.champ.*//g' | sed 's/.trace.*//g') 
row_csv=$row_value
ipc_nopref=$(cat $trace0 | grep "cumulative IPC:" | tail -1 | cut --delimiter=" " -f  5)
cd ../..

for ((j=4 ; j>0; j--))
do
cd output/stream_stride_pref
trace1=$(ls | grep $i | head -$k | tail -1) 
#echo $trace1
ipc_pref=$(cat $trace1 | grep "cumulative IPC:" | tail -1 | cut --delimiter=" " -f  5)
speedup=$(echo "scale=3; $ipc_pref / $ipc_nopref" | bc)
deg=$(echo $trace1 | awk -F_ '{print $NF}' | awk -F, '{print $1}')
dist=$(echo $trace1 | awk -F_ '{print $NF}' | awk -F, '{print $2}' | awk -F. '{print $1}')
row_value="$row_csv,$speedup,$deg,$dist"

cd ../..
(echo $row_value) >> performance_stream_stride.csv 
k=$(($k + 1))
done
done
