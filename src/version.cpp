#include "version.hpp"
#include "version_defs.hpp"

namespace robikzinputtest {

const std::string app_name()
{
	return "Robikz's Input Test";
}

const std::string app_version()
{
	return ROBIKZINPUTTEST_VERSION_STRING;
}

} // namespace robikzinputtest
