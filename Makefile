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

libapib: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@

test-libapib: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@
	mkdir -p ./bin
	cp -f $(BUILD_DIR)/out/$(BUILDTYPE)/$@ ./bin/$@

libapib-parser: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@

test-libapib-parser: config.gypi $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) V=$(V) $@
	mkdir -p ./bin
	cp -f $(BUILD_DIR)/out/$(BUILDTYPE)/$@ ./bin/$@

test-libapib-parser-perf: config.gypi $(BUILD_DIR)/Makefile
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

test: libapib test-libapib libapib-parser test-libapib-parser libdrafter test-libdrafter test-capi drafter
	./bin/test-libapib
	./bin/test-libapib-parser
	./bin/test-libdrafter
	./bin/test-capi

ifdef INTEGRATION_TESTS
	bundle exec cucumber
endif

perf: libapib-parser test-libapib-parser-perf
	./bin/test-libapib-parser-perf ./packages/apib-parser/test/snowcrash/performance/fixtures/fixture-1.apib

.PHONY: all libapib test-libapib libapib-parser libdrafter drafter test test-libapib-parser test-libdrafter perf test-libapib-parser-perf install
