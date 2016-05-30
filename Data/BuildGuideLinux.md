# Build Guide for Linux

Install `git` and clone the repository:

```
sudo apt-get install git
git clone https://mfeemster@bitbucket.org/mfeemster/fractorium.git
```

Install the dependencies.

Ubuntu 15.04 (vivid), 15.10 (wily), 16.04 (xenial):

```
sudo apt-get install g++ libgl1-mesa-dev libgl-dev libglm-dev libjpeg-dev libpng12-dev libqt5opengl5-dev libtbb-dev libxml2-dev ocl-icd-libopencl1 ocl-icd-opencl-dev opencl-headers qt5-default qt5-qmake qtbase5-dev
```

Install the OpenCL drivers and opencl support for your hardware. For AMD get their drivers from their site, build and install .deb package. For Nvidia:

```
sudo apt-get install nvidia-352 nvidia-352-dev nvidia-libopencl1-352 nvidia-modprobe nvidia-opencl-dev nvidia-opencl-icd-352 nvidia-prime
```

Note: There may be a more recent release on their site.

If you have both Qt 4 and 5 installed, select Qt 5 before compilation:

```
export QT_SELECT=qt5
```

Compile the binary:

```
cd fractorium
qmake main.pro -r -spec linux-g++-64 CONFIG+=release
make
```

Or open main.pro in Qt Creator and build all.

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
