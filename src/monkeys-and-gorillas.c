#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MONKEYS_NUM 10
#define GORILLAS_NUM 2
#define MAX_CROSSING_TIME 1
#define MAX_ENTERING_TIME 2
#define MAX_CROSSINGS 10
#define LEFT -1
#define RIGHT 1

// Semáforos para travessia
sem_t monkey_sem, gorilla_sem;

// Quantidade de macacos de cada lado que requisitaram a travessia
short l_monkeys = 0, r_monkeys = 0;

// Quantidade de gorilas de cada lado que requisitaram a travessia
short l_gorillas = 0, r_gorillas = 0;

// Indica se o primeiro macaco de cada lado está esperando para atravessar
bool l_monkey_queue = false, r_monkey_queue = false;

// Funções de rotina
void *monkey_routine(void *arg);
void *gorilla_routine(void *arg);

int main(int argc, char const *argv[]) {
  sem_init(&monkey_sem, 0, 1);
  sem_init(&gorilla_sem, 0, 1);

  pthread_t monkeys_threads[MONKEYS_NUM];
  pthread_t gorillas_threads[GORILLAS_NUM];
  int monkeys_ids[MONKEYS_NUM];
  int gorillas_ids[GORILLAS_NUM];

  for (int i = 0; i < GORILLAS_NUM; i++) {
    gorillas_ids[i] = i;
    if (pthread_create(&gorillas_threads[i], NULL, gorilla_routine, &gorillas_ids[i])) {
      perror("Failed to create thread!\n");
      exit(1);
    };
  }
  for (int i = 0; i < MONKEYS_NUM; i++) {
    monkeys_ids[i] = i;
    if (pthread_create(&monkeys_threads[i], NULL, monkey_routine, &monkeys_ids[i])) {
      perror("Failed to create thread!\n");
      exit(1);
    };
  }

  for (int i = 0; i < GORILLAS_NUM; i++) {
    if (pthread_join(gorillas_threads[i], NULL)) {
      perror("Failed to join thread!\n");
      exit(1);
    }
  }
  for (int i = 0; i < MONKEYS_NUM; i++) {
    if (pthread_join(monkeys_threads[i], NULL)) {
      perror("Failed to join thread!\n");
      exit(1);
    }
  }

  sem_destroy(&gorilla_sem);
  sem_destroy(&monkey_sem);

  return 0;
}

void *monkey_routine(void *arg) {
  int x = *(int *)arg;
  int monkey_location = (rand() % 2) == 0 ? LEFT : RIGHT;

  for (int i = 0; i < MAX_CROSSINGS; i++) {
    // Mudando tempo em que um macaco entra na ponte
    sleep(rand() % MAX_ENTERING_TIME);

    if (monkey_location == LEFT) {
      if (!l_monkeys) {
        l_monkeys++;
        l_monkey_queue = true;
        sem_wait(&monkey_sem);
        sem_wait(&gorilla_sem);
        l_monkey_queue = false;
      } else {
        l_monkeys++;
        while (l_monkey_queue) {
        }
      }

      printf("Monkey ( %.2d )[LEFT]  O..........>> CROSSING >>..........O\n", x);
      fflush(stdout);

      // Mudando o tempo de travessia do macaco
      sleep(rand() % MAX_CROSSING_TIME);

      printf("Monkey ( %.2d )[LEFT]  !!!ARRIVED!!!\n", x);
      fflush(stdout);

      monkey_location = RIGHT;

      if (l_monkeys == 1) {
        sem_post(&monkey_sem);
        sem_post(&gorilla_sem);
      }
      l_monkeys--;

    } else {
      if (!r_monkeys) {
        r_monkeys++;
        r_monkey_queue = true;
        sem_wait(&monkey_sem);
        sem_wait(&gorilla_sem);
        r_monkey_queue = false;
      } else {
        r_monkeys++;
        while (r_monkey_queue) {
        }
      }

      printf("Monkey ( %.2d )[RIGHT] O..........<< CROSSING <<..........O\n", x);
      fflush(stdout);

      // Mudando o tempo de travessia do macaco
      sleep(rand() % MAX_CROSSING_TIME);

      printf("Monkey ( %.2d )[RIGHT] !!!ARRIVED!!!\n", x);
      fflush(stdout);

      monkey_location = LEFT;

      if (r_monkeys == 1) {
        sem_post(&monkey_sem);
        sem_post(&gorilla_sem);
      }
      r_monkeys--;
    }
  }

  pthread_exit(NULL);
}

void *gorilla_routine(void *arg) {
  int x = *(int *)arg;
  int gorilla_location = (rand() % 2) == 0 ? LEFT : RIGHT;

  for (int i = 0; i < MAX_CROSSINGS; i++) {
    // Mudando tempo em que um gorila entra na ponte
    sleep(rand() % MAX_ENTERING_TIME);

    if (gorilla_location == LEFT) {
      l_gorillas++;
      sem_wait(&gorilla_sem);

      printf("Gorilla( %.2d )[LEFT]  O..........>> CROSSING >>..........O\n", x);
      fflush(stdout);

      // Mudando o tempo de travessia do gorila
      sleep(rand() % MAX_CROSSING_TIME);

      printf("Gorilla( %.2d )[LEFT]  !!!ARRIVED!!!\n", x);
      fflush(stdout);

      gorilla_location = RIGHT;

      sem_post(&gorilla_sem);
      l_gorillas--;

    } else {
      r_gorillas++;
      sem_wait(&gorilla_sem);

      printf("Gorilla( %.2d )[RIGHT] O..........<< CROSSING <<..........O\n", x);
      fflush(stdout);

      // Mudando o tempo de travessia do gorila
      sleep(rand() % MAX_CROSSING_TIME);

      printf("Gorilla( %.2d )[RIGHT] !!!ARRIVED!!!\n", x);
      fflush(stdout);

      gorilla_location = LEFT;

      sem_post(&gorilla_sem);
      r_gorillas--;
    }
  }

  pthread_exit(NULL);
}
