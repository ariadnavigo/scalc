# scalc version
VERSION = 0.3.2

# Customize below to your needs

# Paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/man

# Libraries
LIBS = -lm -lsline

# Flags
CPPFLAGS = -I${PREFIX}/include -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L
#CFLAGS = -g -std=c99 -Wpedantic -Wall -Wextra
CFLAGS = -std=c99 -Wpedantic -Wall -Wextra
LDFLAGS = -L${PREFIX}/lib

# Compiler and linker
CC = cc

