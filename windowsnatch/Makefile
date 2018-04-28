TOOLPATH = x86_64-w64-mingw32-
CC = $(TOOLPATH)gcc

BUILDDIR = ./build

CFLAGS  = -std=gnu11 -Wall -Werror -O2
CFLAGS += -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -DUNICODE -D_UNICODE
CFLAGS += -MMD

# LIBS = -lws2_32  # WinSockets
LIBS = -lhid -lsetupapi  # USB

# remove -mwindows to have an stdout.
# LINK_FLAGS = -mwindows
LINK_FLAGS += -s  # Strip debug information

default: windowsnatch.exe

OBJECT_FILES = windowsnatch.o trayicon.o find_window.o
OBJECT_FILES += usb_hid.o common/icd.o common/icd_dispatch.o handle_messages.o

windowsnatch.exe: $(OBJECT_FILES)

common/configuration.h common/icd-dispatch.c common/icd_messages.h: ../config.py
	$<

trayicon.o windowsnatch.o: common/configuration.h

%.exe: %.o
	$(CC) $(LINK_FLAGS) -o $@ $^ $(LIBS)

.PHONY: clean style

style:
	astyle -n -s2 -q -pUH *.c *.h common/*.h common/*.c

-include *.d

clean:
	rm -f *.o *.exe *.d common/*.o common/*.d
