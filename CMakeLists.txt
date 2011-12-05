############################################################################
# 
# Copyright 2010, 2011 BMW Car IT GmbH  
# 
# 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#
#       http://www.apache.org/licenses/LICENSE-2.0 
#
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
# See the License for the specific language governing permissions and 
# limitations under the License.
#
############################################################################

cmake_minimum_required (VERSION 2.6)
project (LayerManager)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 9 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 4 )

SET(VERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL})
if(NOT DEFINED ILM_VERSION)
execute_process(COMMAND git describe --tags WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                OUTPUT_VARIABLE ILM_VERSION 
                OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "-" "_" ILM_VERSION ${ILM_VERSION} )
endif(NOT DEFINED ILM_VERSION)
                
set(LICENSE "APACHE 2.0")

set(GENIVI_PROJECT_VERSION ${VERSION})
message(STATUS
        "Build for Version ${VERSION} build ${ILM_VERSION}"
        )

# set default build type, if not defined by user
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
                         "Choose build type: Debug, Release, RelWithDebInfo, MinSizeRel."
                         FORCE)
    message(STATUS "Build type not defined. Using default build type 'Release'.")
endif (NOT CMAKE_BUILD_TYPE)


##################### RPM CONFIG ########################
set(GENIVI_RPM_RELEASE "1${ILM_VERSION}")
set(SPEC_DIR ".")
#########################################################

# Platforms / A set of indiviual options at once

option (WITH_X11_GLES       "Build for X11 with GLES 2.0"                         ON )
option (WITH_GLESv2_LIB     "Build development library for GLES2 Based Renderers" OFF)
option (WITH_EGL_EXAMPLE    "Build EGL_EXAMPLE "                                  ON )
option (WITH_DESKTOP        "Build for Desktop / GLX"                             OFF)
option (WITH_GLX_LIB        "Build development library for GLX Based Renderers"   OFF)
option (WITH_GLX_EXAMPLE    "Build GLX_EXAMPLE "                                  OFF)
option (WITH_INPUT_EVENTS   "Activate input events handling"                      OFF)
option (WITH_FORCE_COPY     "Force Software Copy of Pixmaps"                      OFF)
option (WITH_XTHREADS       "Build with usage of X11 Threading"                   ON )
option (WITH_CLIENTEXAMPLES "Build client examples "                              ON )
option (WITH_TESTS          "Build Available Tests "                              OFF)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

if (WITH_DESKTOP)
    set (WITH_GLX_LIB ON CACHE STRING "" FORCE)
endif(WITH_DESKTOP)     

if (WITH_X11_GLES)
    set (WITH_GLESv2_LIB ON CACHE STRING "" FORCE)
endif(WITH_X11_GLES)

include_directories ("${PROJECT_SOURCE_DIR}/3rdParty/")

add_subdirectory (3rdParty)
add_subdirectory (LayerManagerPlugins/Communicators/DBUSCommunicator)
add_subdirectory (LayerManagerClient/ilmClient)
add_subdirectory (LayerManagerUtils)
add_subdirectory (LayerManagerCommands)
add_subdirectory (LayerManagerService)

if (WITH_GLESv2_LIB OR WITH_GLX_LIB)
    add_subdirectory (LayerManagerPlugins/Renderers/Graphic)
endif(WITH_GLESv2_LIB OR WITH_GLX_LIB)

if (WITH_DESKTOP)
    add_subdirectory (LayerManagerPlugins/Renderers/Platform/GLXRenderer)
endif(WITH_DESKTOP)     

if (WITH_X11_GLES)
    add_subdirectory (LayerManagerPlugins/Renderers/Platform/X11GLESRenderer)
endif(WITH_X11_GLES)   

if (WITH_EGL_EXAMPLE)
    add_subdirectory (LayerManagerExamples/EGLX11ApplicationExample)
    add_subdirectory (LayerManagerExamples/EGLX11MockNavigation)
endif(WITH_EGL_EXAMPLE)

if (WITH_GLX_EXAMPLE)
    add_subdirectory (LayerManagerExamples/GLXApplicationExample)
endif(WITH_GLX_EXAMPLE)

if (WITH_CLIENTEXAMPLES)
    add_subdirectory (LayerManagerExamples/LayerManagerClientExample)
    add_subdirectory (LayerManagerExamples/LayerManagerToolBox)
    add_subdirectory (LayerManagerExamples/LayerSceneDescriptionExample)
endif(WITH_CLIENTEXAMPLES)

# enable ctest if building with tests
# run 'make test' after build to run all tests
# be aware that some tests require a running layermanager
if (WITH_TESTS)
    enable_testing()
endif(WITH_TESTS)


