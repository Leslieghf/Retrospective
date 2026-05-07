#include "retrospective_loader.hpp"

#include <string>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace {
#if defined(_WIN32)
std::string win32_last_error() {
    const DWORD code = GetLastError();
    if (code == 0) {
        return "unknown";
    }

    LPSTR message_buffer = nullptr;
    const DWORD size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&message_buffer),
        0,
        nullptr);

    std::string message = "win32_error_" + std::to_string(code);
    if (size > 0 && message_buffer != nullptr) {
        message.assign(message_buffer, size);
        LocalFree(message_buffer);
    }
    return message;
}

void* open_module(const char* path, std::string& error_out) {
    HMODULE module = LoadLibraryA(path);
    if (module == nullptr) {
        error_out = "LoadLibraryA failed: " + win32_last_error();
        return nullptr;
    }
    return reinterpret_cast<void*>(module);
}

void close_module(void* handle) {
    FreeLibrary(reinterpret_cast<HMODULE>(handle));
}

template <typename T>
bool bind_symbol(void* handle, const char* name, T& out_fn, std::string& error_out) {
    auto raw = GetProcAddress(reinterpret_cast<HMODULE>(handle), name);
    if (raw == nullptr) {
        error_out = std::string("missing symbol: ") + name + " (GetProcAddress failed: " + win32_last_error() + ")";
        return false;
    }
    out_fn = reinterpret_cast<T>(raw);
    return true;
}
#else
void* open_module(const char* path, std::string& error_out) {
    void* module = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (module == nullptr) {
        const char* err = dlerror();
        error_out = err != nullptr ? err : "dlopen failed";
    }
    return module;
}

void close_module(void* handle) {
    dlclose(handle);
}

template <typename T>
bool bind_symbol(void* handle, const char* name, T& out_fn, std::string& error_out) {
    dlerror();
    out_fn = reinterpret_cast<T>(dlsym(handle, name));
    if (const char* err = dlerror(); err != nullptr) {
        error_out = std::string("missing symbol: ") + name + " (" + err + ")";
        return false;
    }
    return true;
}
#endif
} // namespace

RetrospectiveLoader::~RetrospectiveLoader() {
    unload();
}

bool RetrospectiveLoader::load(const std::string& lib_path, std::string& error_out) {
    unload();

    handle_ = open_module(lib_path.c_str(), error_out);
    if (handle_ == nullptr) {
        return false;
    }

    if (!bind_symbol(handle_, "retro_api_version", retro_api_version_, error_out) ||
        !bind_symbol(handle_, "retro_core_id", retro_core_id_, error_out) ||
        !bind_symbol(handle_, "retro_init", retro_init_, error_out) ||
        !bind_symbol(handle_, "retro_shutdown", retro_shutdown_, error_out) ||
        !bind_symbol(handle_, "retro_frame", retro_frame_, error_out) ||
        !bind_symbol(handle_, "retro_on_map_start", retro_on_map_start_, error_out) ||
        !bind_symbol(handle_, "retro_on_player_spawn", retro_on_player_spawn_, error_out) ||
        !bind_symbol(handle_, "retro_on_player_death", retro_on_player_death_, error_out) ||
        !bind_symbol(handle_, "retro_frame_count", retro_frame_count_, error_out) ||
        !bind_symbol(handle_, "retro_player_spawn_count", retro_player_spawn_count_, error_out) ||
        !bind_symbol(handle_, "retro_player_death_count", retro_player_death_count_, error_out) ||
        !bind_symbol(handle_, "retro_last_spawn_player_slot", retro_last_spawn_player_slot_, error_out) ||
        !bind_symbol(handle_, "retro_last_death_victim_slot", retro_last_death_victim_slot_, error_out) ||
        !bind_symbol(handle_, "retro_last_death_attacker_slot", retro_last_death_attacker_slot_, error_out) ||
        !bind_symbol(handle_, "retro_status_text", retro_status_text_, error_out)) {
        unload();
        return false;
    }

    return true;
}

void RetrospectiveLoader::unload() {
    if (handle_ != nullptr) {
        close_module(handle_);
        handle_ = nullptr;
    }

    retro_api_version_ = nullptr;
    retro_core_id_ = nullptr;
    retro_init_ = nullptr;
    retro_shutdown_ = nullptr;
    retro_frame_ = nullptr;
    retro_on_map_start_ = nullptr;
    retro_on_player_spawn_ = nullptr;
    retro_on_player_death_ = nullptr;
    retro_frame_count_ = nullptr;
    retro_player_spawn_count_ = nullptr;
    retro_player_death_count_ = nullptr;
    retro_last_spawn_player_slot_ = nullptr;
    retro_last_death_victim_slot_ = nullptr;
    retro_last_death_attacker_slot_ = nullptr;
    retro_status_text_ = nullptr;
}

bool RetrospectiveLoader::is_loaded() const {
    return handle_ != nullptr;
}

std::uint32_t RetrospectiveLoader::api_version() const {
    return retro_api_version_ != nullptr ? retro_api_version_() : 0;
}

const char* RetrospectiveLoader::core_id() const {
    return retro_core_id_ != nullptr ? retro_core_id_() : "unloaded";
}

std::int32_t RetrospectiveLoader::init() const {
    return retro_init_ != nullptr ? retro_init_() : -1;
}

void RetrospectiveLoader::shutdown() const {
    if (retro_shutdown_ != nullptr) {
        retro_shutdown_();
    }
}

std::int32_t RetrospectiveLoader::frame(std::uint32_t dt_ms) const {
    return retro_frame_ != nullptr ? retro_frame_(dt_ms) : -1;
}

std::int32_t RetrospectiveLoader::on_map_start(const char* map_name) const {
    return retro_on_map_start_ != nullptr ? retro_on_map_start_(map_name) : -1;
}

std::int32_t RetrospectiveLoader::on_player_spawn(std::uint32_t player_slot) const {
    return retro_on_player_spawn_ != nullptr ? retro_on_player_spawn_(player_slot) : -1;
}

std::int32_t RetrospectiveLoader::on_player_death(std::uint32_t victim_slot, std::uint32_t attacker_slot) const {
    return retro_on_player_death_ != nullptr ? retro_on_player_death_(victim_slot, attacker_slot) : -1;
}

std::uint32_t RetrospectiveLoader::frame_count() const {
    return retro_frame_count_ != nullptr ? retro_frame_count_() : 0;
}

std::uint32_t RetrospectiveLoader::player_spawn_count() const {
    return retro_player_spawn_count_ != nullptr ? retro_player_spawn_count_() : 0;
}

std::uint32_t RetrospectiveLoader::player_death_count() const {
    return retro_player_death_count_ != nullptr ? retro_player_death_count_() : 0;
}

std::uint32_t RetrospectiveLoader::last_spawn_player_slot() const {
    return retro_last_spawn_player_slot_ != nullptr ? retro_last_spawn_player_slot_() : 0;
}

std::uint32_t RetrospectiveLoader::last_death_victim_slot() const {
    return retro_last_death_victim_slot_ != nullptr ? retro_last_death_victim_slot_() : 0;
}

std::uint32_t RetrospectiveLoader::last_death_attacker_slot() const {
    return retro_last_death_attacker_slot_ != nullptr ? retro_last_death_attacker_slot_() : 0;
}

const char* RetrospectiveLoader::status_text(std::int32_t code) const {
    return retro_status_text_ != nullptr ? retro_status_text_(code) : "unloaded";
}
