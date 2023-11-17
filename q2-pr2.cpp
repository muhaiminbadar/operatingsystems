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
static char turn[] = "RINCON";
static bool busy = false;

void *access_one_at_a_time(void *family_void_ptr)
{

	pthread_mutex_lock(&bsem);
	char fam[20];
	strcpy(fam,(char *) family_void_ptr);
	while (busy == true || strcmp(fam,turn)!=0)
    { 
        if(strcmp(fam,"RINCON")==0)
		    pthread_cond_wait(&rincon, &bsem);
        else
            pthread_cond_wait(&castro, &bsem);
    }
    busy = true;
	std::cout << fam << " member inside the house\n";
	pthread_mutex_unlock(&bsem);
    
    usleep(100);

 	pthread_mutex_lock(&bsem);
	std::cout << fam << " member leaving the house\n";
    busy = false;
	if (strcmp(turn,"RINCON") == 0)
    {
        strcpy(turn,"CASTRO");
		pthread_cond_signal(&castro);
    }
    else
    {
        strcpy(turn,"RINCON");
        pthread_cond_signal(&rincon);
    }
 	pthread_mutex_unlock(&bsem);
	return NULL;
}

int main()
{
    int nmembers;
    std::cin >> nmembers;
    pthread_mutex_init(&bsem, NULL); // Initialize access to 1
 	pthread_t *tid= new pthread_t[nmembers];
	char **family=new char*[nmembers];
    for(int i=0;i<nmembers;i++)
        family[i]=new char[20];
	for(int i=0;i<nmembers;i++)
	{
	    if(i%2 == 0)
			strcpy(family[i],"RINCON");
		else
			strcpy(family[i],"CASTRO");
	    if(pthread_create(&tid[i], NULL, access_one_at_a_time,(void *)family[i])) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;

		}
	
	}
	// Wait for the other threads to finish.
	for (int i = 0; i < nmembers; i++)
        	pthread_join(tid[i], NULL);
    for(int i=0;i<nmembers;i++)
        delete [] family[i];
    delete [] family;
    delete [] tid;
	return 0;
}