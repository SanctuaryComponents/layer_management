############################################################################
# 
# Copyright 2010, 2011 BMW Car IT GmbH
# Copyright (C) 2011 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
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

FIND_PATH(WAYLAND_CLIENT_INCLUDE_DIR /wayland-client.h
/usr/include /usr/local/include
)

FIND_LIBRARY(WAYLAND_CLIENT_LIBRARIES
NAMES wayland-client
PATHS /usr/lib /usr/local/lib
)

FIND_PATH(WAYLAND_SERVER_INCLUDE_DIR /wayland-server.h
/usr/include /usr/local/include
)

FIND_LIBRARY(WAYLAND_SERVER_LIBRARIES
NAMES wayland-server
PATHS /usr/lib /usr/local/lib
)

FIND_PATH(FFI_INCLUDE_DIR /ffi.h
/usr/include /usr/local/include /usr/include/i386-linux-gnu
)

FIND_LIBRARY(FFI_LIBRARIES
NAMES ffi
PATHS /usr/lib /usr/local/lib
)

SET( WAYLAND_FOUND "NO" )
IF(WAYLAND_LIBRARIES)
    SET( WAYLAND_FOUND "YES" )
    message(STATUS "Found Wayland-Client libs: ${WAYLAND_CLIENT_LIBRARIES}")
    message(STATUS "Found Wayland-Client includes: ${WAYLAND_CLIENT_INCLUDE_DIR}")
    message(STATUS "Found Wayland-Server libs: ${WAYLAND_SERVER_LIBRARIES}")
    message(STATUS "Found Wayland-Server includes: ${WAYLAND_SERVER_INCLUDE_DIR}")
    message(STATUS "Found ffi need by Wayland libs: ${FFI_LIBRARIES}")
    message(STATUS "Found ffi need by Wayland includes: ${FFI_INCLUDE_DIR}")
ENDIF(WAYLAND_LIBRARIES)

MARK_AS_ADVANCED(
  WAYLAND_CLIENT_INCLUDE_DIR
  WAYLAND_CLIENT_LIBRARIES
  WAYLAND_SERVER_INCLUDE_DIR
  WAYLAND_SERVER_LIBRARIES
  FFI_INCLUDE_DIR
  FFI_LIBRARIES
)
