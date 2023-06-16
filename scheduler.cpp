#include <iostream> // for input output
#include <string> // to use the string class
#include <fstream> // to read input file
#include <queue> // use as a data structure for scheduling algorithm
#include <sstream> // use as tokenizer function
#include <vector> // use as a data structure for scheduling algorithm
#include <iomanip> // use to format output to console
#include <algorithm> // for sorting algorithm
using namespace std;

struct Process {
    string ID; // process ID
    int burst; // remaining CPU run time
    int cpuRunTime; // static CPU run time
    int arriveTime; // a dynamic variable to keep track of when a process is put back in ready queue for Round Robin algorithm
    int submitTime; // keep track of when a job is scheduled
    int completeTime; // keep track of when a job finishes running
    int turnaroundTime; // record turn around time
    int waitTime; // record wait time
};

void roundRobin(vector<Process>);
void shortestJobFirst(vector<Process>);
void shortestRemainingTimeFirst(vector<Process>);

//below are helper functions
void rrQuantumExpired(int, int &, vector<Process> &, queue<Process> &); // used in RR scheduling algorithm to update data structures when quantum expires

void putProcessInReadyQRR(int, vector<Process> &, vector<Process> &); // puts a process from job queue in the ready queue

void putProcessInReadyQSRTF(int, vector<Process> &, vector<Process> &); // puts a process from job queue in the ready queue for the SRTF algorithm

void putProcessInReadyQSJF(int, vector<Process> &, vector<Process> &); // puts a process from job queue in the ready queue for SJF algorithm

void putProcessInRunQ(int, queue<Process> &, vector<Process>  &); // puts a process from job queue in the ready queue for SJF scheduling algorithm

void preemptSRTF(int, queue<Process> &, vector<Process> &); // preempts current process in run queue with a process in ready queue that has shorter burst time

void calcTurnAndWaitTimes(int, queue<Process> &, vector<Process> &); // calculates turnaround and wait times when a process is done executing

void printResults(int, vector<Process>);// prints results of scheduling algorithms

bool srtfSort(Process, Process ); // a method to compare data members inside Process for the sorting function

bool sjfSort(Process, Process); // a a method to compare data members inside Process for the sorting function

int main(int argc, char* argv[])    {
    string line; // stores each line of input file
    string tokens[3]; // stores input as tokens in this array
    fstream inputFile; // stores file name for input file
    vector<Process> jobQ; // used for storing the processes in the input file
    inputFile.open(argv[1]); // opening input file

    if (inputFile.is_open()) {
        while (getline(inputFile, line)) { // read a line from the input file
            Process tmpProc; // create a temporary Process to store the data from current line of input file
            vector<string> tokens; // make a data structure to store the tokens
            string tmpString; // separated tokens are individually stored in this string
            stringstream lineStream(line); // store a line of the input into the stream

            while(getline(lineStream, tmpString, ' ')) {
                tokens.push_back(tmpString);
            }
            
            // populating the Process object then storing it in the jobQ
            tmpProc.ID = tokens.at(0);
            tmpProc.burst = stoi(tokens.at(1));
            tmpProc.cpuRunTime = tmpProc.burst;
            tmpProc.arriveTime = stoi(tokens.at(2));
            tmpProc.submitTime = tmpProc.arriveTime;
            jobQ.push_back(tmpProc); 
        }
    }

    roundRobin(jobQ);
    shortestJobFirst(jobQ);
    shortestRemainingTimeFirst(jobQ);
}

void shortestRemainingTimeFirst(vector<Process> jobQ) {
    int time = 0; // keeps track of time as it's incremented
    bool jobQEmpty = true; // to see if there are still jobs that needs to be scheduled
    vector<Process> readyQ; // represents the ready queue in the scheduling algorithm
    queue<Process> runQ; // represents the process currently running on the CPU in the scheduling algorithm

    // initializing values
    for (int x = 0; x < jobQ.size(); ++x) {
        jobQ.at(x).completeTime = 0;
        jobQ.at(x).turnaroundTime = 0;
        jobQ.at(x).waitTime = 0;
    }

    cout << "Shortest Time Remaining First Scheduling" << endl;

    while(true) {
        // if completeTime = 0 then there are still jobs in the job queue needing to be scheduled
        for (int x = 0; x < jobQ.size(); ++x) {
            if (jobQ.at(x).completeTime == 0)
                jobQEmpty = false;
        }

        if (jobQEmpty == false) {
            putProcessInReadyQSRTF(time, readyQ, jobQ);

            if (readyQ.size() == 0) {
                if (runQ.size() != 0) {
                    if (runQ.front().burst == 0) { // job is finished running
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                    }
                }
            }
            // if ready queue is not empty
            else {
                if (runQ.size() == 0) {
                    putProcessInRunQ(time, runQ, readyQ);
                }
                // if run queue is not empty
                else {
                    if (runQ.front().burst == 0) { // job is finished running
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                        runQ.push(readyQ.front());
                        readyQ.erase(readyQ.begin());
                        cout << time << " " << runQ.front().ID << " " << "Process Terminated" << endl;
                    }
                    // if burst not equal to 0
                    else {
                        preemptSRTF(time, runQ, readyQ);
                    }
                }
            }
        }
        // if job queue is empty
        else {
            if (readyQ.size() == 0) {
                if (runQ.size() == 0) {
                    break;
                }
                // if run queue is not empty
                else {
                    if (runQ.front().burst == 0) { // job is finished running
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                    }
                }
            }
            // if ready queue is not empty
            else {
                if (runQ.size() == 0) {
                    putProcessInRunQ(time, runQ, readyQ);
                }
                // if run queue is not empty
                else {
                    if (runQ.front().burst == 0) { // job is finished running
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                        runQ.push(readyQ.front());
                        readyQ.erase(readyQ.begin());
                        cout << time << " " << runQ.front().ID << " " << "Process Terminated" << endl;
                    }
                    // if burst not equal to 0
                    else {
                        // if burst time of currently running job is less than the first job in the ready queue, then switch them out
                        if(runQ.front().burst > readyQ.front().burst) {
                            preemptSRTF(time, runQ, readyQ);
                        } 
                    }
                }
            }
        }

        jobQEmpty = true; 
        ++time; // increment timer
        --runQ.front().burst; // decrement burst time of currently running job
    }

    printResults(time, jobQ);
}

void shortestJobFirst(vector<Process> jobQ) {
    int time = 0;
    bool jobQEmpty = true;
    vector<Process> readyQ;
    queue<Process> runQ;

    // initializing values
    for (int x = 0; x < jobQ.size(); ++x) {
        jobQ.at(x).completeTime = 0;
        jobQ.at(x).turnaroundTime = 0;
        jobQ.at(x).waitTime = 0;
    }

    cout << "Shortest Job First Scheduling" << endl;

    while (true) {
        // if completeTime = 0 then there are still jobs in the job queue needing to be scheduled
        for (int x = 0; x < jobQ.size(); ++x) {
            if (jobQ.at(x).completeTime == 0)
                jobQEmpty = false;
        }

        //if job queue is not empty
        if (jobQEmpty == false) {
            putProcessInReadyQSJF(time, readyQ, jobQ);

            if (readyQ.size() == 0) {
                if (runQ.size() != 0) {
                    if (runQ.front().burst == 0) { // job is finished running
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                    }
                }
            }
            // if ready queue is not empty
            else {
                if (runQ.size() == 0) {
                    putProcessInRunQ(time, runQ, readyQ);
                }
                // if run queue is not empty
                else {
                    if (runQ.front().burst == 0) { // job is finished running
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                        runQ.push(readyQ.front());
                        readyQ.erase(readyQ.begin());
                        cout << time << " " << runQ.front().ID << " " << "Process Terminated" << endl;
                    }
                }
            }
        }
        // job queue is empty
        else {
            if (readyQ.size() == 0) {
                if (runQ.size() == 0) {
                    break;
                }
                //if run queue is not empty
                else {
                    if (runQ.front().burst == 0) { // job is finished running
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                    }
                }
            }
            // if ready queue is not empty
            else {
                if (runQ.size() == 0) {
                    putProcessInRunQ(time, runQ, readyQ);
                }
                // if run queue is not empty
                else {
                    if (runQ.front().burst == 0) { // job is done running
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                        runQ.push(readyQ.front());
                        readyQ.erase(readyQ.begin());
                        cout << time << " " << runQ.front().ID << " " << "Process Terminated" << endl;
                    }
                }
            }
        }
        
        jobQEmpty = true;
        ++time;
        --runQ.front().burst;
    }
    
   printResults(time, jobQ);
}

void roundRobin(vector<Process> jobQ) {
    int time = 0;
    int quantum = 2;
    bool jobQEmpty = true;
    vector<Process> readyQ;
    queue<Process> runQ;
    
    // initializing values
    for (int x = 0; x < jobQ.size(); ++x) {
        jobQ.at(x).completeTime = 0;
        jobQ.at(x).turnaroundTime = 0;
        jobQ.at(x).waitTime = 0;
    }

    cout << "Round Robin Scheduling" << endl;

    while(true) {
        // if completeTime = 0 then there are still jobs in the job queue needing to be scheduled
        for (int x = 0; x < jobQ.size(); ++x) {
            if (jobQ.at(x).completeTime == 0) {
                jobQEmpty = false;
                break;
            }
        }  

        // if there are jobs in job queue
        if (jobQEmpty == false)  {
            putProcessInReadyQRR(time, readyQ, jobQ);

            if (readyQ.size() == 0) {
                if (runQ.size() == 0) {
                    if (quantum == 0)
                        quantum = 3;
                }
                // run queue is not empty
                else {
                    if (runQ.front().burst == 0) { // job has finished running
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                        quantum = 3;
                    }
                    // if burst is not 0
                    else {
                        if (quantum == 0) { // quantim expired
                            rrQuantumExpired(time, quantum, readyQ, runQ);
                        }
                    }
                }
            }
            // if ready queue is not empty
            else {
                if (runQ.size() == 0) {
                    putProcessInRunQ(time, runQ, readyQ);

                    if (quantum == 0) //reset quantum if it has expired
                        quantum = 3;
                }
                // if run queue is not empty
                else {
                    if (runQ.front().burst == 0) { // job has finished running
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                        runQ.push(readyQ.front());
                        readyQ.erase(readyQ.begin());
                        quantum = 3;
                        cout << time << " " << runQ.front().ID << " " << "Process terminated" << endl;
                    }
                    // burst not equal 0
                    else {
                        if (quantum == 0) {
                            rrQuantumExpired(time, quantum, readyQ, runQ);
                        }
                    }
                }
            }
        }
        // if job queue is empty
        else {
            if (readyQ.size() == 0) {
                if (runQ.size() == 0) {
                   break;
                }
                // if run queue is not empty
                else {
                    if (runQ.front().burst == 0) {
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                        quantum = 3;
                    }
                    // burst is not equal to 0
                    else {
                        if (quantum == 0) {
                            rrQuantumExpired(time, quantum, readyQ, runQ);
                        }
                    }
                }
            }
            // if ready queue is not empty
            else {
                if (runQ.size() == 0) {
                    putProcessInRunQ(time, runQ, readyQ);

                    if (quantum == 0)
                        quantum = 3;
                }
                // runQ not empty
                else {
                    if (runQ.front().burst == 0) {
                        calcTurnAndWaitTimes(time, runQ, jobQ);
                        runQ.push(readyQ.front());
                        readyQ.erase(readyQ.begin());
                        quantum = 3;
                        cout << time << " " << runQ.front().ID << " " << "Process terminated" << endl;
                    }
                    // burst not equal 0
                    else {
                        if (quantum == 0) {
                            rrQuantumExpired(time, quantum, readyQ, runQ);
                        }
                    }
                }
            }
        }
    
        if(runQ.size() != 0){
            --runQ.front().burst;
        }
   
        jobQEmpty = true;
        ++time;
        --quantum;
    }

   printResults(time, jobQ);
}

void printResults(int time, vector<Process> jobQ) {
    int avgTurnaround = 0; // stores average turnaround time
    int avgWaiting = 0; // stores average wait time
    string turntime; // for displaying calculations in the output table
    string waittime; // for displaying calculations in the output table

    // outputting results and table
    cout << time - 1 << " Complete" << endl << endl;
    cout << setfill(' ') << setw(15) << left << "Process ID" << setw(20) << "Turnaround Time" << setw(15) << "Waiting Time" << endl;

    for (int x = 0; x < jobQ.size(); ++x) {
        cout << setfill(' ') << setw(15) << left << jobQ.at(x).ID << setw(20) << jobQ.at(x).turnaroundTime << setw(15) << jobQ.at(x).waitTime << endl;
        avgTurnaround += jobQ.at(x).turnaroundTime;
        avgWaiting += jobQ.at(x).waitTime;
    }

    turntime = to_string(avgTurnaround) + "/" + to_string(jobQ.size()) + "=" + to_string(double(avgTurnaround)/jobQ.size());

    waittime = to_string(avgWaiting) + "/" + to_string(jobQ.size()) + "=" + to_string(double(avgWaiting)/jobQ.size());
    cout << setfill(' ') << setw(15) << left << "Average" << setw(20) << turntime << setw(15) << waittime << endl << endl;
}

void rrQuantumExpired(int time, int &quantum, vector<Process> &readyQ, queue<Process> &runQ) {
    runQ.front().arriveTime = time; // update the time when job was switched out
    readyQ.push_back(runQ.front()); // put job back into ready queue
    runQ.pop(); // remove current job from CPU
    runQ.push(readyQ.front()); // add another job to CPU from head of the ready queue
    readyQ.erase(readyQ.begin()); // remove job just added to CPU from head of ready queue
    quantum = 3; // reset quantum
    cout << time << " " << runQ.front().ID << " " << "Quantum expired" << endl;
}

void putProcessInReadyQRR(int time, vector<Process> &readyQ, vector<Process> &jobQ) {
    // put jobs in ready queue from job queue
    for (int x = 0; x < jobQ.size(); ++x) {
        if (jobQ.at(x).arriveTime == time) {
            readyQ.push_back(jobQ.at(x));

            // reorder processes alphabetically in ready queue if arrival times are the same
            if (readyQ.size() > 1) {
                for (int i = 0; i < readyQ.size(); ++i) {
                    for (int y = i + 1; y < readyQ.size(); ++y) {
                        if (readyQ.at(i).arriveTime == readyQ.at(y).arriveTime) {
                            if (readyQ.at(y).ID < readyQ.at(i).ID) {
                                Process tmp = readyQ.at(i);
                                readyQ.at(i) = readyQ.at(y);
                                readyQ.at(y) = tmp;
                            }
                        }
                    }
                }
            }
        }  
    }
}

void putProcessInReadyQSJF(int time, vector<Process> &readyQ, vector<Process> &jobQ) {
    // put jobs in ready queue from job queue
    for (int x = 0; x < jobQ.size(); ++x) {
        if (jobQ.at(x).arriveTime == time) {
            readyQ.push_back(jobQ.at(x));

            // reorder processes by shorest cpu run time
            if (readyQ.size() > 1)
                sort(readyQ.begin(), readyQ.end(), sjfSort);
            
            // reorder processes alphabetically in ready queue if cpu run times are the same
            if (readyQ.size() > 1) {
                for (int i = 0; i < readyQ.size(); ++i) {
                    for (int y = i + 1; y < readyQ.size(); ++y) {
                        if (readyQ.at(i).cpuRunTime == readyQ.at(y).cpuRunTime) {
                            if (readyQ.at(y).ID < readyQ.at(i).ID) {
                                Process tmp = readyQ.at(i);
                                readyQ.at(i) = readyQ.at(y);
                                readyQ.at(y) = tmp;
                            }
                        }
                    }
                }
            }
        }  
    }
}

void putProcessInReadyQSRTF(int time, vector<Process> &readyQ, vector<Process> &jobQ) {
    // put jobs in ready queue from job queue
    for (int x = 0; x < jobQ.size(); ++x) {
        if (jobQ.at(x).arriveTime == time) {
            readyQ.push_back(jobQ.at(x));

            // reorder processes by burst time
            if (readyQ.size() > 1)
                sort(readyQ.begin(), readyQ.end(), srtfSort);
            
            // reorder processes alphabetically in ready queue if burst times are the same
            if (readyQ.size() > 1) {
                for (int i = 0; i < readyQ.size(); ++i) {
                    for (int y = i + 1; y < readyQ.size(); ++y) {
                        if (readyQ.at(i).burst == readyQ.at(y).burst) {
                            if (readyQ.at(y).ID < readyQ.at(i).ID) {
                                Process tmp = readyQ.at(i);
                                readyQ.at(i) = readyQ.at(y);
                                readyQ.at(y) = tmp;
                            }
                        }
                    }
                }
            }
        }  
    }
}

void putProcessInRunQ(int time, queue<Process> &runQ, vector<Process>  &readyQ) {
    // adding job to CPU by removing a process from the head of the ready queue
    runQ.push(readyQ.front());
    readyQ.erase(readyQ.begin());
    cout << time << " " << runQ.front().ID << " " << "Process Terminated" << endl;
}


void preemptSRTF(int time, queue<Process> &runQ, vector<Process> &readyQ) {
    // if burst time of currently running job is less than the first job in the ready queue, then switch them out
    if(runQ.front().burst > readyQ.front().burst) {
        readyQ.push_back(runQ.front());

        // reorder processes by shorest cpu run time
        if (readyQ.size() > 1)
            sort(readyQ.begin(), readyQ.end(), srtfSort);

        // removing current process from CPU, then adding another to CPU by removing the first process in ready queue
        runQ.pop();
        runQ.push(readyQ.front());
        readyQ.erase(readyQ.begin());
        cout << time << " " << runQ.front().ID << " " << "Process preempted by process with shorter burst time" << endl;
    }
}

void calcTurnAndWaitTimes(int time, queue<Process> &runQ, vector<Process> &jobQ) {
    // calculating completion time, turnaround time, and wait time
    runQ.front().completeTime = time;
    runQ.front().turnaroundTime = runQ.front().completeTime - runQ.front().submitTime;
    runQ.front().waitTime = runQ.front().turnaroundTime - runQ.front().cpuRunTime;
    
    // updating job queue after a process has finished running
    for (int x = 0; x < jobQ.size(); ++x) {
        if (jobQ.at(x).ID == runQ.front().ID) {
            runQ.front().arriveTime = time;
            jobQ.at(x) = runQ.front();
        }
    }

    // removing current process from CPU
    runQ.pop();
}

bool sjfSort(Process a, Process b) {
    return a.cpuRunTime < b.cpuRunTime;
}

bool srtfSort(Process a, Process b) {
    return a.burst < b.burst;
}
