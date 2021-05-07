# thesis-ui

![](https://i.imgur.com/KJDHi5n.gif)

## Implemented rules

* [S1125 - Boolean literals should not be redundant](https://github.com/lyxell/datalog-repair/blob/master/rules/1125.dl)
* [S1132 - Strings literals should be placed on the left side when checking for equality](https://github.com/lyxell/datalog-repair/blob/master/rules/1132.dl)
* [S1155 - Collection.isEmpty() should be used to test for emptiness](https://github.com/lyxell/datalog-repair/blob/master/rules/1155.dl)
* [S1596 - Collections.EMPTY_LIST, EMPTY_MAP, and EMPTY_SET should not be used](https://github.com/lyxell/datalog-repair/blob/master/rules/1596.dl)
* [S2111 - BigDecimal(double) should not be used](https://github.com/lyxell/datalog-repair/blob/master/rules/2111.dl)
* [S2204 - .equals() should not be used to test the values of Atomic classes](https://github.com/lyxell/datalog-repair/blob/master/rules/2204.dl)
* [S2293 - The diamond operator ("<>") should be used](https://github.com/lyxell/datalog-repair/blob/master/rules/2293.dl)
* [S4973 - Strings and Boxed types should be compared using equals()](https://github.com/lyxell/datalog-repair/blob/master/rules/4973.dl)

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
