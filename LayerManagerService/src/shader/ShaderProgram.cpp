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

#include <ShaderProgram.h>
#include <ShaderProgramFactory.h>

// global program list
std::list<ShaderProgram*> ShaderProgram::_programList;

ShaderProgram* ShaderProgram::obtain(const std::string& vertName, const std::string& fragName)
{
	std::list<ShaderProgram*>::iterator it;
	ShaderProgram* program = 0;

	// search for a matching shader program in the global list
	for (it=_programList.begin(); it!=_programList.end(); ++it)
	{
		ShaderProgram* p = *it;
		if (p && p->getVertexName()==vertName && p->getFragmentName()==fragName)
		{
			program = p;
			break;
		}
	}

	if (!program)
	{
		// no matching program object found in global list -> create a new one

		// we are using a factory approach to create platform specific shader programs:
		program = ShaderProgramFactory::createProgram(vertName, fragName);

		if (program)
		{
			// add program to global list
			_programList.push_back(program);
		}
	}

	return program;
}

ShaderProgram::ShaderProgram(const std::string& vertName, const std::string& fragName)
	: _refCounter(0)
	, _vertName(vertName)
	, _fragName(fragName)
{
	// void
}

ShaderProgram::~ShaderProgram()
{
	// void
}

void ShaderProgram::ref(void)
{
	_refCounter++;
}

void ShaderProgram::unref(void)
{
	_refCounter--;
	if (_refCounter<=0)
	{
		// remove program from global list
		_programList.remove(this);

		// destroy object
		delete this;
	}
}

void ShaderProgram::loadCommonUniforms(const CommonUniforms& uniforms) const
{
	if (_xLoc>=0)
		uniform1fv(_xLoc, 1, &uniforms.x);
	if (_yLoc>=0)
		uniform1fv(_yLoc, 1, &uniforms.y);
	if (_widthLoc>=0)
		uniform1fv(_widthLoc, 1, &uniforms.width);
	if (_heightLoc>=0)
		uniform1fv(_heightLoc, 1, &uniforms.height);
	if (_opacityLoc>=0)
		uniform1fv(_opacityLoc, 1, &uniforms.opacity);
	if (_texRangeLoc>=0)
		uniform2fv(_texRangeLoc, 1, uniforms.texRange);
	if (_texOffsetLoc>=0)
		uniform2fv(_texOffsetLoc, 1, uniforms.texOffset);
	if (_texUnitLoc>=0)
		uniform1iv(_texUnitLoc, 1, &uniforms.texUnit);
       if (_matrixLoc>=0)
                uniformMatrix4fv(_matrixLoc, 1,false, uniforms.matrix);

}

void ShaderProgram::updateCommonUniformLocations(void)
{
	// get uniform locations for common surface properties
	_xLoc = getUniformLocation("uX");
	_yLoc = getUniformLocation("uY");
	_widthLoc = getUniformLocation("uWidth");
	_heightLoc = getUniformLocation("uHeight");
	_opacityLoc = getUniformLocation("uAlphaVal");
	_texRangeLoc = getUniformLocation("uTexRange");
	_texOffsetLoc = getUniformLocation("uTexOffset");
	_texUnitLoc = getUniformLocation("uTexUnit");
	_matrixLoc = getUniformLocation("uMatrix");
}

