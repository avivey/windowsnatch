TEENSY_CORE_SPEED = 48000000
COREPATH=../teensy-core

OPTIONS = -D USB_RAWHID -DLAYOUT_US_ENGLISH
CFLAGS = -Wall $(OPTIONS) -DF_CPU=$(TEENSY_CORE_SPEED) -Istubs

.PHONY: clean

runtest: test
	./test

test: test.c ../src/toolset.c
	$(CC) $(CFLAGS) -o "$@" $^

clean:
	rm -f *.o test
