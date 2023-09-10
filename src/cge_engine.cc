#include "cge_engine.hh"

namespace cge {
    //
    // CONSTRUCTOR
    //
    CGE_Engine::CGE_Engine() {

    }

    //
    // DESTRUCTOR
    //
    CGE_Engine::~CGE_Engine() {

    }

    //
    // RUN THE ENGINE
    //
    void
    CGE_Engine::_run() {
        this->_window._open_window();
    }
}
