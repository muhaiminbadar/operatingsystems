#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <pthread.h>

using std::vector;
using std::string;

struct CharData {
    char symbol;
    int frequency;
    std::string code;
};

struct Args {
    pthread_mutex_t readMutex;
    pthread_mutex_t printMutex;
    pthread_cond_t cond;
    vector<CharData>* symbols; 

    vector<CharData>::iterator print;
    int maxChars, curIndex;

    Args(vector<CharData>* s, int m)
        : symbols(s), maxChars(m)
    {
        pthread_mutex_init(&readMutex, NULL);
        pthread_mutex_init(&printMutex, NULL);
        pthread_cond_init(&cond, NULL);
        curIndex = 0;
    
        print = s->begin();
    }
};

void* sfeThread(void* arg) {
    Args* args = static_cast<Args*>(arg);
    CharData myData;
    int myIndex;
    double fbar, prob;

    // Wait to acquire resources.
    pthread_mutex_lock(&args->readMutex);
    myIndex = args->curIndex; // acquire disjoint Index. This will be unique to our (this) thread.
    ++args->curIndex; // Increment vector for assignment to next thread. Atomic operation.
    // We can now freely read data at this index without worrying about race conditions, etc.
    pthread_mutex_unlock(&args->readMutex);
    
    
    // SFE processing here

    auto myTurn = args->symbols->begin() + myIndex; // Assign turn for printing when we're done processing.
    myData = args->symbols->at(myIndex); // keep a local copy of the data
    prob =  (double) myData.frequency / (double) args->maxChars; // set probability
    
    // calculate fbar
    fbar = prob / 2;
    for(int i = myIndex - 1; i >= 0; i--) {
        fbar = fbar + ((double) args->symbols->at(i).frequency / (double)args->maxChars);
    }
    
    // calculate chars for SFE
    int n = ceil(log2(1/prob)) + 1;
    double t = fbar * 2;
    
    // fbar -> binary string
    myData.code = "";
    for(int i = 0; i < n; i ++, t *= 2) {
        int bit = static_cast<int>(t);
        myData.code += (bit == 1) + '0';
        t -= bit;
    }

    // Ready to print. wait until turn.
    pthread_mutex_lock(&args->printMutex);
    while(args->print != myTurn) // keep sleeping until our turn
    {
        pthread_cond_wait(&args->cond, &args->mutex);
    }
    
    // it's our turn, so just print.
    std::cout 
        << "Symbol " << myData.symbol << ", "
        << "Code: " << myData.code << 
    std::endl;

    // increment print turn and wakeup all threads.
    ++args->print;
    pthread_cond_broadcast(&args->cond);
    pthread_mutex_unlock(&args->printMutex);
   
    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {

    vector<CharData> symbols;

    string line;
    std::getline(std::cin, line);

    // Foreach char c in line -> find if c exists, if yes increase freq count else insert it into our symbols table with freq.
    for(const char& c : line) {
        auto it = std::find_if(symbols.begin(), symbols.end(), [&c] (const CharData& e) {
            return e.symbol == c;
        });        
        if(it != symbols.end()) {
            it->frequency++;
        } else {
            symbols.push_back({c, 1, ""});
        }
    }

    // Sort our symbols vector.
    std::sort(symbols.begin(), symbols.end(), [] (const auto& a, const auto& b) { 
        if(a.frequency == b.frequency) // Lexographical sort on same frequency.
            return a.symbol < b.symbol;
            
        return a.frequency > b.frequency; // Higher frequency first.
    });

    const int NUM_THREADS = symbols.size();
    pthread_t* threads = new pthread_t[NUM_THREADS];

    Args ar(&symbols, line.size());
    std::cout << "SHANNON-FANO-ELIAS Codes:\n\n";
    // Launch threads.
    for(int i = 0; i < NUM_THREADS; i++ ) {
        if(pthread_create(&threads[i], nullptr, sfeThread, static_cast<void*>(&ar))) { 
            std::cout << "Unable to create thread" << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Wait for all threads.
    for(int i = 0; i < NUM_THREADS; i++ ) {
        pthread_join(threads[i], nullptr);
    }
    // Delete the allocated memory.
    delete[] threads;

    return EXIT_SUCCESS;
}