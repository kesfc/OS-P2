
#include "FCFS.h"

#include "algorithms.h"

// FCFS constructor implementation
FCFS::FCFS(std::string name, std::vector<Process> processes, int t_cs)
    : Algo(name, processes, t_cs) {}

void FCFS::newProcessRunCheck(){
    if (this->runningProcess == nullptr && !this->isLoadingProcess && !this->isRemovingProcess && !this->readyQueue.empty()) {
        this->isLoadingProcess = true;
        Process* p = this->readyQueue.front();
        Command c(this->currentTime + this->t_cs / 2, 2, p);
        addCommand(c, this->currentTime + this->t_cs / 2);
    }
}