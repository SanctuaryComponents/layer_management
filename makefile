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
include ./LayerManagerPlatform/Make$(CONFIG).defs
else
$(warning No Config defined, building on target platform)
endif
ifeq ($(RENDERER),)
$(error No Renderer defined, skipping build)
endif
ifeq ($(COMMUNICATOR),)
$(error No Communicator defined, skipping build)
endif
ifeq ($(PROJECT_ROOT),)
PROJECT_ROOT=.
endif
all:
	echo "Building LayerManagerUtils for $(CONFIG)"
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerUtils CONFIG=$(CONFIG) all
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerUtils CONFIG=$(CONFIG) install
	echo "Building LayerManagerService for $(CONFIG)"
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerService CONFIG=$(CONFIG) all
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerService CONFIG=$(CONFIG) install	
	echo "Building LayerManagerComunicator for $(CONFIG)"	
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerPlugins/Communicators/$(COMMUNICATOR) CONFIG=$(CONFIG) all
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerPlugins/Communicators/$(COMMUNICATOR) CONFIG=$(CONFIG) install
	echo "Building LayerManagerRenderer $(RENDERER) for $(CONFIG)"	
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerPlugins/Renderers/Platform/$(RENDERER) CONFIG=$(CONFIG) all
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerPlugins/Renderers/Platform/$(RENDERER) CONFIG=$(CONFIG) install	
	
install:
	echo "Installing LayerManagerUtils for $(CONFIG)"
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerUtils CONFIG=$(CONFIG) install
	echo "Installing LayerManagerService for $(CONFIG)"
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerService CONFIG=$(CONFIG) install	
	echo "Installing LayerManagerComunicator for $(CONFIG)"	
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerPlugins/Communicators/DBUSCommunicator CONFIG=$(CONFIG) install
	echo "Installing LayerManagerRenderer $(RENDERER) for $(CONFIG)"	
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerPlugins/Renderers/Platform/$(RENDERER) CONFIG=$(CONFIG) install	

clean:
	echo "Cleaning LayerManagerUtils for $(CONFIG)"
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerUtils CONFIG=$(CONFIG) clean
	echo "Cleaning LayerManagerService for $(CONFIG)"
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerService CONFIG=$(CONFIG) clean	
	echo "Cleaning LayerManagerComunicator for $(CONFIG)"	
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerPlugins/Communicators/DBUSCommunicator CONFIG=$(CONFIG) clean
	echo "Cleaning LayerManagerRenderer $(RENDERER) for $(CONFIG)"	
	$(MAKE) -C $(PROJECT_ROOT)/LayerManagerPlugins/Renderers/Platform/$(RENDERER) CONFIG=$(CONFIG) clean	