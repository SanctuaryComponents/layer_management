/***************************************************************************
 *
 * Copyright 2010,2011 BMW Car IT GmbH
 * Copyright (C) 2011 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
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
#ifndef __CONFIG_H__
#define __CONFIG_H__

// Auto-generated. Do not modify.
// Variables configured by CMake build system

//-----------------------------------------------------------------------------
// string variables
//-----------------------------------------------------------------------------
// version of the LayerManagementService
#define ILM_VERSION          "${ILM_VERSION}"

// CMake build type, e.g. Debug, Release
#define CMAKE_BUILD_TYPE     "${CMAKE_BUILD_TYPE}"

// compiler flags used to build project
#define CMAKE_CXX_FLAGS      "${CMAKE_CXX_FLAGS}"

// install prefix of target platform
#define CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}"


//-----------------------------------------------------------------------------
// build flags
//-----------------------------------------------------------------------------
// build client examples
#cmakedefine WITH_CLIENTEXAMPLES

// build OpenGL/X11 based renderer
#cmakedefine WITH_DESKTOP

// build OpenGL ES 2.0 examples
#cmakedefine WITH_EGL_EXAMPLE

// build OpenGL ES 2.0 Wayland Examples
#cmakedefine WITH_WL_EXAMPLE

// force copy of graphic buffers
#cmakedefine WITH_FORCE_COPY

// build OpenGL/X11 examples
#cmakedefine WITH_GLX_EXAMPLE

// build unit tests for all projects
#cmakedefine WITH_TESTS

// build OpenGL ES 2.0 / X11 renderer
#cmakedefine WITH_X11_GLES

// use fbdev for wayland backend
#cmakedefine WITH_WAYLAND_FBDEV

// use x11 for wayland backend
#cmakedefine WITH_WAYLAND_X11

// use wayland-drm compositor
#cmakedefine WITH_WAYLAND_DRM

// use automotive dlt for logging
#cmakedefine WITH_DLT

// build LayerManagement client library
#cmakedefine WITH_CLIENT_LIB

// build LayerManagerService binary
#cmakedefine WITH_SERVICE_BIN

// build generic communicator plugin
#cmakedefine WITH_COMMUNICATOR_GEN

// build dbus ipc module
#cmakedefine WITH_IPC_MODULE_DBUS

// build tcp/ip ipc module
#cmakedefine WITH_IPC_MODULE_TCP

// build plugin for systemd health monitoring
#cmakedefine WITH_PLUGIN_SYSTEMD_HEALTH_MONITOR

// full static build enabled
#cmakedefine WITH_STATIC_LIBRARIES


//-----------------------------------------------------------------------------
// platform configuration
//-----------------------------------------------------------------------------
// execinfo header file
#cmakedefine HAVE_EXECINFO_H

// backtrace available
#cmakedefine HAVE_BACKTRACE


//-----------------------------------------------------------------------------
// human readable report
//-----------------------------------------------------------------------------
#define DEBUG_FLAG 1
#define INFO_FLAG  2

typedef struct
{
    int type;
    const char* description;
} BuildFlag;

const BuildFlag gBuildFlags[] =
{
    { INFO_FLAG,  "Build Version         = ${ILM_VERSION}" },
    { DEBUG_FLAG, "Build Type            = ${CMAKE_BUILD_TYPE}" },
    { DEBUG_FLAG, "Compiler Flags        = ${CMAKE_CXX_FLAGS}" },
    { DEBUG_FLAG, "Install Prefix        = ${CMAKE_INSTALL_PREFIX}" },
    { DEBUG_FLAG, "WITH_CLIENTEXAMPLES   = ${WITH_CLIENTEXAMPLES}" },
    { DEBUG_FLAG, "WITH_CLIENT_LIB       = ${WITH_CLIENT_LIB}" },
    { DEBUG_FLAG, "WITH_COMMUNICATOR_GEN = ${WITH_COMMUNICATOR_GEN}" },
    { DEBUG_FLAG, "WITH_DESKTOP          = ${WITH_DESKTOP}" },
    { DEBUG_FLAG, "WITH_DLT              = ${WITH_DLT}" },
    { DEBUG_FLAG, "WITH_EGL_EXAMPLE      = ${WITH_EGL_EXAMPLE}" },
    { DEBUG_FLAG, "WITH_FORCE_COPY       = ${WITH_FORCE_COPY}" },
    { DEBUG_FLAG, "WITH_GLX_EXAMPLE      = ${WITH_GLX_EXAMPLE}" },
    { DEBUG_FLAG, "WITH_IPC_MODULE_DBUS  = ${WITH_IPC_MODULE_DBUS}" },
    { DEBUG_FLAG, "WITH_IPC_MODULE_TCP   = ${WITH_IPC_MODULE_TCP}" },
    { DEBUG_FLAG, "WITH_SERVICE_BIN      = ${WITH_SERVICE_BIN}" },
    { DEBUG_FLAG, "WITH_TESTS            = ${WITH_TESTS}" },
    { DEBUG_FLAG, "WITH_WAYLAND_DRM      = ${WITH_WAYLAND_DRM}" },
    { DEBUG_FLAG, "WITH_WAYLAND_FBDEV    = ${WITH_WAYLAND_FBDEV}" },
    { DEBUG_FLAG, "WITH_WAYLAND_X11      = ${WITH_WAYLAND_X11}" },
    { DEBUG_FLAG, "WITH_WL_EXAMPLE       = ${WITH_WL_EXAMPLE}" },
    { DEBUG_FLAG, "WITH_X11_GLES         = ${WITH_X11_GLES}" },
    { DEBUG_FLAG, "WITH_SYSTEMD_HEALTH_MONITOR = ${WITH_SYSTEMD_HEALTH_MONITOR}" },
    { DEBUG_FLAG, "WITH_EXAMPLE_SCENE_PROVIDER = ${WITH_EXAMPLE_SCENE_PROVIDER}" },
    { DEBUG_FLAG, "WITH_STATIC_LIBRARIES = ${WITH_STATIC_LIBRARIES}" }
};

const int gBuildFlagCount = sizeof(gBuildFlags) / sizeof(gBuildFlags[0]);


//-----------------------------------------------------------------------------
// manage list of statically linked plugins
//-----------------------------------------------------------------------------
#define REGISTER_PLUGIN(PLUGIN) \
    extern "C" IPlugin* create ## PLUGIN(ICommandExecutor& executor, Configuration& config); \
    static bool PLUGIN ## _instance = PluginManager::registerStaticPluginCreateFunction(create ## PLUGIN);

#define STATIC_PLUGIN_REGISTRATION ${STATIC_PLUGIN_REGISTRATION}

//-----------------------------------------------------------------------------
// define plugin entry point depending on build settings
//-----------------------------------------------------------------------------
#define DECLARE_LAYERMANAGEMENT_PLUGIN(name) \
extern "C" IPlugin* create ## name(ICommandExecutor& executor, Configuration& config) \
{ \
    return new name(executor, config); \
}

#endif // __CONFIG_H__
