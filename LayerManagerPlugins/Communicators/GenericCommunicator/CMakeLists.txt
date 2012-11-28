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

project(GenericCommunicator)

include_directories(
    "include"
    "${CMAKE_SOURCE_DIR}/LayerManagerClient/ilmClient/include"
    "${CMAKE_SOURCE_DIR}/LayerManagerService/include"
    "${CMAKE_SOURCE_DIR}/LayerManagerUtils/include"
    "${CMAKE_SOURCE_DIR}/LayerManagerCommands/include"
    "${CMAKE_SOURCE_DIR}/LayerManagerPlugins/IpcModules/IpcModuleLoader/include"
)

set(LIBS
    ${LIBS}
    IpcModuleLoader
    LayerManagerCommands
)

set(SRC_FILES
    src/GenericCommunicator.cpp
)


if (WITH_STATIC_LIBRARIES)

    add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
    set (STATICALLY_LINKED_PLUGINS ${STATICALLY_LINKED_PLUGINS}
        ${PROJECT_NAME} CACHE INTERNAL "list of static plugins")

else(WITH_STATIC_LIBRARIES)

    add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

endif(WITH_STATIC_LIBRARIES)

target_link_libraries(${PROJECT_NAME}
    ${LIBS}
)

add_dependencies(${PROJECT_NAME}
    LayerManagerUtils
    LayerManagerCommands
    IpcModuleLoader
)

install (TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION lib/layermanager/communicator
         ARCHIVE DESTINATION lib/layermanager/static
)

if (WITH_TESTS)
    enable_testing()
    add_subdirectory(test)
endif(WITH_TESTS)

