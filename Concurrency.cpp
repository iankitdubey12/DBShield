#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include "Concurrency.h"
using namespace std;
void Concurrency::LostUpdate(){
    int balance=1000;
    int readCount=0;
    mutex balMutex;
    condition_variable cv;
    mutex outMutex;
    auto transaction=[&](int id,int amount){
        int localbal;
        {
            unique_lock<mutex> lock(balMutex);
            localbal=balance;
            readCount++;
        {
            lock_guard<mutex> outputLock(outMutex);
            cout<<"Transaction:"<<id<<"Read balance:"<<localbal<<endl;
        }
            cv.notify_all();
            cv.wait(lock,[&](){return readCount==2;});
        }
            localbal+=amount;
            {
            lock_guard<mutex> lock(balMutex);
            balance=localbal;
            lock_guard<mutex> outputLock(outMutex);
            cout<<"Transaction: "<<id<<"  Writes balance: "<<balance<<endl;}
    };
    thread t1(transaction,1,200);
    thread t2(transaction,2,500);
    t1.join();
    t2.join();
    cout<<"Lost Update Demonstration:-"<<endl;
    cout<<"Initial Balance: 1000"<<endl;
    cout<<"Transaction 1 adds: 200"<<endl;
    cout<<"Transaction 2 adds: 500"<<endl;
    cout<<"Final Balance:"<<balance<<endl;
    if(balance!=1700){
    cout<<"Lost Update demonstration"<<endl;}
}

void Concurrency::Locking(){
    int balance=1000;
    mutex balMutex;
    mutex outMutex;
    auto transaction=[&](int id,int amount){
        lock_guard<mutex>lock(balMutex);
        int oldbal=balance;
        balance=oldbal+amount;
        lock_guard<mutex>outputLock(outMutex);
        cout<<"Transaction "<<id<<":"<< oldbal<<"+"<<amount<<"="<<balance<<endl;
    };
    thread t1(transaction,1,200);
    thread t2(transaction,2,500);
    t1.join();
    t2.join();
    cout<<"Locking Demonstration"<<endl;
    cout<<"Initial Balance:1000"<<endl;
    cout<<"Expected Final Balance: 1700"<<endl;
    cout<<"Actual Final Balance:"<<balance<<endl;
}