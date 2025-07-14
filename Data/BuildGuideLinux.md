# Build Guide for Linux

The following has been tested on Ubuntu 20.

Make sure the package lists are up-to-date:

```
sudo apt-get update
sudo apt-get upgrade
```

Install `git` and clone the repository:

```
sudo apt-get install git
git clone https://github.com/mfeemster/fractorium.git
```

Install the dependencies.

```
sudo apt-get install g++ libxcb-cursor0 libc6-dev libgl1-mesa-dev libgl-dev libglm-dev libjpeg-dev libpng-dev libpng16-16 libxml2-dev libqt6opengl6-dev qt6-base-dev qt6-default qt6-qmake qmake6 ocl-icd-libopencl1 ocl-icd-opencl-dev opencl-headers libopenexr-dev libopenexr-3-1-30
```

Install the OpenCL drivers and opencl support for your hardware.

For AMD get their drivers from their site, build and install the .deb package.

For Nvidia:

```
sudo apt-get install nvidia-modprobe nvidia-prime nvidia-384 nvidia-384-dev 
```

Optionally you can install the Nvidia-specific `nvidia-libopencl1-384` package,
but keep in mind that this will remove the generic ones (`ocl-icd-opencl-dev`
and `ocl-icd-libopencl1`).

If you have both Qt 5 and 6 installed, select Qt 6 before compilation:

```
export QT_SELECT=qt6
```

Compile the binaries, they will be created in the `Bin/release` folder.

```
cd fractorium
qmake main.pro -r -spec linux-g++-64 CONFIG+="release native"
make
```

Or open main.pro in Qt Creator and build all.

`sudo make install` will install the files directly. `sudo make uninstall` is
also available.

You can also compile a `.deb` package to install locally. A few more tools will
be necessary:

```
sudo apt-get install bzr bzr-builddeb dh-make debhelper
```

Tell `bzr` about yourself:

```
bzr whoami "The Person <the.person@email.com>"
```

A helper script is available, use `package-linux.sh` in the project root. It
will create `~/PPA/fractorium-VERSION` as a work folder, by default it builds a
signed source package.

For local use you probably want an unsigned binary package:

```
cd fractorium
./package-linux.sh --binary-only --unsigned
```
