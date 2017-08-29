-include config.mk

BUILDTYPE ?= Release
BUILD_DIR ?= ./build
PYTHON ?= python
GYP ?= ./tools/gyp/gyp
DESTDIR ?= /usr/local
BINDIR ?= $(DESTDIR)/bin

# Default to verbose builds
V ?= 1

# Targets
all: drafter

libmarkdownparser: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@

test-libmarkdownparser: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@
	mkdir -p ./bin
	cp -f $(BUILD_DIR)/out/$(BUILDTYPE)/$@ ./bin/$@

libsnowcrash: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@

test-libsnowcrash: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@
	mkdir -p ./bin
	cp -f $(BUILD_DIR)/out/$(BUILDTYPE)/$@ ./bin/$@

perf-libsnowcrash: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@
	mkdir -p ./bin
	cp -f $(BUILD_DIR)/out/$(BUILDTYPE)/$@ ./bin/$@

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

test-capi: config.gypi $(BUILD_DIR)/Makefile
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

test: libmarkdownparser test-libmarkdownparser libsnowcrash test-libsnowcrash libdrafter test-libdrafter test-capi drafter
	./bin/test-libmarkdownparser
	./bin/test-libsnowcrash
	./bin/test-libdrafter
	./bin/test-capi

perf: libsnowcrash perf-libsnowcrash
	./bin/perf-libsnowcrash ./ext/snowcrash/test/performance/fixtures/fixture-1.apib

ifdef INTEGRATION_TESTS
	bundle exec cucumber
endif

.PHONY: all libmarkdownparser test-libmarkdownparser libsnowcrash libdrafter drafter test test-libsnowcrash test-libdrafter perf perf-libsnowcrash install
