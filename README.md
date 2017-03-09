[![MIT License](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/sop/cygscript/blob/master/LICENSE)

# Cygscript
A helper program that allows you to register `.sh` filetype to be executed
in [Cygwin](https://www.cygwin.com/) by double-clicking a file from
Windows explorer or dropping a file to `.sh` icon.

## Installation
Prepare environment:

    autoheader && autoconf && automake

Compile and install:

    ./configure && make && make install-strip

## Usage
Register `.sh` filetype:

    cygscript -r

Default is to register only for the current user. To register for all
users, use `cygscript -ra`. Note that this prompts for an elevated process.

To unregister filetype:

    cygscript -u

## License
This project is licensed under the [MIT License](https://github.com/sop/cygscript/blob/master/LICENSE).
