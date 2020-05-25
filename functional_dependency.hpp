#include <vector>
#include <string>



class functional_dependency {
	//Indexes for column numbers start at 0
	std::vector<int> determinant_column_numbers;
	std::vector<int> dependent_column_numbers;

	std::vector<std::string> determinant_names;
	std::vector<std::string> dependent_names;

public:
	functional_dependency(std::vector<int> new_determinant_column_numbers, std::vector<int> new_dependent_column_numbers);

};