#include "normalizer.hpp"
#include <iostream>

static std::vector<char*> columnNames;


int callback(void *data, int argc, char **argv, char **azColName){
	for(int i = 0; i<argc; i++){
		columnNames.push_back(azColName[i]);
		//std::cout << columnNames[i];
	}
   return 0;
}


normalizer::normalizer(char* new_filename, std::string table_name) {
	filename = new_filename;
	int rc;
	rc = sqlite3_open(filename, &db);
	char *zErrMsg = 0;
	std::string sql_message = "select * from " + table_name + ";";
	const char * sql = sql_message.c_str();
	const char* data = "Callback function called";
	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		normalizer::set_column_names(columnNames);
	}
	sqlite3_close(db);
}

char* normalizer::get_filename() {
	return filename;
}

void normalizer::set_column_names(std::vector<char*> new_column_names) {
	column_names = new_column_names;
}































































/*
static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
*/



