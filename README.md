# scalc - Simple calculator 

scalc is a very simple stack-based calculator that aims to be small, flexible, 
and extensible.

**This software is still in a very early stage of development. Expect many
changes in the near future!**

## Build

scalc doesn't require any external dependencies.

Build by using:

```
$ make
```

Customize the build process by changing ``config.mk`` to suit your needs.

User configuration is performed by modifying ``config.h``. A set of defaults is
provided in ``config.def.h``.

## Usage

scalc may be used in interactively and non-interactively. If run without any
arguments, scalc will present a prompt for the user to input mathematical
expressions in RPN:

```
$ scalc 
> 8 9 +
17.000000
> 1 +
18.000000
```

The stack is preserved across expressions, thus partial expressions reusing the
last result are possible.

When piped some input from another command or by passing a filename as an 
argument, scalc will only show the result of the last operation and exit:

```
$ echo '4 5 +' | scalc
9.000000
$ cat scalc_ex 
67 9 -
sqrt
$ scalc scalc_ex 
7.615773
```

You may find further usage information in the scalc(1) manual page.

### scalc commands

scalc accepts a series of interactive commands. All commands start with a colon
(:) and are typed in lowercase (currently it's case **sensitive**).

* **:drop** Drop the whole stack (essentially AC on a calculator).
* **:p** Peek last element in the stack.
* **:q** Quit scalc.

### Supported math functions and extensibility

Supported operations can be found in the ``op.c`` file. Currently, scalc only
supports basic arithmetic functions, square root (sqrt), and natural logs (ln).

Unlike other extension systems you may find elsewhere, scalc adheres to a 
dwm-inspired[^1] way of extending itself: via source code. You may add new 
functions by following the instructions in that same source file, but in a 
nutshell you define your operations as unary or binary functions returning a
float and register them into the ``op_defs`` array, specifying the information 
that is required. Don't hesitate sending a patch if you wanna share your 
function!

[^1]: https://dwm.suckless.org/

## Contributing

All contributions are welcome! If you wish to send in patches, ideas, or report
a bug, you may do so by sending an email to the 
[scalc-devel](https://lists.sr.ht/~arivigo/scalc-devel) mailing list.

If interested in getting some news from the project, you may also want to 
subscribe to the low-volume 
[scalc-announce](https://lists.sr.ht/~arivigo/scalc-announce) mailing list!

## License

scalc is published under an MIT/X11/Expat-type License. See ``LICENSE'' file
for copyright and license details.
