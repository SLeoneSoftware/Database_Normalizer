#include <string>
#include <sqlite3.h>
#include <vector>
#include "functional_dependency.hpp"

class normalizer {

char* filename;
sqlite3 *db;
std::vector<const char*> column_names;
std::vector<std::vector<int> > int_column_names;
std::vector<functional_dependency> table_dependencies;


public:

	normalizer(char* new_filename, std::string table_name);

	void find_dependencies();
	
	char* get_filename();
};