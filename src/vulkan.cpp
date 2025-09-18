#include "VulkanContext.hpp"
#include <spdlog/spdlog.h>

int main() {
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#ifdef TRACE
    spdlog::set_level(spdlog::level::trace);
#endif
#endif

    CVulkanContext context(1024);

    return 0;
}
