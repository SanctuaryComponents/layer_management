############################################################################
# 
# Copyright 2010-2012 BMW Car IT GmbH  
# Copyright (C) 2011 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
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

include(${CMAKE_MODULE_PATH}/CMakeVersions.txt)

#==============================================================================
# set default build type, if not defined by user
#==============================================================================
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
                         "Choose build type: Debug, Release, RelWithDebInfo, MinSizeRel."
                         FORCE)
    message(STATUS "Build type not defined. Using default build type 'Release'.")
endif (NOT CMAKE_BUILD_TYPE)


#==============================================================================
# Platforms / A set of indiviual options at once
#==============================================================================
option (WITH_X11_GLES       "Build for X11 with GLES 2.0"                         ON )
option (WITH_GLESv2_LIB     "Build development library for GLES2 Based Renderers" OFF)
option (WITH_EGL_EXAMPLE    "Build EGL_EXAMPLE / X11"                             ON )
option (WITH_WL_EXAMPLE     "Build EGL_EXAMPLE / Wayland"                         OFF)
option (WITH_DESKTOP        "Build for Desktop / GLX"                             OFF)
option (WITH_GLX_LIB        "Build development library for GLX Based Renderers"   OFF)
option (WITH_GLX_EXAMPLE    "Build GLX_EXAMPLE "                                  OFF)
option (WITH_FORCE_COPY     "Force Software Copy of Pixmaps"                      OFF)
option (WITH_CLIENTEXAMPLES "Build client examples "                              ON )
option (WITH_TESTS          "Build Available Tests "                              OFF)
option (WITH_DLT            "Build with DLT support "                             OFF)
option (WITH_WAYLAND_FBDEV  "Build Wayland with FBDEV backend"                    OFF)
option (WITH_WAYLAND_X11    "Build Wayland with X11 backend"                      OFF)
option (WITH_WAYLAND_DRM    "Build Wayland with drm backend"                      OFF)

option (WITH_SERVICE_BIN    "Build LayerManagerService Binary"                    ON)
option (WITH_CLIENT_LIB     "Build LayerManagement Client Lib"                    ON)
option (WITH_COMMUNICATOR_GEN "Build Generic Communicator Plugin"                 ON)

option (WITH_PLUGIN_SYSTEMD_HEALTH_MONITOR "Build plugin for systemd health monitoring" OFF)

option (WITH_TEXT_RENDERER  "Build renderer renderer (only logging)"              OFF)

option (WITH_PLUGIN_EXAMPLE_SCENE_PROVIDER "Build scene provider plugin for example applciations" OFF)


#==============================================================================
# Internal global settings not available to user
#==============================================================================
set (WITH_WAYLAND    OFF)
set (BUILD_UTILS_LIB  OFF)
set (INSTALL_UTILS_LIB  OFF)


#==============================================================================
# make sure, only one wayland based renderer was selected
#==============================================================================
set(NumberOfWaylandRenderers 0)

if (WITH_WAYLAND_X11)
    set (WITH_GLESv2_LIB ON)
    set (WITH_WAYLAND ON)
    MATH(EXPR NumberOfWaylandRenderers "${NumberOfWaylandRenderers} + 1")
endif(WITH_WAYLAND_X11)

if (WITH_WAYLAND_FBDEV)
    set (WITH_GLESv2_LIB)
    set (WITH_WAYLAND ON)
    MATH(EXPR NumberOfWaylandRenderers "${NumberOfWaylandRenderers} + 1")
endif(WITH_WAYLAND_FBDEV)

if (WITH_WAYLAND_DRM)
    set (WITH_GLESv2_LIB ON)
    set (WITH_WAYLAND ON)
    MATH(EXPR NumberOfWaylandRenderers "${NumberOfWaylandRenderers} + 1")
endif(WITH_WAYLAND_DRM)

if(NumberOfWaylandRenderers GREATER 1) 
    MESSAGE(FATAL_ERROR "More than one Wayland based renderer was selected. This is not supported.")
endif(NumberOfWaylandRenderers GREATER 1)


#==============================================================================
# set default build parameters
#==============================================================================
add_subdirectory(cmake/optionalFeatures)
add_subdirectory(config)

include_directories ("${PROJECT_SOURCE_DIR}/config")
include_directories ("${PROJECT_SOURCE_DIR}/3rdParty/")
# ilm_types are used internally to LM, so we can include them by default
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerClient/ilmClient/include")


#==============================================================================
# default compiler settings for 3rd party components
#==============================================================================

add_subdirectory (3rdParty)


#==============================================================================
# use strict compiler settings for examples
#------------------------------------------------------------------------------
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
#==============================================================================

if (WITH_EGL_EXAMPLE)
    add_subdirectory (LayerManagerExamples/EGLX11ApplicationExample)
    add_subdirectory (LayerManagerExamples/EGLX11MockNavigation)
    set (BUILD_UTILS_LIB ON)
endif(WITH_EGL_EXAMPLE)

if (WITH_GLX_EXAMPLE)
    add_subdirectory (LayerManagerExamples/GLXApplicationExample)
    set (BUILD_UTILS_LIB ON)
endif(WITH_GLX_EXAMPLE)

if (WITH_CLIENTEXAMPLES)
    add_subdirectory (LayerManagerExamples/LayerManagerClientExample)
    add_subdirectory (LayerManagerExamples/LayerManagerControl)
    set (BUILD_UTILS_LIB ON)
endif(WITH_CLIENTEXAMPLES)

if (WITH_WL_EXAMPLE)
    add_subdirectory (LayerManagerExamples/EGLWLApplicationExample)
    add_subdirectory (LayerManagerExamples/EGLWLMockNavigation)
    add_subdirectory (LayerManagerExamples/WLChromakeyExample)
    add_subdirectory (LayerManagerExamples/EGLWLInputEventExample)
    set (BUILD_UTILS_LIB ON)
endif(WITH_WL_EXAMPLE)

#==============================================================================
# use very strict compiler settings for internal components
#------------------------------------------------------------------------------
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wno-long-long")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
#==============================================================================

if (WITH_PLUGIN_EXAMPLE_SCENE_PROVIDER)
    add_subdirectory (LayerManagerPlugins/SceneProvider/ExampleSceneProvider)
endif(WITH_PLUGIN_EXAMPLE_SCENE_PROVIDER)

if (WITH_PLUGIN_SYSTEMD_HEALTH_MONITOR)
    add_subdirectory (LayerManagerPlugins/HealthMonitor/SystemdHealthMonitor)
endif(WITH_PLUGIN_SYSTEMD_HEALTH_MONITOR)

if (WITH_COMMUNICATOR_GEN)
    add_subdirectory (LayerManagerCommands)
    add_subdirectory (LayerManagerPlugins/Communicators/GenericCommunicator)
endif(WITH_COMMUNICATOR_GEN)

if (WITH_DESKTOP)
    set (WITH_GLX_LIB ON CACHE BOOL "Build X11 GLX development lib" FORCE)
    add_subdirectory (LayerManagerPlugins/Renderers/Platform/GLXRenderer)
endif(WITH_DESKTOP)     

if (WITH_WAYLAND)
    if (WITH_WAYLAND_VIVANTE_GAL)
        add_subdirectory (LayerManagerPlugins/Renderers/Platform/WaylandVivanteRenderer)
    else (WITH_WAYLAND_VIVANTE_GAL)
        add_subdirectory (LayerManagerPlugins/Renderers/Platform/WaylandGLESRenderer)
    endif (WITH_WAYLAND_VIVANTE_GAL)
endif(WITH_WAYLAND)

if (WITH_X11_GLES)
    set (WITH_GLESv2_LIB ON CACHE BOOL "Build X11 GLES development lib" FORCE)
    add_subdirectory (LayerManagerPlugins/Renderers/Platform/X11GLESRenderer)
endif(WITH_X11_GLES)

if (WITH_TEXT_RENDERER)
    add_subdirectory (LayerManagerPlugins/Renderers/Platform/TextRenderer)
endif(WITH_TEXT_RENDERER)

if (WITH_SERVICE_BIN OR WITH_CLIENT_LIB)
    add_subdirectory (LayerManagerPlugins/IpcModules)

    if (WITH_SERVICE_BIN)
        add_subdirectory (LayerManagerPlugins/Renderers/Graphic)
        add_subdirectory (LayerManagerService)
        set (BUILD_UTILS_LIB ON)
        set (INSTALL_UTILS_LIB ON)
    endif (WITH_SERVICE_BIN)

    if (WITH_CLIENT_LIB)
        add_subdirectory (LayerManagerClient/ilmClient)
    endif (WITH_CLIENT_LIB)

endif (WITH_SERVICE_BIN OR WITH_CLIENT_LIB)

#==============================================================================
# build internal libraries, if required by other components
#==============================================================================
if (BUILD_UTILS_LIB)
    add_subdirectory (LayerManagerUtils)
endif (BUILD_UTILS_LIB)

#==============================================================================
# enable ctest if building with tests
# run 'make test' after build to run all tests
# be aware that some tests require a running layermanager
#==============================================================================
if (WITH_TESTS)
    enable_testing()
endif(WITH_TESTS)
