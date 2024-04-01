#ifndef SJF_H  // Include guard to prevent multiple inclusions
#define SJF_H

#include <string>
#include <vector>  
#include "algorithms.h"
#include "Process.h"

class SJF{
public:
    std::string name;
    std::vector<Process> processes;
    int t_cs;
    int initial_tau;
    int alpha;

    Process* runningProcess = NULL;
    int currentTime = 0;
    int turnAroundTime_cpu = 0;
    int turnAroundTime_io = 0;
    int waitTime_cpu = 0;
    int waitTime_io = 0;
    int numOfSwitch_cpu = 0;
    int numOfSwitch_io = 0;
    int numOfPreemption_cpu = 0;
    int numOfPreemption_io = 0;
    bool isRemovingProcess = false;
    bool isLoadingProcess = false;
    SJF(std::string name, std::vector<Process> processes, int t_cs, int alpha, int lambda);
    void Start();
private:
    void setup();
};


#endif