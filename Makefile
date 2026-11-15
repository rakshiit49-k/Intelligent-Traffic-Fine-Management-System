# Traffic Fine & Violation Management System
CXX      ?= g++
CXXFLAGS  = -std=c++11 -Wall -Wextra
SRC       = src/main.cpp $(wildcard src/*.h)

build/traffic: $(SRC)
	mkdir -p build
	$(CXX) $(CXXFLAGS) src/main.cpp -o build/traffic

run: build/traffic
	./build/traffic

gui: build/traffic
	python3 gui/gui.py

clean:
	rm -rf build records.txt

.PHONY: run gui clean
