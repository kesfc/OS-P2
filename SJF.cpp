#include "algorithms.h"
#include "SJF.h"
#include "cmath"
#include "Process.h"
using namespace std;

SJF::SJF(string name, vector<Process> processes, int t_cs, double alpha)
    : Algo(name, processes, t_cs), alpha(alpha) {}  

void SJF::newProcessRunCheck(){
    if (this->runningProcess == nullptr && !this->isLoadingProcess && !this->isRemovingProcess && !this->readyQueue.empty()) {
        sort(this->readyQueue.begin(), this->readyQueue.end(), compareProcess);
        this->isLoadingProcess = true;
        Process* p = this->readyQueue.front();
        // cout << this->currentTime << " " << p->process_name << " will run on " << this->currentTime + this->t_cs / 2 << endl;
        Command c(this->currentTime + this->t_cs / 2, 2, p);
        addCommand(c, this->currentTime + this->t_cs / 2);
    }
}

string SJF::runningProcessName(Process & process){
    string name(1, process.process_name);
    name = name + " (tau " + to_string(process.tau) + "ms)";
    return name;
}

void SJF::FinishCpu(Process& process) {
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

void SJF::addProcessToQ(Process& process){
    this->readyQueue.push_back(&process);
    sort(this->readyQueue.begin(), this->readyQueue.end(), compareProcess);
}

bool compareProcess(Process* a, Process* b){
    return a->tau < b->tau || (a->tau == b->tau && a->process_name < b->process_name);
}