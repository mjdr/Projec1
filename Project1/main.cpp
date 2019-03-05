#include "Config.h"
#include "ProcessReader.h"
#include "DB.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

int main(){

	Config_load("client.cfg.json");

	DB_open();
	ProcessReader_open();

	VariableValue* mem = (VariableValue*) malloc(sizeof(VariableValue));

	std::clock_t last = clock();

	main:
	while (1) {


		for (int i = 0; i < Config_get()->variablesCount; i++) {
			const Variable* var = Config_get()->variables + i;
			if (!ProcessReader_readVariable(mem, var)) {
				DB_clear();
				fprintf(stderr, "Can't read %s.\n", var->name);
				goto main;
			}
			DB_put(var, (VariableValue*)mem);
		}
		DB_commit();

		while (clock() - last < CLOCKS_PER_SEC / 1000 * Config_get()->delay);
		last = clock();
	}
	free(mem);

 
 

 Config_destroy();
 return 0;
}
