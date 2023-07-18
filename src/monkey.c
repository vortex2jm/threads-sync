#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define MONKEYS_NUM 10

//Semáforo para travessia
sem_t semaphore;

// Quantidade de macacos de cada lado que requisitaram a travessia
short  l_monkeys=0, r_monkeys=0;

// Indica se o primeiro macaco de cada lado está esperando para atravessar
bool l_queue=false, r_queue=false;

// Função de rotina
void * monkey_routine(void * arg);

//===================================//
int main(int argc, char const *argv[])
{   
    sem_init(&semaphore, 0, 1);
    pthread_t monkeys[MONKEYS_NUM];
    
    for (int i = 0; i < MONKEYS_NUM; i++){
        usleep(10000);
        if(pthread_create(&monkeys[i], NULL, monkey_routine, &i)){
            perror("Failed to create thread!\n");
            exit(1);
        };
    }
    for(int i = 0; i < MONKEYS_NUM; i++){
        if(pthread_join(monkeys[i], NULL)){
            perror("Failed to join thread!\n");
            exit(1);
        }
    }

    sem_destroy(&semaphore);
    return 0;
}

//=======================//
void * monkey_routine(void * arg){
    int x = *(int*)arg;

    // Mudando tempo em que um macaco entra na ponte
    sleep(rand() % 10);

    // par = macacos da esquerda
    if(!(x & 1)){
        if(!l_monkeys){
            l_monkeys++;
            l_queue=true;
            sem_wait(&semaphore);
            l_queue=false;
        }
        else{
            l_monkeys++;
            while(l_queue){}
        }
        
        printf("Monkey( %d )[LEFT] O..........CROSSING >>..........O\n", x);
        fflush(stdout);
        // Mudando o tempo de travessia do macaco
        sleep(rand() % 5);
        printf("Monkey( %d )[LEFT] !!!ARRIVED!!!\n", x);
        fflush(stdout);

        if(l_monkeys == 1){
            sem_post(&semaphore);
            l_monkeys--;
        }
        else{
            l_monkeys--;
        }
        pthread_exit(NULL);
    }

    // ímpar = macacos da direita
    if(!r_monkeys){
        r_monkeys++;
        r_queue = true;
        sem_wait(&semaphore);
        r_queue = false;
    }
    else{
        r_monkeys++;
        while(r_queue){}
    }

    printf("Monkey( %d )[RIGHT] O..........CROSSING >>..........O\n", x);
    fflush(stdout);

    // Mudando o tempo de travessia do macaco
    sleep(rand() % 10);
    
    printf("Monkey( %d )[RIGHT] !!!ARRIVED!!!\n", x);
    fflush(stdout);
    if(r_monkeys == 1){
        sem_post(&semaphore);
        r_monkeys--;
    }
    else{
        r_monkeys--;
    }
    
    pthread_exit(NULL);
}
