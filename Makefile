-include config.mk

BUILDTYPE ?= Release
BUILD_DIR ?= ./build
PYTHON ?= python
GYP ?= ./ext/snowcrash/tools/gyp/gyp
DESTDIR ?= /usr/local
BINDIR ?= $(DESTDIR)/bin

# Default to verbose builds
V ?= 1

# Targets
all: drafter

libdrafter: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@

test-libdrafter: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@
	mkdir -p ./bin
	cp -f $(BUILD_DIR)/out/$(BUILDTYPE)/$@ ./bin/$@

drafter: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@
	mkdir -p ./bin
	cp -f $(BUILD_DIR)/out/$(BUILDTYPE)/$@ ./bin/$@

install: drafter
	mkdir -p $(BINDIR)
	cp -f $(BUILD_DIR)/out/$(BUILDTYPE)/drafter $(BINDIR)/drafter

config.gypi: configure
	$(PYTHON) ./configure

$(BUILD_DIR)/Makefile:
	$(GYP) -f make --generator-output $(BUILD_DIR) --depth=.

clean:
	rm -rf $(BUILD_DIR)/out
	rm -rf ./bin

distclean:
	rm -rf ./build
	rm -f ./config.mk
	rm -f ./config.gypi
	rm -rf ./bin

test: libdrafter test-libdrafter drafter
	./bin/test-libdrafter

ifdef INTEGRATION_TESTS
	bundle exec cucumber
endif

.PHONY: all libdrafter drafter test test-libdrafter install
