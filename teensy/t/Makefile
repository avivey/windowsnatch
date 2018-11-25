TEENSY_CORE_SPEED = 48000000
COREPATH=../teensy-core

OPTIONS = -D USB_RAWHID -DLAYOUT_US_ENGLISH
CFLAGS = -Wall $(OPTIONS) -DF_CPU=$(TEENSY_CORE_SPEED) -Istubs

.PHONY: clean

runtest: test
	./test

TEST_FILES  = test.c ../src/toolset.c
TEST_FILES += test_handle_message.c  ../src/handle_messages.c
TEST_FILES += ../src/common/icd.c ../src/common/icd_dispatch.c
test: $(TEST_FILES)
	$(CC) $(CFLAGS) -o "$@" $^

clean:
	rm -f *.o test
