# scalc - Simple calculator

scalc is a very simple stack-based calculator that aims to be small.

## Basic usage

scalc presents the user with an interactive prompt. Operations are written in
Reverse Polish Notation and values are stored in a stack. Supported operations
include basic arithmetics, logarithms, trignometry, etc. scalc also supports
commands that modify the behavior of the stack and scalc itself.

```
$ ./scalc
> 1 2 +
3.000000000
> 4
4.000000000
> +
7.000000000
> 2 / 18 +
21.500000000
> ln
3.068052935
> :quit
```

You may check the ``scalc(1)`` manpage for further usage information.

## Build

scalc requires:

1. A POSIX-like system
2. A C99 compiler
3. [sline 2.0+](https://github.com/ariadnavigo/sline)

Build by using:

```
$ make
```

Customize the build process by changing ``config.mk`` to suit your needs.

User configuration is performed by modifying ``config.h``. A set of defaults is
provided in ``config.def.h``.

## Install

You may install scalc by running the following command as root:

```
# make install
```

This will install the binary under ``$PREFIX/bin``, as defined by your
environment, or ``/usr/local/bin`` by default. The Makefile supports the
``$DESTDIR`` variable as well.

## License

scalc is published under an MIT/X11/Expat-type License. See ``LICENSE`` file
for copyright and license details.
