#include "normalizer.hpp"
#include <iostream>
#include <unordered_map>
#include <algorithm>

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

bool equals_vector_string(std::vector<std::string> one, std::vector<std::string> two) {
	if (!(one.size() == two.size())) {
		return false;
	} else {
		for (int i = 0; i < one.size(); i++) {
			std::string cur = one[i];
			bool in = false;
			for (int j = 0; j < two.size(); j++) {
				if (two[j].compare(cur) == 0) {
					in = true;
				}
			}
			if (!in) {
				return false;
			}
		}
	}
	return true;
}

std::string vector_to_string(std::vector<std::string> vec) {
	std::string result = "";
	for (int i = 0; i < vec.size(); i++) {
		result += vec[i];
		if (i + 1 < vec.size()) {
			result += ",";
		}
	}
	return result;
}

std::string vector_to_string_remove_index(std::vector<std::string> vec, int exclusion_index) {
	std::string result = "";
	for (int i = 0; i < vec.size(); i++) {
		if (i != exclusion_index) {
			result += vec[i];
			if (i + 1 < vec.size()) {
				result += ",";
			}
		}
	}
	return result;
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


std::vector<std::string> split(std::string s, std::string delimiter) {
	std::vector<std::string> split_string;
	std::string cur_word = "";
	for (int i = 0; i < s.size(); i++) {
		std::string cur_char = "";
		cur_char += s[i];
		if (cur_char.compare(delimiter) == 0) {
			split_string.push_back(cur_word);
			cur_word = "";
		} else {
			cur_word += cur_char;
		}
	}
	split_string.push_back(cur_word);
	return split_string;
}



/* Private method to remove useless functional dependencies, including:
   - dependency is also satisfied by a only a subset of it's determinant
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
		} else {
			std::vector<std::string> cur_dependency_vector = cur_functional_dependency.get_dependent_names();
			for (int i = 0; i < cur_dependency_vector.size(); i++) {
				if (determinant_history[cur_determinant].find(cur_dependency_vector[i]) == -1) {
					determinant_history[cur_determinant] += std::string(",") + cur_dependency_vector[i];
				}
			}
		}
	} 
	table_dependencies.clear();
	std::vector<functional_dependency> new_functional_dependencies;
	for (auto& it: determinant_history) {
		//In here, check if dependency is also satisfied by a only a subset of it's determinant

		//std::cout << it.first << " -> " << it.second << "\n";
		functional_dependency revised_fd = functional_dependency(split(it.first, ","), split(it.second, ","));
		table_dependencies.push_back(revised_fd);
	}
}


std::vector<functional_dependency> normalizer::get_dependencies() {
	return table_dependencies;
}

void normalizer::set_dependencies(std::vector<functional_dependency> new_dependencies, std::vector<std::string> schema) {
	table_dependencies = new_dependencies;
}

//Schema is a list of all attributes
void remove_extraneous(std::vector<functional_dependency> functional_dependencies, std::vector<std::string> schema) {
	std::unordered_map<std::string, std::vector<std::string> > determinant_matchings;
	for (int i = 0; i < functional_dependencies.size(); i++) {
		functional_dependency cur_functional_dependency = functional_dependencies[i];
		std::string string_form = cur_functional_dependency.toString();
		size_t index = string_form.find(" -> ");
		std::string cur_determinant = string_form.substr(0, index);
		if (determinant_matchings.find(cur_determinant) == determinant_matchings.end()) {
			determinant_matchings[cur_determinant] = cur_functional_dependency.get_dependent_names();
			for (int j = 0; j < cur_functional_dependency.get_determinant_names().size(); j++) {
				determinant_matchings[cur_determinant].push_back(cur_functional_dependency.get_determinant_names()[j]);
			}
		} else {
			std::vector<std::string> cur_dependency = cur_functional_dependency.get_dependent_names();
			std::vector<std::string> old_dependency = determinant_matchings[cur_determinant];
			for (int j = 0; j < cur_dependency.size(); j++) {
				bool in = false;
				for (int k = 0; k < old_dependency.size(); k++) {
					if (old_dependency[k].compare(cur_dependency[j]) == 0) {
						in = true;
					}
				}
				if (!in) {
					determinant_matchings[cur_determinant].push_back(cur_dependency[j]);
				}
			}
		}
	}

	//Remove any attributes 'A' from determinant where determinant_matchings[determinant] = determinant_matchings[determinant - A]
	for (int i = 0; i < functional_dependencies.size(); i++) {
		functional_dependency cur = functional_dependencies[i];
		for (int j = 0; j < cur.get_determinant_names().size(); j++) {
			if (equals_vector_string(determinant_matchings[vector_to_string(cur.get_determinant_names())], determinant_matchings[vector_to_string_remove_index(cur.get_determinant_names(),j)])) {
				    using std::swap;
				    swap(cur.get_determinant_names()[i], cur.get_determinant_names().back());
				    cur.get_determinant_names().pop_back();
				    j -= 1;
			}
		}
	}


}


//Below Algorithms still require implementations

//Currently working on this algorithm
void normalizer::find_minimum_cover(std::vector<std::string> schema) {
	std::vector<functional_dependency> minimum_cover;
	for (int i = 0; i < table_dependencies.size(); i++) {
		functional_dependency cur_functional_dependency = table_dependencies[i];
		std::vector<std::string> dependants = cur_functional_dependency.get_dependent_names();
		for (int j = 0; j < dependants.size(); j++) {
			std::vector<std::string> one_dependant;
			one_dependant.push_back(dependants[j]);
			minimum_cover.push_back(functional_dependency(cur_functional_dependency.get_determinant_names(), one_dependant));
		}
	}

	for (int k = 0; k < minimum_cover.size(); k++) {
		std::cout << minimum_cover[k].toString() << "\n";
	}
}

void normalizer::normalize_three_nf() {

}

void normalizer::normalize_boyce_codd_nf() {
	
}

