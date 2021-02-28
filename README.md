# scalc - Simple calculator 

scalc is a very simple calculator that aims to be small, flexible, and
extensible.

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

scalc reads one line RPN expressions from a file passed as its first argument 
or, by default, from stdin. It outputs the result as a single-precision float
to stdout.

```
$ scalc test_file 
5 6766 -
-6761.000000
8 7 /
1.142857
$ echo '4 5 +' | ./scalc
9.000000
```

## License

scalc is published under an MIT/X11/Expat-type License. See LICENSE file for 
copyright and license details.
