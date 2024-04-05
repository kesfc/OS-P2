#ifndef SRT_H
#define SRT_H
#include <string>
#include <vector>  
#include "algorithms.h"
#include "Process.h"

class SRT : public Algo {
public:
    SRT(std::string name, std::vector<Process> processes, int t_cs);
    void newProcessRunCheck() override;
    string runningProcessName(Process & process) override;
};
#endif