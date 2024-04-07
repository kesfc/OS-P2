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
    } else if (this->currentTime == this->next_expire_time) {
        cout << "time " << this->currentTime << "ms: Time slice expired; ";
        if(this->readyQueue.empty()){
            cout << "no preemption because ready queue is empty [Q <empty>]" << endl;
        } else {
            // Perform a preemption.
            Process* p = this->readyQueue.front();
            cout << "preempting process "<< this->runningProcessName(*this->runningProcess) << " with "<< this->runningProcess->burst_time_left <<
            "ms remaining [Q" << this->GetQueueString() << "]" << endl;
            
            this->readyQueue.erase(this->readyQueue.begin());
            this->readyQueue.push_back(this->runningProcess);
            Command c(this->currentTime + this->t_cs, 2, p);
            addCommand(c, this->currentTime + this->t_cs);
            this->runningProcess = nullptr;
            this->isRemovingProcess = true;
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