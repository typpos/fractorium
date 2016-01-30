# Building Guide for Linux

Install `git` and clone the repository:

```
sudo apt-get install git
git clone https://github.com/mfeemster/fractorium
```

Install the dependencies.

For Ubuntu 15.04 (vivid) and 15.10 (wily):

```
sudo apt-get install g++ libdbus-1-dev libgl1-mesa-dev libgl-dev libglm-dev libjpeg-dev libpng12-dev libtbb-dev libxml2-dev qt5-default qt5-qmake qtbase5-dev libqt5opengl5-dev ocl-icd-libopencl1
```

Install the OpenCL drivers and opencl support for your hardware. For Nvidia:

```
sudo apt-get install nvidia-352 nvidia-352-dev nvidia-opencl-icd-352 nvidia-libopencl1-352 nvidia-prime nvidia-modprobe
```

If you have both Qt 4 and 5 installed, select Qt 5 before compilation:

```
export QT_SELECT=qt5
```

Compile the binary:

```
cd fractorium
qmake
make
```

Run the binary from the release folder:

```
cd Bin/release
./fractorium
```

`sudo make install` will install the files directly. `sudo make uninstall` is
also available.

You can also compile a `.deb` package to install locally. A few more tools will
be necessary:

```
sudo apt-get install bzr bzr-builddeb dh-make debhelper
```

A helper script is available, use `package-linux.sh` in the project root. It
will create `~/PPA/fractorium-VERSION` as a work folder, by default it builds a
signed source package.

For local use you probably want an unsigned binary package:

```
cd fractorium
./package-linux.sh --binary-only --unsigned
```
