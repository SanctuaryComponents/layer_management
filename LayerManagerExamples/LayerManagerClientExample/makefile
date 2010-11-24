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
include ../../LayerManagerPlatform/Make$(CONFIG).defs
endif

SRC += LayermanagerClientTest.c

TARGET_NAME = LayermanagerClientTest

ADD_INCLUDE_DIR+=include \
				../LayerSceneDescriptionExample/include \
				 $(PREFIX)/usr/include/layermanager

				
ADD_LIB_DIR+= $(PREFIX)/lib \
			$(PREFIX)/usr/lib 
ADD_LIBS+= ilm_client dbus-1

OBJS += $(patsubst %.c,build/%.o,$(SRC))
TARGET = build/$(TARGET_NAME)
INCLUDE= $(patsubst %$,-I%,$(ADD_INCLUDE_DIR))
LIBS+=$(shell pkg-config --libs $(PKG)) $(patsubst %$,-L%,$(ADD_LIB_DIR)) $(patsubst %$,-l%,$(ADD_LIBS)) 

all:	$(TARGET)

build/%.o: src/%.c
	$(shell mkdir -p $(shell dirname $@))
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC)  $(LINKFLAGS) -o $(TARGET) $(OBJS) $(LIBS)
	
install:
	mkdir -p $(PREFIX)
	install -m 755 $(TARGET) $(PREFIX)

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf build

