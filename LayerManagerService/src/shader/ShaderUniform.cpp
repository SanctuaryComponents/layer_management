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

#include <ShaderUniform.h>
#include <ShaderProgram.h>
#include <iostream>
#include <sstream>
#include <string>
#include "Log.h"

using namespace std;

const int ShaderUniform::_vectorSizesOfDataTypes[]=
{
	0,	// Undefined
	1,	// Vector1f
	2,	// Vector2f
	3,	// Vector3f
	4,	// Vector4f
	4,	// Matrix2f
	9,	// Matrix3f
	16	// Matrix4f
};

ShaderUniform* ShaderUniform::createFromStringDescription(const std::string& desc)
{
	istringstream iss(desc);
	vector<float> floatData;
	Type type = Undefined;
	string name;
	string token;

	// get name
	iss >> name;

	// get type
	iss >> token;
	if (token=="1f")
		type = Vector1f;
	else if (token=="2f")
		type = Vector2f;
	else if (token=="3f")
		type = Vector3f;
	else if (token=="4f")
		type = Vector4f;
	else if (token=="m2f")
		type = Matrix2f;
	else if (token=="m3f")
		type = Matrix3f;
	else if (token=="m4f")
		type = Matrix4f;
	else
	{
		LOG_ERROR("Shader Uniform","Can not parse Type " << token);
		return NULL;
	}

	// get element count
	int count;
	if((iss >> count).fail())
	{
		// failed to parse element count
		LOG_ERROR("Shader Uniform","Fail to parse element count " << count);
		return NULL;
	}

	// get transpose parameter if uniform is a matrix
	bool transpose = false;
	if (type==Matrix2f || type==Matrix3f || type==Matrix4f)
	{
		if ((iss >> transpose).fail())
		{
			LOG_ERROR("Shader Uniform","Fail to parse value ");
			return NULL;
		}
	}

	// parse data values
	size_t numValuesExpected = count*_vectorSizesOfDataTypes[type];
	for (size_t i=0; i<numValuesExpected; i++)
	{
		float value;
		if ((iss >> value).fail())
		{
			// failed to parse value
			LOG_ERROR("Shader Uniform","Fail to parse value " << value);
			return NULL;
		}
		floatData.push_back(value);
	}

	if (floatData.size()!=numValuesExpected)
	{
		LOG_ERROR("Shader Uniform","Invalid number of values");
		return NULL;
	}

	ShaderUniform* uniform = new ShaderUniform(name);
	uniform->setData(type, count, floatData, transpose);

	return uniform;
}

ShaderUniform::ShaderUniform(const std::string& name, int location)
	: _name(name)
	, _location(location)
	, _type(Undefined)
	, _count(0)
	, _floatData()
	, _transpose(false)
{
}

ShaderUniform::ShaderUniform(const ShaderUniform& other)
	: _name(other._name)
	, _location(other._location)
	, _type(other._type)
	, _count(other._count)
	, _floatData(other._floatData)
	, _transpose(other._transpose)
{
}

ShaderUniform::~ShaderUniform()
{
}

void ShaderUniform::setData(Type type, int count, const std::vector<float>& values, bool transpose)
{
	size_t numValuesExpected = _vectorSizesOfDataTypes[type] * count;

	if (values.size()==numValuesExpected)
	{
		_floatData = values;
		_type = type;
		_count = count;
		_transpose = transpose;
	}
	else
	{
		// invalid number of floats in vector
	}
}

void ShaderUniform::setData(const ShaderUniform& other)
{
	_type = other._type;
	_count = other._count;
	_floatData = other._floatData;
	_transpose = other._transpose;
}

void ShaderUniform::load(const ShaderProgram& program) const
{
	if (_location==-1)
		return;

	// Get C array from vector. We assume the data is internally stored in consecutive memory locations.
	const float* values = &(*_floatData.begin());

	switch(_type)
	{
		case Vector1f:
			program.uniform1fv(_location, _count, values);
			break;

		case Vector2f:
			program.uniform2fv(_location, _count, values);
			break;

		case Vector3f:
			program.uniform3fv(_location, _count, values);
			break;

		case Vector4f:
			program.uniform4fv(_location, _count, values);
			break;

		case Matrix2f:
			program.uniformMatrix2fv(_location, _count, _transpose, values);
			break;

		case Matrix3f:
			program.uniformMatrix3fv(_location, _count, _transpose, values);
			break;

		case Matrix4f:
			program.uniformMatrix4fv(_location, _count, _transpose, values);
			break;

		default:
			break;
	}
}

