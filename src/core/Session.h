/**
* Copyright 2018 Dynatrace LLC
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

#ifndef _CORE_SESSION_H
#define _CORE_SESSION_H

#include "api/ISession.h"

#include "memory"

namespace core
{
	///
	///  Actual implementation of the ISession interface.
	///
	class Session : public api::ISession
	{
	public:
		///
		/// Destructor
		///
		virtual ~Session() {};
		///
		/// Enters an Action with a specified name in this Session.
		/// @param[in] actionName name of the Action
		/// @returns Action instance to work with
		///
		virtual std::shared_ptr<api::IRootAction> EnterAction();

		///
		///  Ends this Session and marks it as ready for immediate sending.
		///
		virtual void end();
	};
}
#endif