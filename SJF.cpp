#include <iostream>
#include <string>
#include <vector>
#include "algorithms.h"
#include "SJF.h"
#include "cmath"
#include "Process.h"
using namespace std;

SJF::SJF(string name, vector<Process> processes, int t_cs, double alpha)
    : Algo(name, processes, t_cs), alpha(alpha) {}  
void SJF::ProcessArrival(Process& process) {
    this->readyQueue.push_back(&process);
    cout << "time " << this->currentTime << "ms: Process " << process.process_name << " (tau " << process.tau<< "ms) arrived; added to ready queue [Q" << GetQueueString() << "]" << endl;
    //no running process, run
    if (this->runningProcess == nullptr && !this->isLoadingProcess && !this->isRemovingProcess) {
        this->isLoadingProcess = true;

        Command c(process.arrival_time + this->t_cs / 2, 2, &process);
        addCommand(c, process.arrival_time + this->t_cs / 2);
    }
}

void SJF::StartCpu(Process& process) {
    this->isLoadingProcess = false;
    this->isRemovingProcess = false;
    this->runningProcess = &process;
    this->readyQueue.erase(this->readyQueue.begin());
    cout << "time " << this->currentTime << "ms: Process " << process.process_name << " (tau " << process.tau<< "ms)" << 
    " started using the CPU for " << this->runningProcess->getCurrentBurst() << "ms burst [Q" << GetQueueString() << "]" << endl;
    int endTime = this->currentTime + this->runningProcess->getCurrentBurst();
    Command c(endTime, 1, &process);
    addCommand(c, endTime);
}

void SJF::FinishCpu(Process& process) {
    double temp = this->alpha * this->runningProcess->getCurrentBurst() + (1 - this->alpha) * process.tau;
    int Newtau = std::ceil(temp);
    this->runningProcess->burst_remaining--;
    //if not terminated, start IO
    if (this->runningProcess->burst_remaining > 0) {
        cout << "time " << this->currentTime << "ms: Process " << process.process_name << " (tau " << process.tau<< "ms)" <<
        " completed a CPU burst; " << this->runningProcess->burst_remaining << " burst"<< (runningProcess->burst_remaining != 1 ? "s" : "") <<" to go [Q" << GetQueueString() << "]" << endl;
        int endTime = this->runningProcess->getCurrentIOBurst() + this->currentTime + this->t_cs/2;
        cout << "time "<< this->currentTime <<"ms: Recalculating tau for process "<< this->runningProcess->process_name << ": old tau "<< 
        this-> runningProcess -> tau<<"ms ==> new tau "<< Newtau<<"ms [Q"<< GetQueueString() << "]"<<endl;
        cout << "time " << this->currentTime << "ms: Process " << this->runningProcess->process_name << 
            " switching out of CPU; blocking on I/O until time " << endTime << "ms [Q" << GetQueueString() << "]" << endl;
        this->runningProcess->tau = Newtau;
        Command c(endTime, 3, this->runningProcess);
        addCommand(c, endTime);
        //context switching
        this->runningProcess = nullptr;
        this->isRemovingProcess = true;
        Command c2(this->currentTime + this->t_cs / 2, 0, &process);
        addCommand(c2, this->currentTime + this->t_cs / 2);
    }
    else {
        //last burst
        cout << "time " << this->currentTime << "ms: Process " << process.process_name << " terminated [Q" << GetQueueString() << "]" << endl;
        this->runningProcess = nullptr;
        this->isRemovingProcess = true;
        Command c2(this->currentTime + this->t_cs / 2, 0, &process);
        addCommand(c2, this->currentTime + this->t_cs / 2);
    }
}
void SJF::FinishIO(Process& process) {
    this->readyQueue.push_back(&process);
    sort(this->readyQueue.begin(), this->readyQueue.end(), [](Process* a, Process* b) {
            return a->tau < b->tau;
        });
    cout << "time " << this->currentTime << "ms: Process " << process.process_name << " (tau " << process.tau<< "ms)" << " completed I/O; added to ready queue [Q" << GetQueueString() << "]" << endl;
}
void SJF::newProcessRunCheck() {
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


