############################################################################
# 
# Copyright 2010-2012 BMW Car IT GmbH
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

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xlinker -export-dynamic")
if (WITH_DLT)
find_package(AutomotiveDlt REQUIRED)
include_directories(${DLT_INCLUDE_DIR})
endif (WITH_DLT)

include_directories ("include")
include_directories ("${CMAKE_SOURCE_DIR}/config")
include_directories ("${CMAKE_SOURCE_DIR}/LayerManagerUtils/include")
include_directories ("${CMAKE_SOURCE_DIR}/LayerManagerCommands/include")

file(GLOB LM_SOURCES src/*.cpp src/shader/*.cpp)
file(GLOB LM_INCLUDES include/*.h)

add_executable(LayerManagerService ${LM_SOURCES} ${LM_INCLUDES} ${CMAKE_SOURCE_DIR}/config/config.h)

add_dependencies(LayerManagerService LayerManagerUtils)
add_dependencies(LayerManagerService LayerManagerCommands)


find_package (Threads)
set(LIBS ${LIBS}
    LayerManagerUtils 
    LayerManagerCommands
    dl
    ${CMAKE_THREAD_LIBS_INIT}
    ${DLT_LIBRARY}
    ${STATICALLY_LINKED_PLUGINS}
) 

target_link_libraries(LayerManagerService ${LIBS})

install (TARGETS LayerManagerService DESTINATION bin)
install (FILES ${LM_INCLUDES} DESTINATION include/layermanager)


if (WITH_TESTS)
enable_testing()
set (CMAKE_CXX_FLAGS "")
add_executable(LayerManagerService_Test
    tests/SceneTest.cpp
    tests/ScreenTest.cpp
    tests/LayermanagerTest.cpp
    tests/InputManagerTest.cpp
    tests/GraphicalObjectTest.cpp
    tests/GraphicalGroupTest.cpp
    tests/GraphicalSurfaceTest.cpp
    tests/SurfaceTest.cpp
    tests/LayerTest.cpp
    tests/ShaderTest.cpp
    tests/ShaderUniformTest.cpp
    tests/ShaderProgramTest.cpp
    tests/ShaderProgramFactoryTest.cpp
    tests/RectangleTest.cpp
    
    src/GraphicalObject.cpp
    src/GraphicalSurface.cpp
    src/Layermanager.cpp
    src/InputManager.cpp
    src/PluginManager.cpp
    src/PluginBase.cpp
    src/PluginManager.cpp
    src/Scene.cpp
    src/shader/Shader.cpp
    src/shader/ShaderProgram.cpp
    src/shader/ShaderProgramFactory.cpp
    src/shader/ShaderUniform.cpp
    src/Configuration.cpp
)
target_link_libraries(LayerManagerService_Test ${LIBS} gtest gmock)

add_test(LayerManagerService LayerManagerService_Test )

endif(WITH_TESTS) 


#=============================================================
# configuration depending on system
#=============================================================
include(CheckFunctionExists)
check_function_exists(backtrace_symbols HAVE_BACKTRACE)

include(CheckIncludeFiles)
check_include_files(execinfo.h HAVE_EXECINFO_H)
