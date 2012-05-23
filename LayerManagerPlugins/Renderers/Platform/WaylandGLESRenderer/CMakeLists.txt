############################################################################
# 
# Copyright 2010, 2011 BMW Car IT GmbH 
# Copyright (C) 2011 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
# Copyright (C) 2012 Bayerische Motorenwerke Aktiengesellschaft
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

include_directories ("include")
include_directories ("../X11GLESRenderer/include")
include_directories ("../../Base/include")
include_directories ("../../Graphic/include")
include_directories ("${PROJECT_SOURCE_DIR}/config")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerService/include")
link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerService/bin")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/include")
link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/bin")

file(GLOB SOURCES 
        src/WaylandGLESRenderer.cpp
        ../X11GLESRenderer/src/ShaderProgramGLES.cpp 
        )
        
add_library(WaylandGLESRenderer SHARED ${SOURCES})

find_package(Wayland REQUIRED)
include_directories(${WAYLAND_SERVER_INCLUDE_DIR})
include_directories(${DRM_INCLUDE_DIR})

find_package(EGL REQUIRED)
include_directories(${EGL_INCLUDE_DIR})

find_package(GLESv2 REQUIRED)
include_directories(${GLESv2_INCLUDE_DIR})

find_package (Threads)
set(LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT} ${EGL_LIBRARY} ${GLESv2_LIBRARIES} LayerManagerGraphicWaylandGLESv2 LayerManagerUtils )
target_link_libraries(WaylandGLESRenderer ${LIBS})

add_dependencies(WaylandGLESRenderer LayerManagerService)
add_dependencies(WaylandGLESRenderer LayerManagerUtils)
add_dependencies(WaylandGLESRenderer LayerManagerGraphicWaylandGLESv2)

install (TARGETS WaylandGLESRenderer DESTINATION lib/layermanager/renderer)
install (FILES ../X11GLESRenderer/renderer_frag.glslf DESTINATION lib/layermanager/renderer)
install (FILES ../X11GLESRenderer/renderer_frag_no_ualpha.glslf DESTINATION lib/layermanager/renderer)
install (FILES ../X11GLESRenderer/renderer_layer.glslf DESTINATION lib/layermanager/renderer)
install (FILES ../X11GLESRenderer/renderer_layer.glslv DESTINATION lib/layermanager/renderer)
install (FILES ../X11GLESRenderer/renderer_vert.glslv DESTINATION lib/layermanager/renderer)
