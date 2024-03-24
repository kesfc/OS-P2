#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <queue>

using namespace std;

int n, n_CPU, seed, t_cs, t_slice;
double lambda, upper_bound, alpha;

class Process{
    public:
        char process_name;
        int add_Que_time;
        int burst_number;
        int burst_remaining;
        int* burst_time;
        int* io_time;
        Process(){
            this->process_name = 'A';
            this->add_Que_time = 0;
            this->burst_number = 0;
            this->burst_remaining = 0;
            this->burst_time = new int[0];
            this->io_time = new int[0];
        }
        Process(char process_name, int Arrival_time, int Burst_number, int Burst_remaining){
            this->process_name = process_name;
            this->add_Que_time = Arrival_time;
            this->burst_number = Burst_number;
            this->burst_remaining = Burst_remaining;
            this->burst_time = new int[Burst_number];
            this->io_time = new int[Burst_number];
        }
        void add_burst(int burst_time, int io_time){
            this->burst_time[this->burst_number] = burst_time;
            this->io_time[this->burst_number] = io_time;
            this->burst_number++;
        }
};
int compare(const void *a, const void *b){
    const Process *p1 = (Process *)a;
    const Process *p2 = (Process *)b;
    return p1->add_Que_time - p2->add_Que_time;
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

void process_progress(int is_IO_bound, int process_code, Process& process){
    double arrival_time = next_arrival_time();
    int Burst_number = number_burst();

    char process_name = static_cast<char>(process_code);
    string bound = is_IO_bound ? "I/O" : "CPU";
    // Carete an Process 
    process = Process(process_name, arrival_time, Burst_number, Burst_number);
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
}

void FCFS(Process* process, int t_cs, int Process_num){
    int time = 0;
    cout << "time 0ms: Simulator started for FCFS [Q <empty>]" << endl;
    queue<Process> ready_queue;
    while(Process_num != 0){

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
    Process* process = new Process[n];
    for (int i = 0; i < n; i++, asciiValue++){
        if(i < n-n_CPU){
            process_progress(1, asciiValue, process[i]);
        }
        else{
            process_progress(0, asciiValue, process[i]);
        }
    }
    qsort(process, n, sizeof(Process), compare);
    FCFS(process, t_cs, n);
}