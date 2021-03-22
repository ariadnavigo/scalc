# scalc - Simple calculator 

scalc is a very simple stack-based calculator that aims to be 
small, flexible, and extensible.

**This software is still in a very early stage of development.
Expect many changes in the near future!**

## Build

scalc doesn't require any external dependencies.

Build by using:

```
$ make
```

Customize the build process by changing ``config.mk`` to suit your needs.

User configuration is performed by modifying ``config.h``.
A set of defaults is provided in ``config.def.h``.

### Extending scalc

Unlike other extension systems you may find elsewhere, 
scalc adheres to a dwm-inspired[^1] way of extending itself: via source code. 
You may add new functions by following the instructions in that same source 
file, 
but in a nutshell you define your operations as unary or binary functions 
returning a float and register them into the ``op_defs`` array, 
specifying the information that is required. 
Don't hesitate sending a patch if you wanna share your function!

[^1]: https://dwm.suckless.org/

## Install

You may install scalc by running the following command as root:

```
# make install
```

This will install the binary under ``$PREFIX/bin``, 
as defined by your environment, 
or ``/usr/local/bin`` by default. 
The Makefile supports the ``$DESTDIR`` variable as well.

## Contributing

All contributions are welcome! 
If you wish to send in patches, ideas, or report a bug, 
you may do so by sending an email to the 
[scalc-devel](https://lists.sr.ht/~arivigo/scalc-devel) mailing list.

If interested in getting some news from the project, 
you may also want to subscribe to the low-volume 
[scalc-announce](https://lists.sr.ht/~arivigo/scalc-announce) mailing list!

## License

scalc is published under an MIT/X11/Expat-type License. 
See ``LICENSE'' file for copyright and license details.
