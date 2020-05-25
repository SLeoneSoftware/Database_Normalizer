#include <string>
#include <sqlite3.h> 

class normalizer {

std::string filename;
sqlite3 *db;

public:
	normalizer(std::string new_filename);
	
	std::string get_filename();
};