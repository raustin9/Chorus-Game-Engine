CC=g++
CFLAGS=-std=c++17 -O2 -Wall
INCLUDES=-Iinclude
LDFLAGS=-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

all: bin/vulkan_test
	
run: bin/vulkan_test
	./$<

bin/vulkan_test: obj/main.o obj/lve_pipeline.o
	$(CC) $(CFLAGS) obj/main.o obj/lve_pipeline.o -o $@ $(LDFLAGS)

obj/main.o: src/main.cc 
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ $< $(LDFLAGS)

obj/lve_pipeline.o: src/lve_pipeline.cc
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS)

clean:
	rm -f bin/* obj/* lib/*
