#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <climits>
#include <tuple>

#define all(v) v.begin(), v.end()

using namespace std;

/** Declaring the Global Constants **/
const string TRACE = "trace";
const string SHOW_STATISTICS = "stats";
const string ALGORITHMS[9] = {"", "FCFS", "RR-", "SPN", "SRT", "HRRN", "FB-1", "FB-2i", "Aging"};

/** Declaring the Global Variables **/
vector<tuple<string, int, int> > processes;
vector<pair<char, int> > algorithms;
map<string, int> processToIndex;
vector<vector<char> > timeline;
vector<int> finishTime;
vector<int> turnAroundTime;
vector<float> normTurn;
string operation;
int last_instant;
int process_count;

/** Implementation of Parsing Functions **/
void parse_algorithms(string algorithm_chunk) {
    stringstream stream(algorithm_chunk);
    while (stream.good()) {
        string temp_str;
        getline(stream, temp_str, ',');
        stringstream ss(temp_str);
        getline(ss, temp_str, '-');
        char algorithm_id = temp_str[0];
        getline(ss, temp_str, '-');
        int quantum = temp_str.size() >= 1 ? stoi(temp_str) : -1;
        algorithms.push_back(make_pair(algorithm_id, quantum));
    }
}

void parse_processes() {
    string process_chunk, process_name;
    int process_arrival_time, process_service_time;
    for (int i = 0; i < process_count; i++) {
        cin >> process_chunk;

        stringstream stream(process_chunk);
        string temp_str;
        getline(stream, temp_str, ',');
        process_name = temp_str;
        getline(stream, temp_str, ',');
        process_arrival_time = stoi(temp_str);
        getline(stream, temp_str, ',');
        process_service_time = stoi(temp_str);

        processes.push_back(make_tuple(process_name, process_arrival_time, process_service_time));
        processToIndex[process_name] = i;
    }
}

void parse() {
    string algorithm_chunk;
    cin >> operation >> algorithm_chunk >> last_instant >> process_count;
    parse_algorithms(algorithm_chunk);
    parse_processes();
    finishTime.resize(process_count);
    turnAroundTime.resize(process_count);
    normTurn.resize(process_count);
    timeline.resize(last_instant);
    for (int i = 0; i < last_instant; i++)
        for (int j = 0; j < process_count; j++)
            timeline[i].push_back(' ');
}

/**Implementations of Functions **/

void clear_timeline() {
    for (int i = 0; i < last_instant; i++) {
        fill(timeline[i].begin(), timeline[i].end(), ' ');
    }
}

void printFinishTime() {
    for (int i = 0; i < process_count; i++) {
        cout << "Process " << get<0>(processes[i]) << " finished at time " << finishTime[i] << endl;
    }
}

void printTurnAroundTime() {
    for (int i = 0; i < process_count; i++) {
        cout << "Process " << get<0>(processes[i]) << " turnaround time is " << turnAroundTime[i] << endl;
    }
}

void printNormTurn() {
    for (int i = 0; i < process_count; i++) {
        cout << "Process " << get<0>(processes[i]) << " normalized turnaround time is " << normTurn[i] << endl;
    }
}

void printTimeline(int algorithm_index) {
    for (int i = 0; i <= last_instant; i++)
        cout << i % 10 << " ";
    cout << "\n";
    cout << "------------------------------------------------\n";
    for (int i = 0; i < process_count; i++) {
        cout << get<0>(processes[i]) << "     |";
        for (int j = 0; j < last_instant; j++) {
            cout << timeline[j][i] << "|";
        }
        cout << " \n";
    }
    cout << "------------------------------------------------\n";
}

void printStats(int algorithm_index) {
    printFinishTime();
    printTurnAroundTime();
    printNormTurn();
}

/** Implementations of Scheduling algorithms**/
void firstComeFirstServe() {
    int currentTime = 0;
    for (int i = 0; i < process_count; i++) {
        int arrival = get<1>(processes[i]);
        int service = get<2>(processes[i]);
        currentTime = max(currentTime, arrival);
        for (int j = 0; j < service; j++) {
            timeline[currentTime][i] = '*';
            currentTime++;
        }
        finishTime[i] = currentTime;
        turnAroundTime[i] = finishTime[i] - arrival;
        normTurn[i] = (float)turnAroundTime[i] / service;
    }
}

void roundRobin(int quantum) {
    vector<int> remainingServiceTime(process_count);
    for (int i = 0; i < process_count; i++) {
        remainingServiceTime[i] = get<2>(processes[i]);
    }
    queue<int> readyQueue;
    int currentTime = 0;
    int processedCount = 0;

    while (processedCount < process_count) {
        for (int i = 0; i < process_count; i++) {
            if (get<1>(processes[i]) == currentTime) {
                readyQueue.push(i);
            }
        }
        if (!readyQueue.empty()) {
            int index = readyQueue.front();
            readyQueue.pop();
            int executionTime = min(quantum, remainingServiceTime[index]);
            for (int j = 0; j < executionTime; j++) {
                timeline[currentTime][index] = '*';
                currentTime++;
            }
            remainingServiceTime[index] -= executionTime;
            if (remainingServiceTime[index] > 0) {
                readyQueue.push(index);
            } else {
                finishTime[index] = currentTime;
                turnAroundTime[index] = finishTime[index] - get<1>(processes[index]);
                normTurn[index] = (float)turnAroundTime[index] / get<2>(processes[index]);
                processedCount++;
            }
        } else {
            currentTime++;
        }
    }
}

void shortestProcessNext() {
    vector<int> remainingServiceTime(process_count);
    for (int i = 0; i < process_count; i++) {
        remainingServiceTime[i] = get<2>(processes[i]);
    }
    int currentTime = 0;
    int processedCount = 0;

    while (processedCount < process_count) {
        int minServiceTime = INT_MAX;
        int index = -1;
        for (int i = 0; i < process_count; i++) {
            if (get<1>(processes[i]) <= currentTime && remainingServiceTime[i] > 0 && remainingServiceTime[i] < minServiceTime) {
                minServiceTime = remainingServiceTime[i];
                index = i;
            }
        }
        if (index != -1) {
            for (int j = 0; j < remainingServiceTime[index]; j++) {
                timeline[currentTime][index] = '*';
                currentTime++;
            }
            finishTime[index] = currentTime;
            turnAroundTime[index] = finishTime[index] - get<1>(processes[index]);
            normTurn[index] = (float)turnAroundTime[index] / get<2>(processes[index]);
            remainingServiceTime[index] = 0;
            processedCount++;
        } else {
            currentTime++;
        }
    }
}

void shortestRemainingTime() {
    vector<int> remainingServiceTime(process_count);
    for (int i = 0; i < process_count; i++) {
        remainingServiceTime[i] = get<2>(processes[i]);
    }
    int currentTime = 0;
    int processedCount = 0;

    while (processedCount < process_count) {
        int minServiceTime = INT_MAX;
        int index = -1;
        for (int i = 0; i < process_count; i++) {
            if (get<1>(processes[i]) <= currentTime && remainingServiceTime[i] > 0 && remainingServiceTime[i] < minServiceTime) {
                minServiceTime = remainingServiceTime[i];
                index = i;
            }
        }
        if (index != -1) {
            timeline[currentTime][index] = '*';
            currentTime++;
            remainingServiceTime[index]--;
            if (remainingServiceTime[index] == 0) {
                finishTime[index] = currentTime;
                turnAroundTime[index] = finishTime[index] - get<1>(processes[index]);
                normTurn[index] = (float)turnAroundTime[index] / get<2>(processes[index]);
                processedCount++;
            }
        } else {
            currentTime++;
        }
    }
}

void highestResponseRatioNext() {
    vector<int> remainingServiceTime(process_count);
    for (int i = 0; i < process_count; i++) {
        remainingServiceTime[i] = get<2>(processes[i]);
    }
    int currentTime = 0;
    int processedCount = 0;

    while (processedCount < process_count) {
        float maxResponseRatio = -1;
        int index = -1;
        for (int i = 0; i < process_count; i++) {
            if (get<1>(processes[i]) <= currentTime && remainingServiceTime[i] > 0) {
                float responseRatio = (float)(currentTime - get<1>(processes[i]) + remainingServiceTime[i]) / remainingServiceTime[i];
                if (responseRatio > maxResponseRatio) {
                    maxResponseRatio = responseRatio;
                    index = i;
                }
            }
        }
        if (index != -1) {
            for (int j = 0; j < remainingServiceTime[index]; j++) {
                timeline[currentTime][index] = '*';
                currentTime++;
            }
            finishTime[index] = currentTime;
            turnAroundTime[index] = finishTime[index] - get<1>(processes[index]);
            normTurn[index] = (float)turnAroundTime[index] / get<2>(processes[index]);
            remainingServiceTime[index] = 0;
            processedCount++;
        } else {
            currentTime++;
        }
    }
}

void feedbackQ1() {
    queue<int> readyQueue;
    vector<int> remainingServiceTime(process_count);
    for (int i = 0; i < process_count; i++) {
        remainingServiceTime[i] = get<2>(processes[i]);
    }
    int currentTime = 0;
    int processedCount = 0;

    while (processedCount < process_count) {
        for (int i = 0; i < process_count; i++) {
            if (get<1>(processes[i]) == currentTime) {
                readyQueue.push(i);
            }
        }
        if (!readyQueue.empty()) {
            int index = readyQueue.front();
            readyQueue.pop();
            timeline[currentTime][index] = '*';
            currentTime++;
            remainingServiceTime[index]--;
            if (remainingServiceTime[index] > 0) {
                readyQueue.push(index);
            } else {
                finishTime[index] = currentTime;
                turnAroundTime[index] = finishTime[index] - get<1>(processes[index]);
                normTurn[index] = (float)turnAroundTime[index] / get<2>(processes[index]);
                processedCount++;
            }
        } else {
            currentTime++;
        }
    }
}

void feedbackQ2i() {
    vector<queue<int> > readyQueues(32);
    vector<int> remainingServiceTime(process_count);
    for (int i = 0; i < process_count; i++) {
        remainingServiceTime[i] = get<2>(processes[i]);
    }
    int currentTime = 0;
    int processedCount = 0;
    int quantum = 1;

    while (processedCount < process_count) {
        for (int i = 0; i < process_count; i++) {
            if (get<1>(processes[i]) == currentTime) {
                readyQueues[0].push(i);
            }
        }
        int i;
        for (i = 0; i < 32 && readyQueues[i].empty(); i++);
        if (i < 32) {
            int index = readyQueues[i].front();
            readyQueues[i].pop();
            int executionTime = min(quantum, remainingServiceTime[index]);
            for (int j = 0; j < executionTime; j++) {
                timeline[currentTime][index] = '*';
                currentTime++;
            }
            remainingServiceTime[index] -= executionTime;
            if (remainingServiceTime[index] > 0) {
                if (i + 1 < 32) {
                    readyQueues[i + 1].push(index);
                } else {
                    readyQueues[i].push(index);
                }
            } else {
                finishTime[index] = currentTime;
                turnAroundTime[index] = finishTime[index] - get<1>(processes[index]);
                normTurn[index] = (float)turnAroundTime[index] / get<2>(processes[index]);
                processedCount++;
            }
            quantum = 1 << (i + 1);
        } else {
            currentTime++;
        }
    }
}

void aging(int quantum) {
    vector<int> remainingServiceTime(process_count);
    vector<int> priority(process_count, 0);
    for (int i = 0; i < process_count; i++) {
        remainingServiceTime[i] = get<2>(processes[i]);
    }
    int currentTime = 0;
    int processedCount = 0;

    while (processedCount < process_count) {
        int maxPriority = INT_MIN;
        int index = -1;
        for (int i = 0; i < process_count; i++) {
            if (get<1>(processes[i]) <= currentTime && remainingServiceTime[i] > 0 && priority[i] > maxPriority) {
                maxPriority = priority[i];
                index = i;
            }
        }
        if (index != -1) {
            for (int j = 0; j < min(quantum, remainingServiceTime[index]); j++) {
                timeline[currentTime][index] = '*';
                currentTime++;
            }
            remainingServiceTime[index] -= min(quantum, remainingServiceTime[index]);
            if (remainingServiceTime[index] <= 0) {
                finishTime[index] = currentTime;
                turnAroundTime[index] = finishTime[index] - get<1>(processes[index]);
                normTurn[index] = (float)turnAroundTime[index] / get<2>(processes[index]);
                processedCount++;
            }
        } else {
            currentTime++;
        }
        for (int i = 0; i < process_count; i++) {
            if (get<1>(processes[i]) <= currentTime && remainingServiceTime[i] > 0) {
                priority[i]++;
            }
        }
    }
}

/** Main Execution Function **/
void execute_algorithm(char algorithm_id, int quantum, string operation) {
    switch (algorithm_id) {
    case '1':
        if (operation == TRACE) cout << "FCFS  ";
        firstComeFirstServe();
        break;
    case '2':
        if (operation == TRACE) cout << "RR-" << quantum << "  ";
        roundRobin(quantum);
        break;
    case '3':
        if (operation == TRACE) cout << "SPN   ";
        shortestProcessNext();
        break;
    case '4':
        if (operation == TRACE) cout << "SRT   ";
        shortestRemainingTime();
        break;
    case '5':
        if (operation == TRACE) cout << "HRRN  ";
        highestResponseRatioNext();
        break;
    case '6':
        if (operation == TRACE) cout << "FB-1  ";
        feedbackQ1();
        break;
    case '7':
        if (operation == TRACE) cout << "FB-2i ";
        feedbackQ2i();
        break;
    case '8':
        if (operation == TRACE) cout << "Aging ";
        aging(quantum);
        break;
    default:
        break;
    }
}

/** Main Function **/
int main() {
    parse();
    for (int idx = 0; idx < (int)algorithms.size(); idx++) {
        clear_timeline();
        execute_algorithm(algorithms[idx].first, algorithms[idx].second, operation);
        if (operation == TRACE)
            printTimeline(idx);
        else if (operation == SHOW_STATISTICS)
            printStats(idx);
        cout << "\n";
    }
    return 0;
}
