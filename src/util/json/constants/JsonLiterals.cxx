/**
 * Copyright 2018-2020 Dynatrace LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "util/json/constants/JsonLiterals.h"

using namespace util::json::constants;


const char* JsonLiterals::BOOLEAN_TRUE_LITERAL = "true";

const char* JsonLiterals::BOOLEAN_FALSE_LITERAL = "false";

const char* JsonLiterals::NULL_LITERAL = "null";

const char* JsonLiterals::NUMBER_PATTERN_STRING = "^-?(0|[1-9]\\d*)(\\.\\d+)?([eE][+-]?\\d+)?$";

const std::regex& JsonLiterals::getNumberPattern()
{
	static std::regex numberPattern(JsonLiterals::NUMBER_PATTERN_STRING, std::regex::optimize);
	return numberPattern;
}