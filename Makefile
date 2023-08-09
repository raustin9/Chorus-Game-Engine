CC=g++
CFLAGS=-std=c++17 -O2
LDFLAGS=-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

all: bin/vulkan_test
	
run: bin/vulkan_test
	./$<

bin/vulkan_test: src/main.cpp
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f bin/* obj/* lib/*
