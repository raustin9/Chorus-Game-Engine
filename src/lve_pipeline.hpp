#pragma once
#ifndef LVE_PIPELINE
#define LVE_PIPELINE

#include <string>
#include <vector>

namespace lve{

  class LvePipeline {
    public:
      LvePipeline(const std::string& vertexFilepath, std::string& fragmentFilepath);
    private:
      static std::vector<char> read_file(const std::string& filepath);
      void create_graphics_pipeline(const std::string& vertexFilepath, const std::string& fragmentFilepath);
  };

} /* end lve namespace */

#endif /* LVE_PIPELINE */
