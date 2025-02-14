## 37 Hard Disk Drivers

### Homework (Simulation)

This homework uses disk.py to familiarize you with how a modern hard drive works. It has a lot of different options, and unlike most of the other simulations, has a graphical animator to show you exactly what happens when the disk is in action. See the README for details.

### Questions

**Compute the seek, rotation, and transfer times for the following sets of requests: -a 0, -a 6, -a 30, -a 7,30,8, and finally -a 10,11,12,13.**

A:

* -a 0 : 5.5 * 30 + 30
* -a 6 : 11.5 * 30 + 30
* -a 30 : 0.5 * 30(startup) + (40 * 2)(seek) + 30(transfer) + ...
* -a 7,30,8 :  0.5 * 30(startup) + (40 * 4)(seek) + 90(transfer) + ...
* -a 10,11,12,13 :  0.5 * 30(startup)+ (40 * 1)(seek) + 120(transfer) + ...
   

**Do the same requests above, but change the seek rate to different values: -S 2, -S 4, -S 8, -S 10, -S 40, -S 0.1. How do the times change?**

A: See [benchmark_withS.png](benchmark_withS.png)

**Do the same requests above, but change the rotation rate: -R 0.1, -R 0.5, -R 0.01. How do the times change?**

A: See [benchmark_withR.png](benchmark_withR.png)

**FIFO is not always best, e.g., with the request stream -a 7,30,8, what order should the requests be processed in? Run the shortest seek-time first(SSTF) scheduler (-p SSTF) on this workload; how long should it take (seek, rotation, transfer) for each request to be served?**

A: -p SSTF: 375  -p FIFO: 795  差距在Rotate上

**Now use the shortest access-time first (SATF) scheduler (-p SATF). Does it make any difference for -a 7,30,8 workload? Find a set of requests where SATF outperforms SSTF; more generally, when is SATF better than SSTF?**

A: 

* -p SSTF: 375  -p SATF： 375 没有区别
* 寻道时间短于旋转时间

**Here is a request stream to try: -a 10,11,12,13. What goes poorly when it runs? Try adding track skew to address this problem (-o skew). Given the default seek rate, what should the skew be to maximize performance? What about for different seek rates (e.g., -S 2, -S 4)? In general, could you write a formula to figure out the skew?**

A: -o 2 可以在-S默认的情况下达到最优

**Specify a disk with different density per zone, e.g., -z 10,20,30, which specifies the angular difference between blocks on the outer, middle, and inner tracks. Run some random requests (e.g., -a -1 -A 5,-1,0, which specifies that random requests should be used via the -a -1 flag and that five requests ranging from 0 to the max be generated), and compute the seek, rotation, and transfer times. Use different random seeds. What is the bandwidth (in sectors per unit time) on the outer, middle, and inner tracks?**

A: 

**A scheduling window determines how many requests the disk can examine at once. Generate random workloads (e.g., -A 1000,-1,0, with different seeds) and see how long the SATF scheduler takes when the scheduling window is changed from 1 up to the number of requests. How big of a window is needed to maximize performance? Hint: use the -c flag and don’t turn on graphics (-G) to run these quickly. When the scheduling window is set to 1, does it matter which policy you are using?**

A: 窗口为1时不同策略性能相近，See [w_performance.png](w_performance.png)

**Create a series of requests to starve a particular request, assuming an SATF policy. Given that sequence, how does it perform if you use a bounded SATF (BSATF) scheduling approach? In this approach, you specify the scheduling window (e.g., -w 4); the scheduler only moves onto the next window of requests when all requests in the current window have been serviced. Does this solve starvation? How does it perform, as compared to SATF? In general, how should a disk make this trade-off between perfor￾mance and starvation avoidance?**



**All the scheduling policies we have looked at thus far are greedy; they pick the next best option instead of looking for an optimal schedule. Can you find a set of requests in which greedy is not optimal?**


  