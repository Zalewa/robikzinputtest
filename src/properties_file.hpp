#pragma once

#include "variant.hpp"

#include <string>
#include <vector>

namespace robikzinputtest {

struct Property {
	std::string name;
	Variant value;
};

std::vector<Property> read_properties(std::string rawfile);
std::string write_properties(std::vector<Property> properties);

} // namespace robikzinputtest
