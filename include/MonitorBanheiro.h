#pragma once

typedef struct monitor Monitor;

Monitor *monitor_init(int n);
void monitor_free(Monitor *monitor);

void corintianoQuerEntrar(Monitor *monitor);
void conrintianoSai(Monitor *monitor);
void palmeirenseQuerEntrar(Monitor *monitor);
void palmeirenseSai(Monitor *monitor);