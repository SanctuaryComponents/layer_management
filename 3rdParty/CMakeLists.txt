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

# set default compiler flags for 3rdParty code
set (CMAKE_C_FLAGS "")
set (CMAKE_CXX_FLAGS "")

# include base path to 3rdParty code
include_directories (".")

# build systemd reference client library
if (WITH_SYSTEMD_HEALTH_MONITOR)
    add_library(systemd systemd/sd-daemon.c)
endif (WITH_SYSTEMD_HEALTH_MONITOR)

# build unit test frameworks, if tests are enabled
if (WITH_TESTS)
    add_library(gtest gtest/gtest-all.cc gtest/gtest_main.cc)
    add_library(gmock gmock/gmock-gtest-all.cc gmock/gmock_main.cc)
endif(WITH_TESTS)
