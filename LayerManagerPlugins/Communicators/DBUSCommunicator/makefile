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
include ../../../LayerManagerPlatform/Make$(CONFIG).defs
endif

SRC += DBUSCommunicator.cpp

TARGET_NAME = libDBUSCommunicator.so
LIB_NAME = $(TARGET_NAME).1.0

ADD_INCLUDE_DIR=include \
	$(PREFIX)/usr/include/dbus-1.0 \
	$(PREFIX)/usr/lib/dbus-1.0/include \
	$(PREFIX)/usr/include/layermanager \
$(PREFIX)/usr/local/include/dbus-1.0 \
$(PREFIX)/usr/local/lib/dbus-1.0/include 

				
ADD_LIB_DIR+=$(PREFIX)/usr/local/lib/dbus-1 \
	$(PREFIX)/usr/lib/dbus-1 \
	$(PREFIX)/usr/lib/layermanager

ADD_LIBS+= pthread dbus-1 LayerManagerUtils

OBJS += $(patsubst %.cpp,build/%.o,$(SRC))
LINK = 
TARGET = build/$(TARGET_NAME)
INCLUDE= $(patsubst %$,-I%,$(ADD_INCLUDE_DIR))
LIBS+=$(shell pkg-config --libs $(PKG)) $(patsubst %$,-L%,$(ADD_LIB_DIR)) $(patsubst %$,-l%,$(ADD_LIBS)) 

all: $(TARGET)

build/%.o: src/%.cpp
	$(shell mkdir -p $(shell dirname $@))
	$(CXX) -Wall -fPIC -O2 $(INCLUDE) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CXX) -Wall -fPIC -O2 -shared -W1,-soname, -Xlinker -export-dynamic -o $(TARGET) $(OBJS) $(LIBS)

install:
	mkdir -p $(PREFIX)/usr/lib/layermanager/communicator
	install -m 755 $(TARGET) $(PREFIX)/usr/lib/layermanager/communicator

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf build
