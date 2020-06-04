#include "normalizer.hpp"
#include <iostream>

static std::vector<std::vector<int> > columnNames;


//SQL method for callback
static int callback(void *data, int argc, char **argv, char **azColName){
	for(int i = 0; i<argc; i++){
		std::vector<int> word;
		columnNames.push_back(word);
		std::string name = std::string(azColName[i]);
		for (std::string::size_type j = 0; j < name.size(); j++) {
			columnNames[i].push_back(name[j]);
		}
	}
   return 0;
}

//Above function stores all column names as strings to avoid some memory allocation using data parameter. Doing this increased speed on my laptop. (Feel free to change if you use this library)
static std::string vector_int_to_string(std::vector<int> word) {
	std::string toReturn;
	for (int i = 0; i < word.size(); i++) {
		toReturn = toReturn + (char) word[i];
	}
	return toReturn;
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
		std::cout << vector_int_to_string(columnNames[0]);

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



