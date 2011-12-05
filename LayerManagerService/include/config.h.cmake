/***************************************************************************
 *
 * Copyright 2010,2011 BMW Car IT GmbH
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/
#include "Log.h"

// Auto-generated. Do not modify.
// Variables configured by CMake build system

//-----------------------------------------------------------------------------
// string variables
//-----------------------------------------------------------------------------
// version of the LayerManagementService
const char* ILM_VERSION          = "${ILM_VERSION}";

// CMake build type, e.g. Debug, Release
const char* CMAKE_BUILD_TYPE     = "${CMAKE_BUILD_TYPE}";

// compiler flags used to build project
const char* CMAKE_CXX_FLAGS      = "${CMAKE_CXX_FLAGS}";

// install prefix of target platform
const char* CMAKE_INSTALL_PREFIX = "${CMAKE_INSTALL_PREFIX}";


//-----------------------------------------------------------------------------
// build flags
//-----------------------------------------------------------------------------
// build client examples
#cmakedefine WITH_CLIENTEXAMPLES

// build OpenGL/X11 based renderer
#cmakedefine WITH_DESKTOP

// build OpenGL ES 2.0 examples
#cmakedefine WITH_EGL_EXAMPLE

// force copy of graphic buffers
#cmakedefine WITH_FORCE_COPY

// Build development library for GLES2 Based Renderers
#cmakedefine WITH_GLESv2_LIB

// build OpenGL/X11 examples
#cmakedefine WITH_GLX_EXAMPLE

// Build development library for GLX Based Renderers
#cmakedefine WITH_GLX_LIB

// enable input event handling
#cmakedefine WITH_INPUT_EVENTS

// build unit tests for all projects
#cmakedefine WITH_TESTS

// build OpenGL ES 2.0/X11 renderer
#cmakedefine WITH_X11_GLES

// use xthreads library
#cmakedefine WITH_XTHREADS


//-----------------------------------------------------------------------------
// human readable report
//-----------------------------------------------------------------------------
void printConfiguration(void)
{
    LOG_INFO ("Config", "Build Version       = " << ILM_VERSION);
    LOG_DEBUG("Config", "Build Type          = " << CMAKE_BUILD_TYPE);
    LOG_DEBUG("Config", "Compiler Flags      = " << CMAKE_CXX_FLAGS);
    LOG_DEBUG("Config", "Install Prefix      = " << CMAKE_INSTALL_PREFIX);

    LOG_DEBUG("Config", "WITH_CLIENTEXAMPLES = ${WITH_CLIENTEXAMPLES}");
    LOG_DEBUG("Config", "WITH_DESKTOP        = ${WITH_DESKTOP}");
    LOG_DEBUG("Config", "WITH_EGL_EXAMPLE    = ${WITH_EGL_EXAMPLE}");
    LOG_DEBUG("Config", "WITH_FORCE_COPY     = ${WITH_FORCE_COPY}");
    LOG_DEBUG("Config", "WITH_GLESv2_LIB     = ${WITH_GLESv2_LIB}");
    LOG_DEBUG("Config", "WITH_GLX_EXAMPLE    = ${WITH_GLX_EXAMPLE}");
    LOG_DEBUG("Config", "WITH_GLX_LIB        = ${WITH_GLX_LIB}");
    LOG_DEBUG("Config", "WITH_INPUT_EVENTS   = ${WITH_INPUT_EVENTS}");
    LOG_DEBUG("Config", "WITH_TESTS          = ${WITH_TESTS}");
    LOG_DEBUG("Config", "WITH_X11_GLES       = ${WITH_X11_GLES}");
    LOG_DEBUG("Config", "WITH_XTHREADS       = ${WITH_XTHREADS}");
}

