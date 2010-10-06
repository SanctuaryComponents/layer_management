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

SRC += Log.cpp \
	   LogMessageBuffer.cpp 		 

TARGET_NAME = libLayerManagerUtils.a
LIB_NAME = $(TARGET_NAME).1.0


# ADD PACKAGES/INCLUDE_DIRS/LIBS
PKG=
ADD_INCLUDE_DIR+=	include \
					$(PREFIX)/usr/include 
				
ADD_LIB_DIR+= 
ADD_LIBS+= 

OBJS += $(patsubst %.cpp,build/%.o,$(SRC))
LINK = 
TARGET = build/$(TARGET_NAME)
INCLUDE= $(patsubst %$,-I%,$(ADD_INCLUDE_DIR))
LIBS+=$(shell pkg-config --libs $(PKG)) $(patsubst %$,-L%,$(ADD_LIB_DIR)) $(patsubst %$,-l%,$(ADD_LIBS)) 

all:	$(TARGET)

build/%.o: src/%.cpp
	$(shell mkdir -p $(shell dirname $@))
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

$(TARGET): $(OBJS)
	$(AR) rcs $(TARGET) $(OBJS)

install:
	mkdir -p $(PREFIX)/usr/lib/layermanager
	install -m 755 $(TARGET) $(PREFIX)/usr/lib/layermanager
	mkdir -p $(PREFIX)/usr/include/layermanager/	
	install -m 755 include/Log.h $(PREFIX)/usr/include/layermanager/
	install -m 755 include/LogMessageBuffer.h $(PREFIX)/usr/include/layermanager/

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf build
