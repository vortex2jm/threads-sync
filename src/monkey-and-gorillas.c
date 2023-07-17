#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MONKEYS_NUM 10
#define GORILLAS_NUM 2

// Semáforo para travessia
sem_t semaphore;

// Quantidade de macacos de cada lado que requisitaram a travessia
short l_monkeys = 0, r_monkeys = 0;

// Quantidade de gorilas de cada lado que requisitaram a travessia
short l_gorillas = 0, r_gorillas = 0;

// Indica se o primeiro macaco de cada lado está esperando para atravessar
bool l_monkey_queue = false, r_monkey_queue = false;

// Indica se o primeiro gorila de cada lado está esperando para atravessar
bool l_gorilla_queue = false, r_gorilla_queue = false;

// Indica se algum gorila está atravessando
bool gorilla_crossing = false;

// Função de rotina
void *monkey_routine(void *arg);
void *gorilla_routine(void *arg);

//===================================//
int main(int argc, char const *argv[]) {
  sem_init(&semaphore, 0, 1);
  pthread_t monkeys[MONKEYS_NUM];
  pthread_t gorillas[GORILLAS_NUM];

  for (int i = 0; i < MONKEYS_NUM; i++) {
    usleep(1000);
    int i_local = i;
    if (pthread_create(&monkeys[i], NULL, monkey_routine, &i_local)) {
      perror("Failed to create thread!\n");
      exit(1);
    };
  }
  for (int i = 0; i < GORILLAS_NUM; i++) {
    usleep(1000);
    int i_local = i;
    if (pthread_create(&gorillas[i], NULL, gorilla_routine, &i_local)) {
      perror("Failed to create thread!\n");
      exit(1);
    };
  }

  for (int i = 0; i < MONKEYS_NUM; i++) {
    if (pthread_join(monkeys[i], NULL)) {
      perror("Failed to join thread!\n");
      exit(1);
    }
  }
  for (int i = 0; i < GORILLAS_NUM; i++) {
    if (pthread_join(gorillas[i], NULL)) {
      perror("Failed to join thread!\n");
      exit(1);
    }
  }

  sem_destroy(&semaphore);
  return 0;
}

//=======================//
void *monkey_routine(void *arg) {
  int x = *(int *)arg;

  // Mudando tempo em que um macaco entra na ponte
  sleep(rand() % 10);

  // espera até que nenhum gorila esteja querendo atravessar
  while (l_gorillas || r_gorillas) {
  }

  // par = macacos da esquerda
  if (!(x & 1)) {
    if (!l_monkeys) {
      l_monkeys++;
      l_monkey_queue = true;
      sem_wait(&semaphore);
      l_monkey_queue = false;
    } else {
      l_monkeys++;
      while (l_monkey_queue) {
      }
    }

    printf("Monkey ( %.2d )[LEFT]  O..........CROSSING >>..........O\n", x);
    fflush(stdout);
    // Mudando o tempo de travessia do macaco
    sleep(rand() % 5);
    printf("Monkey ( %.2d )[LEFT]  !!!ARRIVED!!!\n", x);
    fflush(stdout);

    if (l_monkeys == 1) {
      sem_post(&semaphore);
      l_monkeys--;
    } else {
      l_monkeys--;
    }
    pthread_exit(NULL);
  }

  // ímpar = macacos da direita
  if (!r_monkeys) {
    r_monkeys++;
    r_monkey_queue = true;
    sem_wait(&semaphore);
    r_monkey_queue = false;
  } else {
    r_monkeys++;
    while (r_monkey_queue) {
    }
  }

  printf("Monkey ( %.2d )[RIGHT] O..........<< CROSSING..........O\n", x);
  fflush(stdout);

  // Mudando o tempo de travessia do macaco
  sleep(rand() % 10);

  printf("Monkey ( %.2d )[RIGHT] !!!ARRIVED!!!\n", x);
  fflush(stdout);
  if (r_monkeys == 1) {
    sem_post(&semaphore);
    r_monkeys--;
  } else {
    r_monkeys--;
  }

  pthread_exit(NULL);
}

void *gorilla_routine(void *arg) {
  int x = *(int *)arg;

  // Mudando tempo em que um gorila entra na ponte
  sleep(rand() % 10);

  // par = gorilas da esquerda
  if (!(x & 1)) {
    if (!l_gorillas) {
      l_gorillas++;
      l_gorilla_queue = true;
      sem_wait(&semaphore);
      l_gorilla_queue = false;
    } else {
      l_gorillas++;
      while (l_gorilla_queue) {
      }
    }

    while (gorilla_crossing) {
    }
    gorilla_crossing = true;

    printf("Gorilla( %.2d )[LEFT]  O..........CROSSING >>..........O\n", x);
    fflush(stdout);
    // Mudando o tempo de travessia do gorila
    sleep(rand() % 5);
    printf("Gorilla( %.2d )[LEFT]  !!!ARRIVED!!!\n", x);
    fflush(stdout);

    gorilla_crossing = false;

    if (l_gorillas == 1) {
      sem_post(&semaphore);
      l_gorillas--;
    } else {
      l_gorillas--;
    }

    pthread_exit(NULL);
  }

  // ímpar = gorilas da direita
  if (!r_gorillas) {
    r_gorillas++;
    r_gorilla_queue = true;
    sem_wait(&semaphore);
    r_gorilla_queue = false;
  } else {
    r_gorillas++;
    while (r_gorilla_queue) {
    }
  }

  while (gorilla_crossing) {
  }
  gorilla_crossing = true;

  printf("Gorilla( %.2d )[RIGHT] O..........<< CROSSING..........O\n", x);
  fflush(stdout);

  // Mudando o tempo de travessia do gorila
  sleep(rand() % 10);

  printf("Gorilla( %.2d )[RIGHT] !!!ARRIVED!!!\n", x);
  fflush(stdout);

  gorilla_crossing = false;

  if (r_gorillas == 1) {
    sem_post(&semaphore);
    r_gorillas--;
  } else {
    r_gorillas--;
  }

  pthread_exit(NULL);
}
