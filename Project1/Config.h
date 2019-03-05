#pragma once
#include "DeepPointer.h"

#define MAX_VARIABLE_COUNT 1024

typedef struct DBCredits {
 char* host;
 char* user;
 char* password;
 char* database;
 char* table;
} DBCredits;

typedef enum VariableType {
 VARTYPE_FLOAT
} VariableType;

typedef struct Variable {
 char* name;
 VariableType type;
 DeepPointer pointer;
} Variable;

typedef struct Config {
 DBCredits db;
 char* process;
 int delay;
 Variable variables[MAX_VARIABLE_COUNT];
 int variablesCount;
} Config;


void Config_load(const char* filename);
const Config* Config_get();
void Config_destroy();
