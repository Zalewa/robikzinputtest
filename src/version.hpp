#pragma once

#include <string>

namespace robikzinputtest {
/**
 * Author of the application.
 */
const std::string app_author();
/**
 * Copyright one-liner information of the application.
 */
const std::string app_copyright();
/**
 * Name and version of the application.
 */
const std::string app_full_signature();
/**
 * The organization part of the app identifier.
 */
const std::string app_identifier_organization();
/**
 * The application name part of the app identifier.
 */
const std::string app_identifier_appname();
/**
 * Identifier of the application, in reverse domain name notation.
 *
 * Is a concatenation of app_identifier_organization()
 * and app_identifier_appname().
 */
const std::string app_identifier();
/**
 * What the app is licensed under.
 */
const std::string app_license();
/**
 * Name of the application.
 */
const std::string app_name();
/**
 * Version of the application as a string, but without the 'v' prefix.
 */
const std::string app_version();
} // namespace robikzinputtest
