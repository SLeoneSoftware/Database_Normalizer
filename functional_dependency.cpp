#include "functional_dependency.hpp"
#include <iostream>

functional_dependency::functional_dependency(std::vector<int> new_determinant_column_numbers, std::vector<int> new_dependent_column_numbers) {
	determinant_column_numbers = new_determinant_column_numbers;
	dependent_column_numbers = new_dependent_column_numbers;
}