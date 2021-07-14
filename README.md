# C15 Firmware

## How to build

Preconditions:
- g++-8 has to be installed and must be the default g++ see: [how to have different g++ installations for ubuntu](https://askubuntu.com/a/1028656)
- git, cmake, docker have to be installed, docker has to be enabled/started
- current user has to be in group 'docker' ``sudo usermod -aG docker $USER`` (pitfall: logout/login required to make it effective) 
- at least one target expects a qemu installation
- needed Ubuntu packages for an Developer Setup: 
```
sudo apt install libfreetype-dev libglibmm-2.4-dev libgio2.0-cil-dev libsoup2.4-dev libgtkmm-3.0-dev libboost-dev libpng++-dev cmake git build-essential libasound2-dev openjdk-16-jdk-headless openjdk-16-jre-headless libflac-dev
```

Then:
- checkout the repository, for example git clone https://github.com/nonlinear-labs-dev/C15.git ~/C15
- create a build folder, for example ~/builds/C15
- change into that folder and type 
```sh
cmake -DDEV_PC=On|Off -DBUILD_AUDIOENGINE=On|Off -DBUILD_BBBB=On|Off -DBUILD_PLAYGROUND=On|Off -DBUILD_ONLINEHELP=On|Off -DBUILD_TEXT2SOLED=On|Off -DBUILD_TESTING=On|Off -DBUILD_BBB=On|Off -DBUILD_EPC=On|Off -DCMAKE_BUILD_TYPE=Debug|Release ~/C15
```
### DEV_PC

This option enables some tests, assertions and debug features.

### DOCKER_GROUP_ID

You might use for example

```
cmake -DDOCKER_GROUP_ID=`getent group vboxfs | cut -d: -f3` .
```

in order to fix permission issues with virtual box shared folders.

### BUILD_AUDIOENGINE

If switched 'On', the build system will make the audioengine binary for the current platform. The engine contains the synth algorithm driving the C15 keyboard synthesizer. 

### BUILD_BBBB

The BeagleBoneBlackBridge (BBBB) is a small utility for connecting a 'playground' instance to the C15 hardware UI and the C15 LPC microprocessor. If the 'DEV_PC'-option is switch On, it will provide a debug UI providing most of the C15 hardware buttons and displays.

### BUILD_PLAYGROUND

The 'playground' is the application, that connects all the parts the C15 is made off. It
- organizes presets and banks
- controls the hardware buttons, rotary and displays
- manages parameters
- provides the WebUI
- manages settings

### BUILD_ONLINEHELP

The Online help is a HTML project describing in detail most of the features of the C15.

### BUILD_TEXT2SOLED

'text2soled' is a small command line tools to be uses in update- or maintenance use cases. Scripts can utilize it to show text messages on the C15 displays. It is useless on a developer pc.

### BUILD_TESTING

The option will enable building the playground-test binary, which contains most of the playgrounds unit tests.

### BUILD_BBB

This option will enable a cross compiling build system. Here, it becomes really complicated, but, in a nutshell:
- a docker image (based on Ubuntu 16.04) will be created
- inside the docker, the nonlinux-buildroot repository (https://github.com/nonlinear-labs-dev/nonlinux.git) will be checked out
- The nonlinux will be built.
- Nonlinux includes a C15 package, which will rsync the sources from the current source folder. So, changes to files in the ~/C15 (or whereever you checked out the sources to) will be refelected in the build produced by the nonlinux-buildroot inside the docker.

#### bbb-rootfs

The BUILD_BBB option enables the *bbb-rootfs* target. When reqested, it will make the file `~/builds/C15/build-tools/bbb/rootfs.tar.gz` containing the whole root fdile system for the BeagleBone.

### BUILD_EPC

Here, it gets even more complicated:
- Again, a docker image is created, based on Ubuntu 19.10.
- The docker is used to enable building the following targets:
  - *epc-nonlinux-iso*: This downloads the AudioPhile-Linux iso image, unpacks it, pimps the iso according to our needs, and creates the NonlinuxAP.iso image in turn.
  - *epc-install-medium*: This target will copy the NonlinuxAP.iso onto an USB stick. The path to the stick has to be provided by the environment variable EPC_INSTALL_MEDIUM, e.g. 
  ```sh 
  EPC_INSTALL_MEDIUM=/dev/sde make epc-install-medium
  ```
  - *epc-nonlinux-vm-installation*: This target will create a virtual machine on your development PC, install NonlinuxAP into it and boot the machine. It is a testing target, used to proof the functionality of *epc-nonlinux-iso* and for illustration purpose.
  - *epc-create-update*: This target creates a docker'ed NonLinuxAP installation. It then injects the current source tree into it and builds the C15 in that environment. By this, we make sure all needed libraries and system components exist on the target device. After building, the system will create an overlay-filesystem and install the built artifacts there. This overlay will be used to create an update.tar. If the update.tar is offered to the C15-ePC at booting time on a USB stick or via http, the ePC will apply the update.
  
### BUILD_LPC

This target will build the realtime-software for our LPC processor. It will generate blobs for both, the M0 and the M4 core.

### BUILD_BBB AND BUILD_EPC AND BUILD_LPC

If BUILD_BBB, BUILD_EPC and BUILD_LPC are switched On, there will be another target, *c15-update*. This will combine updates for all C15 components and bundle it into a single update file.

The "c15-ota-update" target will create a complete c15 update.tar and immediately try to update the device handed in by the environment variables UNIT_NAME and UNIT_PASSWORD like this:

```
UNIT_NAME=NL-C15-Unit-xxxxx UNIT_PASSWORD=**** make c15-ota-update
```





   
