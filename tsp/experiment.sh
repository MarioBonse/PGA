#!/bin/bash

# rm data/thread_asus_multiple.txt
# rm data/omp_asus_multiple.txt
# rm data/ff_asus_multiple.txt

for((i=1;i<$1;i+=1));
do
    echo $i
  # ./build/TSP_thread graph/rat783.tsp config.txt $i >> data/thread_asus_multiple.txt
  # ./build/TSP_parfor graph/rat783.tsp config.txt $i >> data/omp_asus_multiple.txt
  ./build/TSP_ff graph/rat783.tsp config.txt $i >> data/ff_asus_multiple_v3.txt
done