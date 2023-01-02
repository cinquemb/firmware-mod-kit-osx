# firmware-mod-kit-osx

Firmware Modification Kit forked from https://code.google.com/p/firmware-mod-kit/  modded for OSX 10.9

## Installation and Prerequisites

### Checkout [kiwiroy's README.md](https://github.com/kiwiroy/firmware-mod-kit-osx/blob/f/readme/README.md)

## Cannot find `lzma.h`

### Homebrew
```bash
LZMA=$(brew info xz | grep homebrew/ | cut -d ' ' -f 1)  \
./configure LDFLAGS="-L${LZMA}/lib" CPPFLAGS="-I${LZMA}/include"

# LZMA=$(brew info xz | grep homebrew/ | cut -d ' ' -f 1)  \
make all EXTRA_CFLAGS="-I${LZMA}/include"
```

### Macports
As mentioned by kiwiroy, macports installs at `/opt/local/`

```bash
./configure LDFLAGS="-L/opt/local/lib" CPPFLAGS="-I/opt/local/include"

make all EXTRA_CFLAGS="-I/opt/local/include"
```
