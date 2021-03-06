.PHONY: clean style lint

default style clean docker_image:
	make -C teensy $@
	make -C windowsnatch $@

clean-gen:
	rm -f \
		common/configuration.h common/icd_messages.h common/icd_dispatch.c	\
		teensy/src/button_names.h	teensy/src/button_setup.inc \
		teensy/src/toolsets_init.inc

style: lint

lint:
	black config.py
	arc lint
