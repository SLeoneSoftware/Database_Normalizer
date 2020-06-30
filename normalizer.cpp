#include "normalizer.hpp"
#include <iostream>
#include <unordered_map>

static bool columns_set = false;
static bool add_functional_dependency = true;
static std::vector<std::vector<int> > callback_column_names;
static std::vector<std::vector<int> > cur_det;
static std::vector<std::vector<int> > cur_nondets;
std::unordered_map<std::string, std::string> fd_history_map;


//Method for obtaining all column headers from sqlite db
static int callback(void *data, int argc, char **argv, char **azColName){
	if (!columns_set) {
		for(int i = 0; i<argc; i++){
			std::vector<int> word;
			callback_column_names.push_back(word);
			std::string name = std::string(azColName[i]);
			for (std::string::size_type j = 0; j < name.size(); j++) {
				callback_column_names[i].push_back(name[j]);
			}
		}
		columns_set = true;
	}
   return 0;
}

/* Above function stores all column names as vector ints to avoid some memory allocation using data parameter.
   Doing this increased speed on my laptop. Feel free to change if you use this library */
static std::string vector_int_to_string(std::vector<int> word) {
	std::string string_form;
	for (int i = 0; i < word.size(); i++) {
		string_form += (char) word[i];
	}
	return string_form;
}

/* Brute force method to ensure there are no contradictions to the
   current determinant -> dependant relationship */
static int callback_two(void *data, int argc, char **argv, char **azColName){
	std::string determinant = "";
	std::string dependant = "";
	for (int i = 0; i < argc; i++) {
		for (int j = 0; j < cur_det.size(); j++) {
			if (vector_int_to_string(cur_det[j]).compare(std::string(azColName[i])) == 0) {
				determinant = determinant + std::string(argv[i]);
			} 
		}
		for (int j = 0; j < cur_nondets.size(); j++) {
			if (vector_int_to_string(cur_nondets[j]).compare(std::string(azColName[i])) == 0) {
				dependant = dependant + std::string(argv[i]);
			} 
		}
	}
	if (fd_history_map.find(determinant) == fd_history_map.end()) {
		fd_history_map[determinant] = dependant;
	} else {
		if (!(fd_history_map[determinant] == dependant)) {
			add_functional_dependency = false;
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

normalizer::normalizer(char* new_filename, std::string new_table_name) {
	filename = new_filename;
	table_name = new_table_name;
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
		for (int i = 0; i < callback_column_names.size(); i++) {
			int_column_names.push_back(callback_column_names[i]);
			const char *toAdd =  vector_int_to_string(callback_column_names[i]).c_str();
			column_names.push_back(toAdd);
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
	/* Now that all possibilities have been obtained, each one needs to be tested to see if it is a determinant for any dependency
	   This Brute Force Algorithm will mark down any potential determinant -> dependent relationship in which the dependent is
	   always the same for the same determinant */
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
		/* Here, check if any subset of dependents is consistent for every one of the same instances of a determinant
		   First get all possible dependents for current determinant */
		int m = columns_not_in_determinant.size();
		for(int k = 0; k < (1<<m); k++) {
			cur_nondets.clear();
			for (int j = 0; j < m; j++) {
				if ((k & (1 << j)) > 0) {
					cur_nondets.push_back(columns_not_in_determinant[j]);
				}
			}
			//Brute force checker to see if this determinant -> dependent relationship holds true
				int rc;
				rc = sqlite3_open(filename, &db);
				char *zErrMsg = 0;
				std::string sql_message = "select * from " + table_name + ";";
				const char * sql = sql_message.c_str();
				char **data;
				rc = sqlite3_exec(db, sql, callback_two, (void*)data, &zErrMsg);
				if( rc != SQLITE_OK ) {
					sqlite3_free(zErrMsg);
				} else {
					if (add_functional_dependency && cur_nondets.size() > 0 && determinant_possibilities[i].size() > 0) {
					//TODO: Make sure there is no subset yet
						std::vector<std::string>  determinant;
						std::vector<std::string>  dependant;
						for (int l = 0; l < cur_nondets.size(); l++) {
							dependant.push_back(vector_int_to_string(cur_nondets[l]));
						}
						for (int l = 0; l < determinant_possibilities[i].size(); l++) {
							determinant.push_back(vector_int_to_string(determinant_possibilities[i][l]));
						}
						table_dependencies.push_back(functional_dependency(determinant, dependant));
					}
					fd_history_map.clear();
					add_functional_dependency = true;
				}
				sqlite3_close(db);
		}
		columns_not_in_determinant.clear();
		cur_det.clear();
	}
	clean_dependencies();
}

/* Private method to remove useless functional dependencies, including:
   - determinant is also satisfied by a subset of that determinant
   - dependant is also satsified by same determinant for subsets of dependants that add up to it
   - work in progress
   */
void normalizer::clean_dependencies() {
	std::unordered_map<std::string, std::string> determinant_history;
	for (int i = 0; i < table_dependencies.size(); i++) {
		functional_dependency cur_functional_dependency = table_dependencies[i];
		std::string string_form = cur_functional_dependency.toString();
		size_t index = string_form.find(" -> ");
		std::string cur_determinant = string_form.substr(0, index);
		std::string cur_dependency = string_form.substr(index + 4, string_form.length());
		if (determinant_history.find(cur_determinant) == determinant_history.end()) {
			determinant_history[cur_determinant] = cur_dependency;
		}
	}
	/*
	for (int i = 0; i < table_dependencies.size(); i++) {
		functional_dependency cur_dependency = table_dependencies[i];
		if (determinant_history.find(cur_dependency.get_determinant_names()) == determinant_history.end()) {
			//determinant_history[cur_dependency.get_determinant_names()] = cur_dependency.get_dependent_names();
		}
	} */
}

std::vector<functional_dependency> normalizer::get_dependencies() {
	return table_dependencies;
}

void normalizer::set_dependencies(std::vector<functional_dependency> new_dependencies) {
	table_dependencies = new_dependencies;
}

//Below Algorithms still require implementations

void normalizer::normalize_three_nf() {

}

void normalizer::normalize_boyce_codd_nf() {
	
}

