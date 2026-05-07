#include "retrospective_loader.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    const std::string default_path = "target/release/libretrospective_core.so";
    const std::string lib_path = argc > 1 ? argv[1] : default_path;

    RetrospectiveLoader loader;
    std::string error;
    if (!loader.load(lib_path, error)) {
        std::cerr << "load failed: " << error << "\n";
        std::cerr << "tip: pass explicit path to .so if needed\n";
        return EXIT_FAILURE;
    }

    std::cout << "api_version: " << loader.api_version() << "\n";
    std::cout << "core_id: " << loader.core_id() << "\n";

    const auto init_code = loader.init();
    std::cout << "init: " << loader.status_text(init_code) << " (" << init_code << ")\n";

    const auto map_code = loader.on_map_start("a1_experimental_01");
    std::cout << "map_start: " << loader.status_text(map_code) << " (" << map_code << ")\n";

    for (int i = 0; i < 3; ++i) {
        const auto frame_code = loader.frame(16);
        std::cout << "frame[" << i << "]: " << loader.status_text(frame_code) << " (" << frame_code << ")\n";
    }

    std::cout << "frame_count: " << loader.frame_count() << "\n";
    loader.shutdown();
    return EXIT_SUCCESS;
}

