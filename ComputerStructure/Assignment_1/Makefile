CXX = g++
CXXFLAGS = -std=c++11 -Wall

TARGET = test

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f $(TARGET)
