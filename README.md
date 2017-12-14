# firmware-mod-kit-osx

Firmware Modification Kit forked from https://code.google.com/p/firmware-mod-kit/  modded for OSX 10.9

## Installation

```bash
cd src
./configure CPPFLAGS=-I/opt/local/include
make all EXTRA_CFLAGS=-I/opt/local/include
```

**N.B.** The path component of the values for `CPPFLAGS` and `EXTRA_CFLAGS` should
be amended as necessary to fit with the install location of `lzma.h` - see below.

## Prerequisites

* lzma, now part of xz
* zlib
* binwalk

### Macports

```bash
port install xz binwalk
```

By default `/opt/local` is the target for the installation of these so the path
component above should be `/opt/local/include`.

### Homebrew

```bash
brew install xz binwalk
```
