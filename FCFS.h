#ifndef FCFS_H  // Include guard to prevent multiple inclusions
#define FCFS_H

#include <string>
#include <vector>  
#include "algorithms.h"
#include "Process.h"

class FCFS : public Algo {
public:
    FCFS(std::string name, std::vector<Process> processes, int t_cs);
    void newProcessRunCheck() override;
};

#endif