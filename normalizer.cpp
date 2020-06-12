#include "normalizer.hpp"
#include <iostream>
#include <unordered_map>

static bool set = false;
static bool add = true;
static std::vector<std::vector<int> > columnNames;
static std::vector<std::vector<int> > cur_det;
static std::vector<std::vector<int> > cur_nondets;


//SQL method for callback
static int callback(void *data, int argc, char **argv, char **azColName){
	if (!set) {
		for(int i = 0; i<argc; i++){
			std::vector<int> word;
			columnNames.push_back(word);
			std::string name = std::string(azColName[i]);
			for (std::string::size_type j = 0; j < name.size(); j++) {
				columnNames[i].push_back(name[j]);
			}
		}
		set = true;
	}
   return 0;
}


//SQL method for checking dependencies
static int callback_two(void *data, int argc, char **argv, char **azColName){
	for (int i = 0; i < argc; i++) {
		std::cout << azColName[i];
	}
   return 0;
}


//Above function stores all column names as ints to avoid some memory allocation using data parameter. Doing this increased speed on my laptop. (Feel free to change if you use this library)
static std::string vector_int_to_string(std::vector<int> word) {
	std::string toReturn;
	for (int i = 0; i < word.size(); i++) {
		toReturn = toReturn + (char) word[i];
	}
	return toReturn;
}

static int equals_vector_int(std::vector<int> one, std::vector<int> two) {
	if (!(one.size() == two.size())) {
		return 0;
	} else {
		for (int i = 0; i < one.size(); i++) {
			if (!(one[i] == two[i])) {
				return 0;
			}
		}
	}
	return 1;
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
			int_column_names.push_back(columnNames[i]);
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



	std::vector<std::vector<std::vector<int> > > determinant_possibilities;

	//Obtain all subsets of table that could make up a determinant, load them into determinant_possibilities
	int n = int_column_names.size();
	for(int i = 0; i < (1<<n); i++) {
		std::vector<std::vector<int> > empty_set;
		determinant_possibilities.push_back(empty_set);
		for (int j = 0; j < n; j++) {
			if ((i & (1 << j)) > 0) {
				determinant_possibilities[i].push_back(int_column_names[j]);
			}
		}
	}
	//Now that all possibilities have been obtained, each one needs to be tested to see if it is a determinant for any dependency
	//This Brute Force Algorithm will mark down any potential determinant -> dependent relationship in which the dependent is always the same for the same determinant, and there is no subset determinant for which this is also true
	//Not yet finished
	for (int i = 0; i < determinant_possibilities.size(); i++) {
		std::vector<std::vector<int> > columns_not_in_determinant;
		for (int j = 0; j < int_column_names.size(); j++) {
			bool toAdd = true;
			for (int k = 0; k < determinant_possibilities[i].size(); k++) {
				if (equals_vector_int(int_column_names[j], determinant_possibilities[i][k]) == 1) {
					toAdd = false;
				}
			}
			if (toAdd) {
				columns_not_in_determinant.push_back(int_column_names[j]);
			}
		}
		//Here, check if any subset of dependents is consistent for every one of the same instances of a determinant
		for (int k = 0; k < columns_not_in_determinant.size(); k++) {
			cur_nondets.push_back(columns_not_in_determinant[k]);
		}
		for (int k = 0; k < determinant_possibilities[i].size(); k++) {
			cur_det.push_back(determinant_possibilities[i][k]);
		}

		int rc;
		rc = sqlite3_open("test.db", &db);
		char *zErrMsg = 0;
		std::string sql_message = "select * from COMPANY;";
		const char * sql = sql_message.c_str();
		char **data;
		rc = sqlite3_exec(db, sql, callback_two, (void*)data, &zErrMsg);
		if( rc != SQLITE_OK ) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		} else {
			sqlite3_close(db);
			break; //stubbing logic
		}
		sqlite3_close(db);



		/*
		for (int stubber = 0; stubber < cur_nondets.size(); stubber++) {
			std::cout << vector_int_to_string(cur_nondets[stubber]) << ",";
		}
		std::cout << "\n";
		for (int stubber = 0; stubber < cur_det.size(); stubber++) {
			std::cout << vector_int_to_string(cur_det[stubber]) << ",";
		}
		std::cout << "\n";
		std::cout << "\n";
		*/

		
		cur_nondets.clear();
		cur_det.clear();
		columns_not_in_determinant.clear();
	}



/*
	for (int i = 0; i < determinant_possibilities.size(); i++) {
		std::cout << "\n";
		for (int j = 0; j < determinant_possibilities[i].size(); j++) {
			std::cout << vector_int_to_string(determinant_possibilities[i][j]) << ",";
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



