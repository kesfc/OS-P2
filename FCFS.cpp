
#include "FCFS.h"

#include "algorithms.h"

// FCFS constructor implementation
FCFS::FCFS(std::string name, std::vector<Process> processes, int t_cs)
    : Algo(name, processes, t_cs) {}

