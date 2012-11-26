############################################################################
# 
# Copyright 2010, 2011 BMW Car IT GmbH
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

project(ExampleSceneProvider)

include_directories(
    include
    ${CMAKE_SOURCE_DIR}/LayerManagerCommands/include
    ${CMAKE_SOURCE_DIR}/LayerManagerService/include
)

add_library(${PROJECT_NAME} SHARED
    src/ExampleSceneProvider.cpp
)

target_link_libraries(${PROJECT_NAME} LayerManagerCommands ${LIBS})

add_dependencies(${PROJECT_NAME} LayerManagerCommands)

install (TARGETS ${PROJECT_NAME} DESTINATION lib/layermanager/sceneprovider)
