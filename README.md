Fractorium
==========

A Qt-based fractal flame editor which uses a C++ re-write of the flam3 algorithm
named Ember and a GPU capable version named EmberCL which implements a portion
of the cuburn algorithm in OpenCL.

# Install

## Windows

Download: [Fractorium_23.23.8.1.msi](https://drive.google.com/file/d/1AskP9JLRfKBcVOV0a4m6m4reQh_ogyqJ/view?usp=drive_link)

## Mac

Download: [Fractorium_23.23.8.1.dmg](https://drive.google.com/file/d/1M_HSzNMGfzS6NWCVPK-VmkWpMOaLdSuk/view?usp=drive_link)

## Linux

### Install from App Image .deb

Install ubuntu 20 or greater.

Download: [Fractorium-23.23.8.1.x86_64.deb](https://drive.google.com/file/d/1kMvTDgQpSsGmee-nUT99sb97m90F5ALd/view?usp=drive_link)

```
cd ~/Downloads
sudo dpkg -i Fractorium-23.23.8.1.x86_64.deb
```

### Install from App Image .rpm

Download: [Fractorium-23.23.8.1.x86_64.rpm](https://drive.google.com/file/d/1woZBP8f6qoB53vRfXfrj0XfxEznQD_CD/view?usp=drive_link)

# Building from git

All builds are 64-bit.

## Windows

[Build Guide for Visual Studio 2019 or Qt Creator](Data/BuildGuideQtCreator.md)

## Linux

[Build Guide for Linux](./Data/BuildGuideLinux.md)

## Mac OS/X Sierra, El Capitan, Yosemite and Mavericks

[Build Guide for Mac OS/X](./Data/BuildGuideMacOSX.md)