default: all

all: build/elfreloctool

build/:
	mkdir -p $@

CXXFLAGS += -Ielfio -I.

build/elfreloctool: expose_relocation_table.cpp | build/
	c++ $(CXXFLAGS) -o $@ $<
