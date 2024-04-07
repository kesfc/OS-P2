#include "RR.h"
#include "algorithms.h"
#include "cmath"
#include "Process.h"
#include <iostream>
using namespace std;

RR::RR(std::string name, std::vector<Process> processes, int t_cs, int t_slice)
: Algo(name, processes, t_cs), t_slice(t_slice){}

void RR::newProcessRunCheck(){
    if (this->runningProcess == nullptr && !this->isLoadingProcess && !this->isRemovingProcess && !this->readyQueue.empty()) {
        this->isLoadingProcess = true;
        Process* p = this->readyQueue.front();
        Command c(this->currentTime + this->t_cs / 2, 2, p);
        addCommand(c, this->currentTime + this->t_cs / 2);
    } else if (this->currentTime == this->next_expire_time && this->runningProcess != nullptr && this->runningProcess->burst_time_left > 0) {
        if (timeCheck())cout << "time " << this->currentTime << "ms: Time slice expired; ";
        if(this->readyQueue.empty()){
            if (timeCheck())cout << "no preemption because ready queue is empty [Q <empty>]" << endl;
        } else {
            // Perform a preemption.
            
            if (timeCheck())cout << "preempting process "<< this->runningProcessName(*this->runningProcess) << " with "<< this->runningProcess->burst_time_left <<
            "ms remaining [Q" << this->GetQueueString() << "]" << endl;
            // Schedule to remove the current running progress
            Process* p = this->readyQueue.front();
            Command c(this->currentTime + this->t_cs, 2, p);
            addCommand(c, this->currentTime + this->t_cs);

            // Run the front process in the ready queue
            if (runningProcess->isCpuBound) {
                this->cpuPreemption++;
            }
            else {
                this->ioPreemption++;
            }
            //this->readyQueue.erase(this->readyQueue.begin());
            Command c0(this->currentTime + this->t_cs / 2, -1, this->runningProcess);
            this->addCommand(c0, this->currentTime + this->t_cs / 2);

            this->runningProcess = nullptr;
            this->isRemovingProcess = true;
            this->isLoadingProcess = true;
        }
        
        this->next_expire_time = this->runningProcess == nullptr ? 999999 : this->currentTime + this->t_slice;
    }
}

void RR::StartCpu(Process & process){
    this->next_expire_time = this->currentTime + this->t_slice;
    Algo::StartCpu(process);
}

void RR::FinishCpu(Process& process){
    this->next_expire_time = 999999;
    Algo::FinishCpu(process);
}