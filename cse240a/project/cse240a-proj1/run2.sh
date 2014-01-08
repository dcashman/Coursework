#!/bin/bash
traces=proj-traces
cd ${traces}
for entry in *
do
    cd ..
    #echo ${build}/output/${entry}
    ./cacheSim ${traces}/${entry} 
    cd ${traces}
done
