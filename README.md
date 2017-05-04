[![MIT License](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/sop/cygextreg/blob/master/LICENSE)

# CygExtReg

A utility program allowing to register an extension (eg. `.sh`) to be
executed in [Cygwin][] by double-clicking a file from Windows File Explorer
or by dragging and dropping files to an icon of a registered extension.

[cygwin]: https://www.cygwin.com/

You can use any scripting language installed on your Cygwin.
Just define the preferred [shebang][] line, eg. `#!/usr/bin/python3`

[shebang]: https://en.wikipedia.org/wiki/Shebang_(Unix)

## Installing

Install `cygextreg` via Cygwin setup.

To install manually, download the [latest version][] for
32-bit (*i686*) or 64-bit (*x86\_64*) Cygwin installation.
Extract the contents of a zip to the root of your Cygwin directory.

[latest version]: https://github.com/sop/cygextreg/releases/latest

## Installing from source

Install Cygwin packages:

    gcc-g++ make automake autoconf

Get the source:

    git clone https://github.com/sop/cygextreg.git

Prepare environment:

    aclocal && autoheader && automake --add-missing && autoconf

Compile and install:

    ./configure && make && make install-strip

## Usage

Register default (`.sh`) filetype:

    cygextreg -r

Default is to register only for the current user. To register for all
users, use `cygextreg -ra`. Note that this prompts for an elevated process.

To unregister default filetype:

    cygextreg -u

To register another filetype (eg. `.bash`), pass the extension
as an `--ext` argument:

    cygextreg -r --ext bash

## Internals

Scripts are executed with bash in an interactive login shell.
This means that your `~/.bash_profile` will be executed first, which usually
sources `~/.bashrc` as well. This way you can alter your environment,
eg. by modifying the `$PATH` variable.

Bash is started in a [MinTTY] terminal with UTF-8 charset.
All arguments that are Windows style paths are automatically converted to
Cygwin equivalents (`/cygdrive/...`). So if you drag and drop a file to
a script icon, the script receives the dropped file's path in Cygwin format
as a first argument.

[MinTTY]: https://mintty.github.io/

Multiple files can be dragged and dropped to a registered file type.
The script receives paths as separate arguments,
eg. `$1`, `$2`, `$3`, etc. (or `$@`) in bash script or `sys.argv[1:]` in Python.

If the executed script exits with a non-zero code, MinTTY window shall be
kept open so that you have a chance to review the output.
If the script succeeds (exits with code 0), MinTTY window is closed
automatically.

## License

This project is licensed under the
[MIT License](https://github.com/sop/cygextreg/blob/master/LICENSE).
