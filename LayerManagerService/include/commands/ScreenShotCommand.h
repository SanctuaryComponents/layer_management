/***************************************************************************
*
* Copyright 2010 BMW Car IT GmbH
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*		http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
****************************************************************************/

#ifndef _SCREENSHOTCOMMAND_H_
#define _SCREENSHOTCOMMAND_H_

#include "Command.h"
#include <string.h>

class ScreenShotCommand : public Command{
public:
	ScreenShotCommand(char* givenfilename) : Command(ScreenShot){
		filename = std::string(givenfilename);
	};
	std::string filename;

	virtual void execute(LayerList& layerlist){};
};


#endif /* _SCREENSHOTCOMMAND_H_ */
