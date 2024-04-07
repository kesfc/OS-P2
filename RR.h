#ifndef RR_H
#define RR_H
#include "algorithms.h"
class RR : public Algo {
public:
    RR(std::string name, std::vector<Process> processes, int t_cs, int t_slice);
    void newProcessRunCheck() override;
    void StartCpu(Process& process) override;
    void FinishCpu(Process& process) override;
private:
    int t_slice;
    int next_expire_time = 999999;
};

#endif
