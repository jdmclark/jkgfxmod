#include "native_file_system.hpp"
#include "base/file_block.hpp"
#include "base/filesystem.hpp"
#include "base/log.hpp"

jkgm::native_file_system::native_file_system(fs::path const &base_path)
    : base_path(base_path)
{
    return;
}

std::unique_ptr<jkgm::input_stream> jkgm::native_file_system::open(fs::path const &filename) const
{
    try {
        return jkgm::make_file_input_block(base_path / filename);
    }
    catch(std::exception const &e) {
        std::string generic_fn = filename.generic_string();
        jkgm::diagnostic_context dc(generic_fn.c_str());
        LOG_ERROR(e.what());
        throw;
    }
}

std::tuple<fs::path, std::unique_ptr<jkgm::input_stream>>
    jkgm::native_file_system::find(fs::path const &filename,
                                   std::vector<fs::path> const &prefixes) const
{
    for(auto const &prefix : prefixes) {
        fs::path assembled = base_path / prefix / filename;
        if(fs::is_regular_file(assembled)) {
            return std::make_tuple(assembled, jkgm::make_file_input_block(assembled));
        }
    }

    // Check in the same directory
    return make_tuple(filename, open(filename));
}
