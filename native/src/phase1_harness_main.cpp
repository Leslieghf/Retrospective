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

    bool ok = true;
    auto expect_code = [&](const char* label, std::int32_t actual, std::int32_t expected) {
        if (actual != expected) {
            std::cerr << label << " expected " << expected << " but got " << actual << " (" << loader.status_text(actual)
                      << ")\n";
            ok = false;
        }
    };
    auto expect_u32 = [&](const char* label, std::uint32_t actual, std::uint32_t expected) {
        if (actual != expected) {
            std::cerr << label << " expected " << expected << " but got " << actual << "\n";
            ok = false;
        }
    };

    const auto init_code = loader.init();
    std::cout << "init: " << loader.status_text(init_code) << " (" << init_code << ")\n";
    expect_code("init", init_code, 0);

    const auto map_code = loader.on_map_start("a1_experimental_01");
    std::cout << "map_start: " << loader.status_text(map_code) << " (" << map_code << ")\n";
    expect_code("map_start", map_code, 0);

    const auto spawn_code = loader.on_player_spawn(1);
    std::cout << "player_spawn: " << loader.status_text(spawn_code) << " (" << spawn_code << ")\n";
    expect_code("player_spawn", spawn_code, 0);

    const auto death_code = loader.on_player_death(1, 0);
    std::cout << "player_death: " << loader.status_text(death_code) << " (" << death_code << ")\n";
    expect_code("player_death", death_code, 0);

    for (int i = 0; i < 3; ++i) {
        const auto frame_code = loader.frame(16);
        std::cout << "frame[" << i << "]: " << loader.status_text(frame_code) << " (" << frame_code << ")\n";
        expect_code("frame", frame_code, 0);
    }

    std::cout << "frame_count: " << loader.frame_count() << "\n";
    std::cout << "player_spawn_count: " << loader.player_spawn_count() << "\n";
    std::cout << "player_death_count: " << loader.player_death_count() << "\n";
    std::cout << "last_spawn_player_slot: " << loader.last_spawn_player_slot() << "\n";
    std::cout << "last_death_victim_slot: " << loader.last_death_victim_slot() << "\n";
    std::cout << "last_death_attacker_slot: " << loader.last_death_attacker_slot() << "\n";

    expect_u32("frame_count", loader.frame_count(), 3);
    expect_u32("player_spawn_count", loader.player_spawn_count(), 1);
    expect_u32("player_death_count", loader.player_death_count(), 1);
    expect_u32("last_spawn_player_slot", loader.last_spawn_player_slot(), 1);
    expect_u32("last_death_victim_slot", loader.last_death_victim_slot(), 1);
    expect_u32("last_death_attacker_slot", loader.last_death_attacker_slot(), 0);

    const auto invalid_spawn_code = loader.on_player_spawn(0);
    std::cout << "player_spawn_invalid: " << loader.status_text(invalid_spawn_code) << " (" << invalid_spawn_code << ")\n";
    expect_code("player_spawn_invalid", invalid_spawn_code, -3);

    const auto invalid_death_code = loader.on_player_death(0, 1);
    std::cout << "player_death_invalid: " << loader.status_text(invalid_death_code) << " (" << invalid_death_code << ")\n";
    expect_code("player_death_invalid", invalid_death_code, -4);

    loader.shutdown();
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
