#!/bin/sh
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

export DBUS_SESSION_BUS_ADDRESS=tcp:host=0.0.0.0,port=12434
export LD_LIBRARY_PATH=/usr/lib:/usr/local/lib
export DISPLAY=:0.0
PIDSURFACE1=/var/run/surface1.pid

init_scene()
{
	./LayerManagerExamples/LayerManagerClientExample/build/LayermanagerClientTest
}

deinit_scene()
{
	./LayerManagerExamples/LayerManagerClientExample/build/LayermanagerClientTest 2
}

start_example_application()
{
	./LayerManagerExamples/EGLX11ApplicationExample/build/eglX11_application &
	pidofdlt=`ps aux | grep eglX11_application | grep -v grep | awk '{print $2}'`
	echo $pidofdlt > $PIDSURFACE1
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
	deinit_scene
}



start()
{
	init_scene
	sleep 1
	start_example_application
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
