# scalc version
VERSION = 0.0.0

# Customize below to your needs

# Paths
PREFIX = /usr/local

# Libraries
LIBS = -lm

# Flags
CPPFLAGS = -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L
CFLAGS = -g -std=c99 -Wpedantic -Wall -Wextra ${CPPFLAGS}
#CFLAGS = -std=c99 -Wpedantic -Wall -Wextra ${CPPFLAGS}
#LDFLAGS = -static ${LIBS}
LDFLAGS = ${LIBS}

# Compiler and linker
CC = cc

