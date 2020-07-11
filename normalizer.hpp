#include <string>
#include <sqlite3.h>
#include <vector>
#include "functional_dependency.hpp"

class normalizer {

char* filename;
std::string table_name;
sqlite3 *db;
std::vector<const char*> column_names;
std::vector<std::vector<int> > int_column_names;
std::vector<functional_dependency> table_dependencies;

void clean_dependencies();


public:

	normalizer(char* new_filename, std::string table_name);

	void find_dependencies();

	void set_dependencies(std::vector<functional_dependency>, std::vector<std::string> schema);
	
	char* get_filename();

	std::vector<functional_dependency> get_dependencies();

	void find_minimum_cover(std::vector<std::string> schema);

	void normalize_three_nf();

	void normalize_boyce_codd_nf();
	
};