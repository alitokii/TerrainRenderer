CC = g++
CFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lGLEW -lglfw -lGL -lm

# Add any additional source files here
SOURCES = main.cpp

OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = terrain_renderer

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
