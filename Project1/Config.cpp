#include <cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Config.h"

static Config config;
static char* content = NULL;
static cJSON* root;

static void loadContent(const char* filename) {
 FILE *fp;
 long lSize;
 
 fp = fopen (filename, "rb" );
 if(!fp){
  perror(filename);
  exit(1);
 }

 fseek(fp, 0L, SEEK_END);
 lSize = ftell(fp);
 rewind( fp );
 
 content = (char*)calloc(1, lSize+1);
 if(!content) {
  fclose(fp);
  fputs("memory alloc fails",stderr);
  exit(1);
 }
 if(1 != fread( content, lSize, 1, fp)){
  fclose(fp);
  free(content);
  fputs("entire read fails",stderr);
  exit(1);
 }
 fclose(fp);
}

static cJSON* get(cJSON* parent, const char* attr, cJSON_bool (__stdcall *isFunc)(const cJSON* const)) {
	cJSON* node = cJSON_GetObjectItemCaseSensitive(parent, attr);
	if(!isFunc(node)) {
	 printf("Config: Argument %s missing or has unexpected type.\n", attr);
	 exit(-1);
	}
 return node;
}

void Config_load(const char* filename){
 
 
 loadContent(filename);
 
 
 root = cJSON_Parse(content);
 if(root == NULL) {
  const char *error_ptr = cJSON_GetErrorPtr();
  if (error_ptr != NULL) {
   fprintf(stderr, "Error before: %s\n", error_ptr);
   exit(-1);
  }
 }
 config.process = get(root, "process", cJSON_IsString)->valuestring;
 config.delay = (int)(get(root, "delay", cJSON_IsNumber)->valuedouble);
 
 cJSON* db = get(root, "db", cJSON_IsObject);
 cJSON* vars = get(root, "variables", cJSON_IsArray);
 cJSON* var;
 cJSON* offsets;
 cJSON* offset;
 
 config.db.host = get(db, "host", cJSON_IsString)->valuestring;
 config.db.user = get(db, "user", cJSON_IsString)->valuestring;
 config.db.password = get(db, "password", cJSON_IsString)->valuestring;
 config.db.database = get(db, "database", cJSON_IsString)->valuestring;
 config.db.table = get(db, "table", cJSON_IsString)->valuestring;
 
 root = cJSON_Parse(content);
 int i = 0,j;
 cJSON_ArrayForEach(var, vars) {
  if(i >= MAX_VARIABLE_COUNT) {
	 printf("Config: Can handle only %d variables.\n", MAX_VARIABLE_COUNT);
	 exit(-1);
  }
  if(!cJSON_IsObject(var)) {
	 printf("Config: Variable must be an object.\n");
	 exit(-1);
  }
  config.variables[i].name = get(var, "name", cJSON_IsString)->valuestring;
  config.variables[i].pointer.module = get(var, "module", cJSON_IsString)->valuestring;
  const char* type = get(var, "type", cJSON_IsString)->valuestring;
  if(strcmp(type, "float") == 0)
  	config.variables[i].type = VARTYPE_FLOAT;
  else {
	 printf("Config: Unknown type: %s.\n", type);
	 exit(-1);
  }
  offsets = get(var, "offsets", cJSON_IsArray);
  j = 0;
  cJSON_ArrayForEach(offset, offsets){
   if(!cJSON_IsString(offset)) {
	  printf("Config: Offset must be a string.\n");
	  exit(-1);
   }
   config.variables[i].pointer.offsets[j] = (int)strtol(offset->valuestring, NULL, 0);
   j++;
  }
  config.variables
  [i].pointer.offsetsCount = j;
  
 	i++;
 }
 config.variablesCount = i;
}
const Config* Config_get() {
 return &config;
}
void Config_destroy(){
 cJSON_Delete(root);
 free(content);
}
