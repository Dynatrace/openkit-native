/**
 * Copyright 2018-2019 Dynatrace LLC
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

#include "OpenKitComposite.h"

using namespace core::objects;

void OpenKitComposite::storeChildInList(const std::shared_ptr<IOpenKitObject> childObject)
{
	mChildren.push_back(childObject);
}

void OpenKitComposite::removeChildFromList(const std::shared_ptr<IOpenKitObject> childObject)
{
	mChildren.remove(childObject);
}

int32_t OpenKitComposite::getActionId() const
{
	return DEFAULT_ACTION_ID;
}