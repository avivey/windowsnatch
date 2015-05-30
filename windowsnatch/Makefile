TOOLPATH = i586-mingw32msvc-
CC = $(TOOLPATH)cc

CFLAGS  = -Wall -Werror -O2
CFLAGS += -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -DUNICODE -D_UNICODE

# LIBS = -lws2_32  # WinSockets

# remove -mwindows to have an stdout.
# LINK_FLAGS = -mwindows

windowsnatch.exe: windowsnatch.o trayicon.o
	$(CC) $(LINK_FLAGS) -o $@ $^ $(LIBS)

.PHONY: clean style

style:
	astyle -n -s2 -q -pUH *.c *.h

clean:
	rm -f *.o *.exe
