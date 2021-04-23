Fractorium
==========

A Qt-based fractal flame editor which uses a C++ re-write of the flam3 algorithm
named Ember and a GPU capable version named EmberCL which implements a portion
of the cuburn algorithm in OpenCL.

# Install

## Windows

Download: [Fractorium_22.21.4.2.msi](https://drive.google.com/file/d/1eeUNoAvkrfSG6fCObAYE1e2vmvUQY6MR/view?usp=sharing)

## Mac

Download: [Fractorium_22.21.4.2.dmg](https://drive.google.com/file/d/19cT7EyqmC-bMFl5PdBMaQE-qNbLz0wrR/view?usp=sharing)

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

### Install from App Image .deb

Install ubuntu.

Download: [Fractorium-22.21.4.2.x86_64.deb](https://drive.google.com/file/d/1KJTQEh7Ll4fMTloTDPIZIJGkTL2eEbIj/view?usp=sharing)

```
cd ~/Downloads
sudo dpkg -i Fractorium-22.21.4.2.x86_64.deb
```

### Install from App Image .rpm

Download: [Fractorium-22.21.4.2.x86_64.rpm](https://drive.google.com/file/d/1VNT77_V51O4o01_Q_3SToZjr-L4SX_cy/view?usp=sharing)

# Building from git

All builds are 64-bit.

## Windows

[Build Guide for Visual Studio 2019 or Qt Creator](Data/BuildGuideQtCreator.md)

## Linux

[Build Guide for Linux](./Data/BuildGuideLinux.md)

## Mac OS/X Sierra, El Capitan, Yosemite and Mavericks

[Build Guide for Mac OS/X](./Data/BuildGuideMacOSX.md)