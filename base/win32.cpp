#include "win32.hpp"

void jkgm::win32::unique_handle_traits::destroy(handle_id fd)
{
    try {
        win32::close_handle(fd);
    }
    catch(std::system_error const & /*e*/) {
        // Errors closing at handle destruction are unrecoverable
    }
}

#ifdef PLATFORM_WINDOWS

#include "system_string.hpp"
#include "win32.hpp"
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#include <detours/detours.h>

namespace {
    HANDLE to_handle(jkgm::win32::handle_id hid)
    {
        return reinterpret_cast<HANDLE>(hid.get());
    }

    jkgm::win32::handle_id to_wrapper(HANDLE h)
    {
        return jkgm::win32::handle_id(reinterpret_cast<size_t>(h));
    }

    class win32_error_category : public std::error_category {
    public:
        char const *name() const noexcept override
        {
            return "win32";
        }

        std::string message(int ec) const override
        {
            wchar_t buf[4096] = {0};
            auto res = ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                        NULL,
                                        static_cast<DWORD>(ec),
                                        0U,
                                        buf,
                                        4095,
                                        NULL);
            if(res != 0) {
                return jkgm::native_to_utf8(buf);
            }

            return "Fatal error formatting system error message";
        }
    } win32_error_category_instance;
}

std::error_category const &jkgm::win32::win32_category()
{
    return win32_error_category_instance;
}

void jkgm::win32::close_handle(handle_id handle)
{
    auto rv = ::CloseHandle(to_handle(handle));
    if(rv == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }
}

jkgm::win32::unique_handle jkgm::win32::create_file(fs::path const &name,
                                                    flag_set<file_access_right> desired_access,
                                                    flag_set<file_share_mode> share_mode,
                                                    bool children_inherit_handle,
                                                    creation_disposition create_mode,
                                                    flag_set<file_attribute> attributes)
{
    SECURITY_ATTRIBUTES sa;
    ::ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = children_inherit_handle ? TRUE : FALSE;

    auto rv = ::CreateFileW(name.native().c_str(),
                            static_cast<uint32_t>(desired_access),
                            static_cast<uint32_t>(share_mode),
                            &sa,
                            static_cast<uint32_t>(create_mode),
                            static_cast<uint32_t>(attributes),
                            NULL);

    if(rv == INVALID_HANDLE_VALUE) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    return unique_handle(to_wrapper(rv));
}

std::array<jkgm::win32::unique_handle, 2> jkgm::win32::create_pipe(bool children_inherit_handles)
{
    SECURITY_ATTRIBUTES sa;
    ::ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = children_inherit_handles ? TRUE : FALSE;

    HANDLE read_end, write_end;
    auto res = ::CreatePipe(&read_end, &write_end, &sa, 0U);
    if(res == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    return {unique_handle(to_wrapper(read_end)), unique_handle(to_wrapper(write_end))};
}

jkgm::win32::unique_handle
    jkgm::win32::create_process(std::optional<fs::path> progname,
                                std::string_view cmdline,
                                bool children_inherit_handle,
                                bool this_process_inherits_handles,
                                flag_set<process_creation_flag> creation_flags,
                                std::optional<std::unordered_map<std::string, std::string>> env,
                                std::optional<fs::path> working_directory,
                                std::optional<handle_id> std_input,
                                std::optional<handle_id> std_output,
                                std::optional<handle_id> std_error)
{
    auto wcmdline = utf8_to_native(cmdline);

    SECURITY_ATTRIBUTES sa;
    ::ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = children_inherit_handle ? TRUE : FALSE;

    DWORD dw_creation_flags = static_cast<uint32_t>(creation_flags);
    dw_creation_flags |= CREATE_UNICODE_ENVIRONMENT;

    std::optional<system_string> maybe_prepared_env;
    if(env.has_value()) {
        system_string prepared_env;

        size_t num_chars = 0;
        for(auto const &em : env.value()) {
            // Add part lengths, equals, null terminator
            num_chars += em.first.size() + em.second.size() + 2;
        }

        // Add final null terminator
        prepared_env.reserve(num_chars + 1);

        for(auto const &em : env.value()) {
            prepared_env.append(utf8_to_native(em.first));
            prepared_env.push_back('=');
            prepared_env.append(utf8_to_native(em.second));
            prepared_env.push_back(0);
        }

        prepared_env.push_back(0);

        maybe_prepared_env = std::move(prepared_env);
    }

    STARTUPINFO si;
    ::ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    if(std_input.has_value()) {
        si.hStdInput = to_handle(*std_input);
    }
    else {
        si.hStdInput = to_handle(get_std_handle(std_device::std_input));
    }

    if(std_output.has_value()) {
        si.hStdOutput = to_handle(*std_output);
    }
    else {
        si.hStdOutput = to_handle(get_std_handle(std_device::std_output));
    }

    if(std_error.has_value()) {
        si.hStdError = to_handle(*std_error);
    }
    else {
        si.hStdError = to_handle(get_std_handle(std_device::std_error));
    }

    si.dwFlags = STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;

    auto res =
        ::CreateProcessW(progname.has_value() ? progname->native().c_str() : NULL,
                         wcmdline.data(),
                         &sa,
                         &sa,
                         this_process_inherits_handles ? TRUE : FALSE,
                         dw_creation_flags,
                         maybe_prepared_env.has_value() ? maybe_prepared_env->data() : NULL,
                         working_directory.has_value() ? working_directory->native().c_str() : NULL,
                         &si,
                         &pi);

    if(res == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    // Clean up unused handles
    close_handle(to_wrapper(pi.hThread));

    return unique_handle(to_wrapper(pi.hProcess));
}

jkgm::win32::unique_handle jkgm::win32::duplicate_handle(handle_id handle,
                                                         bool children_inherit_handle,
                                                         flag_set<duplicate_flag> flags)
{
    HANDLE rv;
    auto res = ::DuplicateHandle(::GetCurrentProcess(),
                                 to_handle(handle),
                                 ::GetCurrentProcess(),
                                 &rv,
                                 0U,
                                 children_inherit_handle ? TRUE : FALSE,
                                 static_cast<unsigned int>(flags));
    if(res == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    return unique_handle(to_wrapper(rv));
}

fs::path jkgm::win32::get_current_module_file_name()
{
    std::wstring rbuf;
    rbuf.resize(1024);

    for(;;) {
        ::SetLastError(ERROR_SUCCESS);
        auto res = ::GetModuleFileNameW(NULL, rbuf.data(), narrow<DWORD>(rbuf.size()));
        auto le = ::GetLastError();

        if(res == rbuf.size() && le == ERROR_INSUFFICIENT_BUFFER) {
            rbuf.resize(rbuf.size() * 2);
            continue;
        }

        rbuf.resize(res);
        return fs::path(rbuf);
    }
}

jkgm::win32::process_id jkgm::win32::get_current_process_id()
{
    return process_id(::GetCurrentProcessId());
}

std::unordered_map<std::string, std::string> jkgm::win32::get_environment_strings()
{
    wchar_t *peb = ::GetEnvironmentStringsW();
    if(peb == nullptr) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    std::unordered_map<std::string, std::string> rv;

    std::wstring last_key, last_val;
    wchar_t *curr = peb;
    for(;;) {
        if(*curr == 0) {
            break;
        }

        last_key.clear();
        last_val.clear();

        for(; *curr != '='; ++curr) {
            last_key.push_back(*curr);
        }

        ++curr;

        for(; *curr != 0; ++curr) {
            last_val.push_back(*curr);
        }

        ++curr;

        rv.emplace(native_to_utf8(last_key), native_to_utf8(last_val));
    }

    auto res = ::FreeEnvironmentStrings(peb);
    if(res == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    return rv;
}

std::optional<std::string> jkgm::win32::get_environment_variable(std::string_view name)
{
    auto w_name = utf8_to_native(name);
    std::wstring buf;
    for(;;) {
        auto bufsz =
            ::GetEnvironmentVariableW(w_name.c_str(), buf.data(), narrow<DWORD>(buf.size() + 1));
        if(bufsz == 0) {
            auto le = ::GetLastError();
            if(le == ERROR_ENVVAR_NOT_FOUND || le == ERROR_SUCCESS) {
                return std::nullopt;
            }

            throw std::system_error(le, win32_category());
        }
        else if(bufsz <= buf.size()) {
            return native_to_utf8(buf);
        }

        buf.resize(bufsz - 1);
    }
}

unsigned int jkgm::win32::get_exit_code_process(handle_id file)
{
    DWORD rv = 0;
    auto res = ::GetExitCodeProcess(to_handle(file), &rv);

    if(res == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    return rv;
}

size_t jkgm::win32::get_file_pointer(handle_id file)
{
    LARGE_INTEGER li;
    li.QuadPart = 0;

    LARGE_INTEGER rv;

    auto res = ::SetFilePointerEx(to_handle(file), li, &rv, FILE_CURRENT);
    if(res == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    return narrow<size_t>(rv.QuadPart);
}

size_t jkgm::win32::get_file_size(handle_id file)
{
    LARGE_INTEGER rv;
    auto res = ::GetFileSizeEx(to_handle(file), &rv);

    if(res == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    return narrow<size_t>(rv.QuadPart);
}

jkgm::win32::handle_id jkgm::win32::get_std_handle(std_device device)
{
    auto rv = ::GetStdHandle(static_cast<DWORD>(device));
    if(rv == INVALID_HANDLE_VALUE) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    return to_wrapper(rv);
}

size_t jkgm::win32::read_file(handle_id file, span<char> buffer)
{
    DWORD num_read = 0U;
    auto res =
        ::ReadFile(to_handle(file), buffer.data(), narrow<DWORD>(buffer.size()), &num_read, NULL);

    if(res == 0) {
        auto le = ::GetLastError();
        if(le == ERROR_BROKEN_PIPE) {
            return 0;
        }

        throw std::system_error(le, win32_category());
    }

    return num_read;
}

void jkgm::win32::set_environment_variable(std::string_view name,
                                           std::optional<std::string_view> value)
{
    BOOL result;
    auto w_name = utf8_to_native(name);
    if(value.has_value()) {
        auto w_val = utf8_to_native(*value);
        result = ::SetEnvironmentVariableW(w_name.c_str(), w_val.c_str());
    }
    else {
        result = ::SetEnvironmentVariableW(w_name.c_str(), NULL);
    }

    if(result == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }
}

void jkgm::win32::set_file_pointer(handle_id file, int64_t distance, move_method method)
{
    LARGE_INTEGER li;
    li.QuadPart = distance;

    auto res = ::SetFilePointerEx(to_handle(file), li, NULL, static_cast<DWORD>(method));
    if(res == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }
}

void jkgm::win32::set_process_dpi_aware()
{
    ::SetProcessDPIAware();
}

void jkgm::win32::set_std_handle(std_device device, handle_id file)
{
    auto res = ::SetStdHandle(static_cast<DWORD>(device), to_handle(file));
    if(res == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }
}

jkgm::win32::wait_state
    jkgm::win32::wait_for_input_idle(handle_id file,
                                     std::optional<std::chrono::milliseconds> timeout)
{
    DWORD real_timeout = INFINITE;
    if(timeout.has_value()) {
        real_timeout = static_cast<DWORD>(timeout->count());
    }

    auto res = ::WaitForInputIdle(to_handle(file), real_timeout);
    if(res == WAIT_FAILED) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    return wait_state(res);
}

jkgm::win32::wait_state
    jkgm::win32::wait_for_single_object(handle_id file,
                                        std::optional<std::chrono::milliseconds> timeout)
{
    DWORD real_timeout = INFINITE;
    if(timeout.has_value()) {
        real_timeout = static_cast<DWORD>(timeout->count());
    }

    auto res = ::WaitForSingleObject(to_handle(file), real_timeout);
    if(res == WAIT_FAILED) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    return wait_state(res);
}

size_t jkgm::win32::write_file(handle_id file, span<char const> buffer)
{
    DWORD num_written = 0U;
    auto res = ::WriteFile(
        to_handle(file), buffer.data(), narrow<DWORD>(buffer.size()), &num_written, NULL);

    if(res == 0) {
        throw std::system_error(::GetLastError(), win32_category());
    }

    return num_written;
}

// Value assertions: Certain constants defined in win32.hpp must be equal to values defined in the
// Win32 API. Assert it here.

static_assert(sizeof(wchar_t) == sizeof(TCHAR), "wchar_t and TCHAR must be the same size");
static_assert(sizeof(wchar_t) == sizeof(char16_t), "wchar_t and uchar16_t must be the same size");

static_assert(static_cast<DWORD>(jkgm::win32::win32_error::success) == ERROR_SUCCESS,
              "win32_error::success value mismatch");

static_assert(static_cast<DWORD>(jkgm::win32::creation_disposition::create_new) == CREATE_NEW,
              "creation_disposition::create_new value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::creation_disposition::create_always) == CREATE_ALWAYS,
              "creation_disposition::create_always value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::creation_disposition::open_existing) == OPEN_EXISTING,
              "creation_disposition::open_existing value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::creation_disposition::open_always) == OPEN_ALWAYS,
              "creation_disposition::open_always value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::creation_disposition::truncate_existing) ==
                  TRUNCATE_EXISTING,
              "creation_disposition::truncate_existing value mismatch");

static_assert(static_cast<DWORD>(jkgm::win32::duplicate_flag::close_source) ==
                  DUPLICATE_CLOSE_SOURCE,
              "duplicate_flag::close_source value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::duplicate_flag::same_access) == DUPLICATE_SAME_ACCESS,
              "duplicate_flag::same_access value mismatch");

static_assert(static_cast<DWORD>(jkgm::win32::file_access_right::read_data) == FILE_READ_DATA,
              "file_access_right::read_data value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::file_access_right::write_data) == FILE_WRITE_DATA,
              "file_access_right::write_data value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::file_access_right::append_data) == FILE_APPEND_DATA,
              "file_access_right::append_data value mismatch");

static_assert(static_cast<DWORD>(jkgm::win32::file_attribute::normal) == FILE_ATTRIBUTE_NORMAL,
              "file_attribute::normal value mismatch");

static_assert(static_cast<DWORD>(jkgm::win32::file_share_mode::exclusive) == 0U,
              "file_share_mode::exclusive value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::file_share_mode::share_read) == FILE_SHARE_READ,
              "file_share_mode::share_read value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::file_share_mode::share_write) == FILE_SHARE_WRITE,
              "file_share_mode::share_write value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::file_share_mode::share_delete) == FILE_SHARE_DELETE,
              "file_share_mode::share_delete value mismatch");

static_assert(static_cast<DWORD>(jkgm::win32::move_method::begin) == FILE_BEGIN,
              "move_method::begin value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::move_method::current) == FILE_CURRENT,
              "move_method::current value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::move_method::end) == FILE_END,
              "move_method::end value mismatch");

static_assert(static_cast<DWORD>(jkgm::win32::process_creation_flag::detached) == DETACHED_PROCESS,
              "process_creation_flag::detached value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::process_creation_flag::create_new_process_group) ==
                  CREATE_NEW_PROCESS_GROUP,
              "process_creation_flag::create_new_process_group value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::process_creation_flag::create_no_window) ==
                  CREATE_NO_WINDOW,
              "process_creation_flag::create_no_window value mismatch");

static_assert(std::is_same_v<std::underlying_type_t<jkgm::win32::process_id>, DWORD>,
              "process_id type mismatch");

static_assert(static_cast<DWORD>(jkgm::win32::std_device::std_input) == STD_INPUT_HANDLE,
              "std_device::std_input value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::std_device::std_output) == STD_OUTPUT_HANDLE,
              "std_device::std_output value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::std_device::std_error) == STD_ERROR_HANDLE,
              "std_device::std_error value mismatch");

static_assert(static_cast<DWORD>(jkgm::win32::wait_state::signaled_object) == WAIT_OBJECT_0,
              "wait_state::signaled_object value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::wait_state::abandoned) == WAIT_ABANDONED,
              "wait_state::abandoned value mismatch");
static_assert(static_cast<DWORD>(jkgm::win32::wait_state::timeout) == WAIT_TIMEOUT,
              "wait_state::timeout value mismatch");

static_assert(sizeof(std::underlying_type_t<jkgm::win32::handle_id>) == sizeof(HANDLE),
              "win32::handle_id size mismatch");
static_assert(jkgm::win32::invalid_handle_value.get() == (size_t)INVALID_HANDLE_VALUE,
              "win32::invalid_handle_value value mismatch");

static_assert(std::is_same_v<std::underlying_type_t<jkgm::win32::socket_id>, SOCKET>,
              "win32::socket_id type mismatch");
static_assert(jkgm::win32::invalid_socket.get() == INVALID_SOCKET,
              "win32::invalid_socket value mismatch");

#endif // PLATFORM_WINDOWS
