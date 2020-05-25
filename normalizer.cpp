#include "normalizer.hpp"
#include <iostream>

normalizer::normalizer(std::string new_filename) {
	filename = new_filename;
	int rc;
	rc = sqlite3_open("test.db", &db);
}

std::string normalizer::get_filename() {
	return filename;
}