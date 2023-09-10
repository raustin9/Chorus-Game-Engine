#pragma once
#ifndef ENGINE
#define ENGINE

#include "cge_window.hh"
#include "cge_pipeline.hh"

namespace cge {
    class CGE_Engine {
        public:
            CGE_Engine();
            ~CGE_Engine();
            
            void _run();

        private:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;
            CGE_Window _window = CGE_Window(WIDTH, HEIGHT, "Chorus Engine");
            CGE_Pipeline _pipeline = CGE_Pipeline("shaders/simple.vert.spv", "shaders/simple.frag.spv");
    };
}

#endif /* ENGINE */
