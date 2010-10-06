############################################################################
# 
# Copyright 2010 BMW Car IT GmbH  
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

ifneq ($(CONFIG),)
include ../../../../LayerManagerPlatform/Make$(CONFIG).defs
endif

SRC = X11GLESRenderer.cpp \
      GLESGraphicSystem.cpp \
      BaseRenderer.cpp \
      X11WindowSystem.cpp \
      ShaderProgramGLES.cpp \
      X11Copy.cpp \
      X11CopyGLES.cpp \
      X11EglImage.cpp

TARGET_NAME = libX11GLESRenderer.so

ADD_INCLUDE_DIR+=include \
			../../Base/include \
			../../Graphic/include \
			$(PREFIX)/usr/include/layermanager 
				
ADD_LIB_DIR+= $(PREFIX)/usr/lib/layermanager
				
ADD_LIBS+= EGL GLESv2 pthread Xcomposite LayerManagerUtils

OBJS = $(patsubst %.cpp,build/%.o,$(SRC))
TARGET = build/$(TARGET_NAME)
INCLUDE= $(patsubst %$,-I%,$(ADD_INCLUDE_DIR))
LIBS+=$(shell pkg-config --libs $(PKG)) $(patsubst %$,-L%,$(ADD_LIB_DIR)) $(patsubst %$,-l%,$(ADD_LIBS)) 

all:	$(TARGET)

build/%.o: src/%.cpp
	$(shell mkdir -p $(shell dirname $@))
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<
	
build/%.o: ../../Base/src/%.cpp
	$(shell mkdir -p $(shell dirname $@))
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<
	
build/%.o: ../../Graphic/src/GraphicSystems/%.cpp
	$(shell mkdir -p $(shell dirname $@))
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<
	
build/%.o: ../../Graphic/src/WindowSystems/%.cpp
	$(shell mkdir -p $(shell dirname $@))
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<
	
build/%.o: ../../Graphic/src/TextureBinders/%.cpp
	$(shell mkdir -p $(shell dirname $@))
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<


$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -W1,-soname,$(TARGET) -o $(TARGET) $(OBJS) $(LIBS)

install:
	mkdir -p $(PREFIX)/usr/lib/layermanager/renderer
	install -m 755 $(TARGET) $(PREFIX)/usr/lib/layermanager/renderer

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf build
	
