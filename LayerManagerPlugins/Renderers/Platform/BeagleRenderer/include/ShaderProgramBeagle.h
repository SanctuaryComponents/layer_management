/***************************************************************************
*
* Copyright 2010,2011 BMW Car IT GmbH
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

#ifndef BEAGLESHADERPROGRAM_H_
#define BEAGLESHADERPROGRAM_H_

#include <ShaderProgram.h>
#include <GLES2/gl2.h>


/// pointer to shader program creator function
typedef ShaderProgram* (*PfnShaderProgramCreator)(const std::string&, const std::string&);

/**
 * Factory for creating platform specific shader programs.
 */
class ShaderProgramBeagle : public ShaderProgram
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
	 * Destructor
	 */
	virtual ~ShaderProgramBeagle(void);

	/**
	 * Start using the shader program for rendering.
	 */
	virtual void use(void) const;

	virtual int getUniformLocation(const char* name) const
	{
		return glGetUniformLocation(_progHandle, name);
	}

	virtual void uniform1iv(int location, int count, const int* v) const
	{
		glUniform1iv(location, count, v);
	}

	virtual void uniform1fv(int location, int count, const float* v) const
	{
		glUniform1fv(location, count, v);
	}

	virtual void uniform2fv(int location, int count, const float* v) const
	{
		glUniform2fv(location, count, v);
	}

	virtual void uniform3fv(int location, int count, const float* v) const
	{
		glUniform3fv(location, count, v);
	}

	virtual void uniform4fv(int location, int count, const float* v) const
	{
		glUniform4fv(location, count, v);
	}

	virtual void uniformMatrix2fv(int location, int count, bool transpose, const float* v) const
	{
		glUniformMatrix2fv(location, count, transpose, v);
	}

	virtual void uniformMatrix3fv(int location, int count, bool transpose, const float* v) const
	{
		glUniformMatrix3fv(location, count, transpose, v);
	}

	virtual void uniformMatrix4fv(int location, int count, bool transpose, const float* v) const
	{
		glUniformMatrix4fv(location, count, transpose, v);
	}

protected:
	/**
	 * Protected constructor.
	 * New instances of this class are supposed to be created by ShaderProgramBeagle::createProgram.
	 *
	 * @param vertName    File name of vertex shader.
	 * @param fragName    File name of fragment shader.
	 */
	ShaderProgramBeagle(const std::string& vertName, const std::string& fragName, GLuint handle);

private:
	/// OpenGL ES program handle
	GLuint _progHandle;
};

#endif /* BEAGLESHADERPROGRAM_H_ */
