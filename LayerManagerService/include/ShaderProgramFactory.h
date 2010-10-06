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

#ifndef _SHADERPROGRAMFACTORY_H_
#define _SHADERPROGRAMFACTORY_H_

#include <string>
#include <ShaderProgram.h>

/// pointer to shader program creator function
typedef ShaderProgram* (*PfnShaderProgramCreator)(const std::string&, const std::string&);

/**
 * Factory for creating platform specific shader programs.
 */
class ShaderProgramFactory
{
public:
	/**
	 * Create a new shader program.
	 *
	 * @param vertName   File name of vertex shader.
	 * @param fragName   File name of fragment shader.
	 * @return new Program instance, NULL if program could not be created.
	 */
	static ShaderProgram* createProgram(const std::string& vertName, const std::string& fragName);

	/**
	 * Set shader program creator function
	 *
	 * @param creatorFunc   Pointer to shader program creator function
	 */
	static void setCreatorFunc(PfnShaderProgramCreator creatorFunc);

private:
	/**
	 * Dummy private constructor. Prevents creation of instances of this class.
	 */
	ShaderProgramFactory(void);

private:
	/// pointer to shader program creator function
	static PfnShaderProgramCreator _creator;
};

#endif /* _SHADERPROGRAMFACTORY_H_ */

