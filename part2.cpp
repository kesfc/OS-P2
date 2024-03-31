#include <iostream>
#include <string>
#include <format>
#include <cstdlib>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <vector>
#include <functional>
#include "Process.h"
#include "algorithms.h"
using namespace std;

const int n, n_CPU, seed, t_cs, t_slice;
double lambda, upper_bound, alpha;
float cpuBurstTime_io, cpuBurstTime_cpu;

int compare(const void *a, const void *b){
    const Process *p1 = (const Process *)a;
    const Process *p2 = (const Process *)b;
    return p1->arrival_time - p2->arrival_time;
}

double next_exp(){
    //Exponential distribution pseudo-random number generation function
    double next_value;
    while(1){
        next_value = drand48();
        next_value = (-log(next_value) / lambda);
        if(next_value <= upper_bound){
            return next_value;
        }
    }
    return EXIT_FAILURE;
}

int next_arrival_time(){
    return floor(next_exp());
}

int number_burst(){
    double rand_uniform = drand48();
    return ceil(rand_uniform*64);
}

int CPU_burst_time(){
    return ceil(next_exp());
}

int IO_burst_time(){
    return ceil(next_exp());
}

//output info to txt
void OutputSimout() {
    
    //average(123 needs calculation)
    float cpuUtilization = 123;
    float cpuBurstTime = 123;
    float waitTime = 123;
    float turnAroundTime = 123;
    int switches = currentAlgo.numOfSwitch_cpu + currentAlgo.numOfSwitch_io;
    int preemptions = currentAlgo.numOfPreemption_cpu + currentAlgo.numOfPreemption_io;

    cpuUtilization = std::ceil(cpuUtilization * 1000) / 1000;
    waitTime = std::ceil(waitTime * 1000) / 1000;
    //...

    //start output
    outputFile << "Algorithm " << currentAlgo.name << endl;
    // Set the precision and format of the output stream
    std::cout << std::fixed << std::setprecision(3);

    std::string result = std:; format("-- CPU utilization: {0}%", cpuUtilization);
    std::cout << result << std::endl;

    result = std::format("-- average CPU burst time: {0} ms ({1} ms/{2} ms)", cpuBurstTime, cpuBurstTime_cpu, cpuBurstTime_io);
    std::cout << result << std::endl;

    result = std::format("-- average wait time: {0} ms ({1} ms/{2} ms)",);
    std::cout << result << std::endl;

    result = std::format("-- average turnaround time: {0} ms ({1} ms/{2} ms)", );
    std::cout << result << std::endl;

    // Set the precision and format of the output stream
    std::cout << std::fixed << std::setprecision(0);

    result = std::format("-- number of context switches: {0} ({1}/{2})", );
    std::cout << result << std::endl;

    result = std::format("-- number of preemptions: {0} ({1}/{2})", );
    std::cout << result << std::endl;

}

Process process_progress(int is_IO_bound, int process_code){
    double arrival_time = next_arrival_time();
    int Burst_number = number_burst();
    char process_name = static_cast<char>(process_code);
    string bound = is_IO_bound ? "I/O" : "CPU";
    
    // Carete an Process 
    cout << process_name << " " << arrival_time << " " << Burst_number << endl;
    //Process process(process_name, arrival_time, Burst_number, Burst_number);
    Process process = Process(process_name, arrival_time, Burst_number, Burst_number, is_IO_bound);
    // Header info
   std:: cout << bound << "-bound process " << process_name << ": arrival time " << arrival_time <<
        "ms; " << Burst_number << " CPU burst" << (Burst_number != 1 ? "s" : "") << ":" << endl;

    for(int i = 0; i < Burst_number-1; i++){
        int cpu_burst_time = CPU_burst_time();
        int io_burst_time = IO_burst_time();
        
        io_burst_time *= 10;
        if(!is_IO_bound){
            cpu_burst_time *= 4;
            io_burst_time /= 8;
        }
        // Add burst to the process
        process.add_burst(cpu_burst_time, io_burst_time);

        // Print burst info
        cout << "--> CPU burst " << cpu_burst_time << "ms --> I/O burst " << io_burst_time << "ms" << endl;
    }

    // Special for the last CPU burst
    int final_cpu_burst_time = CPU_burst_time();
     if(!is_IO_bound){
        final_cpu_burst_time *= 4;
    }
    cout << "--> CPU burst " << final_cpu_burst_time << "ms" << endl;
    return process;
}

void CalculateCpuBurstTime(std::vector<Process> &processes) {
    for (const auto& process : processes) {
        // Sum up the burst_time 
        if (process.isCpuBound) {
            cpuBurstTime_cpu += std::accumulate(std::begin(process.cpu_bursts), std::end(process.cpu_bursts), 0);
        }
        else {
            cpuBurstTime_io += std::accumulate(std::begin(process.cpu_bursts), std::end(process.cpu_bursts), 0);
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 9){
        fprintf(stderr, "Error: Invalid number of arguments\n");
        return EXIT_FAILURE;
    }
    // argv[1]: number of process to stimulate; argv[2]: number of CPU;
    // argv[3]: the seed for psudo-random number sequense;
    // argv[4]: lamda, 1/lamda is the average arrival rate of the process;
    // argv[5]: upper_bound for the random number generation;
    try{
        n = stoi(argv[1]);
        n_CPU = stoi(argv[2]);
        seed = stoi(argv[3]);
        lambda = stod(argv[4]);
        upper_bound = stod(argv[5]);
        t_cs = stod(argv[6]);
        alpha = stoi(argv[7]);
        t_slice = stod(argv[8]);
    }
    catch(std::invalid_argument& e){
        fprintf(stderr, "Error: Invalid argument\n");
        return EXIT_FAILURE;
    }
    srand48(seed);
    std::cout << "<<< PROJECT PART I -- process set (n=" << n << ") with " 
    << n_CPU << " CPU-bound process" << (n_CPU > 1 ? "es" : "") << " >>>" << endl;
    
    int asciiValue = 65;
    vector<Process> processes;
    for (int i = 0; i < n; i++, asciiValue++){
        if(i < n-n_CPU){
            processes.push_back(process_progress(1, asciiValue));
        }
        else{
            processes.push_back(process_progress(0, asciiValue));
        }
    }

    CalculateCpuBurstTime(processes);
    // qsort(process, n, sizeof(Process), compare);

    //generate simout
    outputFile.open("simout.txt");
    //print part 2 start info
    std::cout << "<<< PROJECT PART II -- t_cs=" << t_cs << "ms; alpha=" << alpha << "; t_slice=" << t_slice << "ms >>>" << endl;
    
    FCFS fcfs = FCFS("FCFS", processes);
    fcfs.start();

    //free memory
    for(int i = 0; i < n; i++){
        processes[i].free_self();
    }
}

class Algo {
public:
    string name = "";
    vector<Process> processes;
    queue<Process> readyQueue;
    int currentTime = 0;
    int turnAroundTime_cpu = 0;
    int turnAroundTime_io = 0;
    int waitTime_cpu = 0;
    int waitTime_io = 0;
    int numOfSwitch_cpu = 0;
    int numOfSwitch_io = 0;
    int numOfPreemption_cpu = 0;
    int numOfPreemption_io = 0;

    Algo(string name, vector<process> processes) : name(name), processes(processes) {}
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
        Process &process;
        std::function<void(Process)> callback;

        Command(int t, int tp, Process* p, std::function<void(Process&)> cb)
            : time(t), type(tp), process(*p), callback(cb) {}
    };

    std::unordered_map<int, vector<Command>> commandBuffer = new();
    
    bool hasCommand(int time) {
        if (commandBuffer[time].empty)
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

            c.callback(c.time);
        }
    }

    void setup(vector<Process> process) {
        currentTime = 0;
        ready_queue = new();
        processes = process;
        
        Start();
    }

    virtual void Start() {
        cout << "time 0ms: Simulator started for " << name <<" [Q <empty>]" << endl;
        //add all process in queue command
        for (Process process : processes) {
            Command c(process.arrival_time, 4, process, ProcessArrival);
            addCommand(c, process.arrival_time);
        }
        //start sim
        while (!(allProcessCompleted() && isAvailable) && currentTime < 999999) {
            //Can process next process
            if (runningProcess == nullptr && !isLoadingProcess && isAvailable && !readyQueue.empty()) {
                isLoadingProcess = true;
                isAvailable = false;
                Process p = readyQueue.pop();
                Command c(currentTime + t_cs / 2, 2, p, StartCpu);
                addCommand(c, currentTime + t_cs /2);
            }

            while (hasCommand(currentTime) {
                executeCommands(currentTime);
            }
            currentTime++;
        }
        cout << "time " << currentTime << "ms: Simulator ended for " << currentAlgo.name << " [Q" << GetQueueString() << "]" << endl;
        
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
    virtual void ProcessArrival(Process &process) {
        readyQueue.push(process);
        cout << "time " << currentTime << "ms: Process " << process.name << " arrived; added to ready queue [Q" << GetQueueString() << "]" << endl;
        //no running process, run
        if (runningProcess == nullptr && !isLoadingProcess && isAvailable) {
            isLoadingProcess = true;
            Command c(process.arrival_time + t_cs/2, 2, process, StartCpu);
            addCommand(c, process.arrival_time);
        }
        
    }

    virtual void StartCpu(Process &process) {
        isLoadingProcess = false;
        isAvailable = true;
        runningProcess = &process;
        readyQueue.pop();
        cout << "time " << currentTime << "ms: Process " << runningProcess->process_name << " started using the CPU for "<< runningProcess->getCurrentBurst() << "ms burst [Q " << GetQueueString() << "]" << endl;

        int endTime = currentTime + runningProcess->getCurrentBurst();
        Command c(endTime, 1, process, FinishCpu);
        addCommand(c, endTime);
    }

    virtual void FinishCpu(Process &process) {

        cout << "time " << currentTime << "ms: Process "<< runningProcess->process_name <<" completed a CPU burst; "<< runningProcess->burst_remaining <<" bursts to go [Q"<< GetQueueString() << "]" << endl;
        runningProcess->burst_remaining--;
        //if not terminated, start IO
        if (runningProcess->burst_remaining > 0) {
            int endTime = runningProcess->getCurrentIOBurst() + currentTime;
            cout << "time " << currentTime << "ms: Process " << runningProcess->process_name << " switching out of CPU; blocking on I/O until time " << endTime << "ms [Q" << GetQueueString() << "]" << endl;
            Command c(endTime, 3, runningProcess, FinishIO);
            addCommand(c, endTime);

            isAvailable = false;
            Command c(currentTime + t_cs/2, 0, process, SwitchingDone);
            addCommand(c, currentTime + t_cs/2);
        }
        else {
            //last burst
            cout << "time " << currentTime << "ms: Process " << runningProcess->process_name << " switching out of CPU." << endl;
            cout << "time " << currentTime << "ms: Process " << runningProcess->process_name << " terminated [Q"<< GetQueueString() << "]" << endl;
        }
        //all terminated
        else {
            
        }
    }

    virtual void TauRecalculated(Process &process) {

    }

    virtual void Preemption(Process &process) {

    }


    virtual void FinishIO(Process &process) {
        readyQueue.push(process);
        cout << "time "<< currentTime <<"ms: Process "<< process.process_name <<" completed I/O; added to ready queue [Q"<<GetQueueString() << "]"<<endl;
    }

    virtual void LastCpuBurst(Process &process) {

    }

    virtual void SwitchingDone(Process& process) {
        isAvailable = true;
    }

    string GetQueueString() {
        string queueString = "";
        if (readyQueue.empty){
            queueString = " <empty>";
        }
        else
        {
            for (int i = 0; i < readyQueue.size(); i++) {
                queueString += " " + readyQueue[i].name;
            }
        }
        
        return queueString;
    }
};