# Build Guide for Mac OS/X

Install Xcode from the App Store

Install Qt 5.4.2 (Note, newer versions of Qt up to and including 5.8 have a bug with drawing OpenGL lines)

Install [homebrew](http://brew.sh/).

Install `git` and clone the repository:

```
brew install git
git clone https://mfeemster@bitbucket.org/mfeemster/fractorium.git
```

Install the dependencies:

```
brew install qt5 tbb glm jpeg libpng glib libxml2 openexr
```

Add the Qt `bin` folder to `PATH` to make `qmake` available. In
`~/.bash_profile` or `~/.bashrc`:

```
PATH=/usr/local/opt/qt5/bin:$PATH
export PATH
```

Building:

```
cd fractorium
qmake CONFIG+=release
make
```
Or open main.pro in Qt Creator, select release and build all.

Creating the app bundle:

```
cd archive
./build.sh
```

Running the binary from the release folder:

```
cd ..
cd Bin/release
./fractorium
```

Installing:

```
Open Fractorium.dmg and copy Fractorium.app to /Applications.
```
