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
include_directories ("../Base/include")

include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerService/include")
link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerService/bin")
include_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/include")
link_directories ("${PROJECT_SOURCE_DIR}/LayerManagerUtils/bin")

if (WITH_GLESv2_LIB) 
set(GRAPHIC_LIB_NAME LayerManagerGraphicGLESv2)
file(GLOB SOURCES 
		../Base/src/BaseRenderer.cpp 
		src/GraphicSystems/GLESGraphicSystem.cpp
		src/WindowSystems/X11WindowSystem.cpp  
		src/TextureBinders/X11Copy.cpp
		src/TextureBinders/X11CopyGLES.cpp
		src/TextureBinders/X11EglImage.cpp
        src/Transformation/ViewportTransform.cpp
		)

file(GLOB GRAPHIC_BASE_INCLUDES 
    ../Base/include/*.h
)

file(GLOB GRAPHIC_GRAPHICSYSTEM_INCLUDES 
    include/GraphicSystems/BaseGraphicSystem.h
    include/GraphicSystems/GLESGraphicSystem.h
)

file(GLOB GRAPHIC_TEXTUREBINDERS_INCLUDES 
    include/TextureBinders/ITextureBinder.h
    include/TextureBinders/X11Copy.h
    include/TextureBinders/X11CopyGLES.h
    include/TextureBinders/X11EglImage.h
)

file(GLOB GRAPHIC_PLATFORM_INCLUDES 
    include/PlatformSurfaces/XPlatformSurface.h
    include/PlatformSurfaces/EglXPlatformSurface.h
)

file(GLOB GRAPHIC_WINDOWSYSTEMS_INCLUDES 
    include/WindowSystems/BaseWindowSystem.h
    include/WindowSystems/X11WindowSystem.h
)

file(GLOB GRAPHIC_TRANSFORM_INCLUDES 
    include/Transformation/ViewportTransform.h
)

if (WITH_TESTS)
    add_subdirectory (tests)
endif(WITH_TESTS)

endif(WITH_GLESv2_LIB)

if (WITH_GLX_LIB) 
set(GRAPHIC_LIB_NAME LayerManagerGraphicGLX)
file(GLOB SOURCES 
    ../Base/src/BaseRenderer.cpp 
    src/GraphicSystems/GLXGraphicSystem.cpp
    src/WindowSystems/X11WindowSystem.cpp  
    src/TextureBinders/X11TextureFromPixmap.cpp
    src/TextureBinders/X11CopyGLX.cpp
    src/TextureBinders/X11Copy.cpp
    src/Transformation/ViewportTransform.cpp    
)

file(GLOB GRAPHIC_BASE_INCLUDES 
    ../Base/include/*.h
)

file(GLOB GRAPHIC_WINDOWSYSTEMS_INCLUDES 
    include/WindowSystems/BaseWindowSystem.h
    include/WindowSystems/X11WindowSystem.h
)


file(GLOB GRAPHIC_GRAPHICSYSTEM_INCLUDES 
    include/GraphicSystems/BaseGraphicSystem.h
    include/GraphicSystems/GLXGraphicsystem.h
)

file(GLOB GRAPHIC_TEXTUREBINDERS_INCLUDES 
    include/TextureBinders/ITextureBinder.h
    include/TextureBinders/X11Copy.h    
    include/TextureBinders/X11CopyGLX.h
    include/TextureBinders/X11TextureFromPixmap.h
)

file(GLOB GRAPHIC_PLATFORM_INCLUDES 
    include/PlatformSurfaces/XPlatformSurface.h
    include/PlatformSurfaces/GLXPlatformSurface.h
)        
file(GLOB GRAPHIC_TRANSFORM_INCLUDES 
    include/Transformation/ViewportTransform.h
)

endif(WITH_GLX_LIB)

		
add_library(${GRAPHIC_LIB_NAME} SHARED ${SOURCES})

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

find_package(EGL REQUIRED)
include_directories(${EGL_INCLUDE_DIR})

find_package(GLESv2 REQUIRED)
include_directories(${GLESv2_INCLUDE_DIR})

find_package (Threads)
set(LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT} ${EGL_LIBRARY} ${GLESv2_LIBRARIES} ${X11_Xcomposite_LIB} ${X11_Xdamage_LIB} LayerManagerUtils )
target_link_libraries(${GRAPHIC_LIB_NAME} ${LIBS})

add_dependencies(${GRAPHIC_LIB_NAME} LayerManagerService)
add_dependencies(${GRAPHIC_LIB_NAME} LayerManagerUtils)

install (TARGETS ${GRAPHIC_LIB_NAME} DESTINATION lib)
install (FILES ${GRAPHIC_BASE_INCLUDES} DESTINATION include/layermanager/graphic/Base)
install (FILES ${GRAPHIC_GRAPHICSYSTEM_INCLUDES} DESTINATION include/layermanager/graphic/GraphicSystems)
install (FILES ${GRAPHIC_TRANSFORM_INCLUDES} DESTINATION include/layermanager/graphic/Tranformation)
install (FILES ${GRAPHIC_PLATFORM_INCLUDES} DESTINATION include/layermanager/graphic/PlatformSurfaces)
install (FILES ${GRAPHIC_WINDOWSYSTEMS_INCLUDES} DESTINATION include/layermanager/graphic/WindowSystems)
install (FILES ${GRAPHIC_WINDOWSYSTEMS_INCLUDES} DESTINATION include/layermanager/graphic/WindowSystems)

