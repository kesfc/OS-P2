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
    Process *process;  // Change to a pointer

    Command(int t, int tp, Process* p) : time(t), type(tp), process(p) {}

    // Move constructor
    Command(Command&& other) noexcept
        : time(other.time), type(other.type), process(other.process) {}

    // Move assignment operator
    Command& operator=(Command&& other) noexcept {
        if (this != &other) {
            time = other.time;
            type = other.type;
            process = other.process;
        }
        return *this;
    }

    // Copy constructor
    Command(const Command& other)
        : time(other.time), type(other.type), process(other.process) {}

    // Copy assignment operator
    Command& operator=(const Command& other) {
        if (this != &other) {
            time = other.time;
            type = other.type;
            process = other.process;
        }
        return *this;
    }
};

class Algo {
public:
    string name = "";
    vector<Process> processes;
    vector<Process*> readyQueue;
    
    Process* runningProcess = NULL;
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

    // For SJF and SRT
    int alpha = 0;
    //is context switching?
    bool isRemovingProcess = false;
    bool isLoadingProcess = false;

    Algo(string name, vector<Process> processes, int t_cs) : name(name), processes(processes), t_cs(t_cs) {}

    std::unordered_map<int, vector<Command>> commandBuffer;

    bool hasCommand(int time);

    void addCommand(Command command, int time);

    void executeCommands(int time);

    virtual void Start();

    bool allProcessCompleted();

    virtual void ProcessArrival(Process& process);

    virtual void StartCpu(Process& process);

    virtual void FinishCpu(Process& process);

    virtual void TauRecalculated(Process& process);

    virtual void Preemption(Process& process);

    virtual void FinishIO(Process& process);

    virtual void LastCpuBurst(Process& process);

    virtual void SwitchingDone();

    string GetQueueString();

    static bool compareCommand(Command a, Command b);

    virtual void newProcessRunCheck() = 0;

private:
    void setup();
};
#endif

