#!/usr/bin/python
################################################################################
# process_reuslts.py - a script to aggregate the output from all program traces 
#   into a summary file with mean, and median information for each line
#   
################################################################################


import os
import sys
import numpy

#get a list of all files from target directory
target_dir = sys.argv[1]
output_filename = sys.argv[2]

files = [file for file in os.listdir(target_dir)]

#create our lists for different properties (each property is on a diff line)
#could implement as a list of lists, but am not for now
tot_run_time = []
dCache_tot_hit_rate = []
l2_tot_hit_rate = []
AMAT = []
AMQL = []
dCache_to_l2_bandwidth_util = []
mem_bandwidth_util = []

#read each file and add its contents to each list
for filename in files:
    filename = target_dir + "/" + filename
    file = open(filename)
    lines = file.readlines()
    tot_run_time.append(float(lines[0]))
    dCache_tot_hit_rate.append(float(lines[1]))
    l2_tot_hit_rate.append(float(lines[2]))
    AMAT.append(float(lines[3]))
    AMQL.append(float(lines[4]))
    dCache_to_l2_bandwidth_util.append(float(lines[5]))
    mem_bandwidth_util.append(float(lines[6]))
    file.close()
    
#calculate statistics and print to file
test = "AMAT mean: %10.02f\t\tmedian:%10.02f\n" \
% (numpy.mean(AMAT), numpy.median(AMAT))
print test

output_file = open(output_filename, 'a') 
output_file.write("%-30s mean: %10.02f\t\tmedian:%10.02f\n" \
% ("tot_run_time", numpy.mean(tot_run_time), numpy.median(tot_run_time)))

output_file.write("%-30s mean: %10.02f\t\tmedian:%10.02f\n" \
% ("dCache_tot_hit_rate", numpy.mean(dCache_tot_hit_rate), numpy.median(dCache_tot_hit_rate)))

output_file.write("%-30s mean: %10.02f\t\tmedian:%10.02f\n" \
% ("l2_tot_hit_rate", numpy.mean(l2_tot_hit_rate), numpy.median(l2_tot_hit_rate)))

output_file.write("%-30s mean: %10.02f\t\tmedian:%10.02f\n" \
% ("AMAT", numpy.mean(AMAT), numpy.median(AMAT)))

output_file.write("%-30s mean: %10.02f\t\tmedian:%10.02f\n" \
% ("AMQL", numpy.mean(AMQL), numpy.median(AMQL)))

output_file.write("%-30s mean: %10.02f\t\tmedian:%10.02f\n" \
% ("dCache_to_l2_bandwidth_util", numpy.mean(dCache_to_l2_bandwidth_util), numpy.median(dCache_to_l2_bandwidth_util)))

output_file.write("%-30s mean: %10.02f\t\tmedian:%10.02f\n" \
% ("mem_bandwidth_util", numpy.mean(mem_bandwidth_util), numpy.median(mem_bandwidth_util)))



