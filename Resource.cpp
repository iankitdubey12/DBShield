#include<iostream>
#include "Resource.h"
using namespace std;
    Resource::Resource(int i){
        this->id=i;
        this->owner=-1;
    }
    int Resource::getid(){
        return id;
    }
    int Resource::getowner(){
        return owner;
    }
    bool Resource::acquire(int T){
        if(this->owner==-1){
            this->owner=T;
            cout<<"Resource "<<id<<" is acquired by "<<T<<endl;
        return true;
        }
        else{
            cout<<"Resource "<<id<<" is requested by "<<T<<" but held by "<<owner<<endl;
        }
     return false;
    }
    void Resource::release(int T){
        if(owner==T){
            owner=-1;
            cout<<"Transaction "<<T<<" Releases Resource "<<id<<endl;
        }
        else{
            cout<<"Transaction "<<T<<" Can't release Resource "<<id<<endl;
        }
    }
     void Resource::show(){
        if(owner==-1){
            cout<<"Resource "<<id<<" is free"<<endl;
        }
        else{
            cout<<"Resource "<<id<<" is held by Transaction "<<owner<<endl;
        }
     }