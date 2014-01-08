#!/bin/bash
build=$1  #directory in which our prefetcher version is found
traces=proj-traces
echo $build

#build new version
rm cacheSim
rm Makefile
rm ${build}/prefetcher.o
echo "PREFETCH_DIR = ${build}" > Makefile
cat makefile_source.txt >> Makefile
make

#create output dir
rm -r ./${build}/output
mkdir ./${build}/output

#run tests
cd ${traces}
for entry in *
do
    cd ..
    #echo ${build}/output/${entry}
    ./cacheSim ${traces}/${entry} ${build}/output/${entry}
    cd ${traces}
done

#now aggregate results using python script
cd ..
./process_results.py ${build}/output ${build}/output/aggregate.out



