#include <string>
#include <sqlite3.h>
#include <vector>

class normalizer {

char* filename;
sqlite3 *db;
std::vector<const char*> column_names;
//Commented Out As Is Currently Unused
//std::vector<functional_dependency> table_dependencies;


public:

	normalizer(char* new_filename, std::string table_name);
	
	char* get_filename();
};