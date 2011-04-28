###########################################################################
# 
# Copyright 2010, 2011 BMW Car IT GmbH  
# 
# 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#
#		http://www.apache.org/licenses/LICENSE-2.0 
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
SET( ${PROJECT_NAME}_MINOR_VERSION 3 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )
set(VERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL})

set(LICENSE "APACHE 2.0")

set(GENIVI_PROJECT_VERSION ${VERSION})


##################### RPM CONFIG ########################
set(GENIVI_RPM_RELEASE "1${SVN_REVISION}")
set(SPEC_DIR ".")
#########################################################


# Platforms / A set of indiviual options at once
# Choose only ONE of these!!

option (WITH_X11_GLES 
        "Build for X11 with GLES 2.0" ON)   
               
option (WITH_DESKTOP 
        "Build for Desktop / GLX" OFF)

option (WITH_EGL_EXAMPLE 
        "Build EGL_EXAMPLE " ON)

option (WITH_GLX_EXAMPLE 
        "Build GLX_EXAMPLE " OFF)
        
option (WITH_TESTS
        "Build Available Tests " OFF)

option (WITH_FORCE_COPY 
	"Force Software Copy from X11 pixmaps" OFF)

if (WITH_FORCE_COPY)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGLX_GRAPHICSYSTEM_FORCE_COPY -DGLES_FORCE_COPY" CACHE STRING "" FORCE)
endif(WITH_FORCE_COPY)

add_subdirectory (LayerManagerPlugins/Communicators/DBUSCommunicator)
add_subdirectory (LayerManagerClient/ilmClient)
add_subdirectory (LayerManagerUtils)
add_subdirectory (LayerManagerService)
add_subdirectory (LayerManagerExamples/LayerManagerClientExample)

	
if (WITH_DESKTOP)
	add_subdirectory (LayerManagerPlugins/Renderers/Platform/GLXRenderer)
endif(WITH_DESKTOP)     

if (WITH_X11_GLES)
  	add_subdirectory (LayerManagerPlugins/Renderers/Platform/X11GLESRenderer)
endif(WITH_X11_GLES)   

if (WITH_EGL_EXAMPLE)
  	add_subdirectory (LayerManagerExamples/EGLX11ApplicationExample)
endif(WITH_EGL_EXAMPLE)

if (WITH_GLX_EXAMPLE)
  	add_subdirectory (LayerManagerExamples/GLXApplicationExample)
endif(WITH_GLX_EXAMPLE)


