#include <iostream>
#include <pthread.h>


struct Args {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    
    int* numbers = nullptr;
    unsigned int turn = 0;
    unsigned int secondTurn = 0;
    Args(int size) {
        numbers = new int[size];
    }
    ~Args() {
        delete[] numbers;
    }
};

void* threadFxn(void* void_arg) {
    Args* arg = (Args*) void_arg;
    pthread_mutex_t* mutex = &arg->mutex;
    pthread_cond_t* cond = &arg->condition;
    
    unsigned int myTurn;
    int myNumber;

    pthread_mutex_lock(mutex);
    myTurn = arg->turn;
    arg->turn++;
    pthread_mutex_unlock(mutex);
    

    myNumber = arg->numbers[myTurn];
    myNumber = myNumber / 10;

    pthread_mutex_lock(mutex);
    while(arg->secondTurn != myTurn)
        pthread_cond_wait(cond, mutex);

    std::cout << myNumber << std::endl;
    arg->secondTurn++;
    pthread_cond_broadcast(cond);
    pthread_mutex_unlock(mutex);
    return nullptr;
}


int main() {
    const int size = 1000;

    Args arg(size);
    arg.numbers = new int[size];
    arg.mutex = PTHREAD_MUTEX_INITIALIZER;
    arg.condition = PTHREAD_COND_INITIALIZER; 

    for(int i = size; i >= 0; i --) {
        arg.numbers[i] = i * 10;
    }
    
    pthread_t* tid = new pthread_t[size];
    for(int i = 0; i < size; i ++)
        pthread_create(&tid[i], nullptr, threadFxn, (void *) &arg);
    for(int i = 0; i < size; i++)
        pthread_join(tid[i], nullptr);

    delete[] tid;
    return 0;
}