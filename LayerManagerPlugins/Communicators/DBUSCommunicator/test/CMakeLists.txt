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


include_directories (".")
include_directories ("../include")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerService/include")
#link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerService/bin")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/include")
#link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/bin")
link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerCommands")

find_package(DBUS REQUIRED)
include_directories(${DBUS_INCLUDE_DIR})
include_directories(${DBUS_ARCH_INCLUDE_DIR})

find_package (Threads)
set(LIBS ${LIBS} ${DBUS_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} LayerManagerUtils)

file(GLOB LM_SOURCES "${PROJECT_SOURCE_DIR}/LayerManagerService/src/Scene.cpp"
                     "${PROJECT_SOURCE_DIR}/LayerManagerService/src/Layermanager.cpp"
                     "${PROJECT_SOURCE_DIR}/LayerManagerService/src/LogicalGraphicsObject.cpp"
                     "${PROJECT_SOURCE_DIR}/LayerManagerService/src/GraphicalSurface.cpp"
                     "${PROJECT_SOURCE_DIR}/LayerManagerService/src/shader/*.cpp"
                     "${PROJECT_SOURCE_DIR}/LayerManagerService/src/commands/*.cpp")

file(GLOB DBUSCommunicator_SOURCES "../src/*.cpp")

add_executable(DBUSCommunicatorTest DBUSCommunicatorTest.cpp ${LM_SOURCES} ${DBUSCommunicator_SOURCES})
target_link_libraries(DBUSCommunicatorTest gmock LayerManagerCommands ${LIBS})

add_test(DBUSCommunicatorTest DBUSCommunicatorTest)
