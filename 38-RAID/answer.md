## 38 RAID

### Homework (Simulation)

This section introduces raid.py, a simple RAID simulator you can use to shore up your knowledge of how RAID systems work. See the README for details.

### Questions

## TODO

**Use the simulator to perform some basic RAID mapping tests. Run with different levels (0, 1, 4, 5) and see if you can figure out the mappings of a set of requests. For RAID-5, see if you can figure out the difference between left-symmetric and left-asymmetric layouts.**

**Use some different random seeds to generate different problems than above.**

**Do the same as the first problem, but this time vary the chunk size with -C. How does chunk size change the mappings?**

**Do the same as above, but use the -r flag to reverse the nature of each problem.**

**Now use the reverse flag but increase the size of each request with the -S flag. Try specifying sizes of 8k, 12k, and 16k, while varying the RAID level. What happens to the underlying I/O pattern when the size of the request increases? Make sure to try this with the sequential workload too (-W sequential); for what request sizes are RAID-4 and RAID-5 much more I/O efficient?**

**Use the timing mode of the simulator (-t) to estimate the performance of 100 random reads to the RAID, while varying the RAID levels, using 4 disks.**

**Do the same as above, but increase the number of disks. How does the performance of each RAID level scale as the number of disks increases?**

**Do the same as above, but use all writes (-w 100) instead of reads. How does the performance of each RAID level scale now? Can you do a rough estimate of the time it will take to complete the workload of 100 random writes?**

**Run the timing mode one last time, but this time with a sequential workload (-W sequential). How does the performance vary with RAID level, and when doing reads versus writes? How about when varying the size of each request? What size should you write to a RAID when using RAID-4 or RAID-5?**
