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
include ../LayerManagerPlatform/Make$(CONFIG).defs
endif

SRC = LogicalGraphicsObject.cpp \
	LayerList.cpp \
	Layermanager.cpp \
	main.cpp \
	shader/Shader.cpp \
	shader/ShaderProgram.cpp \
	shader/ShaderProgramFactory.cpp \
	shader/ShaderUniform.cpp
	
TARGET_NAME = layermanager

ADD_INCLUDE_DIR+= include \
			$(PREFIX)/usr/include/layermanager
				
ADD_LIB_DIR+=$(PREFIX)/usr/lib/layermanager
			
ADD_LIBS+= pthread dl LayerManagerUtils

OBJS = $(patsubst %.cpp,build/%.o,$(SRC))
TARGET = build/$(TARGET_NAME)
INCLUDE= $(patsubst %$,-I%,$(ADD_INCLUDE_DIR))
LIBS=$(shell pkg-config --libs $(PKG)) $(patsubst %$,-L%,$(ADD_LIB_DIR)) $(patsubst %$,-l%,$(ADD_LIBS)) 

all:	$(TARGET)
		
build/%.o: src/%.cpp
	$(shell mkdir -p $(shell dirname $@))
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CXX)  -Xlinker -export-dynamic -o $(TARGET) $(OBJS) $(LIBS)

install:
	mkdir -p $(PREFIX)/usr/include/layermanager
	install -m 755 include/*.h $(PREFIX)/usr/include/layermanager
	mkdir -p $(PREFIX)/usr/include/layermanager/commands
	install -m 755 include/commands/*.h $(PREFIX)/usr/include/layermanager/commands

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf build
