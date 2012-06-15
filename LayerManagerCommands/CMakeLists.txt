############################################################################
# 
# Copyright 2010-2012 BMW Car IT GmbH
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
include_directories ("${PROJECT_SOURCE_DIR}/config")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerService/include")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/include")

file(GLOB LM_SOURCES src/*.cpp)
file(GLOB LM_INCLUDES include/*.h)

add_library(LayerManagerCommands SHARED ${LM_SOURCES})

set(LIBS ${LIBS} LayerManagerUtils)
target_link_libraries(LayerManagerCommands ${LIBS})

install (TARGETS LayerManagerCommands DESTINATION lib)
install (FILES ${LM_INCLUDES} DESTINATION include/layermanager)
