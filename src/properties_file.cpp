#include "properties_file.hpp"

#include "text.hpp"

#include <algorithm>
#include <sstream>

namespace robikzinputtest {

std::vector<Property> read_properties(std::string rawfile) {
	std::vector<Property> properties;
	std::istringstream iss(rawfile);
	std::string line;

	while (std::getline(iss, line)) {
		if (line.empty())
			continue;
		auto equal_pos = line.find('=');
		if (equal_pos == std::string::npos)
			continue;

		std::string name = text::trim(line.substr(0, equal_pos));
		std::string value = text::trim(line.substr(equal_pos + 1));

		std::transform(
			name.begin(), name.end(), name.begin(),
			[](char c) { return std::tolower(c); }
		);

		properties.push_back({ name, value });
	}

	return properties;
}

std::string write_properties(std::vector<Property> properties) {
	std::stringstream ss;
	for (const Property &property : properties) {
		std::string lowername(property.name.size(), '\0');
		std::transform(
			property.name.begin(), property.name.end(), lowername.begin(),
			[](char c) { return std::tolower(c); }
		);
		ss << lowername << "=" << property.value.as_string() << std::endl;
	}
	return ss.str();
}

} // namespace robikzinputtest
