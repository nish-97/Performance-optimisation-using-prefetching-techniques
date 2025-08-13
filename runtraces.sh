#a=$(grep PREFETCH_DEGREE prefetcher/ip_stride/ip_stride.cc|cut --delimiter="=" -f 2|head -1|sed 's/ \|;//g')
#!/bin/bash
<<COMMENT
dir=pwd
cd traces
for i in $(ls) #traces names
do
cd ..
sed -i '86s/"prefetcher": ".*"/"prefetcher": "no"/g' champsim_config.json
chmod +x config.sh
./config.sh champsim_config.json
make
bin/champsim --warmup_instructions=25000000 --simulation_instructions=25000000 traces/$i > output/no_pref/${i}_noprefetcher.txt &
cd traces
done
wait
COMMENT



#cd ..
#j=10
#while [ $j -ge 1 ]; #prefetch degree
sed -i '86s/"prefetcher": ".*"/"prefetcher": "stream_plus_stride_prefetcher"/g' champsim_config.json
pf_dg=(2 3)
pf_dis=(10 20)


for val in ${pf_dg[@]}
do
sed -i "s/constexpr int PREFETCH_DEGREE = .*;/constexpr int PREFETCH_DEGREE = $val;/g" prefetcher/stream_plus_stride_prefetcher/stream_plus_stride_prefetcher.cc

for nval in ${pf_dis[@]}
do
sed -i "s/constexpr int PREFETCH_DISTANCE = .*;/constexpr int PREFETCH_DISTANCE = $nval;/g" prefetcher/stream_plus_stride_prefetcher/stream_plus_stride_prefetcher.cc
./config.sh champsim_config.json
make

for i in $(cat traces.txt) #traces names
do
bin/champsim --warmup_instructions=25000000 --simulation_instructions=25000000 traces/$i > output/stream_stride_pref/${i}_stream_stride_$val,$nval.txt &
done
wait

done
done
#j=$((j-1))


