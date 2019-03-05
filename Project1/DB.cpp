#include "DB.h"
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char names[10240];
static char values[10240];
static char status[2*10240];

static MYSQL* connection;
void DB_open() {
 connection = mysql_init(NULL);
 if (connection == NULL) {
  fprintf(stderr, "%s\n", mysql_error(connection));
  exit(1);
 }
 if (mysql_real_connect(
 			connection, 
 			Config_get()->db.host, 
 			Config_get()->db.user,
			Config_get()->db.password,
			Config_get()->db.database,
      0, NULL, 0) == NULL) 
  {
   fprintf(stderr, "%s\n", mysql_error(connection));
   mysql_close(connection);
   exit(1);
  }
  DB_clear();
}
void DB_close() {
	mysql_close(connection);
}
void DB_put(Variable const * var, VariableValue const * value) {
	snprintf(names, 10240, "%s%s, ", names, var->name);
	snprintf(status, 10240, "%s%s: ", status, var->name);
 if(var->type == VARTYPE_FLOAT) {
	 snprintf(values, 10240, "%s%f, ", values, value->floatvalue);
	 snprintf(status, 10240, "%s%6.1f  ", status, value->floatvalue);
 }
}
void DB_commit() {
  names[strlen(names) - 2] = 0;
  values[strlen(values) - 2] = 0;
  char sql[2048];
  snprintf((char* const)&sql, sizeof(sql), "INSERT INTO %s (%s) VALUES (%s);", Config_get()->db.table, &names, &values);
  if (!mysql_query(connection, sql))
 	  fprintf(stderr, "%s\n", mysql_error(connection));
  
  printf("\r%s", &sql);
  DB_clear();
}
void DB_clear() {
 names[0] = 0;
 values[0] = 0;
 status[0] = 0;
}

