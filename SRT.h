#ifndef SRT_H  // Include guard to prevent multiple inclusions
#define SRT_H

#include <string>
#include <vector>  
#include "algorithms.h"
#include "Process.h"
using namespace std;

class SRT : public Algo {
public:
    SRT(std::string name, vector<Process> processes, int t_cs, double alpha);
    void ProcessArrival(Process& process) override;
    void StartCpu(Process& process) override;
    void FinishCpu(Process& process) override;
    void FinishIO(Process& process) override;
    void newProcessRunCheck() override;
private:
    double alpha;
};


#endif