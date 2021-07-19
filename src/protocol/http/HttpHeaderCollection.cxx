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

#include "HttpHeaderCollection.h"

#include <stdexcept>

using namespace protocol;

HttpHeaderCollection::HttpHeaderCollection()
	: mContainer()
{
}

HttpHeaderCollection::HttpHeaderCollection(std::initializer_list<ValueType> initializerList)
	: mContainer(initializerList)
{
}

std::list<std::string> HttpHeaderCollection::getHeaderNames() const
{
	std::list<std::string> headerNames;
	for (const auto& entry : mContainer)
	{
		headerNames.push_back(entry.first);
	}

	return headerNames;
}

bool HttpHeaderCollection::contains(const std::string& name) const
{
	return mContainer.find(name) != mContainer.end();
}

const std::list<std::string>& HttpHeaderCollection::getHeader(const std::string& name) const
{
	const auto iter = mContainer.find(name);
	if (iter == mContainer.end())
	{
		throw std::invalid_argument(std::string("Header \"") + name + "\" not found");
	}

	return iter->second;
}

void HttpHeaderCollection::setHeader(const std::string& name, const std::string& value)
{
	// erase old entry, just in case new one has different case for header name
	mContainer.erase(name);
	mContainer.insert({ name, std::list<std::string>{ value } });
}

void HttpHeaderCollection::appendHeader(const std::string& name, const std::string& value)
{
	auto iter = mContainer.find(name);
	if (iter != mContainer.end())
	{
		iter->second.push_back(value);
	}
	else
	{
		mContainer.insert({ name, std::list<std::string>{ value } });
	}
}

bool HttpHeaderCollection::empty() const
{
	return mContainer.empty();
}

HttpHeaderCollection::Iterator HttpHeaderCollection::begin() const
{
	return mContainer.begin();
}

HttpHeaderCollection::Iterator HttpHeaderCollection::end() const
{
	return mContainer.end();
}

void HttpHeaderCollection::clear()
{
	mContainer.clear();
}