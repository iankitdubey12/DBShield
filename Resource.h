#ifndef RESOURCE_H
#define RESOURCE_H
class Resource{
    int id;
    int owner;
    public:
    Resource(int i);
    int getid();
    int getowner();
    bool acquire(int TransactionID);
    void release(int TransactionID);
    void show();
};
#endif