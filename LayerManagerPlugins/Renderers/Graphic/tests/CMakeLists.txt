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

include_directories (".")
include_directories ("../include")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerService/include")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/include")

enable_testing()
find_package (Threads)
set(LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT} LayerManagerUtils)

file(GLOB LM_SOURCES ../../../../LayerManagerService/src/Scene.cpp ../../../../LayerManagerService/src/GraphicalSurface.cpp ../../../../LayerManagerService/src/LogicalGraphicsObject.cpp)

add_executable(ViewportTransformTest ${LM_SOURCES} ../src/Transformation/ViewportTransform.cpp ViewportTransformTest.cpp gmock/gmock_main.cc gmock/gmock-gtest-all.cc)
target_link_libraries(ViewportTransformTest ${LIBS})

add_test(AllTestsInViewportTransformTest ViewportTransformTest)

ADD_CUSTOM_COMMAND(TARGET ViewportTransformTest
    COMMAND ViewportTransformTest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} 
)
