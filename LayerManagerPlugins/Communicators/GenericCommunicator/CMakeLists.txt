############################################################################
# 
# Copyright 2012 BMW Car IT GmbH 
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

find_package(Threads)

include_directories(
    "include"
    "${CMAKE_SOURCE_DIR}/LayerManagerClient/ilmClient/include"
    "${PROJECT_SOURCE_DIR}/LayerManagerService/include"
    "${PROJECT_SOURCE_DIR}/LayerManagerUtils/include"
    "${PROJECT_SOURCE_DIR}/LayerManagerCommands/include"
    "${PROJECT_SOURCE_DIR}/LayerManagerPlugins/Communicators/GenericCommunicatorClient/include"
    "${PROJECT_SOURCE_DIR}/LayerManagerPlugins/IpcModules/IpcModuleLoader/include"
    "${PROJECT_SOURCE_DIR}/LayerManagerPlugins/IpcModules/DBUSIpcModule/include"
)

set(LIBS
    ${LIBS}
    IpcModuleLoader
    LayerManagerCommands
    ${CMAKE_THREAD_LIBS_INIT}
)

add_library(GenericCommunicator SHARED
    src/GenericCommunicator.cpp
)

target_link_libraries(GenericCommunicator
    ${LIBS}
)

add_dependencies(GenericCommunicator
    LayerManagerService
    LayerManagerUtils
    LayerManagerCommands
    IpcModuleLoader
)

install (TARGETS GenericCommunicator LIBRARY DESTINATION lib/layermanager/communicator)

if (WITH_TESTS)
    enable_testing()
    add_subdirectory(test)
endif(WITH_TESTS)

