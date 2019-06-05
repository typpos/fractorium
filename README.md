Fractorium
==========

A Qt-based fractal flame editor which uses a C++ re-write of the flam3 algorithm
named Ember and a GPU capable version named EmberCL which implements a portion
of the cuburn algorithm in OpenCL.

# Install

## Windows

Download: [Fractorium_1.0.0.16.msi](https://drive.google.com/open?id=1FY43J-NbiaOIqgspGJDugFqVjan43xAl)

## Mac

Download: [Fractorium_1.0.0.16.dmg](https://drive.google.com/open?id=1Jbwkg53ncjFaQhBYnRrXykyUDEao5mra)

## Linux - Only bionic builds of ubuntu are supported.

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

Install Bionic ubuntu or later.

Download: [Fractorium-1.0.0.16-.x86_64.deb](https://drive.google.com/open?id=14EJv5zKt6iohgTOT8J0AIyi8u1NIM_v5)

```
cd ~/Downloads
sudo dpkg -i Fractorium-1.0.0.16-.x86_64.deb
```

### Install from App Image .rpm

Download: [Fractorium-1.0.0.16.x86_64.rpm](https://drive.google.com/open?id=1nzvIJ6gc7uAk6zubTPy4NIuMWCnCC2dx)

# Building from git

All builds are 64-bit.

## Windows

[Build Guide for Visual Studio 2017 or Qt Creator](Data/BuildGuideQtCreator.md)

## Linux

[Build Guide for Linux](./Data/BuildGuideLinux.md)

## Mac OS/X Sierra, El Capitan, Yosemite and Mavericks

[Build Guide for Mac OS/X](./Data/BuildGuideMacOSX.md)