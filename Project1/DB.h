#pragma once

#include "Config.h"
#include "DeepPointer.h"


void DB_open();
void DB_close();
void DB_put(const Variable* var, const VariableValue* value);
void DB_commit();
void DB_clear();


