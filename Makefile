default: all

all: build/elfreloctool

build/:
	mkdir -p $@

OPTIMIZE := -g -O0 -gdwarf-4 -ggdb3

CXXFLAGS += -Ielfio -I. -Iinclude $(OPTIMIZE)

CXXFILES := $(wildcard src/*.cpp) $(wildcard src/*.cc)

build/elfreloctool: $(CXXFILES) | build/
	c++ $(CXXFLAGS) -o $@ $^

clean:
	rm -rf build/

.PHONY: all clean

print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true

