#include <string>
#include <sqlite3.h>
#include <vector>

class normalizer {

char* filename;
sqlite3 *db;
static std::vector<char*> columnNames;
//Commented Out As Is Currently Unused
//std::vector<functional_dependency> table_dependencies;

public:
	normalizer(char* new_filename);

	static void set_column_names(std::vector<char*> new_column_names);
	
	char* get_filename();
};