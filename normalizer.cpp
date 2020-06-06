#include "normalizer.hpp"
#include <iostream>
#include <unordered_map>

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
		for (int i = 0; i < columnNames.size(); i++) {
			const char *toAdd =  vector_int_to_string(columnNames[i]).c_str();
			column_names.push_back(toAdd);
			std::cout << column_names[0] << "\n";
		}

	}
	sqlite3_close(db);
}

char* normalizer::get_filename() {
	return filename;
}

void normalizer::find_dependencies() {


	std::vector<std::vector<std::string> > determinant_possibilities;

	//Obtain all subsets of table that could make up a determinant
	for(int i = 0; i < column_names.size(); i++) {
		std::string cur_column = std::string(column_names[i]);
		std::vector<std::string> empty_set;
		determinant_possibilities.push_back(empty_set);
		int determinant_possibilities_size = determinant_possibilities.size();
		for (int j = 0; j < determinant_possibilities_size; j++) {
			std::vector<std::string> cur_set = determinant_possibilities[j];
			std::vector<std::string> new_set;
			new_set.push_back(cur_column);
			int cur_set_size = cur_set.size();
			for(int k = 0; k < cur_set_size; k++) {
				new_set.push_back(cur_set[k]);
			}
			determinant_possibilities.push_back(new_set);
		}
	}

/*
	for (int i = 0; i < determinant_possibilities.size(); i++) {
		std::cout << "\n";
		for (int j = 0; j < determinant_possibilities[i].size(); j++) {
			std::cout << determinant_possibilities[i][j] << ",";
		}
	} */

	//std::vector<std::string>  determinant;
	//std::vector<std::string>  dependant;



	/*
	std::vector<std::string> determinant;
	std::vector<std::string>  dependant;
	determinant.push_back("A");
	dependant.push_back("B");
	table_dependencies.push_back(functional_dependency(determinant, dependant));
	*/


	//unordered_map<string, int> umap;
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



