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
		src/X11GLESRenderer.cpp 
		../../Base/src/BaseRenderer.cpp 
		../../Graphic/src/GraphicSystems/GLESGraphicSystem.cpp
		../../Graphic/src/WindowSystems/X11WindowSystem.cpp  
		../../Graphic/src/TextureBinders/X11Copy.cpp
		../../Graphic/src/TextureBinders/X11CopyGLES.cpp
		../../Graphic/src/TextureBinders/X11EglImage.cpp
		src/ShaderProgramGLES.cpp
		)
		
add_library(X11GLESRenderer SHARED ${SOURCES})

find_package(X11 REQUIRED)
include_directories(${X11_Xcomposite_INCLUDE_PATH})

find_package(EGL REQUIRED)
include_directories(${EGL_INCLUDE_DIR})

find_package(GLESv2 REQUIRED)
include_directories(${GLESv2_INCLUDE_DIR})

find_package (Threads)
set(LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT} ${EGL_LIBRARY} ${GLESv2_LIBRARIES} ${X11_Xcomposite_LIB} LayerManagerUtils )
target_link_libraries(X11GLESRenderer ${LIBS})

add_dependencies(X11GLESRenderer LayerManagerService)
add_dependencies(X11GLESRenderer LayerManagerUtils)

install (TARGETS X11GLESRenderer DESTINATION lib/layermanager/renderer)
install (FILES renderer_frag.glslf DESTINATION lib/layermanager/renderer)
install (FILES renderer_frag_no_ualpha.glslf DESTINATION lib/layermanager/renderer)
install (FILES renderer_layer.glslf DESTINATION lib/layermanager/renderer)
install (FILES renderer_layer.glslv DESTINATION lib/layermanager/renderer)
install (FILES renderer_vert.glslv DESTINATION lib/layermanager/renderer)
