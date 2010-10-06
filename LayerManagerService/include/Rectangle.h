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

#ifndef _RECTANGLE_H_
#define _RECTANGLE_H_

#include <ostream>

class Rectangle {
public:
	Rectangle(){};
	Rectangle(int x, int y, int width, int height) : x(x),y(y),width(width),height(height){};
	int x;
	int y;
	int width;
	int height;

	bool operator ==(const Rectangle rhs) const{
		return (x==rhs.x && y == rhs.y && width==rhs.width && height==rhs.height);
	}

	friend std::ostream& operator<<(std::ostream& stream, const Rectangle& obj);
};

#endif /* _RECTANGLE_H_ */
