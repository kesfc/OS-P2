#ifndef __ALGO_H__
#define __ALGO_H__
#include "Process.h"
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>
using namespace std;

class Algo {
public:
    string name = "";
    vector<Process> processes;
    queue<Process> readyQueue;
    
    Process* runningProcess;
    int currentTime = 0;
    int turnAroundTime_cpu = 0;
    int turnAroundTime_io = 0;
    int waitTime_cpu = 0;
    int waitTime_io = 0;
    int numOfSwitch_cpu = 0;
    int numOfSwitch_io = 0;
    int numOfPreemption_cpu = 0;
    int numOfPreemption_io = 0;
    int t_cs;

    Algo(string name, vector<Process> &processes, int t_cs) : name(name), processes(processes), t_cs(t_cs) {}
    /// <summary>
    /// [0 = context switch done]
    /// [1 = CPU completion]
    /// [2 = starts using CPU]
    /// [3 = I/O burst completion]
    /// [4 = new process arrival]
    /// </summary>
    struct Command {
        int time;
        int type;
        Process& process;

        Command(int t, int tp, Process* p)
            : time(t), type(tp), process(*p) {}
    };

    std::unordered_map<int, vector<Command>> commandBuffer;

    bool hasCommand(int time) {
        if (commandBuffer[time].empty())
            return false;
        else {
            return true;
        }
    }

    bool compareCommand(Command a, Command b) {
        if (a.type == b.type) {
            return a.process.process_name < b.process.process_name;
        }
        else
            return a.type < b.type;
    }

    void addCommand(Command command, int time) {
        if (hasCommand(time)) {
            commandBuffer[time].push_back(command);
            //sort commands
            std::sort(commandBuffer[time].begin(), commandBuffer[time].end(), compareCommand);
        }
        else {
            commandBuffer[time].push_back(command);
        }
    }

    void executeCommands(int time) {
        if (hasCommand(time)) {
            Command c = commandBuffer[time][0];
            //pop first command
            commandBuffer[time].erase(commandBuffer[time].begin());

            switch (c.type) {
                case 0:
                    SwitchingDone();
                    break;
                case 1:
                    FinishCpu(c.process);
                    break;
                case 2: 
                    StartCpu(c.process);
                    break; 
                case 3:
                    FinishIO(c.process);
                    break;
                case 4:
                    ProcessArrival(c.process);
                    break;
            }
        }
    }

    void setup(vector<Process> process) {
        currentTime = 0;
        processes = process;

        Start();
    }

    virtual void Start() {
        cout << "time 0ms: Simulator started for " << name << " [Q <empty>]" << endl;
        //add all process in queue command
        for (Process process : processes) {
            Command c(process.arrival_time, 4, &process);
            addCommand(c, process.arrival_time);
        }
        //start sim
        while (!(allProcessCompleted() && isAvailable) && currentTime < 999999) {
            //Can process next process
            if (this->runningProcess == nullptr && !isLoadingProcess && isAvailable && !readyQueue.empty()) {
                isLoadingProcess = true;
                isAvailable = false;
                Process p = readyQueue.front();
                readyQueue.pop();
                Command c(currentTime + this->t_cs / 2, 2, &p);
                addCommand(c, currentTime + this->t_cs / 2);
            }

            while (hasCommand(currentTime)) {
                executeCommands(currentTime);
            }
            currentTime++;
        }
        cout << "time " << currentTime << "ms: Simulator ended for " << this->name << " [Q" << GetQueueString() << "]" << endl;

    }

    bool allProcessCompleted() {
        if (processes.size() == 0) {
            EXIT_FAILURE;
        }

        bool completed = true;
        for (int i = 0; i < processes.size(); i++) {
            if (!processes[i].isCompleted()) {
                completed = false;
                break;
            }
        }
        return completed;
    }

    //is context switching?
    bool isAvailable = true;
    bool isLoadingProcess = false;
    virtual void ProcessArrival(Process& process) {
        readyQueue.push(process);
        cout << "time " << currentTime << "ms: Process " << process.process_name << " arrived; added to ready queue [Q" << GetQueueString() << "]" << endl;
        //no running process, run
        if (this->runningProcess == nullptr && !isLoadingProcess && isAvailable) {
            isLoadingProcess = true;
            Command c(process.arrival_time + this->t_cs / 2, 2, &process);
            addCommand(c, process.arrival_time);
        }

    }

    virtual void StartCpu(Process& process) {
        isLoadingProcess = false;
        isAvailable = true;
        this->runningProcess = &process;
        readyQueue.pop();
        cout << "time " << currentTime << "ms: Process " << this->runningProcess->process_name << " started using the CPU for " << runningProcess->getCurrentBurst() << "ms burst [Q " << GetQueueString() << "]" << endl;

        int endTime = currentTime + this->runningProcess->getCurrentBurst();
        Command c(endTime, 1, &process);
        addCommand(c, endTime);
    }

    virtual void FinishCpu(Process& process) {

        cout << "time " << currentTime << "ms: Process " << this->runningProcess->process_name << " completed a CPU burst; " << this->runningProcess->burst_remaining << " bursts to go [Q" << GetQueueString() << "]" << endl;
        this->runningProcess->burst_remaining--;
        //if not terminated, start IO
        if (this->runningProcess->burst_remaining > 0) {
            int endTime = this->runningProcess->getCurrentIOBurst() + currentTime;
            cout << "time " << currentTime << "ms: Process " << this->runningProcess->process_name << " switching out of CPU; blocking on I/O until time " << endTime << "ms [Q" << GetQueueString() << "]" << endl;
            Command c(endTime, 3, this->runningProcess);
            addCommand(c, endTime);

            isAvailable = false;
            Command c(currentTime + this->t_cs / 2, 0, &process);
            addCommand(c, currentTime + this->t_cs / 2);
        }
        else {
            //last burst
            cout << "time " << currentTime << "ms: Process " << this->runningProcess->process_name << " switching out of CPU." << endl;
            cout << "time " << currentTime << "ms: Process " << this->runningProcess->process_name << " terminated [Q" << GetQueueString() << "]" << endl;
        }
    }

    virtual void TauRecalculated(Process& process) {

    }

    virtual void Preemption(Process& process) {

    }


    virtual void FinishIO(Process& process) {
        readyQueue.push(process);
        cout << "time " << currentTime << "ms: Process " << process.process_name << " completed I/O; added to ready queue [Q" << GetQueueString() << "]" << endl;
    }

    virtual void LastCpuBurst(Process& process) {

    }

    virtual void SwitchingDone() {
        isAvailable = true;
    }

    string GetQueueString() {
        string queueString = "";
        if (readyQueue.empty()) {
            queueString = " <empty>";
        }
        else
        {
            std::queue<Process> tempQueue = readyQueue;
            while (!tempQueue.empty()) {
                Process& process = tempQueue.front();
                queueString += " " + process.process_name;
                tempQueue.pop();
            }
        }

        return queueString;
    }
};
#endif

