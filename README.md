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

The result should be a local executable file called `editor` that
when executed shows a graphical user interface.

The graphical user interface has two main views: a text editor
containing a Java file and a view containing the AST for the file
that is currently editing.
