#include "functii.h"


int main(int argc, char const *argv[])
{
	int nr_senzori_alocati, nr_senzori, index;
	sensor *senzori = citire_senzori(argv[1], &nr_senzori_alocati);	
	if (!senzori)
		return 0;
	nr_senzori = nr_senzori_alocati;
	char *command = malloc(30 * sizeof(char));
	if (!command) {
		exitt(&senzori, nr_senzori_alocati);
		return 0;
	}
	void **operations = malloc(8 * sizeof(void*));
	if (!operations) {
		exitt(&senzori, nr_senzori_alocati);
		free(command);
		command = NULL;

	}
	/*
		Se citest si se executa comenzi pana la primirea comenzii "exit" prin care se elibereaza memoria asociata vectorului de senzori
		Dupa se elibereaza memoria alocata pentru stringul folosit la citire si pentru vectorul de operatii (+ setarea pointerilor asociati cu NULL) 
	*/
	get_operations(operations);
	while (scanf("%s",command) && !strstr(command, "exit")) {
		if (strstr(command, "print")) {
			scanf("%i", &index);
			print(senzori, nr_senzori, index, stdout);
		} else if (strstr(command, "analyze")) {
			scanf("%i", &index);
			analyze(senzori, nr_senzori, operations, index);
		} else if (strstr(command, "clear")) {
			clear(&senzori, &nr_senzori_alocati, &nr_senzori);
		}
	}
	exitt(&senzori, nr_senzori_alocati);
	free(command);
	command = NULL;
	free(operations);
	operations = NULL;
	return 0;
}
