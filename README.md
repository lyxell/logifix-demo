# thesis-ui

![](https://i.imgur.com/KJDHi5n.gif)

## Getting started

### Debian/Ubuntu

#### Step 1 (Install Soufflé)

Building might take ~15 minutes to complete.

```bash
git clone --depth 1 https://github.com/souffle-lang/souffle.git
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

#### Step 2 (Building)

```bash
# Dependencies
sudo apt install -y libsdl2-dev re2c
# Build
git clone https://github.com/lyxell/thesis-ui.git
cd thesis-ui
git submodule update --init --recursive
export SOUFFLE_PATH=/path/to/where/you/built/souffle
make
```

## Usage

**Using the user interface**

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

**Defining new repairs**

1. Create a new file in `datalog-repair/rules/`

2. Include this file in `datalog-repair/repair.dl`

3. Re-run `make`

See `datalog-repair/rules/1155.dl` for reference. However,
currently there is no API reference for how to match AST nodes,
this is a work in progress.
