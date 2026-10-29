#include<iostream>
#include "Scheduler.h"
using namespace std;
void Scheduler::addTask(int tid){
    tasks.push_back(tid);
}
void Scheduler::showTasks(){
    cout<<"Scheduled Tasks:";
    if(tasks.empty()){
        cout<<"None";
    }
    for(int taskid:tasks){
        cout<<taskid<<" ";
    }
    cout<<endl;
}
void Scheduler::clearTasks(){
    tasks.clear();
    cout<<"Schedule cleared."<<endl;
}