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

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Xlinker -export-dynamic")

include_directories ("include")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/include")
link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/bin")

file(GLOB LM_SOURCES src/*.cpp src/shader/*.cpp)
file(GLOB LM_INCLUDES include/*.h)

add_executable(LayerManagerService ${LM_SOURCES})

add_dependencies(LayerManagerService LayerManagerUtils)


find_package (Threads)
set(LIBS ${LIBS} LayerManagerUtils dl ${CMAKE_THREAD_LIBS_INIT}) 
target_link_libraries(LayerManagerService ${LIBS})

install (TARGETS LayerManagerService DESTINATION bin)
install (FILES ${LM_INCLUDES} DESTINATION include/layermanager)
