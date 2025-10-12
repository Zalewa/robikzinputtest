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
 * Identifier of the application, usually in reverse domain name notation.
 */
const std::string app_identifier();
/**
 * Name of the application.
 */
const std::string app_name();
/**
 * Version of the application as a string, but without the 'v' prefix.
 */
const std::string app_version();
} // namespace robikzinputtest
