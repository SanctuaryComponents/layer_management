############################################################################
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

include_directories ("include")
include_directories ("../../Base/include")
include_directories ("../../Graphic/include")

include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerService/include")
link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerService/bin")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/include")
link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/bin")

file(GLOB SOURCES 
		src/X11GLXRenderer.cpp 
		../../Base/src/BaseRenderer.cpp 
		../../Graphic/src/GraphicSystems/GLXGraphicSystem.cpp
		../../Graphic/src/WindowSystems/X11WindowSystem.cpp  
		../../Graphic/src/TextureBinders/X11TextureFromPixmap.cpp
		../../Graphic/src/TextureBinders/X11CopyGLX.cpp
		../../Graphic/src/TextureBinders/X11Copy.cpp
		)

add_library(X11GLXRenderer SHARED ${SOURCES})

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

find_package(GLX REQUIRED)
include_directories(${GLX_INCLUDE_DIR})

find_package (Threads)
set(LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT} ${X11_X11_LIB} ${X11_Xcomposite_LIB} ${X11_Xdamage_LIB} ${GLX_LIBRARIES} LayerManagerUtils)
target_link_libraries(X11GLXRenderer ${LIBS})

add_dependencies(X11GLXRenderer LayerManagerService)
add_dependencies(X11GLXRenderer LayerManagerUtils)

install (TARGETS X11GLXRenderer DESTINATION lib/layermanager/renderer)
