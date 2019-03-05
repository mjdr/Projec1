#pragma once

#define MAX_OFFSETS_COUNT 128

typedef struct DeepPointer {
 const char* module;
 int offsets[MAX_OFFSETS_COUNT]; 
 int offsetsCount;
} DeepPointer;

typedef union VariableValue {
 float floatvalue;
} VariableValue;

