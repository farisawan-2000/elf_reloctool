default: all

all: build/elfreloctool

build/:
	mkdir -p $@

CXXFLAGS += -Ielfio -I. -Iinclude

CXXFILES := $(wildcard src/*.cpp) $(wildcard src/*.cc)

build/elfreloctool: $(CXXFILES) | build/
	c++ $(CXXFLAGS) -o $@ $^

print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true

