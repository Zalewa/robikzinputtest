#include "version.hpp"
#include "version_defs.hpp"

namespace robikzinputtest {

const std::string app_author()
{
	return "Robert \"Robikz\" Zalewski";
}

const std::string app_copyright()
{
	return "Copyright (c) 2025-2026 Robert \"Robikz\" Zalewski";
}

const std::string app_full_signature()
{
	return app_name() + " v" + app_version();
}

const std::string app_identifier_organization()
{
	return "com.github.zalewa";
}

const std::string app_identifier_appname()
{
	return "robikzinputtest";
}

const std::string app_identifier()
{
	return app_identifier_organization() + "." + app_identifier_appname();
}

const std::string app_license()
{
	return "MIT License";
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
