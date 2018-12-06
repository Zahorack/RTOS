#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int global_var =0;
int pole[20];
int i =0;
int sum1 =0, sum2 = 0;
void * thread1()
{
        while(1){
                printf("Thread 1\n");
//		global_var++;
//      	printf("global_var: %d", global_var);

		sum1 += pole[i++];
		if(i >= sizeof(pole)/2) break;
	}
}

void * thread2()
{
        while(1){
              printf("Thread 2\n");
//		global_var++;
//		printf("global_var: %d", global_var);

		sum2 += pole[i++];
		if(i >= sizeof(pole)) break;
	}
}

int main()
{
        int status;
        pthread_t tid1,tid2;	
	
	pole[0] = 0;
	for(int i =1; i < sizeof(pole); i++){
		pole[i] = pole[i-1] +1;
	}

        pthread_create(&tid1,NULL,thread1,NULL);
	pthread_create(&tid2,NULL,thread2,NULL);

	pthread_join(tid1, NULL);        
	printf("suma1 = %d\r\n", sum1);

        pthread_join(tid2,NULL);
printf("suma2 = %d\r\n", sum2);
printf("Suma celkova = %d", sum1 + sum2);

        return 0;
}

