Fractorium
==========

A Qt-based fractal flame editor which uses a C++ re-write of the flam3 algorithm
named Ember and a GPU capable version named EmberCL which implements a portion
of the cuburn algorithm in OpenCL.

# Install

## Windows

Download: [Fractorium_1.0.0.19.msi](https://drive.google.com/open?id=1GerFr8VRLFtfvV57acJawZg0bJVfhac-)

## Mac

Download: [Fractorium_1.0.0.19.dmg](https://drive.google.com/open?id=1YH49dE858cUrPXl92jfclB00LDPQfBtY)

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

Download: [Fractorium-1.0.0.19.x86_64.deb](https://drive.google.com/open?id=1sYoLK27-w7RERxmh10GK4eTcwI2wuagO)

```
cd ~/Downloads
sudo dpkg -i Fractorium-1.0.0.19.x86_64.deb
```

### Install from App Image .rpm

Download: [Fractorium-1.0.0.19.x86_64.rpm](https://drive.google.com/open?id=17zHmJghSM_hCjSNRXyf5PL6oWoy--Sq4)

# Building from git

All builds are 64-bit.

## Windows

[Build Guide for Visual Studio 2017 or Qt Creator](Data/BuildGuideQtCreator.md)

## Linux

[Build Guide for Linux](./Data/BuildGuideLinux.md)

## Mac OS/X Sierra, El Capitan, Yosemite and Mavericks

[Build Guide for Mac OS/X](./Data/BuildGuideMacOSX.md)