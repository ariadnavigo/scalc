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

Customize the build process by changing config.mk to suit your needs.

## Usage

scalc reads RPN expressions from a file passed as its first argument or, by 
default, from stdin. It outputs the result as a single-precision float to 
stdout.

```
$ scalc test_file 
5 6766 -
-6761.000000
8 7 /
1.142857
$ echo '4 5 +' | scalc
9.000000
```

The stack is persistent through the whole session, so you can use partial 
expressions if you need to operate on the last answer you were given!

```
$ scalc test_file2
9 9 *
81.000000
sqrt
9.000000
```

### scalc commands

scalc accepts a series of interactive commands. All commands start with a colon
(:) and are typed in lowercase (currently it's case **sensitive**).

1. **:q** Quit scalc.
2. **:p** Peek last element in the stack.
3. **:drop** Drop the whole stack (essentially AC on a calculator).

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

## License

scalc is published under an MIT/X11/Expat-type License. See LICENSE file for 
copyright and license details.
