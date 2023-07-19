#include "../include/MonitorBanheiro.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct monitor {
  pthread_mutex_t bathroom_mutex;
  pthread_cond_t bathroom_cond;
  int amount_corintianos;
  int amount_palmeirenses;
  int amount_vacancies;
  int corintianos_on_queue;
};

Monitor *monitor_init(int n) {
  Monitor *new_monitor = calloc(1, sizeof(Monitor));

  new_monitor->amount_vacancies = n;
  pthread_mutex_init(&new_monitor->bathroom_mutex, NULL);
  pthread_cond_init(&new_monitor->bathroom_cond, NULL);

  return new_monitor;
}

void corintianoQuerEntrar(Monitor *monitor) {
  pthread_mutex_lock(&monitor->bathroom_mutex);

  monitor->corintianos_on_queue++;
  int total_people_on_bathroom = monitor->amount_corintianos + monitor->amount_palmeirenses;
  while (monitor->amount_palmeirenses != 0 || total_people_on_bathroom >= monitor->amount_vacancies) {
    pthread_cond_wait(&monitor->bathroom_cond, &monitor->bathroom_mutex);
  }

  monitor->corintianos_on_queue--;
  monitor->amount_corintianos++;

  printf("Corintianos no banheiro: %d\n", monitor->amount_corintianos);
  printf("Palmeirenses no banheiro: %d\n", monitor->amount_palmeirenses);
  pthread_mutex_unlock(&monitor->bathroom_mutex);
}

void conrintianoSai(Monitor *monitor) {
  pthread_mutex_lock(&monitor->bathroom_mutex);
  monitor->amount_corintianos--;
  pthread_cond_signal(&monitor->bathroom_cond);
  pthread_mutex_unlock(&monitor->bathroom_mutex);
}

void palmeirenseQuerEntrar(Monitor *monitor) {
  pthread_mutex_lock(&monitor->bathroom_mutex);

  int total_people_on_bathroom = monitor->amount_corintianos + monitor->amount_palmeirenses;
  while (
      monitor->amount_corintianos != 0 || total_people_on_bathroom >= monitor->amount_vacancies || monitor->corintianos_on_queue != 0) {
    pthread_cond_signal(&monitor->bathroom_cond);
    pthread_cond_wait(&monitor->bathroom_cond, &monitor->bathroom_mutex);
  }

  monitor->amount_palmeirenses++;

  printf("Corintianos no banheiro: %d\n", monitor->amount_corintianos);
  printf("Palmeirenses no banheiro: %d\n", monitor->amount_palmeirenses);
  pthread_mutex_unlock(&monitor->bathroom_mutex);
}

void palmeirenseSai(Monitor *monitor) {
  pthread_mutex_lock(&monitor->bathroom_mutex);

  monitor->amount_palmeirenses--;
  pthread_cond_signal(&monitor->bathroom_cond);
  pthread_mutex_unlock(&monitor->bathroom_mutex);
}

void monitor_free(Monitor *monitor) {
  pthread_mutex_destroy(&monitor->bathroom_mutex);
  pthread_cond_destroy(&monitor->bathroom_cond);
  free(monitor);
}