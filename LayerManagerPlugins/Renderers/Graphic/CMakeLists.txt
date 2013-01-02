############################################################################
# 
# Copyright 2010-2012 BMW Car IT GmbH 
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

cmake_minimum_required (VERSION 2.6)

#===========================================================================
# Common
#===========================================================================

include_directories(
    "include"
    "../Base/include"
    "${PROJECT_SOURCE_DIR}/config"
    "${PROJECT_SOURCE_DIR}/LayerManagerBase/include"
    "${PROJECT_SOURCE_DIR}/LayerManagerUtils/include"
)

#===========================================================================
if (WITH_WAYLAND) 
#===========================================================================

    find_package(Wayland)

    set(LIBS
            ${LIBS}
            ${WAYLAND_SERVER_LIBRARIES}
            ${WAYLAND_CLIENT_LIBRARIES}
    )
    
    include_directories(${WAYLAND_SERVER_INCLUDE_DIR})

    #=======================================================================
    if (WITH_WAYLAND_X11)
    #=======================================================================

        set(GRAPHIC_LIB_NAME LayerManagerGraphicWaylandGLESv2)

        find_package(X11 REQUIRED)
        
        include_directories(${X11_INCLUDE_DIR})
        
        set(LIBS
                ${LIBS}
                ${X11_X11_LIB}
                ${WAYLAND_EGL_LIBRARIES}
                ${XKB_LIBRARIES}
        )
        
        set(SOURCES
            ../Base/src/BaseRenderer.cpp
            src/GraphicSystems/GLESGraphicSystem.cpp
            src/WindowSystems/BaseWindowSystem.cpp
            src/WindowSystems/WaylandBaseWindowSystem.cpp
            src/WindowSystems/WaylandX11WindowSystem.cpp
            src/WindowSystems/WaylandServerinfoProtocol.cpp  
            src/WindowSystems/WaylandInputDevice.cpp
            src/WindowSystems/WaylandInputEvent.cpp
            src/WindowSystems/WaylandX11InputEvent.cpp
            src/TextureBinders/WaylandGLESTexture.cpp
        )
        
    endif (WITH_WAYLAND_X11)
    
    #=======================================================================
    if (WITH_WAYLAND_FBDEV)
    #=======================================================================

        set(GRAPHIC_LIB_NAME LayerManagerGraphicWaylandGLESv2)

        set(LIBS
                ${LIBS}
                ${UDEV_LIBRARIES}
                ${WAYLAND_EGL_LIBRARIES}
                ${MTDEV_LIBRARIES}
                ${XKB_LIBRARIES}
        )

        file(GLOB SOURCES 
            ../Base/src/BaseRenderer.cpp
            src/GraphicSystems/GLESGraphicSystem.cpp
            src/WindowSystems/BaseWindowSystem.cpp
            src/WindowSystems/WaylandBaseWindowSystem.cpp
            src/WindowSystems/WaylandFbdevWindowSystem.cpp
            src/WindowSystems/WaylandServerinfoProtocol.cpp  
            src/WindowSystems/WaylandInputDevice.cpp
            src/WindowSystems/WaylandInputEvent.cpp
            src/WindowSystems/WaylandEvdevInputEvent.cpp
            src/TextureBinders/WaylandGLESTexture.cpp
        )

    endif (WITH_WAYLAND_FBDEV)
    
    #=======================================================================
    if (WITH_WAYLAND_DRM)
    #=======================================================================

        set(GRAPHIC_LIB_NAME LayerManagerGraphicWaylandGLESv2)

        find_package (DRM REQUIRED)
        
        include_directories(${DRM_INCLUDE_DIR})
        
        set(LIBS
                ${LIBS}
                ${GRAPHIC_NATIVE_LIBS}
                ${UDEV_LIBRARIES}
                ${GBM_LIBRARIES}
                ${DRM_LIBRARIES}
                ${WAYLAND_EGL_LIBRARIES}
                ${MTDEV_LIBRARIES}
                ${XKB_LIBRARIES}
        )
        

        file(GLOB SOURCES
            ../Base/src/BaseRenderer.cpp
            src/GraphicSystems/GLESGraphicSystem.cpp
            src/GraphicSystems/DrmGLESGraphicSystem.cpp
            src/WindowSystems/BaseWindowSystem.cpp
            src/WindowSystems/WaylandBaseWindowSystem.cpp
            src/WindowSystems/WaylandDrmWindowSystem.cpp
            src/WindowSystems/WaylandServerinfoProtocol.cpp
            src/WindowSystems/WaylandInputDevice.cpp
            src/WindowSystems/WaylandInputEvent.cpp
            src/WindowSystems/WaylandEvdevInputEvent.cpp
            src/TextureBinders/WaylandGLESTexture.cpp
        )

    endif (WITH_WAYLAND_DRM)


    #=======================================================================
    # common
    #=======================================================================

    if(WITH_STATIC_LIBRARIES)
            add_library(${GRAPHIC_LIB_NAME} STATIC ${SOURCES})
        else(WITH_STATIC_LIBRARIES)
            add_library(${GRAPHIC_LIB_NAME} SHARED ${SOURCES})
    endif(WITH_STATIC_LIBRARIES)
    
    target_link_libraries(${GRAPHIC_LIB_NAME} ${LIBS})

    install(TARGETS
                ${GRAPHIC_LIB_NAME}
            DESTINATION
                lib
    )

    install(FILES
                include/GraphicSystems/BaseGraphicSystem.h
                include/GraphicSystems/GLESGraphicSystem.h
                include/GraphicSystems/DrmGLESGraphicSystem.h
            DESTINATION
                include/layermanager/graphic/GraphicSystems
    )

    install(FILES
                include/TextureBinders/ITextureBinder.h
                include/TextureBinders/WaylandGLESTexture.h
            DESTINATION
                include/layermanager/graphic/TextureBinders
    )

    install(FILES
                include/PlatformSurfaces/WaylandPlatformSurface.h
                include/PlatformSurfaces/EglWaylandPlatformSurface.h
            DESTINATION
                include/layermanager/graphic/PlatformSurfaces
    )

    install(FILES
                include/WindowSystems/BaseWindowSystem.h
                include/WindowSystems/WaylandBaseWindowSystem.h
                include/WindowSystems/WaylandFbdevWindowSystem.h
                include/WindowSystems/WaylandX11WindowSystem.h
                include/WindowSystems/WaylandDrmWindowSystem.h
            DESTINATION
                include/layermanager/graphic/WindowSystems
    )

    install(FILES
                ../Base/include/BaseRenderer.h
                ../Base/include/RenderUtil.h
            DESTINATION
                include/layermanager/graphic/Base
    )

endif(WITH_WAYLAND)


#===========================================================================
# Tests
#===========================================================================

if (WITH_TESTS)

    set (CMAKE_CXX_FLAGS "")

    enable_testing()
    add_subdirectory (tests)
    
endif(WITH_TESTS)

