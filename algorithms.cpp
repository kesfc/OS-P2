#include "algorithms.h"
#include "Process.h"
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cmath> 
using namespace std;

bool Algo::timeCheck() {
    if (this->currentTime <= 9999)
        return true;
    else
        return false;
}

bool Algo::hasCommand(int time) {
    if (this->commandBuffer[time].empty()) {
        return false;
    }
    else {
        return true;
    }
}

bool Algo::checkPreempt(Process & process){
    return false;
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
            case -1:
                RemovingPreemptedProcessDone(*c.process);
            case 0:
                SwitchingDone(*c.process);
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

string Algo::runningProcessName(Process & process){
    string name(1, process.process_name);
    return name;
}

void Algo::setup() {
    this->currentTime = 0;
    isRemovingProcess = false;
    isLoadingProcess = false;
}

void Algo::updateWaitTime() {
    for (Process* processPtr : readyQueue) {
        processPtr->waitTime++;
    }
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
    while (!(allProcessCompleted() && !isRemovingProcess)) {
        //corner case
        newProcessRunCheck();
        while (hasCommand(this->currentTime)) {
            while (hasCommand(this->currentTime)){
                executeCommands(this->currentTime);
            }
            //normal case
            newProcessRunCheck();
            // cout << "Checked for running process... " << this->isLoadingProcess << " " << this->isRemovingProcess << endl;
        }
        processRunningProcess();
        updateWaitTime();
        this->currentTime++;
    }
    currentTime--;
    this->endTime = currentTime;
    cout << "time " << this->currentTime << "ms: Simulator ended for " << this->name << " [Q" << GetQueueString() << "]" << endl;
}

bool Algo::allProcessCompleted() {
    if (this->processes.size() == 0) {
        EXIT_FAILURE;
    }
    bool completed = true;
    for (vector<Process>::size_type i = 0; i < this->processes.size(); i++) {
        if (!this->processes[i].isCompleted()) {
            completed = false;
            break;
        }
    }
    return completed;
}

void Algo::processRunningProcess() {
    if (runningProcess != nullptr) {
        runningProcess->burst_time_left--;

        if (runningProcess->burst_time_left == 0) {
            Command c(currentTime + 1, 1, runningProcess);
            addCommand(c, currentTime + 1);
        }
    }
}

void Algo::addProcessToQ(Process& process) {
    this->readyQueue.push_back(&process);
    return;
}


void Algo::ProcessArrival(Process& process) {
    this->addProcessToQ(process);

    if (timeCheck())cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(process) << " arrived; added to ready queue [Q" << GetQueueString() << "]" << endl;
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
    //remove
    auto it = std::find_if(readyQueue.begin(), readyQueue.end(), [&process](Process* p) {
        return p == &process; // Compare addresses to find the matching process
        });
    if (it != readyQueue.end()) {
        // This means that it is a preemption case.
        // cout << "erasing.. " << (*it)->process_name << endl;
        readyQueue.erase(it); // Remove the process from readyQueue
    }
    
    if (this->runningProcess->burst_time_left == -1 || this->runningProcess->burst_time_left == this->runningProcess->getCurrentBurst()){
        this->runningProcess->burst_time_left = this->runningProcess->getCurrentBurst();
        if (timeCheck())cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(*this->runningProcess) <<
    " started using the CPU for " << this->runningProcess->getCurrentBurst() << "ms burst [Q" << GetQueueString() << "]" << endl;
    } else {
        if (timeCheck())cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(*this->runningProcess) <<
    " started using the CPU for remaining " << this->runningProcess->burst_time_left << "ms of " 
    << this->runningProcess->getCurrentBurst() << "ms burst [Q" << GetQueueString() << "]" << endl;
    }
    this->runningProcess->burst_start_time = currentTime;

    if (this->contain_preemption && this->checkPreempt(*this->readyQueue.front())){
        Process* p = this->readyQueue.front();

        if (timeCheck())cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(*p) << " will preempt " <<
        this->runningProcess->process_name << " [Q" << this->GetQueueString() << "]" << endl;
        
        this->readyQueue.erase(this->readyQueue.begin());
        this->Preemption(*p);
    }
}

void Algo::FinishCpu(Process& process) {
    if (process.isCpuBound) {
        this->cpuTurnAroundTime += this->t_cs / 2 + this->currentTime - runningProcess->new_arrival_time;
    }
    else {
        this->ioTurnAroundTime += this->t_cs / 2 + this->currentTime - runningProcess->new_arrival_time;
    }

    this->runningProcess->burst_remaining--;
    this->runningProcess->burst_time_left = -1;
    //if not terminated, start IO
    if (this->runningProcess->burst_remaining > 0) {
        if (timeCheck())cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(*this->runningProcess) <<
        " completed a CPU burst; " << this->runningProcess->burst_remaining << " burst" << 
        (this->runningProcess->burst_remaining > 1 ? "s" : "") <<" to go [Q" << GetQueueString() << "]" << endl;
        int endTime = this->runningProcess->getCurrentIOBurst() + this->currentTime + this->t_cs/2;
        if (timeCheck())cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(*this->runningProcess) <<
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
        cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(*this->runningProcess) << " terminated [Q" << GetQueueString() << "]" << endl;
        process.terminatedTime = currentTime;

        this->runningProcess = nullptr;
        this->isRemovingProcess = true;
        Command c2(this->currentTime + this->t_cs / 2, 0, &process);
        addCommand(c2, this->currentTime + this->t_cs / 2);
    }
}

void Algo::TauRecalculated(Process& process){

}

void Algo::RemovingPreemptedProcessDone(Process& process) {
    this->addProcessToQ(process);
    Command c1(this->currentTime, 0, &process);
    this->addCommand(c1, this->currentTime);
}

void Algo::Preemption(Process& process){
    // Theoretically only algo that uses preemption will have the chance to reach this function.
    // putting the current running process to queue, and execute the param process.
    if (runningProcess->isCpuBound) {
        this->cpuPreemption++;
    }
    else {
        this->ioPreemption++;
    }
    
    this->readyQueue.insert(this->readyQueue.begin(), &process);

    Command c0(this->currentTime + this->t_cs / 2, -1, this->runningProcess);
    this->addCommand(c0, this->currentTime + this->t_cs / 2);

    this->runningProcess = nullptr;
    this->isRemovingProcess = true;
    // this->isLoadingProcess = true;

    // Command c1(this->currentTime+this->t_cs, 2, &process);
    // this->addCommand(c1, this->currentTime+this->t_cs);
}

void Algo::FinishIO(Process& process) {
    process.new_arrival_time = currentTime;

    if(this->contain_preemption && this->checkPreempt(process)){
        if (timeCheck())cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(process) << " completed I/O; preempting " << this->runningProcess->process_name << " [Q";
        this->Preemption(process);
        if (timeCheck())cout<< this->GetQueueString() << "]" << endl;
    } else {
        this->addProcessToQ(process);
        if (timeCheck())cout << "time " << this->currentTime << "ms: Process " << this->runningProcessName(process) <<
    " completed I/O; added to ready queue [Q" << this->GetQueueString() << "]" << endl;
    }
}

void Algo::LastCpuBurst(Process& process){

}

void Algo::SwitchingDone(Process& process) {
    if (isRemovingProcess) {
        if (process.isCpuBound)
            this->cpuSwitchCount++;
        else
            this->ioSwitchCount++;
    }
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

void Algo::printInfo(std::ofstream& file) {
    file << "Algorithm " << name << endl;

    float cpuBurstTime_io = 0;
    float cpuBurstTime_cpu = 0;
    float cpuBoundedProcessCount = 0;
    float ioBoundedProcessCount = 0;
    float cpuBoundBurstCount = 0;
    float ioBoundBurstCount = 0;
    float cpuWaitTime = 0;
    float ioWaitTime = 0;
    

    for (vector<Process>::size_type i = 0; i < processes.size(); i++) {
        if (processes[i].isCpuBound) {
            cpuBoundedProcessCount++;
            cpuWaitTime += processes[i].waitTime;
            cpuBoundBurstCount += processes[i].burst_number;
        }
        else {
            ioBoundedProcessCount++;
            ioWaitTime += processes[i].waitTime;
            ioBoundBurstCount += processes[i].burst_number;
        }

        for (int j = 0; j < processes[i].burst_number; j++) {
            if (processes[i].isCpuBound) {
                cpuBurstTime_cpu += processes[i].cpu_bursts[j];
            }
            else {
                cpuBurstTime_io += processes[i].cpu_bursts[j];
            }
        }
    }

    float cpuUtilization = std::ceil((cpuBurstTime_cpu + cpuBurstTime_io) / this->endTime * 1000 * 100) / 1000.0f;
    //magic offset:
    cpuWaitTime -= t_cs * cpuSwitchCount / 2;
    ioWaitTime -= t_cs * ioSwitchCount / 2;

    cpuTurnAroundTime = cpuWaitTime + t_cs * cpuSwitchCount + cpuBurstTime_cpu;
    ioTurnAroundTime = ioWaitTime + t_cs * ioSwitchCount + cpuBurstTime_io;
    float avgTurnAroundTime = std::ceil((cpuTurnAroundTime + ioTurnAroundTime) / (cpuBoundBurstCount + ioBoundBurstCount) * 1000.0) / 1000.0f;
    cpuTurnAroundTime = std::ceil(cpuTurnAroundTime / cpuBoundBurstCount * 1000.0) / 1000.0f;
    ioTurnAroundTime = std::ceil(ioTurnAroundTime / ioBoundBurstCount * 1000.0) / 1000.0f;

    float avgCpuBurstTime = std::ceil((cpuBurstTime_cpu + cpuBurstTime_io) / (cpuBoundBurstCount + ioBoundBurstCount) * 1000.0) / 1000.0f;
    cpuBurstTime_cpu = std::ceil(cpuBurstTime_cpu / (cpuBoundBurstCount) * 1000.0) / 1000.0f;
    cpuBurstTime_io = std::ceil(cpuBurstTime_io / (ioBoundBurstCount) * 1000.0) / 1000.0f;

    float avgWaitTime = std::ceil((cpuWaitTime + ioWaitTime) / (cpuBoundBurstCount + ioBoundBurstCount) * 1000.0) / 1000.0f;
    cpuWaitTime = std::ceil(cpuWaitTime / cpuBoundBurstCount * 1000.0) / 1000.0f;
    ioWaitTime = std::ceil(ioWaitTime / ioBoundBurstCount * 1000.0) / 1000.0f;

    file << std::fixed << std::setprecision(3);
    file << "-- CPU utilization: " << cpuUtilization << "%" << endl
        << "-- average CPU burst time: " << avgCpuBurstTime << " ms (" << cpuBurstTime_cpu << " ms/" << cpuBurstTime_io << " ms)" << endl
        << "-- average wait time: " << avgWaitTime << " ms (" << cpuWaitTime << " ms/" << ioWaitTime << " ms)" << endl
        << "-- average turnaround time: " << avgTurnAroundTime << " ms (" << cpuTurnAroundTime << " ms/" << ioTurnAroundTime << " ms)" << endl;
    file << std::fixed << std::setprecision(0);
    file << "-- number of context switches: " << (this->cpuSwitchCount + this->ioSwitchCount) << " (" << this->cpuSwitchCount << "/" << this->ioSwitchCount << ")" << endl
        << "-- number of preemptions: " << (this->cpuPreemption + this->ioPreemption) << " (" << this->cpuPreemption << "/" << this->ioPreemption << ")" << endl;
}