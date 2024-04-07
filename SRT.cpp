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
        sort(this->readyQueue.begin(), this->readyQueue.end(), compareProcesses);
        this->isLoadingProcess = true;
        Process* p = this->readyQueue.front();
        
        Command c(this->currentTime + this->t_cs / 2, 2, p);
        addCommand(c, this->currentTime + this->t_cs / 2);

    }
}

string SRT::runningProcessName(Process & process){
    string name(1, process.process_name);
    name = name + " (tau " + to_string(process.tau) + "ms)";
    return name;
}

void SRT::FinishCpu(Process& process) {
    double temp = this->alpha * this->runningProcess->getCurrentBurst() + (1.0 - this->alpha) * process.tau;
    int Newtau = std::ceil(temp);
    this->runningProcess->burst_remaining--;
    this->runningProcess->burst_time_left = -1;
    //if not terminated, start IO
    if (this->runningProcess->burst_remaining > 0) {
        if (timeCheck())cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(process) <<
        " completed a CPU burst; " << this->runningProcess->burst_remaining << " burst" << (this->runningProcess->burst_remaining > 1 ? "s" : "")
        << " to go [Q" << GetQueueString() << "]" << endl;

        if (timeCheck())cout << "time "<< this->currentTime <<"ms: Recalculating tau for process "<< this->runningProcess->process_name << ": old tau "<<
        this-> runningProcess -> tau<<"ms ==> new tau "<< Newtau<<"ms [Q"<< GetQueueString() << "]"<<endl;
        this->runningProcess->tau = Newtau;

        int endTime = this->runningProcess->getCurrentIOBurst() + this->currentTime + this->t_cs/2;
        if (timeCheck())cout << "time " << this->currentTime << "ms: Process " << this->runningProcess->process_name <<
            " switching out of CPU; blocking on I/O until time " << endTime << "ms [Q" << GetQueueString() << "]" << endl;

        Command c(endTime, 3, this->runningProcess);
        addCommand(c, endTime);
    }
    else {
        //last burst
        process.terminatedTime = currentTime;
        cout << "time " << this->currentTime << "ms: Process " << this->runningProcess->process_name << 
        " terminated [Q" << GetQueueString() << "]" << endl;
    }
    //context switching
    this->runningProcess = nullptr;
    this->isRemovingProcess = true;
    Command c2(this->currentTime + this->t_cs / 2, 0, &process);
    addCommand(c2, this->currentTime + this->t_cs / 2);
}

bool SRT::checkPreempt(Process &process){
    if (this->runningProcess != nullptr){
       
        return process.tau < this->runningProcess->tau - (this->runningProcess->getCurrentBurst() - this->runningProcess->burst_time_left);
    }
    return false;
}

void SRT::addProcessToQ(Process& process){
    this->readyQueue.push_back(&process);
    sort(this->readyQueue.begin(), this->readyQueue.end(), compareProcesses);
}

bool compareProcesses(Process* a, Process* b){
    int remain_time_a = a->burst_time_left==-1 ? a->tau : a->tau - (a->getCurrentBurst() - a->burst_time_left);
    int remain_time_b = b->burst_time_left==-1 ? b->tau : b->tau - (b->getCurrentBurst() - b->burst_time_left);
    if (remain_time_a < remain_time_b){
        return true;
    } else if (remain_time_a == remain_time_b){
        return a->process_name < b->process_name;
    } else{
        return false;
    }
}