class Process{
    public:
        char process_name;
        int add_Que_time;
        int burst_number;
        int burst_remaining;
        int* burst_time;
        int* io_time;
        Process(){}
        Process(char process_name, int Arrival_time, int Burst_number, int Burst_remaining);
        void add_burst(int burst_time, int io_time);
};