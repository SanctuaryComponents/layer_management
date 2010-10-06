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

#ifndef _SHADER_H_
#define _SHADER_H_

#include <ShaderProgram.h>
#include <ShaderUniform.h>
#include <string>
#include <map>

/**
 * Represents a shader instance.
 *
 * It stores a set of uniform parameters and refers to an OpenGL program object.
 * Some uniform variables are pre-defined, like surface position and size, 
 * opacity, etc... Additionally, there may be user-defined uniforms.
 */
class Shader
{
public:
	/**
	 * Creates a new shader instance by vertex and fragment shader name.
	 * @param vertName   File name of vertex shader.
	 * @param fragName   File name of fragment shader.
	 * @return new Shader instance, NULL if shader could not be loaded, compiled or linked.
	 */
	static Shader* createShader(const std::string& vertName, const std::string& fragName);

	/**
	 * Destructor
	 */
	~Shader();

	/**
	 * @return Unique ID of this instance
	 */
	int getId(void) const
	{
		return _id;
	}

	/**
	 * Start using this shader for rendering.
	 */
	void use(void) const
	{
		_program.use();
	}

	/**
	 * Set a uniform.
	 */
	void setUniform(const ShaderUniform& uniform);

	/**
	 * Update uniform values.
	 * Please note that this method doesn't update the standard uniforms
	 * used for position, size, etc... They need to be set separately
	 * by loadCommonUniforms().
	 */
	void loadUniforms(void) const;
	
	/**
	 * Load uniform values for common surface properties, like position, 
	 * size, opacity, etc...
	 *
	 * @param uniforms   Uniform values
	 */
	void loadCommonUniforms(const ShaderProgram::CommonUniforms& uniforms) const
	{
		_program.loadCommonUniforms(uniforms);
	}

private:
	/**
	 * Private constructor.
	 * New instances of this class are supposed to be created by createShader(...).
	 *
	 * @param program  Program object to be used	 
	 */
	Shader(ShaderProgram& program);

private:
	/// unique shader ID
	const int _id;

	/// reference to OpenGL program object used by this shader instance
	ShaderProgram& _program;

	/// a map of user-defined uniforms
	std::map<std::string,ShaderUniform> _uniformMap;
	
	/// next unique ID
	static int _nextId;
};

#endif /* _SHADER_H_ */

