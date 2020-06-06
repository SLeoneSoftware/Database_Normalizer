#include "functional_dependency.hpp"
#include <iostream>

functional_dependency::functional_dependency(std::vector<std::string> new_determinant_names, std::vector<std::string> new_dependent_names) {
	determinant_names = new_determinant_names;
	dependent_names = new_dependent_names;
}