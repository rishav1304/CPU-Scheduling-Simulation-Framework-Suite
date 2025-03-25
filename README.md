# CPU-Scheduling-Simulation-Framework-Suite

CPU Scheduling Simulation Framework Suite

This suite contains various CPU scheduling algorithms implemented in C++. These algorithms are designed to simulate and analyze different scheduling strategies commonly used in operating systems.

Algorithms Included First Come First Serve (FCFS) Round Robin (RR) with varying time quantum Shortest Process Next (SPN) Shortest Remaining Time (SRT) Highest Response Ratio Next (HRRN) Feedback (FB) Feedback with varying time quantum (FBV) Aging Each algorithm is implemented to demonstrate its specific scheduling strategy and behavior under different scenarios.

Input Format The input format for running simulations is as follows :-
                      Line 1: Specify whether to trace or output stats (trace or stats). 
                      Line 2: Comma-separated list of scheduling policies to analyze/visualize. 
                      Line 3: Last instant to be used in the simulation. 
                      Line 4: Number of processes to simulate. Subsequent lines: Description of each process according to the algorithm requirements.
