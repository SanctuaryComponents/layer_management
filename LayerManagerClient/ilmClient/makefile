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

SRC += ilm_client.c ilm_client_priv.c

TARGET_NAME = libilm_client.so

ADD_INCLUDE_DIR+=include \
		$(PREFIX)/usr/lib/dbus-1.0/include \
        $(PREFIX)/usr/local/lib/dbus-1.0/include \
        $(PREFIX)/usr/local/include/dbus-1.0 \

				
ADD_LIB_DIR+= $(PREFIX)/lib \
			$(PREFIX)/usr/lib 
ADD_LIBS+= dbus-1

OBJS += $(patsubst %.c,build/%.o,$(SRC))
TARGET = build/$(TARGET_NAME)
INCLUDE= $(patsubst %$,-I%,$(ADD_INCLUDE_DIR))
LIBS+=$(shell pkg-config --libs $(PKG)) $(patsubst %$,-L%,$(ADD_LIB_DIR)) $(patsubst %$,-l%,$(ADD_LIBS)) 

all:	$(TARGET)

build/%.o: src/%.c
	$(shell mkdir -p $(shell dirname $@))
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -W1,-soname,$(TARGET) -o $(TARGET) $(OBJS) $(LIBS)
	

install:
	mkdir -p $(PREFIX)/usr/lib
	install -m 755 $(TARGET) $(PREFIX)/usr/lib
	mkdir -p $(PREFIX)/usr/include/layermanager/
	install -m 755 include/ilm_client.h $(PREFIX)/usr/include/layermanager/
	install -m 755 include/ilm_types.h $(PREFIX)/usr/include/layermanager/
	install -m 755 include/ilm_client_platform.h $(PREFIX)/usr/include/layermanager/
	install -m 755 ../../LayerManagerExamples/LayerSceneDescriptionExample/include/LayerScene.h $(PREFIX)/usr/include/layermanager/
	
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf build
