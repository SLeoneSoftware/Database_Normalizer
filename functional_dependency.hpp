#include <vector>
#include <string>



class functional_dependency {
	std::vector<std::string> determinant_names;
	std::vector<std::string> dependent_names;

public:
	functional_dependency(std::vector<std::string> new_determinant_names, std::vector<std::string> new_dependent_names);

};