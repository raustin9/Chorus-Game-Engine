CC=g++
GLSLC=/usr/local/bin/glslc
CFLAGS=-std=c++17
INCLUDES=-Iinclude -Ilib
#LDFLAGS=-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
LDFLAGS=-lglfw -lvulkan -ldl -lX11 -lXxf86vm -lXrandr -lXi
OBJS=obj/cge_engine.o obj/cge_game_object.o obj/keyboard_movement_controller.o obj/cge_camera.o obj/simple_render_system.o obj/cge_renderer.o obj/cge_model.o obj/cge_device.o obj/cge_swap_chain.o obj/cge_pipeline.o obj/cge_window.o


# Compile the shaders
vertsources = $(shell find ./shaders/vert -type f -name "*.vert")
vertobjfiles = $(patsubst %.vert, %.vert.spv, $(vertsources))
fragsources = $(shell find ./shaders/frag -type f -name "*.frag")
fragobjfiles = $(patsubst %.frag, %.frag.spv, $(fragsources))

# Main targets
all: bin/vulkan_test $(vertobjfiles) $(fragobjfiles)
	
run: bin/vulkan_test $(vertobjfiles) $(fragobjfiles)
	./$<

clean:
	rm -f bin/* obj/* lib/* shaders/*/*.spv

# Shader targets
%.spv: %
	$(GLSLC) $< -o $@

bin/vulkan_test: obj/main.o $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@ $(LDFLAGS)

obj/%.o: src/%.cc
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS)
