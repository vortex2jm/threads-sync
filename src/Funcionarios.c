#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/MonitorBanheiro.h"

#define AMOUNT_TIME_FANS 20
#define AMOUNT_BATHROOMS 3

Monitor *monitor;

void *thread_corintiano(void *id);
void *thread_palmeirense(void *id);

int main(int argc, char *argv[]) {
  monitor = monitor_init(AMOUNT_BATHROOMS);
  pthread_t threads[AMOUNT_TIME_FANS];

  // id counters for palmeirenses and corintianos
  int k = 0, l = 0;

  // create threads for corintianos and palmeirenses
  for (int i = 0; i < AMOUNT_TIME_FANS; i++) {
    int *id = malloc(sizeof(int));

    if (i % 2 == 0) {
      *id = ++k;
      if (pthread_create(&threads[i], NULL, thread_palmeirense, id) != 0) {
        perror("error when creating thread: ");
        exit(EXIT_FAILURE);
      }
    } else {
      *id = ++l;
      if (pthread_create(&threads[i], NULL, thread_corintiano, id) != 0) {
        perror("error when creating thread: ");
        exit(EXIT_FAILURE);
      }
    }
  }

  // join threads
  for (int i = 0; i < AMOUNT_TIME_FANS; i++) {
    if (pthread_join(threads[i], NULL) != 0) {
      perror("error when joining thread: ");
      exit(EXIT_FAILURE);
    }
  }

  // free allocated monitor
  monitor_free(monitor);
}

void *thread_corintiano(void *id) {
  while (true) {
    corintianoQuerEntrar(monitor);
    printf("Eu sou corintiano-%d: ... UFA! Entrei no banheiro!\n", *(int *)id);
    sleep(3);
    conrintianoSai(monitor);
    printf("Eu sou corintiano-%d: ... Estou aliviado! Vou trabalhar!\n", *(int *)id);
    sleep(5);
  }
}

void *thread_palmeirense(void *id) {
  while (true) {
    palmeirenseQuerEntrar(monitor);
    printf("Eu sou palmeirense-%d: ... UFA! Entrei no banheiro!\n", *(int *)id);
    sleep(3);
    palmeirenseSai(monitor);
    printf("Eu sou palmeirense-%d: ... Estou aliviado! Vou trabalhar!\n", *(int *)id);
    sleep(5);
  }
}