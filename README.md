Fractorium
==========

A Qt-based fractal flame editor which uses a C++ re-write of the flam3 algorithm
named Ember and a GPU capable version named EmberCL which implements a portion
of the cuburn algorithm in OpenCL.

# Install

## Windows

Download: [Fractorium_1.0.0.2.msi](https://drive.google.com/file/d/0Bws5xPbHJph6Y2tlQ2tmdHFwNnc/view?usp=sharing)

## Mac

Download: [Fractorium_1.0.0.2.dmg](https://drive.google.com/file/d/0Bws5xPbHJph6MkpQZjVaV1dVMWM/view?usp=sharing)

## Linux

### Install from PPA

Enable `universe` in the Ubuntu Software Center:

- open the Edit menu
- select Software Sources...
- enable "Community maintained ... `universe`"

Add the [Fractorium Ubuntu PPA](https://launchpad.net/~fractorium/+archive/ubuntu/ppa) and install:

```
sudo apt-add-repository ppa:fractorium/ppa
sudo apt-get update
sudo apt-get install fractorium
```

### Install from .deb

Download: [fractorium_1.0.0.2b-0ubuntu1_amd64.deb](https://launchpad.net/~fractorium/+archive/ubuntu/ppa/+build/12136710/+files/fractorium_1.0.0.2b-0ubuntu1_amd64.deb)

```
cd ~/Downloads
sudo dpkg -i fractorium_1.0.0.2b-0ubuntu1_amd64.deb
```

# Building from git

All builds are 64-bit.

## Windows

[Build Guide for Visual Studio 2015 or Qt Creator](Data/BuildGuideQtCreator.md)

## Linux

[Build Guide for Linux](./Data/BuildGuideLinux.md)

## Mac OS/X Sierra, El Capitan, Yosemite and Mavericks

[Build Guide for Mac OS/X](./Data/BuildGuideMacOSX.md)