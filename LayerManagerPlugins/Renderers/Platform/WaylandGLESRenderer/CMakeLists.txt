############################################################################
# 
# Copyright 2010-2012 BMW Car IT GmbH 
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

#===========================================================================
# plugin configuration
#===========================================================================
project(WaylandGLESRenderer)

find_package(Wayland REQUIRED)
find_package(EGL REQUIRED)
find_package(GLESv2 REQUIRED)
find_package(Threads REQUIRED)

include_directories(
    include
    ../X11GLESRenderer/include
    ../../Base/include
    ../../Graphic/include
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/LayerManagerService/include
    ${CMAKE_SOURCE_DIR}/LayerManagerUtils/include
    ${WAYLAND_SERVER_INCLUDE_DIR}
    ${DRM_INCLUDE_DIR}
    ${EGL_INCLUDE_DIR}
    ${GLESv2_INCLUDE_DIR}
)

set(LIBS
    ${CMAKE_THREAD_LIBS_INIT}
    ${EGL_LIBRARY}
    ${GLESv2_LIBRARIES}
    LayerManagerGraphicWaylandGLESv2
    LayerManagerUtils
)

set(SRC_FILES
    src/WaylandGLESRenderer.cpp
    ../X11GLESRenderer/src/ShaderProgramGLES.cpp
)

set(PLUGIN_INSTALL_PATH lib/layermanager/renderer)

install(
    FILES
        renderer_frag_clear.glslf
        renderer_frag.glslf
        renderer_frag_no_blend.glslf
        renderer_frag_no_ualpha.glslf
        renderer_frag_add_uchromakey.glslf
        renderer_frag_no_blend_no_ualpha.glslf
        renderer_frag_2surf.glslf
        renderer_frag_2surf_no_blend.glslf
        renderer_frag_2surf_no_ualpha.glslf
        renderer_frag_2surf_no_blend_no_ualpha.glslf
        renderer_frag_2surf_no_ualpha_0.glslf
        renderer_frag_2surf_no_blend_no_ualpha_0.glslf
        renderer_frag_2surf_no_ualpha_1.glslf
        renderer_frag_2surf_no_blend_no_ualpha_1.glslf
        renderer_vert.glslv
        renderer_vert_2surf.glslv
    DESTINATION
        lib/layermanager/renderer
)

#===========================================================================
# create statically linked plugin
#===========================================================================
if (WITH_STATIC_LIBRARIES)

    add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

    set (STATICALLY_LINKED_PLUGINS ${STATICALLY_LINKED_PLUGINS}
        ${PROJECT_NAME} CACHE INTERNAL "list of static plugins")

#===========================================================================
# create dynamically linked plugin
#===========================================================================
else(WITH_STATIC_LIBRARIES)

    add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

endif(WITH_STATIC_LIBRARIES)


#===========================================================================
# external libraries
#===========================================================================
target_link_libraries(${PROJECT_NAME} ${LIBS})

add_dependencies(${PROJECT_NAME} ${LIBS})
