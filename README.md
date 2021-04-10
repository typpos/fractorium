Fractorium
==========

A Qt-based fractal flame editor which uses a C++ re-write of the flam3 algorithm
named Ember and a GPU capable version named EmberCL which implements a portion
of the cuburn algorithm in OpenCL.

# Install

## Windows

Download: [Fractorium_21.21.4.1.msi](https://drive.google.com/file/d/1O-8Hh7rH1911teQDnc8iTeC1oCtn6320/view?usp=sharing)

## Mac

Download: [Fractorium_21.21.4.1.dmg](https://drive.google.com/file/d/1dJB9uYA-JCjPOqe_0MjlaCE_oNX0KUPq/view?usp=sharing)

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

Download: [Fractorium-21.21.4.1.x86_64.deb](https://drive.google.com/file/d/1LfaHuWr5_Ns_V_NDiFy7H8OrlveJJG_e/view?usp=sharing)

```
cd ~/Downloads
sudo dpkg -i Fractorium-21.21.4.1.x86_64.deb
```

### Install from App Image .rpm

Download: [Fractorium-21.21.4.1.x86_64.rpm](https://drive.google.com/file/d/1gGV55CP4KxY9vc42SGvWRWhlo4NkUXK9/view?usp=sharing)

# Building from git

All builds are 64-bit.

## Windows

[Build Guide for Visual Studio 2019 or Qt Creator](Data/BuildGuideQtCreator.md)

## Linux

[Build Guide for Linux](./Data/BuildGuideLinux.md)

## Mac OS/X Sierra, El Capitan, Yosemite and Mavericks

[Build Guide for Mac OS/X](./Data/BuildGuideMacOSX.md)