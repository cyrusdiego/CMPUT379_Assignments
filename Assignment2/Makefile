CXXFLAGS = -Wall -O -std=c++11 -pthread
CXX = g++
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)
TARGET = prodcon
LFLAGS = -lpthread

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f core *.o $(TARGET)

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(OBJECTS) -o $(TARGET)