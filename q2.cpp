#include <pthread.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

static pthread_mutex_t bsem;
static pthread_cond_t rincon = PTHREAD_COND_INITIALIZER;
static pthread_cond_t castro = PTHREAD_COND_INITIALIZER;
static int members = 0;

void *access_one_at_a_time(void *family_void_ptr)
{
    char* fam = static_cast<char*>(family_void_ptr);
    bool is_rincon = strcmp(fam, "RINCON") == 0;

    pthread_mutex_lock(&bsem);
    if(members > 0) 
    {
        if(is_rincon) {
            pthread_cond_wait(&rincon, &bsem);
        } else {
            pthread_cond_wait(&castro, &bsem);
        } 
    }
    members++;
    std::cout << fam << " member inside" << std::endl;
    pthread_mutex_unlock(&bsem);

    pthread_mutex_lock(&bsem);
    std::cout << fam << " member outside" << std::endl;
    pthread_mutex_unlock(&bsem);

    if(is_rincon) {
        while(pthread_cond_signal(&castro) != 0);
    } else {
        while(pthread_cond_signal(&rincon) != 0);
    }

    return NULL;
}

int main()
{
    static int members = 0;
    int nmembers;
    std::cin >> nmembers;
    pthread_mutex_init(&bsem, NULL); // Initialize access to 1
 	pthread_t* tid = new pthread_t[nmembers];
	char** family = new char*[nmembers];
    for(int i = 0; i < nmembers; i++) 
        family[i] = new char[20];

	for(int i=0;i<nmembers;i++)
	{
	    if(i % 2 == 0)
	        strcpy(family[i], "RINCON");
	    else
	        strcpy(family[i], "CASTRO");
	        
	    // Write the code to call pthread_create
	    if(pthread_create(&tid[i], nullptr, access_one_at_a_time, (void*) family[i])) {
            return EXIT_FAILURE;
        }
	}
	// Wait for the other threads to finish.
	for (int i = 0; i < nmembers; i++) {
        pthread_join(tid[i], NULL);
        delete[] family[i];
    }

    delete[] family;
    delete[] tid;
	return EXIT_SUCCESS;
}