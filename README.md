# Job_Scheduler_Comparison_Simulation

## What it does

This is a simple C++ program written to simulate the performance of the Round Robin, non-preemptive Shortest Job First, and Shortest Remaining Time First scheduling algorithms. It will take an input file with a list of processes, their burst time, and arrival time separated by a space in between. A sample input file has been provided named "processes_input.txt".

The program will output the results in a Gantt chart format. It will print the name of the scheduling algorithm used on the top. First it will output an integer time stamp each time a process is scheduled, then the process ID, and then the reason for the context switch. After the program accepts the last process in the input list, the program will output the last time stamp and "Complete". There are three cases for context switching: process termination, quantum expried, and process preempted by another process with a shorter burst time. The program will output an average for the turnaround time and waiting time at the bottom of each Gantt chart.

Processes with the same burst length will be handled in alphabetical order. If a process A is preempted at the same time a new process B arrives, place process A in the before process B. A quantum of 3 is used for the Round Robin scheduler. Assume no preemption in the Shorest-Job First scheduler.

## How to compile the program
Type the command, "make" in the terminal to compile the program. The "make" command will access the file, makefile, and execute the instructions within. Type in terminal "make clean" to remove the program executable.

## Sample Run of the program
![](https://github.com/liuyang3141/Job_Scheduler_Comparison_Simulation/blob/main/img/Scheduler.gif)

## Disclaimer
I wrote this program as part of my computer science homework assignments at California State University East Bay. I've uploaded this particular assignment to Github as part of my portfolio. To students, you may use this as a study guide, but do not copy my work and plagiarize.