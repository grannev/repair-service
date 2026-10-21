CPPC = g++
CPPFLAGS = -g -Wall -std=c++98
TARGET = ./bin/server
SOURCE = *.cpp

static: mk-build-dir build

mk-build-dir:
	mkdir -p ./bin

build:
	$(CPPC) $(CPPFLAGS) $(SOURCE) -o $(TARGET)

run: mk-build-dir build
	$(TARGET)

clean:
	rm -rf bin

