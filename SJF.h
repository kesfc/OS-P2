#ifndef SJF_H  // Include guard to prevent multiple inclusions
#define SJF_H

#include <string>
#include <vector>  
#include "algorithms.h"
#include "Process.h"

class SJF : public Algo {
public:
    SJF(std::string name, std::vector<Process> processes, int t_cs, double alpha);
    void ProcessArrival(Process& process) override;
    void StartCpu(Process& process) override;
    void FinishCpu(Process& process) override;
    void FinishIO(Process& process) override;
private:
    double alpha;
};


#endif