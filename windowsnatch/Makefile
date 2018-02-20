TOOLPATH = x86_64-w64-mingw32-
CC = $(TOOLPATH)gcc

CFLAGS  = -Wall -Werror -O2
CFLAGS += -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -DUNICODE -D_UNICODE

# LIBS = -lws2_32  # WinSockets

# remove -mwindows to have an stdout.
# LINK_FLAGS = -mwindows
LINK_FLAGS += -s  # Strip debug information

default: windowsnatch.exe

windowsnatch.exe: windowsnatch.o trayicon.o

%.exe: %.o
	$(CC) $(LINK_FLAGS) -o $@ $^ $(LIBS)

.PHONY: clean style

style:
	astyle -n -s2 -q -pUH *.c *.h

clean:
	rm -f *.o *.exe
