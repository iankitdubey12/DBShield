#ifndef SCHEDULER_H
#define SCHEDULER_H
#include<vector>
using namespace std;
class Scheduler{
    vector<int>tasks;
    public:
    void addTask(int taskid);
    void showTasks();
    void clearTasks();
};
#endif