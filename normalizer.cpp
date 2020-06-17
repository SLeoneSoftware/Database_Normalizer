#include "normalizer.hpp"
#include <iostream>
#include <unordered_map>

static bool set = false;
static bool add = true;
static std::vector<std::vector<int> > columnNames;
static std::vector<std::vector<int> > cur_det;
static std::vector<std::vector<int> > cur_nondets;
std::unordered_map<std::string, std::string> umap;


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

//Above function stores all column names as vector ints to avoid some memory allocation using data parameter. Doing this increased speed on my laptop. (Feel free to change if you use this library)
static std::string vector_int_to_string(std::vector<int> word) {
	std::string toReturn;
	for (int i = 0; i < word.size(); i++) {
		toReturn = toReturn + (char) word[i];
	}
	return toReturn;
}

//SQL method for checking dependencies
static int callback_two(void *data, int argc, char **argv, char **azColName){
	std::string determinant = "";
	std::string dependant = "";
	for (int i = 0; i < argc; i++) {
		for (int j = 0; j < cur_det.size(); j++) {
			if (vector_int_to_string(cur_det[j]).compare(std::string(azColName[i])) != 0) {
				determinant = determinant + std::string(argv[i]);
			} 
		}
		for (int j = 0; j < cur_nondets.size(); j++) {
			if (vector_int_to_string(cur_nondets[j]).compare(std::string(azColName[i])) != 0) {
				dependant = dependant + std::string(argv[i]);
			} 
		}
	}
	if (umap.find(determinant) == umap.end()) {
		umap[determinant] = dependant;
		//std::cout << "This tuple is okay" << "\n";
	} else {
		if (!(umap[determinant] == dependant)) {
			//std::cout << "Remove Dependency from chart";
			add = false;
		} else {
			//std::cout << "This tuple is okay" << "\n";
		}
	}
   return 0;
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
			//std::cout << column_names[0] << "\n";
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
		for (int k = 0; k < determinant_possibilities[i].size(); k++) {
			cur_det.push_back(determinant_possibilities[i][k]);
		}
		//Here, check if any subset of dependents is consistent for every one of the same instances of a determinant
		//First get all possible dependents for current determinant
		int m = columns_not_in_determinant.size();
		for(int k = 0; k < (1<<m); k++) {
			cur_nondets.clear();
			for (int j = 0; j < m; j++) {
				if ((k & (1 << j)) > 0) {
					cur_nondets.push_back(columns_not_in_determinant[j]);
					//std::cout << vector_int_to_string(columns_not_in_determinant[j]);
				}
			}
			//now, run brute force checker to see if this determinant -> dependent relationship should be added
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
					if (add) {
					//TODO: Make sure there is no subset yet
						std::vector<std::string>  determinant;
						std::vector<std::string>  dependant;
						for (int l = 0; l < cur_nondets.size(); l++) {
							dependant.push_back(vector_int_to_string(cur_nondets[l]));
							//std::cout << vector_int_to_string(cur_nondets[l]) << "\n";
						}
						for (int l = 0; l < determinant_possibilities[i].size(); l++) {
							determinant.push_back(vector_int_to_string(determinant_possibilities[i][l]));
						}
						table_dependencies.push_back(functional_dependency(determinant, dependant));
					}
					//
					umap.clear();
					add = true;
				}
				sqlite3_close(db);
		}
		columns_not_in_determinant.clear();
		cur_det.clear();
	}

}

std::vector<functional_dependency> normalizer::get_dependencies() {
	return table_dependencies;
}

void normalizer::set_dependencies(std::vector<functional_dependency> new_dependencies) {
	table_dependencies = new_dependencies;
}






























































