#include "algorithms.h"
#include "SRT.h"

void SRT::newProcessRunCheck(){
    if (this->runningProcess == nullptr && !this->isLoadingProcess && !this->isRemovingProcess && !this->readyQueue.empty()) {
        sort(this->readyQueue.begin(), this->readyQueue.end(), [](Process* a, Process* b) {
            return a->tau < b->tau;
        });
        this->isLoadingProcess = true;
        Process* p = this->readyQueue.front();
        Command c(this->currentTime + this->t_cs / 2, 2, p);
        addCommand(c, this->currentTime + this->t_cs / 2);
    }
}
string SRT::runningProcessName(Process & process){
    string name(1, process.process_name);
    name = name + " (tau " + to_string(process.tau) + "ms) ";
    return name;
}