CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -I.
LDFLAGS = -lGLEW -lglfw -lGL -lm

# Source files
SOURCES = main.cpp terrain.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = terrain.h


EXECUTABLE = terrain_renderer

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
