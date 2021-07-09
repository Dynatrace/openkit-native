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


#ifndef _PROTOCOL_HTTP_HTTPHEADERCOLLECTION_H
#define _PROTOCOL_HTTP_HTTPHEADERCOLLECTION_H

#include "HttpHeaderUtil.h"

#include <list>
#include <string>
#include <unordered_map>
#include <initializer_list>

namespace protocol
{
	///
	/// Container utility class for storing HTTP request&response headers
	///
	class HttpHeaderCollection
	{
	private:

		using ContainerType = std::unordered_map<std::string, std::list<std::string>, HeaderNameHasher, HeaderNameEqualityComparer>;

	public:

		using Iterator = ContainerType::const_iterator;
		using ValueType = ContainerType::value_type;

		HttpHeaderCollection();

		HttpHeaderCollection(std::initializer_list<ValueType> initializerList);

		bool contains(const std::string& name) const;

		const std::list<std::string>& getHeader(const std::string& name) const;

		void setHeader(const std::string& name, const std::string& value);

		void appendHeader(const std::string& name, const std::string& value);

		bool empty() const;

		Iterator begin() const;

		Iterator end() const;

		void clear();

	private:

		ContainerType mContainer;
	};
}

#endif
