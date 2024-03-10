#ifndef _FUNCTII_H_
#define _FUNCTII_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

void get_operations(void **operations);

sensor* citire_senzori(const char *bin, int *nr_senzori_alocati);
int cmp(const void *a, const void *b);
void print(sensor *senzori, int nr_senzori, int index, FILE *fo);
void analyze(sensor *senzori, int nr_senzori, void **operations, int index);
void delete(sensor *senzori, int* nr_senzori, int index);
void clear(sensor **senzori, int* nr_senzori_alocati, int* nr_senzori);
void exitt(sensor **senzori, int nr_senzori);

#endif