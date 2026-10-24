#include<iostream>
#include "Transaction.h"
using namespace std;
    Transaction::Transaction(int i){
        this->id=i;
    }
    int Transaction::getid(){
        return id;
    }
    void Transaction::show(){
        cout<<"Transaction "<<id<<" is running"<<endl;
    }