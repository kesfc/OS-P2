#include "algorithms.h"
#include "SRT.h"
#include "cmath"
#include "Process.h"
using namespace std;

SRT::SRT(string name, vector<Process> processes, int t_cs, double alpha)
    : Algo(name, processes, t_cs), alpha(alpha) {
        this->contain_preemption = true;
}  

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

void SRT::FinishCpu(Process& process) {
    double temp = this->alpha * this->runningProcess->getCurrentBurst() + (1 - this->alpha) * process.tau;
    int Newtau = std::ceil(temp);
    this->runningProcess->burst_remaining--;
    //if not terminated, start IO
    if (this->runningProcess->burst_remaining > 0) {
        cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(process) << this->runningProcess->process_name <<
        " completed a CPU burst; " << this->runningProcess->burst_remaining << " bursts to go [Q" << GetQueueString() << "]" << endl;
        int endTime = this->runningProcess->getCurrentIOBurst() + this->currentTime + this->t_cs/2;
        cout << "time " << this->currentTime << "ms: Process " << this->runningProcess->process_name << 
            " switching out of CPU; blocking on I/O until time " << endTime << "ms [Q" << GetQueueString() << "]" << endl;
        cout << "time "<< this->currentTime <<"ms: Recalculating tau for process "<< this->runningProcess->process_name << ": old tau "<< 
        this-> runningProcess -> tau<<"ms ==> new tau "<< Newtau<<"ms [Q"<< GetQueueString() << "]"<<endl;
        this->runningProcess->tau = Newtau;
        Command c(endTime, 3, this->runningProcess);
        addCommand(c, endTime);
    }
    else {
        //last burst
        cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(*this->runningProcess) << 
        "terminated [Q" << GetQueueString() << "]" << endl;
    }
    //context switching
    this->runningProcess = nullptr;
    this->isRemovingProcess = true;
    Command c2(this->currentTime + this->t_cs / 2, 0, &process);
    addCommand(c2, this->currentTime + this->t_cs / 2);
}