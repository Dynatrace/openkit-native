/**
 * Copyright 2018-2021 Dynatrace LLC
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

#include "util/json/JsonWriter.h"
#include <iomanip>

using namespace openkit::json;

void JsonWriter::openArray()
{
	mStringStream << '[';
}

void JsonWriter::closeArray()
{
	mStringStream << "]";
}

void JsonWriter::openObject()
{
	mStringStream << "{";
}

void JsonWriter::closeObject()
{
	mStringStream << "}";
}

void JsonWriter::insertKey(const std::string& key)
{
	mStringStream << "\"" << escapeString(key) << "\"";
}

void JsonWriter::insertStringValue(const std::string& value)
{
	mStringStream << "\"" << escapeString(value) << "\"";
}

void JsonWriter::insertValue(const std::string& value)
{
	mStringStream << escapeString(value);
}

void JsonWriter::insertKeyValueSeperator()
{
	mStringStream << ":";
}

void JsonWriter::insertElementSeperator()
{
	mStringStream << ",";
}

const std::string JsonWriter::toString()
{
	return mStringStream.str();
}

const std::string JsonWriter::escapeString(const std::string& value)
{
    std::ostringstream esc;

    for (auto c = value.cbegin(); c != value.cend(); c++)
    {
        switch (*c)
        {
            
            case '\b': esc << "\\b"; break;
            case '\f': esc << "\\f"; break;
            case '\n': esc << "\\n"; break;
            case '\r': esc << "\\r"; break;
            case '\t': esc << "\\t"; break;
            case '/': esc << "\\/"; break;
            case '"': esc << "\\\""; break;
            case '\\': esc << "\\\\"; break;
            default:
                if ('\x00' <= *c && *c <= '\x1f')
                {
                    esc << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(*c);
                }
                else
                {
                    esc << *c;
                }
        }
    }

    return esc.str();
}

