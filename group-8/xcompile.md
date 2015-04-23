# 2015-mini-smart-vehicles

##Instructions for cross-compiling from an Ubuntu linux machine. 

First up set yourself up a keypair

[https://www.digitalocean.com/community/tutorials/how-to-set-up-ssh-keys--2](https://www.digitalocean.com/community/tutorials/how-to-set-up-ssh-keys--2)


Now, on the odroid create the folder /opt/msv

```mkdir /opt/msv```

Then check and take note of the compiler version with

```gcc --version```

Moving now to your client machine, make sure you have the ARM cross-compiler installed using the same version as on the odroid e.g. if the odroid has the version 4.8 you will do:

```sudo apt-get install gcc-4.8-arm-linux-gnueabihf```

```sudo apt-get install g++-4.8-arm-linux-gnueabihf```

Now power down the Odroid and mount the SD card on your client machine (using a card reader) and find the path to the attached device using 

```df -h```

it should have two partitions for the SD-card, the one you want likely looks like this /media/msv/trusty. Assuming that, copy the bin, lib, sbin and usr folders across to a new folder that you must create called PandaBoard-SD e.g.

```mkdir opt/PandaBoard-SD```

```sudo cp -Ri /media/msv/trusty/usr /opt/PandaBoard-SD/usr```

```sudo cp -Ri /media/msv/trusty/bin /opt/PandaBoard-SD/bin```

And so on.

Now you are ready to go into the PandaBoard-Toolchain.cmake file in the OpenDaVinci-msv folder. change the Compiler seetings to match your intended compiler version (as above) so instead of:

SET(CMAKE_C_COMPILER   /usr/bin/arm-linux-gnueabihf-gcc)

you would want 

SET(CMAKE_C_COMPILER   /usr/bin/arm-linux-gnueabihf-gcc-4.8)

Now go into the CMakeLists.txt in the OpenDaVinci-msv/apps/ folder

You will see the following:

	IF("${PANDABOARD}" STREQUAL "YES")
	ADD_CUSTOM_TARGET(push2meili-1
	#    COMMAND rsync -ave ssh /opt/msv-ARM/ meili@192.168.0.101:/opt/msv/
	    COMMAND rsync -ave ssh /opt/msv-ARM/ meili@192.168.0.101:/opt/msv/bin/msv
	    DEPENDS cc-data sensorboard lanedetector driver proxy parker
	)

	ADD_CUSTOM_TARGET(push2meili-2
	#    COMMAND rsync -ave ssh /opt/msv-ARM/ meili@192.168.0.101:/opt/msv/
	    COMMAND rsync -ave ssh /opt/msv-ARM/ meili@192.168.0.101:/opt/msv/bin/msv
	    DEPENDS cc-data sensorboard lanedetector driver proxy parker
	)
	ENDIF("${PANDABOARD}" STREQUAL "YES")```

Assuming that your odroid lives on 192.168.1.137 (adjust as necessary) change it to look like this (you should be able to change the target name from push2meili-1 but I didn't bother)

	IF("${PANDABOARD}" STREQUAL "YES")
	ADD_CUSTOM_TARGET(push2meili-1
		COMMAND rsync -ave ssh --rsync-path='sudo rsync' /opt/msv-ARM/ odroid@192.168.1.137:/opt/msv/
    	DEPENDS lanedetector driver proxy
	)
	ENDIF("${PANDABOARD}" STREQUAL "YES")

Now you can build the file as per Christian's instructions for Ubuntu 64 bit except instead of /opt/odv create the folder /opt/msv-ARM (so you have a special install for the ARM version) and where you cd into build and run the cmake command you will instead do:

```cd build && cmake -DCMAKE_TOOLCHAIN_FILE=~/2015-mini-smart-vehicles/group-8/sources/OpenDaVINCI-msv/PandaBoard-Toolchain.cmake -DCMAKE_INSTALL_PREFIX=/opt/msv-ARM ..```

Which invokes the panda board tool chain and directs the install to be to your ARM folder. 

If, when you `make all`	you get errors that look like this (will a bunch more stuff afterwards):

```/usr/lib/gcc-cross/arm-linux-gnueabihf/4.9/../../../../arm-linux-gnueabihf/bin/ld: cannot find /lib/arm-linux-gnueabihf/libpthread.so.0 inside```

You may need to create symlinks to your pandaboard files so do the following:

```mkdir /lib/arm-linux-gnueabihf/```

```mkdir /usr/lib/arm-linux-gnueabihf/```

```ln -s /opt/PandaBoard/lib/arm-linux-gnueabihf/ /lib/arm-linux-gnueabihf/```

```ln -s /opt/PandaBoard/usr/lib/arm-linux-gnueabihf/ /usr/lib/arm-linux-gnueabihf/```

After you `make install`, make sure you are able to connect to the odroid (sd card is back in and you know you can ssh to it)

```make push2meili-1```

This should push the binaries in your /opt/msv-ARM folder to the /opt/msv folder on the odroid. You should now be able to run those binaries just like you would normally with

```/opt/msv/bin/supercomponent --cid=111 --freq=20``` 
