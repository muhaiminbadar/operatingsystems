// Write your program here
#include <iostream>
#include <string> 
#include <vector>
#include <cmath>
#include <pthread.h>
#include <fstream>
#include <algorithm>
using namespace std; 

struct myStruct {
    vector<string> symbols;                     //String of symbols ex: A B C D 
    vector<double> probab;                      //Probabilities of each string ex: 0.3 0.25 0.2
	vector<double> fx;
	vector<double> fbar; 		 
	vector<double> length; 
	vector<string> binary; 
    pthread_mutex_t *semphore_1; // first semaphore
    pthread_mutex_t *semphore_2; // second semaphore 
    pthread_cond_t *wait_signal; // wait for the next turn 
    int turn = 0; // control the ordering of the symbols/probabilities
    int thread_id; // id of the thread  
};

void *calculateFunc(void *Shannon) {
	struct myStruct *Temp = (struct myStruct*)Shannon; 

    // Calculating fx
    double temp_acc = 0;    //accumulator to hold vector values
    for (int i = 0; i < Temp->probab; i++)
    {
        temp_acc += Temp->probab;       // Doing the fx calculation  
        Temp->fx = temp_acc;            // push back the result as a double 
    }
    
	//fbar(x)
	double temp_fbar = 0; 
	for (int i = 0; i < Temp->fx; i++)
	{
		temp_fbar = Temp->probab / 2 + (Temp->fx - Temp->probab); 
		Temp->fbar = temp_fbar; 
	}

	//length of x
	double temp_len; //To store length of x
    for (int i = 0; i < Temp->probab; i++)
    {
        temp_len = ceil(log2 (1 / Temp->probab) + 1); 
        Temp->length = temp_len;  
    }

	//binary of x
	double temp_fbarx;   //store fbar of x 
    temp_fbarx = Temp->fbar; 
	for (int i = 0; i < Temp->length; i++)
    {
		temp_fbarx *= 2; 
		if (temp_fbarx >= 1)
		{
			Temp->binary += '1'; 
			temp_fbarx -= 1; 
		}
		else {
			Temp->binary += '0'; 
		}
    }
    pthread_mutex_lock(Temp->semphore_2); 
    while(Temp->turn != Temp->thread_id)
        pthread_cond_wait(Temp->wait_signal, Temp->semphore_2);
    pthread_mutex_unlock(Temp->semphore_2);
    for (int i = 0; i < Temp->probab.size(); i++)
        //std::cout << "Symbol " << Temp->symbols << ", Code: " << Temp->binary << std::endl;

    pthread_mutex_lock(Temp->semphore_2); 
    Temp->turn++; 
    pthread_cond_broadcast(Temp->wait_signal); 
    pthread_mutex_unlock(Temp->semphore_2); 
	return NULL; 
}

int main()
{
    myStruct SHANNON;  // struct object 
    std::vector<std::string> temp_input;    // Used to store temporary input 
    std::vector<std::string> SYMBOLS;       // Used to store symbols in a vector and push it back to the struct 
    std::vector<double>      PROBABILITIES; // Used to store the probabilities and push it back to the struct 
    std::vector<double>      fx;            // Used to store the fx and push it back to the struct 
    
    pthread_mutex_t in_main_1; // critical section for main 
    pthread_mutex_t in_main_2; // critical section for main 
    pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER; // wait in main for critical section 
    pthread_mutex_init(&in_main_1, NULL); 
    pthread_mutex_init(&in_main_2, NULL);

    // Getting the input as a string 
    std::string input; 
    std::cin >> input; 

    // Converting the input into vector
    for (int i = 0; i < input.size(); i++)
    {
        std::string temp; 
        temp = input[i];            // getting each character of a input as a string and sending it to a vector
        temp_input.push_back(temp); // push back the string to a temp_input vector
    }

    // Sorting the symbols by ascending order A, B, C... 1,2,3 
    sort(temp_input.begin(),temp_input.end());

    // Storing unique symbols 
    for (int i = 0; i < temp_input.size(); i++)
    {
        if (temp_input[i] == temp_input[i+1])
            continue;
        else 
        {
            std::string temp_character;
            temp_character = temp_input[i];     // getting each unique character as a string and sending it to a vector 
            SYMBOLS.push_back(temp_character);  // push back the string to a SYMBOLS vector
        }
    }


    // storing probabilities
    double accumulator = 1; 
    for (int i = 0; i < temp_input.size(); i++)
    {
        if (temp_input[i] == temp_input[i+1])
        {
            accumulator++;              // If two characters are the same AA, BB, CC, ... and so on
        }
        else 
        {
            //calculating the probability of each symbol
            double temp_probab; 
            temp_probab = accumulator / temp_input.size();  // delete the number of characters / total number of characters 
            PROBABILITIES.push_back(temp_probab);           // push back the result as a double 
            accumulator = 1; 
        }
    }
    sort(PROBABILITIES.begin(), PROBABILITIES.end(), std::greater<double>());      // Sort the probabilities from an ascending order 

    //Pushing back to struct
    for (int i = 0; i < SYMBOLS.size(); i++)
    {
        myStruct Temp;
        // pthread_mutex_lock(&in_main_1); 
        // Temp.thread_id = i;
        string temp = ""; 
        double temp2 = 0; 
        temp = SYMBOLS[i];   
        Temp.symbols.push_back(temp);
        temp2 =  PROBABILITIES[i]; 
        Temp.probab.push_back(temp2); 

        SHANNON = Temp;
        // pthread_mutex_unlock(&in_main_1); 
    }
    
    //Creating a thread
    const int SIZE = PROBABILITIES.size(); 
    pthread_t tid[SIZE];

    std::cout << "SHANNON-FANO-ELIAS Codes: " << std::endl << std::endl; 
	for(int i = 0; i < SIZE; i++){
		if(pthread_create(&tid[i], NULL, calculateFunc, &SHANNON)) 
		{
            std::cout << i << std::endl; 
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}	
        for(int i = 0; i < SIZE; i++){
		    pthread_join(tid[i], NULL);
	    }
	}
    return 0; 
}                                                                                         