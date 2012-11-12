#!/bin/bash

# Ensure CPU Frequencies are set to the highest value (avoids dynamic clock).
# for i in {0..7}
# do
#     echo cpufreq-selector -g performance $i
#     cpufreq-selector -g performance $i
#     cpufreq-selector -g performance $i

#     echo

#     echo sudo cat /sys/devices/system/cpu/cpu$i/cpufreq/cpuinfo_cur_freq 
#     sudo cat /sys/devices/system/cpu/cpu$i/cpufreq/cpuinfo_cur_freq 
# done

# I disable dynamic clock in BIOS so all CPU's use the default frequency of 2.668 Ghz for my machine
./do_timings

