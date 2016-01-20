Fractorium
==========

A Qt-based fractal flame editor which uses a C++ re-write of the flam3 algorithm
named Ember and a GPU capable version named EmberCL which implements a portion
of the cuburn algorithm in OpenCL.

# Install

## Windows

Download: [Fractorium_Beta_0.9.9.3.msi](https://drive.google.com/file/d/0Bws5xPbHJph6Z3JiTHlYZ21YVFU/view?usp=sharing)

## Linux

Enable `universe` in the Ubuntu Software Center:

- open the Edit menu
- select Software Sources...
- enable "Community maintained ... `universe`"

Add the Ubuntu PPA and install:

```
sudo apt-add-repository ppa:fractorium/ppa
sudo apt-get update
sudo apt-get install fractorium
```

## Mac OS/X (10.9+)

TODO

# Building from git

## Windows

Install Git-GUI and clone `https://github.com/mfeemster/fractorium.git`

Then follow:

[**Building Guide for Fractorium Using MSVC2013 and Qt Creator (64 bit)**](Data/BuildGuideQtCreator.md)

## Linux

See [Building Guide for Linux](./Data/BuildGuideLinux.md)

## Mac OS/X 10.9+

See [Building Guide for Mac OS/X](./Data/BuildGuideMacOSX.md)
