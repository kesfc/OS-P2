#ifndef SJF_H  // Include guard to prevent multiple inclusions
#define SJF_H

#include <string>
#include <vector>  
#include "algorithms.h"
#include "Process.h"
using namespace std;

class SJF : public Algo {
public:
    SJF(std::string name, vector<Process> processes, int t_cs, double alpha);
    void newProcessRunCheck() override;
    string runningProcessName(Process & process) override;
    void FinishCpu(Process& process) override;
    void addProcessToQ(Process& process) override;

private:
    float alpha;
};
bool compareProcess(Process* a, Process* b);


#endif