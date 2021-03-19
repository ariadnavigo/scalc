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

To exit scalc, hit Ctrl+D (EOF).

## License

scalc is published under an MIT/X11/Expat-type License. See LICENSE file for 
copyright and license details.
