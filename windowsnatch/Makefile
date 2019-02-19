DOCKER_IMAGE_NAME = aviv-mingw:latest

BUILDDIR = ./build

RUN_COMMAND = \
	docker run --rm -ti \
		-v $(abspath .):/workdir/src/:ro \
		-v $(abspath ../common):/workdir/src/common/:ro	\
		-v $(abspath Makefile.indocker):/workdir/Makefile:ro	\
		-v $(abspath $(BUILDDIR)):/workdir/$(BUILDDIR) \

.PHONY: shell docker_image all default generated

default all: generated $(BUILDDIR)
	$(RUN_COMMAND) $(DOCKER_IMAGE_NAME) make TARGET=$(TARGET)
	cp $(BUILDDIR)/*.exe .

shell: generated $(BUILDDIR)
	$(RUN_COMMAND) -ti $(DOCKER_IMAGE_NAME) bash

GENERATED_FILES = common/configuration.h common/icd_dispatch.c \
	common/icd_messages.h

generated: $(GENERATED_FILES)

$(GENERATED_FILES): ../config.py
	$<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/common/

docker_image:
	# This form means the "context" is not copied over.
	docker build -t $(DOCKER_IMAGE_NAME) - < Dockerfile

style:
	astyle -n -s2 -q -pUH *.c *.h common/*.h common/*.c

clean:
	rm -rf *.o *.exe *.d common/*.o common/*.d build/
