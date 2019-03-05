#pragma once

#include "cstring_view.hpp"
#include "filesystem.hpp"
#include "flag_set.hpp"
#include "id.hpp"
#include "span.hpp"
#include "unique_handle.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <system_error>

namespace jkgm::win32 {
    MAKE_ID_TYPE(handle, size_t);
    constexpr handle_id invalid_handle_value(std::numeric_limits<size_t>::max());

    MAKE_ID_TYPE(socket, size_t);
    constexpr socket_id invalid_socket(std::numeric_limits<size_t>::max());

    enum class win32_error : int { success = 0 };
    std::error_category const &win32_category();

    enum class creation_disposition : uint32_t {
        create_new = 1,
        create_always = 2,
        open_existing = 3,
        open_always = 4,
        truncate_existing = 5
    };

    enum class duplicate_flag : uint32_t { close_source = 1, same_access = 2 };

    enum class file_access_right : uint32_t { read_data = 1, write_data = 2, append_data = 4 };

    enum class file_attribute : uint32_t { normal = 128 };

    enum class file_share_mode : uint32_t {
        exclusive = 0,
        share_read = 0x01,
        share_write = 0x02,
        share_delete = 0x04
    };

    enum class move_method : uint32_t { begin = 0, current = 1, end = 2 };

    enum class process_creation_flag : uint32_t {
        detached = 0x8,
        create_new_process_group = 0x200,
        create_no_window = 0x08000000
    };

    MAKE_ID_TYPE(process, unsigned long);

    enum class std_device : uint32_t {
        std_input = std::numeric_limits<uint32_t>::max() - 9,
        std_output = std::numeric_limits<uint32_t>::max() - 10,
        std_error = std::numeric_limits<uint32_t>::max() - 11
    };

    enum class wait_state : uint32_t { signaled_object = 0, abandoned = 0x80, timeout = 0x102 };

    struct unique_handle_traits {
        using value_type = handle_id;

        static inline handle_id create(handle_id fd)
        {
            return fd;
        }

        static void destroy(handle_id fd);
    };

    using unique_handle = jkgm::unique_handle<unique_handle_traits>;

    void close_handle(handle_id handle);
    unique_handle create_file(fs::path const &name,
                              flag_set<file_access_right> desired_access,
                              flag_set<file_share_mode> share_mode,
                              bool children_inherit_handle,
                              creation_disposition create_mode,
                              flag_set<file_attribute> attributes);
    std::array<unique_handle, 2> create_pipe(bool children_inherit_handles);
    unique_handle create_process(std::optional<fs::path> progname,
                                 std::string_view cmdline,
                                 bool children_inherit_handle,
                                 bool this_process_inherits_handles,
                                 flag_set<process_creation_flag> creation_flags,
                                 std::optional<std::unordered_map<std::string, std::string>> env,
                                 std::optional<fs::path> working_directory,
                                 std::optional<handle_id> std_input,
                                 std::optional<handle_id> std_output,
                                 std::optional<handle_id> std_error);
    unique_handle duplicate_handle(handle_id handle,
                                   bool children_inherit_handle,
                                   flag_set<duplicate_flag> flags);
    fs::path get_current_module_file_name();
    process_id get_current_process_id();
    std::unordered_map<std::string, std::string> get_environment_strings();
    std::optional<std::string> get_environment_variable(std::string_view name);
    unsigned int get_exit_code_process(handle_id file);
    size_t get_file_pointer(handle_id file);
    size_t get_file_size(handle_id file);
    handle_id get_std_handle(std_device device);
    size_t read_file(handle_id file, span<char> buffer);
    void set_environment_variable(std::string_view name, std::optional<std::string_view> value);
    void set_file_pointer(handle_id file, int64_t distance, move_method method);
    void set_process_dpi_aware();
    void set_std_handle(std_device device, handle_id file);
    wait_state wait_for_input_idle(handle_id file,
                                   std::optional<std::chrono::milliseconds> timeout);
    wait_state wait_for_single_object(handle_id file,
                                      std::optional<std::chrono::milliseconds> timeout);
    size_t write_file(handle_id file, span<char const> buffer);
}

namespace std {
    template <>
    struct is_error_condition_enum<jkgm::win32::win32_error> : public true_type {
    };
}
