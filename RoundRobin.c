#include<stdio.h>
#include<stdlib.h>

// Ready Queue
typedef struct process_struct{
    int pid;
    int arrival_time;
    int burst_time;
    int tempBurstTime;
    int start_time;
    int end_time;
    float turnaround_time;
    float waiting_time;
    int isCompleted;
    struct process_struct* next;
} process;

process* front = NULL;
process* rear = NULL;

int isReadyQueueEmpty(){
    if(front == NULL && rear == NULL) return 1;
    return 0;
}

void enqueue(process* p){
    if(isReadyQueueEmpty()){
        front = rear = p;
        rear->next = front;
        return;
    }
    rear->next = p;
    rear = rear->next;
    rear->next = front;
}

process* dequeue(){
    if(isReadyQueueEmpty()){
        printf("Ready queue empty\n");
        return NULL;
    }
    else if(front == rear){
        process* temp = front;
        front = rear = NULL;
        temp->next = NULL;
        return temp;
    }
    else{
        process* temp = front;
        front = front->next;
        rear->next = front;
        temp->next = NULL;
        return temp;
    }
}

// Gantt Chart
typedef struct Gantt_Chart{
    int pid;
    int start_time;
    int end_time;
    struct Gantt_Chart *next;
} gc;
gc *head = NULL;
gc *tail = NULL;
int GCEntryCount = 0;

gc* createNode(int d, int st, int et){
    gc* newNode = (gc*)malloc(sizeof(gc));
    if(newNode == NULL){
        printf("Memory is full\n");
        return NULL;
    }
    newNode->pid = d;
    newNode->start_time = st;
    newNode->end_time = et;
    newNode->next = NULL;

    return newNode;
}

void insert(int d, int st, int et){
    if(tail != NULL){
        if(d == tail->pid){
            tail->end_time = et;
            return;
        }
    }
    gc* newNode = createNode(d, st, et);
    if(newNode != NULL){
        if(head == NULL && tail == NULL){
            head = tail = newNode;
        }
        else{
            tail->next = newNode;
            tail = tail->next;
        }
    }
    GCEntryCount++;
}

int countDigit(int num){
    int digit = 1;
    while(num >= 10){
        num = num / 10;
        digit++;
    }
    return digit;
}

void printGC(){
    printf("\nGantt Chart\n\n");
    if(head == NULL && tail == NULL){
        printf("Gantt Chart empty\n");
        return;
    }
    int leadingSpaces, trailingSpaces;
    for(int i = 0; i < GCEntryCount; i++) printf("+------");
    printf("+\n");
    gc *temp = head;
    while(temp != NULL){
        printf("|");
        if(temp->pid == -1){
            printf(" Idle ");
        }
        else{
            int digitCount = countDigit(temp->pid);
            leadingSpaces = (6 - digitCount)/2;
            trailingSpaces = 6 - leadingSpaces - digitCount;
            for(int i = 0; i < leadingSpaces; i++) printf(" ");
            printf("%d", temp->pid);
            for(int i = 0; i < trailingSpaces; i++) printf(" ");
        }
        temp = temp->next;
    }
    printf("|\n");
    for(int i = 0; i < GCEntryCount; i++) printf("+------");
    printf("+\n");

    temp = head;
    while(temp != NULL){
        int digitCount = countDigit(temp->start_time);
        int spaces = 6 - digitCount + 1;
        printf("%d", temp->start_time);
        for(int i = 0; i < spaces; i++) printf(" ");

        temp = temp->next;
    }
    printf("%d\n", tail->end_time);
}

void freeGCMemory(){
    gc *temp = head;
    gc *prev = NULL;
    while(temp != NULL){
        prev = temp;
        temp = temp->next;
        free(prev);
    }
}

// Sorting in ascending order according to Arrival Time
void processArrivalSort(process *p, int size){
    for(int i = 0; i < size - 1; i++){
        int minIdx = i;
        for(int j = i + 1; j < size; j++){
            if(p[j].arrival_time < p[minIdx].arrival_time){
                minIdx = j;
            }
            else if((p[j].arrival_time == p[minIdx].arrival_time) && (p[j].pid < p[minIdx].pid)){
                minIdx = j;
            }
        }

        if(minIdx != i){
            process temp = p[i];
            p[i] = p[minIdx];
            p[minIdx] = temp;
        }
    }
}

// Sorting in ascending order according to Process ID
void processIDSort(process* p, int size){
    for(int i = 0; i < size - 1; i++){
        int minIdx = i;
        for(int j = i + 1; j < size; j++){
            if(p[j].pid < p[minIdx].pid) minIdx = j;
        }

        if(minIdx != i){
            process temp = p[i];
            p[i] = p[minIdx];
            p[minIdx] = temp;
        }
    }
}

int min(int x, int y){
    return (x < y) ? x : y;
}

int main(){
    printf("Round Robin CPU scheduling simulator\n\n");
    int processCount, timeQuantum;
    int runningProcessCount = 0;
    float totalTurnaroundTime = 0, totalWaitingTime = 0;
    float averageTurnaroundTime, averageWaitingTime;
    float totalExecutionTime = 0, totalIdleTime = 0;
    float cpuUtil = 0;
    float cpuThroughput = 0;

    printf("Enter the number of processes: ");
    if(scanf("%d", &processCount) != 1 || processCount <= 0){
        printf("Process count must be a positive integer\n");
        exit(1);
    }
    process arr[processCount];

    printf("Enter the time quantum: ");
    if(scanf("%d", &timeQuantum) != 1 || timeQuantum <= 0){
        printf("Time quantum must be a positive integer\n");
        exit(1);
    }

    printf("\nEnter the Arrival Time and Burst Time of the processes:\n");
    for(int i = 0; i < processCount; i++){
        printf("Process%d: ", i);
        if(scanf("%d %d", &arr[i].arrival_time, &arr[i].burst_time) != 2 || arr[i].arrival_time < 0 || arr[i].burst_time < 0){
            printf("Arrival Time and Burst Time must be >= 0\n");
            exit(1);
        }
        arr[i].tempBurstTime = arr[i].burst_time;
        arr[i].pid = i;
        arr[i].start_time = -1;
        arr[i].end_time = 0;
        arr[i].turnaround_time = 0;
        arr[i].waiting_time = 0;
        arr[i].isCompleted = 0;
        arr[i].next = NULL;
    }
    processArrivalSort(arr, processCount);
    runningProcessCount = processCount;

    int timeCounter = 0;
    int i = 0;
    while(runningProcessCount > 0){

        process *currentProcess;
        int remainingTime = -1;
        
        if(!isReadyQueueEmpty()){
            currentProcess = dequeue();
            remainingTime = currentProcess->tempBurstTime;
            int execTime = min(remainingTime, timeQuantum);

            if(currentProcess->burst_time == currentProcess->tempBurstTime) currentProcess->start_time = timeCounter;
            insert(currentProcess->pid, timeCounter, timeCounter + execTime);
            timeCounter += execTime;
            remainingTime -= execTime;
            totalExecutionTime += execTime;
            currentProcess->tempBurstTime = remainingTime;
        }
        else if(i < processCount && arr[i].arrival_time > timeCounter){
            insert(-1, timeCounter, arr[i].arrival_time);
            timeCounter = arr[i].arrival_time;
        }

        while(i < processCount && arr[i].arrival_time <= timeCounter){
            if(arr[i].burst_time == 0){
                arr[i].isCompleted = 1;
                arr[i].start_time = arr[i].end_time = arr[i].arrival_time;
                runningProcessCount--;
            }
            else{
                enqueue(&arr[i]);
            }
            i++;
        }

        if(remainingTime > 0) enqueue(currentProcess);
        else if(remainingTime == 0){
            currentProcess->isCompleted = 1;
            currentProcess->end_time = timeCounter;
            runningProcessCount--;
        }
    }

    for(int i = 0; i < processCount; i++){
        arr[i].turnaround_time = arr[i].end_time - arr[i].arrival_time;
        arr[i].waiting_time = arr[i].turnaround_time - arr[i].burst_time;
        totalTurnaroundTime += arr[i].turnaround_time;
        totalWaitingTime += arr[i].waiting_time;
    }

    averageTurnaroundTime = totalTurnaroundTime / processCount;
    averageWaitingTime = totalWaitingTime / processCount;
    totalIdleTime = timeCounter - totalExecutionTime;
    if(timeCounter > 0){
        cpuUtil = (totalExecutionTime / timeCounter) * 100;
        cpuThroughput = ((float)processCount)/ timeCounter;
    }

    processIDSort(arr, processCount);
    printf("\nProcess ID\tArrival Time\tBurst Time\tTurnaround Time    Waiting Time\n");
    for(int i = 0; i < processCount; i++){
        printf("%d\t\t%d\t\t%d\t\t%d\t\t   %d\n", arr[i].pid, arr[i].arrival_time, arr[i].burst_time, (int)arr[i].turnaround_time, (int)arr[i].waiting_time);
    }

    printGC(); // Gantt Chart printing

    printf("\nAverage Turnaround Time: %.2f unit(s)\n", averageTurnaroundTime);
    printf("Average Waiting Time: %.2f unit(s)\n", averageWaitingTime);

    printf("\nTotal Idle Time: %.2f unit(s)\n", totalIdleTime);
    printf("CPU Utilization: %.2f%%\n", cpuUtil);

    printf("\nCPU Throughput: %.2f\n", cpuThroughput);

    freeGCMemory(); // Free memory space used by Gantt Chart
    return 0;
}
