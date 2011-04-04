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

#ifndef _SHADERPROGRAM_H_
#define _SHADERPROGRAM_H_

#include <string>
#include <list>

/**
 * Represents an OpenGL shader program.
 *
 * There is a global list of allocated shader programs. Each instance is reference
 * counted, so if program is no longer in use by any shader instance, it will
 * be deleted automatically.
 */
class ShaderProgram
{
public:
	/**
	 * Stores uniform values for common surface properties,
	 * like position, size, opacity, etc...
	 */
	struct CommonUniforms
	{
		float x;
		float y;
		float width;
		float height;
		float opacity;
		float texRange[2];
		float texOffset[2];
		int texUnit;
		float* matrix;
	};

	/**
	 * Return a shader program from the gobal list. If no matching instance is found, a new
	 * one will be created and added to the list.
	 *
	 * @param vertName   File name of vertex shader.
	 * @param fragName   File name of fragment shader.
	 * @return new Program instance, NULL if shader could not be loaded, compiled or linked.
	 */
	static ShaderProgram* obtain(const std::string& vertName, const std::string& fragName);

	/**
	 * Destructor
	 */
	virtual ~ShaderProgram();

	/**
	 * Start using the shader program for rendering.
	 */
	virtual void use(void) const = 0;

	/**
	 * Load uniform values for common surface properties, like position,
	 * size, opacity, etc...
	 *
	 * @param uniforms   Uniform values
	 */
	void loadCommonUniforms(const CommonUniforms& uniforms) const;

	/**
	 * @return The name of the vertex shader
	 */
	const std::string& getVertexName(void) const
	{
		return _vertName;
	}

	/**
	 * @return The name of the fragment shader
	 */
	const std::string& getFragmentName(void) const
	{
		return _fragName;
	}

	/**
	 * Increment usage counter for this object.
	 */
	void ref(void);

	/**
	 * Decrement usage counter for this object.
	 * It will be delete if usage counter reaches 0.
	 */
	void unref(void);

	virtual int getUniformLocation(const char* name) const = 0;

	virtual void uniform1iv(int location, int count, const int* v) const = 0;

	virtual void uniform1fv(int location, int count, const float* v) const = 0;

	virtual void uniform2fv(int location, int count, const float* v) const = 0;

	virtual void uniform3fv(int location, int count, const float* v) const = 0;

	virtual void uniform4fv(int location, int count, const float* v) const = 0;

	virtual void uniformMatrix2fv(int location, int count, bool transpose, const float* v) const = 0;

	virtual void uniformMatrix3fv(int location, int count, bool transpose, const float* v) const = 0;

	virtual void uniformMatrix4fv(int location, int count, bool transpose, const float* v) const = 0;

protected:
	/**
	 * Protected constructor.
	 * New instances of this class are supposed to be created by the shader program factory.
	 *
	 * @param vertName    File name of vertex shader.
	 * @param fragName    File name of fragment shader.
	 */
	ShaderProgram(const std::string& vertName, const std::string& fragName);

	/**
	 * Update the list of uniform locations
	 */
	void updateCommonUniformLocations(void);

private:
	/// usage counter
	int _refCounter;

	/// name of the vertex shader
	std::string _vertName;

	/// name of the fragment shader
	std::string _fragName;

	// uniform locations
	int _xLoc;
	int _yLoc;
	int _widthLoc;
	int _heightLoc;
	int _opacityLoc;
	int _texRangeLoc;
	int _texOffsetLoc;
	int _texUnitLoc;
	int _matrixLoc;
	/// global list of programs
	static std::list<ShaderProgram*> _programList;
};

#endif /* _SHADERPROGRAM_H_ */

