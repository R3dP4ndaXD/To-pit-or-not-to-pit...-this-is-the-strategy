#include "functii.h"

/*
    Citeste informatiilor din fisierul binar, aloca memorie pentru vectorul de senzori,iar pentru fiecare senzor
    in parte aloca si initializeaza structura cu informatii si vectorul de indecsi (+ verificarea fiecarei alocari)
*/
sensor* citire_senzori(const char *bin, int *nr_senzori_alocati) {
	FILE *fbin = fopen(bin, "rb");
	int i, j;
	fread(nr_senzori_alocati, sizeof(int), 1, fbin);
	sensor* senzori = malloc(*nr_senzori_alocati * sizeof(sensor));
	if (!senzori){
		return NULL;
	}
	for (i = 0; i < *nr_senzori_alocati; i++) {
		fread(&senzori[i].sensor_type, sizeof(enum sensor_type), 1, fbin);
		if (senzori[i].sensor_type == TIRE) {
			senzori[i].sensor_data = malloc(sizeof(tire_sensor));
			if (!senzori[i].sensor_data) {
				for (j = 0; j < i; j++) {
					free(senzori[j].sensor_data);
					free(senzori[j].operations_idxs);
				}
				free(senzori);
				return NULL;
			}
			fread(senzori[i].sensor_data, sizeof(float), 2, fbin);
			fread(senzori[i].sensor_data + 2 * sizeof(float), sizeof(int), 2, fbin);
		} else {
			senzori[i].sensor_data = malloc(sizeof(power_management_unit));
			if (!senzori[i].sensor_data) {
				for (j = 0; j < i; j++) {
					free(senzori[j].sensor_data);
					free(senzori[j].operations_idxs);
				}
				free(senzori);
				return NULL;
			}
			fread(senzori[i].sensor_data, sizeof(float), 3, fbin);
			fread(senzori[i].sensor_data + 3 * sizeof(float), sizeof(int), 2, fbin);
		}
		fread(&senzori[i].nr_operations, sizeof(int), 1, fbin);
		senzori[i].operations_idxs = malloc(senzori[i].nr_operations * sizeof(int));
		if (!senzori[i].operations_idxs) {
			for (j = 0; j < i; j++) {
				free(senzori[j].sensor_data);
				free(senzori[j].operations_idxs);
			}
			free(senzori[i].sensor_data);
			free(senzori);
			return NULL;
		}
		fread(senzori[i].operations_idxs, sizeof(int), senzori[i].nr_operations, fbin);

	}
	fclose(fbin);
    // sorteaza vectorul de senzori dupa tipul lor: mai intai sezorii PMU si dupa senzorii TIRE
	qsort(senzori, *nr_senzori_alocati, sizeof(sensor), cmp);
	return senzori;	
}

int cmp(const void *a, const void *b) {
	sensor *x = (sensor *)a;
	sensor *y = (sensor *)b;
	return y->sensor_type - x->sensor_type;
}

// Afiseaza informatiile din structura senzorului de pe pozitia index din vectorul de senzori
void print(sensor *senzori, int nr_senzori, int index, FILE *fo) {
	if (index < 0 || index >= nr_senzori) {
		fprintf(fo, "Index not in range!\n");
		return;
	}
	if (senzori[index].sensor_type == TIRE) {
		tire_sensor *data = (tire_sensor *)senzori[index].sensor_data;
		fprintf(fo, "Tire Sensor\nPressure: %.2f\nTemperature: %.2f\nWear Level: %i%%\nPerformance Score: ", data->pressure, data->temperature, data->wear_level);
		if (data->performace_score) {
			fprintf(fo, "%i\n", data->performace_score);
		}
		else {
			fprintf(fo, "Not Calculated\n");
		}
	} else {
		power_management_unit *data = (power_management_unit *)senzori[index].sensor_data;
		fprintf(fo, "Power Management Unit\nVoltage: %.2f\nCurrent: %.2f\nPower Consumption: %.2f\nEnergy Regen: %i%%\nEnergy Storage: %i%%\n", data->voltage, data->current, data->power_consumption, data->energy_regen, data->energy_storage);
	}
}

/*
    Pentru senzorul de la pozitia index din vector se apeleaza, prin intermediul vectorului de functii,
    functiile indicate prin vectorul de indecsi
*/
void analyze(sensor *senzori, int nr_senzori, void **operations, int index) {
	if (index < 0 || index >= nr_senzori) {
		printf("Index not in range!\n");
		return;
	}
	int i;
	for(i = 0; i < senzori[index].nr_operations; i++) {
		if ((senzori[index].sensor_type == TIRE && senzori[index].operations_idxs[i] > 3) || (senzori[index].sensor_type == PMU && senzori[index].operations_idxs[i] < 4)) {
			continue;
		}
        /*
            casteaza void* -ului din vectorul de operatii la pointer la functie ce intoarce void si primeste ca parametru un void*; 
            dupa apeleaza functia pentru structura cu informatii a senzorului
        */
		((void (*)(void *))operations[senzori[index].operations_idxs[i]])(senzori[index].sensor_data);
	}
}
//sterge (eliberand memoria) informatiile asociate nodului de eliminat si regrupeaza senzorii in vector;
void delete(sensor *senzori, int* nr_senzori, int index) {
	int i;
	free(senzori[index].sensor_data);
	free(senzori[index].operations_idxs);
	for (i = index; i < *nr_senzori - 1; i++) {
		senzori[i] = senzori[i+1];
	}
	(*nr_senzori)--;
}
/*
    Pargurge vectorul de senzori si sterge senzorii invalizi. Dupa ce se elimina toti senzorii invalizi,
    se realoca memoria vectorului si se retine noua marime a vecturului
*/
void clear(sensor **senzori, int* nr_senzori_alocati, int* nr_senzori) {
	int i = 0;
	while (i < *nr_senzori) {
		if((*senzori)[i].sensor_type == TIRE) {
			tire_sensor *t = (tire_sensor *)(*senzori)[i].sensor_data;
			if (t->pressure < 19 || t->pressure > 28 || t->temperature < 0 || t->temperature > 120 || t->wear_level < 0 || t->wear_level > 1000) {
				delete(*senzori, nr_senzori, i);
			} else {
				i++;
			}
		} else {
			power_management_unit *pmu = (power_management_unit *)(*senzori)[i].sensor_data;
			if (pmu->voltage < 10 || pmu->voltage > 20 || pmu->current < -100 || pmu->current > 100 || pmu->power_consumption < 0 || pmu->power_consumption > 1000 || pmu->energy_regen < 0 || pmu->energy_regen > 100 || pmu->energy_storage < 0 || pmu->energy_storage > 100) {
				delete(*senzori, nr_senzori, i);
			} else {
				i++;
			}
		}
	}
	if (*nr_senzori != *nr_senzori_alocati) {
        /*
            Se foloseste un pointer auxiliar pentru a nu piede accesul la memoria alocara in cazul unei realocari esuate 
            Daca dupa eliminari vectorul ramana cu 0 vectori, realocarea poate intorce NULL (in functie de implemenarea de C)
            chiar daca in cazul nostru se considera ca a reusit
        */
		sensor *temp = realloc(*senzori, *nr_senzori * sizeof(sensor));
		if (*nr_senzori == 0 || temp) {
			*senzori = temp;
			*nr_senzori_alocati= *nr_senzori;
		}
	}
    /*
        Alta varianta ar fi fost sa alocam un nou vector cu senzorii valizi si in final vectorul intial se elibera
        si variabila senzori se seta la noul vector alocat;
        Problema: daca nu se putea aloca acest nou vector, operatia de clear nu s-ar mai fi putut realiza
    */
}

/*
    Elibereaza integral memoria vectorului de senzori, impreuna cu structura de informatii si cu vectorul de operatii a fiecarui senzor in parte
    La final, pointerul care a fost asociat vectorului e setat la NULL
*/
void exitt(sensor **senzori, int nr_senzori) {
	int i;
	for(i = 0; i < nr_senzori; i++) {
		free((*senzori)[i].sensor_data);
		free((*senzori)[i].operations_idxs);
	}
	free(*senzori);
	*senzori = NULL;
}