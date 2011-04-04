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

add_library(ilmClient SHARED src/ilm_client_priv.c src/ilm_client.c)

find_package(DBUS REQUIRED)
include_directories(${DBUS_INCLUDE_DIR})
include_directories(${DBUS_ARCH_INCLUDE_DIR})

set(LIBS ${LIBS} ${DBUS_LIBRARY})
target_link_libraries(ilmClient ${LIBS})

install (TARGETS ilmClient LIBRARY DESTINATION lib)
install (FILES include/ilm_client.h include/ilm_types.h include/ilm_client_platform.h DESTINATION include/ilm)



	
if (WITH_TESTS)
enable_testing()

include_directories(tests)

add_executable(ilmClient_Test tests/IlmCommandTest.cpp tests/gtest/gtest_main.cc tests/gtest/gtest-all.cc )
target_link_libraries(ilmClient_Test ilmClient ${LIBS} ${DBUS_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

add_test(AllTestsInTest ilmClient_Test)

endif(WITH_TESTS) 