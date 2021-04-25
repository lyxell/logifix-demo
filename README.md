# thesis-ui

## Getting started

### Debian/Ubuntu

#### Step 1 (Install Soufflé)

Building might take ~15 minutes to complete.

```
git clone https://github.com/souffle-lang/souffle.git
cd souffle
sudo apt install -y \
    autoconf \
    automake \
    bison \
    build-essential \
    clang \
    doxygen \
    flex \
    g++ \
    git \
    libffi-dev \
    libncurses5-dev \
    libtool \
    libsqlite3-dev \
    make \
    mcpp \
    python \
    sqlite \
    zlib1g-dev
./bootstrap
./configure
make
```

#### Step 2

```
git clone https://github.com/lyxell/thesis-ui.git
cd thesis-ui
git submodule update --init --recursive
sudo apt install -y \
    libsdl2-dev \
    re2c
make
```

#### Usage

The result of the build process should be a local executable file
called `editor` that when executed shows a graphical user
interface.

The graphical user interface has two main views: the source code view
containing a Java file and the AST view containing the AST for
the file in the source code view.

There is two main ways to interact with the source code view.

1. You may edit the file using the keyboard (use arrow keys to
   move the cursor).
2. Any available repairs will be highlighted with a red
   background. You may double-click to perform this repair.

![](https://i.imgur.com/DV55pn4.gif)
