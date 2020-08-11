
.PHONY: all clean

all:
	touch src/.depend
	@$(MAKE) -C src depend
	@$(MAKE) -C src all

clean:
	@$(MAKE) -C src clean

