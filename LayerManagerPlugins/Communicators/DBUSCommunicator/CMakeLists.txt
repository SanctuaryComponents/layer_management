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
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerService/include")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/include")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerCommands/include")

link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/bin")
link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerCommands")

find_package(DBUS REQUIRED)
include_directories(${DBUS_INCLUDE_DIR})
include_directories(${DBUS_ARCH_INCLUDE_DIR})

find_package (Threads)
set(LIBS ${LIBS} ${DBUS_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

add_library(DBUSCommunicator SHARED src/DBUSCommunicator.cpp
                                    src/DBUSIntrospection.cpp
                                    src/DBUSMessageHandler.cpp)

target_link_libraries(DBUSCommunicator LayerManagerCommands ${LIBS})

add_dependencies(DBUSCommunicator LayerManagerService)
add_dependencies(DBUSCommunicator LayerManagerUtils)
add_dependencies(DBUSCommunicator LayerManagerCommands)

install (TARGETS DBUSCommunicator LIBRARY DESTINATION lib/layermanager/communicator)

if (WITH_TESTS)
enable_testing()
add_subdirectory (test)
endif(WITH_TESTS)