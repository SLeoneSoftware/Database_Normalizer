#include <string>
#include <sqlite3.h>
#include <vector>

class normalizer {

std::string filename;
sqlite3 *db;
//Commented Out As Is Currently Unused
//std::vector<functional_dependency> table_dependencies;

public:
	normalizer(std::string new_filename);
	
	std::string get_filename();
};