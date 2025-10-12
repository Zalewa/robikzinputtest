#include "version.hpp"
#include "version_defs.hpp"

namespace robikzinputtest {

const std::string app_author()
{
	return "Robert \"Robikz\" Zalewski";
}

const std::string app_copyright()
{
	return "Copyright (c) 2025 Robert \"Robikz\" Zalewski";
}

const std::string app_full_signature()
{
	return app_name() + " v" + app_version();
}

const std::string app_identifier()
{
	return "com.github.zalewa.robikzinputtest";
}

const std::string app_name()
{
	return "Robikz's Input Test";
}

const std::string app_version()
{
	return ROBIKZINPUTTEST_VERSION_STRING;
}

} // namespace robikzinputtest
