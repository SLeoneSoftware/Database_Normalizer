#include "functional_dependency.hpp"
#include <iostream>

functional_dependency::functional_dependency(std::vector<std::string> new_determinant_names, std::vector<std::string> new_dependent_names) {
	determinant_names = new_determinant_names;
	dependent_names = new_dependent_names;
}

std::vector<std::string> functional_dependency::get_determinant_names() {
	return determinant_names;
}

std::vector<std::string> functional_dependency::get_dependent_names() {
	return dependent_names;
}

void functional_dependency::set_determinant_names(std::vector<std::string> new_determinant_names) {
	determinant_names = new_determinant_names;
}

void functional_dependency::set_dependent_names(std::vector<std::string> new_dependent_names) {
	dependent_names = new_dependent_names;
}

std::string functional_dependency::toString() {
		std::string determinant = "";
		std::string dependant = "";
		std::vector<std::string> dmn = determinant_names;
		std::vector<std::string> dpn = dependent_names;
		for (int i = 0; i < dmn.size(); i++) {
			determinant += dmn[i];
			if (i + 1 < dmn.size()) {
				determinant += ",";
			}
		}
		for (int i = 0; i < dpn.size(); i++) {
			dependant += dpn[i];
			if (i + 1 < dpn.size()) {
				dependant += ",";
			}
		}
		return determinant + std::string(" -> ") + dependant;
}