# Building Guide for Mac OS/X

Install Xcode from the App Store. Install [homebrew](http://brew.sh/).

Install `git` and clone the repository:

```
brew install git
git clone https://github.com/mfeemster/fractorium
```

Install the dependencies:

```
brew install qt5 tbb glm dbus jpeg libpng glib libxml2
```

TODO: Confirm if `glib` and `libxml2` are actually needed.

Add the Qt `bin` folder to `PATH` to make `qmake` available. In
`~/.bash_profile` or `~/.bashrc`:

```
PATH=/usr/local/opt/qt5/bin:$PATH
export PATH
```

Compile the binary:

```
cd fractorium
qmake CONFIG-=app_bundle
make
```

Run the binary from the release folder:

```
cd Bin/release
./fractorium
```

