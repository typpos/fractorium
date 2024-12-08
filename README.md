Fractorium
==========

A Qt-based fractal flame editor which uses a C++ re-write of the flam3 algorithm
named Ember and a GPU capable version named EmberCL which implements a portion
of the cuburn algorithm in OpenCL.

# Installing

## Windows

Download: [Fractorium_24.24.12.1.msi](https://drive.google.com/file/d/130ZUkkFscptPjZSo__WFIwz62YJeUa5j/view?usp=drive_link)

## Mac

Download: [Fractorium_23.23.8.1.dmg](https://drive.google.com/file/d/1M_HSzNMGfzS6NWCVPK-VmkWpMOaLdSuk/view?usp=drive_link)

## Linux

### Install from App Image .deb

Install ubuntu 20 or greater.

Download: [Fractorium-24.24.12.1.x86_64.deb](https://drive.google.com/file/d/130ZUkkFscptPjZSo__WFIwz62YJeUa5j/view?usp=drive_link)

```
cd ~/Downloads
sudo dpkg -i Fractorium-24.24.12.1.x86_64.deb
```

### Install from App Image .rpm

Download: [Fractorium-23.23.8.1.x86_64.rpm](https://drive.google.com/file/d/1te2UijE3OyR5EayWQT2sE4OIr4U0kUad/view?usp=drive_link)

# Building from git

All builds are 64-bit.

## Windows

[Build Guide for Visual Studio 2022 or Qt Creator](Data/BuildGuideQtCreator.md)

## Linux

[Build Guide for Linux](./Data/BuildGuideLinux.md)

## Mac OS/X Sierra, El Capitan, Yosemite and Mavericks

[Build Guide for Mac OS/X](./Data/BuildGuideMacOSX.md)