#include <pthread.h>
#include <iostream>
#include <iomanip>

#define NOPER 4

struct operation
{
	int val1;
	int val2;
    int op;
    double result;
};

void *calculator(void *pos_void_ptr)
{
	struct operation *pos_ptr = (struct operation *)pos_void_ptr;
    switch(pos_ptr->op)
    {
        case 0: pos_ptr->result = pos_ptr->val1 + pos_ptr->val2; 
                break;
        case 1: pos_ptr->result = pos_ptr->val1 - pos_ptr->val2; 
                break;
        case 2: pos_ptr->result = pos_ptr->val1 * pos_ptr->val2; 
                break;
        case 3: if (pos_ptr->val2 != 0)
                    pos_ptr->result = (double) pos_ptr->val1 / pos_ptr->val2; 
                else
                    pos_ptr->result = 0;         
                break;
    }
	return NULL;
}

int main()
{
	static struct operation operations[NOPER];
 	pthread_t tid[NOPER];
        
	for(int i=0;i<NOPER;i++)
	{
		operations[i].op = i;
        std::cin >> operations[i].val1;
        std::cin >> operations[i].val2;
		if(pthread_create(&tid[i], NULL, calculator, &operations[i])) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;

		}		
	}
	// Wait for the other threads to finish.
	for (int i = 0; i < NOPER; i++)
        	pthread_join(tid[i], NULL);
    for (int i = 0; i < NOPER; i++)
    {
        switch(operations[i].op)
        {
            case 0: std::cout << operations[i].val1 << " + " << operations[i].val2 << " = " << std::fixed << std::setprecision(2) << operations[i].result << std::endl;
                    break;
            case 1: std::cout << operations[i].val1 << " - " << operations[i].val2 << " = " << std::fixed << std::setprecision(2) << operations[i].result << std::endl;
                    break;
            case 2: std::cout << operations[i].val1 << " * " << operations[i].val2 << " = " << std::fixed << std::setprecision(2) << operations[i].result << std::endl;
                    break;
            case 3: std::cout << operations[i].val1 << " / " << operations[i].val2 << " = " << std::fixed << std::setprecision(2) << operations[i].result << std::endl;
                    break;
        }
    }
    return 0;
}