CC=g++
CFLAGS=-std=c++17 -O2 -Wall
INCLUDES=-Iinclude
LDFLAGS=-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

all: bin/vulkan_test
	
run: bin/vulkan_test
	./$<

bin/vulkan_test: obj/main.o obj/cge_engine.o obj/cge_device.o obj/cge_swap_chain.o obj/cge_pipeline.o obj/cge_window.o
	$(CC) $(CFLAGS) obj/main.o obj/cge_engine.o obj/cge_device.o obj/cge_swap_chain.o obj/cge_window.o obj/cge_pipeline.o  -o $@ $(LDFLAGS)

obj/main.o: src/main.cc 
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ $< $(LDFLAGS)

obj/cge_pipeline.o: src/cge_pipeline.cc
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS)

obj/cge_window.o: src/cge_window.cc
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS)

obj/cge_engine.o: src/cge_engine.cc
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS)

obj/cge_device.o: src/cge_device.cc
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS)

obj/cge_swap_chain.o: src/cge_swap_chain.cc
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS)

clean:
	rm -f bin/* obj/* lib/*
