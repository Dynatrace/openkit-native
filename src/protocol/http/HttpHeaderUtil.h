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


#ifndef _PROTOCOL_HTTP_HTTPHEADERUTIL_H
#define _PROTOCOL_HTTP_HTTPHEADERUTIL_H

#include <string>

namespace protocol
{

	///
	/// struct for hashing HTTP headers
	///
	/// @par
	/// Use this hasher if HTTP headers are stored in
	/// @c std::unordered_map or std::unordered_set
	///
	struct HeaderNameHasher
	{
		std::size_t operator()(const std::string& name) const;
	};

	///
	/// struct for comparing HTTP headers
	///
	/// @remarks Comparison works case insensitive
	///
	struct HeaderNameEqualityComparer
	{
		bool operator()(const std::string& lhs, const std::string& rhs) const;
	};
}

#endif
