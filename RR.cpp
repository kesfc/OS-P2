#include "RR.h"

class RR : public Algo {
public:
    RR(std::string name, std::vector<Process> processes, int t_cs, double alpha);
    void newProcessRunCheck() override;
    string runningProcessName(Process & process) override;
    void FinishCpu(Process& process) override;
    bool checkPreempt(Process & process) override; 

private:
    double alpha;
};
