#include <iostream>
#include <string>
#include "algorithms.h"
#include "SJF.h"
#include "cmath"

SJF::SJF(std::string name, std::vector<Process> processes, int t_cs, double alpha)
    : Algo(name, processes, t_cs), alpha(alpha) {}  
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

void SJF::StartCpu(Process& process) {
    this->isLoadingProcess = false;
    this->isRemovingProcess = false;
    this->runningProcess = &process;
    this->readyQueue.pop();
    cout << "time " << this->currentTime << "ms: Process " << process.process_name << " (tau " << process.tau<< "ms) " << this->runningProcess->process_name << 
    " started using the CPU for " << this->runningProcess->getCurrentBurst() << "ms burst [Q " << GetQueueString() << "]" << endl;
    int endTime = this->currentTime + this->runningProcess->getCurrentBurst();
    Command c(endTime, 1, &process);
    addCommand(c, endTime);
}

void SJF::FinishCpu(Process& process) {
    this->runningProcess->burst_remaining--;
    cout << "time " << this->currentTime << "ms: Process " << process.process_name << " (tau " << process.tau<< "ms) "<< this->runningProcess->process_name <<
        " completed a CPU burst; " << this->runningProcess->burst_remaining << " bursts to go [Q" << GetQueueString() << "]" << endl;
    
    //if not terminated, start IO
    if (this->runningProcess->burst_remaining > 0) {
        int endTime = this->runningProcess->getCurrentIOBurst() + this->currentTime + this->t_cs/2;
        cout << "time " << this->currentTime << "ms: Process " << this->runningProcess->process_name << 
            " switching out of CPU; blocking on I/O until time " << endTime << "ms [Q" << GetQueueString() << "]" << endl;
        this->runningProcess->burst_remaining++;
        double temp = this->alpha * this->runningProcess->getCurrentBurst() + (1 - this->alpha) * process.tau;
        int Newtau = std::ceil(temp);
        cout << "time "<< this->currentTime <<": Recalculating tau for process "<< this->runningProcess->process_name << ": old tau "<< 
        this-> runningProcess -> tau<<"ms ==> new tau "<< Newtau<<"ms [Q"<< GetQueueString() << "]"<<endl;
        this->runningProcess->tau = Newtau;
        this->runningProcess->burst_remaining--;

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
        cout << "time " << this->currentTime << "ms: Process " << process.process_name << " (tau " << process.tau<< "ms) " << this->runningProcess->process_name << " switching out of CPU." << endl;
        cout << "time " << this->currentTime << "ms: Process " << process.process_name << " (tau " << process.tau<< "ms) " << this->runningProcess->process_name << " terminated [Q" << GetQueueString() << "]" << endl;

        this->runningProcess = nullptr;
        this->isRemovingProcess = true;
        Command c2(this->currentTime + this->t_cs / 2, 0, &process);
        addCommand(c2, this->currentTime + this->t_cs / 2);
    }
}
void SJF::FinishIO(Process& process) {
    this->readyQueue.push(&process);
    cout << "time " << this->currentTime << "ms: Process " << process.process_name << " (tau " << process.tau<< "ms) " << process.process_name << " completed I/O; added to ready queue [Q" << GetQueueString() << "]" << endl;
}
