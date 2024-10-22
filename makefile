CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -I.
LDFLAGS = -lGLEW -lglfw -lGL -lm

# Source files
SOURCES = main.cpp shaders.cpp perlin.cpp terrain.cpp window.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = shaders.h perlin.h terrain.h window.h


EXECUTABLE = terrain_renderer

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
