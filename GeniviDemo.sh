#!/bin/sh
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

export LD_LIBRARY_PATH=/usr/lib:/usr/local/lib
export DISPLAY=:0.0
export DBUS_SESSION_BUS_ADDRESS=tcp:host=0.0.0.0,port=12434
PIDSURFACE1=/var/run/surface1.pid
PIDSURFACE2=/var/run/surface2.pid

init_scene()
{
	LayerManagerClientExample
}

deinit_scene()
{
	LayerManagerClientExample 2
}

start_egl_example_application()
{
	EGLX11ApplicationExample &
	pidofdlt=`ps aux | grep EGLX11ApplicationExample | grep -v grep | awk '{print $2}'`
	echo $pidofdlt > $PIDSURFACE1
}

start_glx_example_application()
{
	GLX11ApplicationExample &
	pidofdlt=`ps aux | GLX11ApplicationExample | grep -v grep | awk '{print $2}'`
	echo $pidofdlt > $PIDSURFACE2
}


killprocess()
{
	if [ -f $1 ]; then
		kill -9 `cat $1`
		rm -f $1
	fi
}
stop() 
{
	killprocess $PIDSURFACE1
	killprocess $PIDSURFACE2
	deinit_scene
}



start()
{
	init_scene
	sleep 1
	start_egl_example_application
	sleep 2
	start_glx_example_application
}

case "$1" in
	start)
		start
		;;
	init_scene)
		init_scene
		;;
	start_example_application)
		start_example_application
		;;
	stop)
		stop
		;;

	*)
		echo "Usage: $0 {init_scene|start|start_example_application|stop"
		;;
esac
exit 0
