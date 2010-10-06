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

#include <Shader.h>

int Shader::_nextId=1;

Shader* Shader::createShader(const std::string& vertName, const std::string& fragName)
{
	Shader* shader = 0;
	ShaderProgram* program;

	// Obtain shader program. If a shader program of the same name already exists, 
	// its instance will be returned. If it doesn't exist, a new program is returned.
	program = ShaderProgram::obtain(vertName, fragName);
	
	if (program)
	{
		// create shader instance
		shader = new Shader(*program);
	}
	else
	{
		// failed to load, compile or link the program
	}
	
	return shader;
}

Shader::Shader(ShaderProgram& program)
	: _id(_nextId++)
	, _program(program)
	, _uniformMap()
{
	// increase program reference counter
	_program.ref();
}

Shader::~Shader()
{
	// decrease program reference counter. 
	// This might destroy the program if no longer in use.
	_program.unref();
}

void Shader::setUniform(const ShaderUniform& uniform)
{
	const std::string& name = uniform.getName();
	std::map<std::string,ShaderUniform>::iterator it;
	
	it = _uniformMap.find(name);

	if (it==_uniformMap.end())
	{
		// add new uniform
		int location = _program.getUniformLocation(name.c_str());
		ShaderUniform u(name, location);
		u.setData(uniform);
		_uniformMap.insert(std::pair<std::string,ShaderUniform>(name, u));
	}
	else
	{
		// update uniform data
		(*it).second.setData(uniform);
	}
}

void Shader::loadUniforms(void) const
{
	std::map<std::string,ShaderUniform>::const_iterator it;
	
	for (it=_uniformMap.begin(); it!=_uniformMap.end(); it++)
	{
		const ShaderUniform& uniform = (*it).second;
		uniform.load(_program);
	}
}

