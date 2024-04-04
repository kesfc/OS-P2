#include "algorithms.h"
#include "Process.h"
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>
using namespace std;

bool Algo::hasCommand(int time) {
    if (this->commandBuffer[time].empty()) {
        return false;
    }
    else {
        return true;
    }
}

void Algo::addCommand(Command command, int time) {
    if (hasCommand(time)) {
        this->commandBuffer[time].push_back(command);
        //sort commands
        std::sort(this->commandBuffer[time].begin(), this->commandBuffer[time].end(), compareCommand);
    }
    else {
        this->commandBuffer[time].push_back(command);
    }
}

void Algo::executeCommands(int time) {
    if (hasCommand(time)) {
        Command c = this->commandBuffer[time][0];
        //pop first command
        this->commandBuffer[time].erase(this->commandBuffer[time].begin());
        switch (c.type) {
            case 0:
                SwitchingDone();
                break;
            case 1:
                FinishCpu(*c.process);
                break;
            case 2: 
                StartCpu(*c.process);
                break; 
            case 3:
                FinishIO(*c.process);
                break;
            case 4:
                ProcessArrival(*c.process);
                break;
        }
    }
}

void Algo::setup() {
    this->currentTime = 0;
    isRemovingProcess = false;
    isLoadingProcess = false;
}


void Algo::Start() {
    setup();
    cout << "time 0ms: Simulator started for " << this->name << " [Q <empty>]" << endl;
    //add all process in queue command
    for (Process& process : this->processes) {
        Command c(process.arrival_time, 4, &process);
        addCommand(c, process.arrival_time);
    }
    this->runningProcess = nullptr;
    //start sim
    while (!(allProcessCompleted() && !isRemovingProcess) && this->currentTime < 9999999) {
        //corner case
        newProcessRunCheck();
        while (hasCommand(this->currentTime)) {
            executeCommands(this->currentTime);
            //normal case
            newProcessRunCheck();
        }
        this->currentTime++;
    }
    currentTime--;
    cout << "time " << this->currentTime << "ms: Simulator ended for " << this->name << " [Q" << GetQueueString() << "]" << endl;
}

bool Algo::allProcessCompleted() {
    if (this->processes.size() == 0) {
        EXIT_FAILURE;
    }

    bool completed = true;
    for (int i = 0; i < this->processes.size(); i++) {
        if (!this->processes[i].isCompleted()) {
            //if (currentTime >= 324062 && currentTime <= 324063)cout << "Debug:"<<processes[i].process_name << processes[i].burst_remaining << endl;
            completed = false;
            break;
        }
    }
    return completed;
}

void Algo::ProcessArrival(Process& process) {
    this->readyQueue.push_back(&process);
    cout << process.process_name << endl;
    cout << "time " << this->currentTime << "ms: Process " << process.process_name << " arrived; added to ready queue [Q" << GetQueueString() << "]" << endl;
    //no running process, run
    if (this->runningProcess == nullptr && !this->isLoadingProcess && !this->isRemovingProcess) {
        this->isLoadingProcess = true;

        Command c(process.arrival_time + this->t_cs / 2, 2, &process);
        addCommand(c, process.arrival_time + this->t_cs / 2);
    }
}

void Algo::StartCpu(Process& process) {
    this->isLoadingProcess = false;
    this->isRemovingProcess = false;
    this->runningProcess = &process;
    this->readyQueue.erase(this->readyQueue.begin());
    cout << "time " << this->currentTime << "ms: Process " << this->runningProcess->process_name << 
    " started using the CPU for " << this->runningProcess->getCurrentBurst() << "ms burst [Q " << GetQueueString() << "]" << endl;
    int endTime = this->currentTime + this->runningProcess->getCurrentBurst();
    Command c(endTime, 1, &process);
    addCommand(c, endTime);
}

void Algo::FinishCpu(Process& process) {
    this->runningProcess->burst_remaining--;
    cout << "time " << this->currentTime << "ms: Process " << this->runningProcess->process_name <<
        " completed a CPU burst; " << this->runningProcess->burst_remaining << " bursts to go [Q" << GetQueueString() << "]" << endl;
    
    //if not terminated, start IO
    if (this->runningProcess->burst_remaining > 0) {
        int endTime = this->runningProcess->getCurrentIOBurst() + this->currentTime + this->t_cs/2;
        cout << "time " << this->currentTime << "ms: Process " << this->runningProcess->process_name << 
            " switching out of CPU; blocking on I/O until time " << endTime << "ms [Q" << GetQueueString() << "]" << endl;
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
        cout << "time " << this->currentTime << "ms: Process " << this->runningProcess->process_name << " switching out of CPU." << endl;
        cout << "time " << this->currentTime << "ms: Process " << this->runningProcess->process_name << " terminated [Q" << GetQueueString() << "]" << endl;

        this->runningProcess = nullptr;
        this->isRemovingProcess = true;
        Command c2(this->currentTime + this->t_cs / 2, 0, &process);
        addCommand(c2, this->currentTime + this->t_cs / 2);
    }
}

void Algo::TauRecalculated(Process& process){

}

void Algo::Preemption(Process& process){

}

void Algo::FinishIO(Process& process) {
    this->readyQueue.push_back(&process);
    cout << "time " << this->currentTime << "ms: Process " << process.process_name << " completed I/O; added to ready queue [Q" << GetQueueString() << "]" << endl;
}

void Algo::LastCpuBurst(Process& process){

}

void Algo::SwitchingDone() {
    this->isRemovingProcess = false;
}

string Algo::GetQueueString() {
    string queueString = "";
    if (this->readyQueue.empty()) {
        queueString = " <empty>";
    }
    else
    {
        for(Process* p : this->readyQueue){
            queueString += " ";
            queueString += p->process_name;
        }
    }
    return queueString;
}


bool Algo::compareCommand(Command a, Command b) {
    if (a.type == b.type) {
        return a.process->process_name < b.process->process_name;
    }
    else
        return a.type < b.type;
}