#include "normalizer.hpp"
#include <iostream>

static std::vector<std::vector<int> > columnNames;



static int callback(void *data, int argc, char **argv, char **azColName){
	for(int i = 0; i<argc; i++){
		std::vector<int> word;
		columnNames.push_back(word);
		//strcpy(columnNames[i], azColName[i]);
		std::string name = std::string(azColName[i]);
		for (std::string::size_type j = 0; j < name.size(); j++) {
			//std::cout << name[j] << ' ';
			columnNames[i].push_back(name[j]);
		}
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
	char **data;
	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		std::cout << (char) columnNames[0][0];

	}
	sqlite3_close(db);
}

char* normalizer::get_filename() {
	return filename;
}

void normalizer::set_column_names(std::vector<std::string> new_column_names) {

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



