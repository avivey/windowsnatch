DOCKER_IMAGE_NAME = aviv-teensy:latest

# The name of your project (used to name the compiled .hex file)
TARGET ?= $(notdir $(CURDIR))

# directory to build in
BUILDDIR = build

# path location for Teensy 3 core
COREPATH = teensy-core

# path location for Arduino libraries
LIBRARYPATH = libraries

RUN_COMMAND = \
	docker run --rm -ti \
		-v $(abspath ./src):/workdir/src:ro \
		-v $(abspath ../common):/workdir/src/common:ro	\
		-v $(abspath $(LIBRARYPATH)):/workdir/$(LIBRARYPATH):ro \
		-v $(abspath $(COREPATH)):/workdir/$(COREPATH):ro \
		-v $(abspath Makefile.indocker):/workdir/Makefile:ro	\
		-v $(abspath $(BUILDDIR)):/workdir/$(BUILDDIR) \

.PHONY: shell docker_image all default generated

default all: generated $(BUILDDIR)
	$(RUN_COMMAND) $(DOCKER_IMAGE_NAME) make TARGET=$(TARGET)
	cp $(BUILDDIR)/*.hex .

shell: generated $(BUILDDIR)
	$(RUN_COMMAND) -ti $(DOCKER_IMAGE_NAME) bash

GENERATED_FILES = src/common/configuration.h src/common/icd_messages.h\
		src/common/icd_dispatch.c src/button_names.h src/button_setup.inc \
		src/toolsets_init.inc

generated: $(GENERATED_FILES)

$(GENERATED_FILES): ../config.py
	$<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

docker_image:
	# This form means the "context" is not copied over.
	docker build -t $(DOCKER_IMAGE_NAME) - < Dockerfile

style:
	astyle -n -s2 -q -pUH src/*.c src/*.h src/*.inc t/*.c t/*.c t/stubs/*.h

test:
	make -C t

clean:
	rm -rf "$(BUILDDIR)" *.elf "$(TARGET).hex"  \
				t/test t/*.o
