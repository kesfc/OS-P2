#include <iostream>
#include <string>
#include "algorithms.h"
#include "SJF.h"

SJF::SJF(std::string name, std::vector<Process> processes, int t_cs, int alpha) : Algo(name, processes, t_cs) {
    this->alpha = alpha;
}

void SJF::ProcessArrival(Process& process) {
    this->readyQueue.push(&process);
    cout << process.process_name << endl;
    cout << "time " << this->currentTime << "ms: Process " << process.process_name << " (tau " << process.tau<< "ms) arrived; added to ready queue [Q" << GetQueueString() << "]" << endl;
    //no running process, run
    if (this->runningProcess == nullptr && !this->isLoadingProcess && !this->isRemovingProcess) {
        this->isLoadingProcess = true;

        Command c(process.arrival_time + this->t_cs / 2, 2, &process);
        addCommand(c, process.arrival_time + this->t_cs / 2);
    }
}
