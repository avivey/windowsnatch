.PHONY: clean style

default style clean:
	make -C teensy $@
	make -C windowsnatch $@

style: lint

lint:
	arc lint
