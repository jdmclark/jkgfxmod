#pragma once

#include "build_traits.hpp"
#include "filesystem.hpp"
#include "input_block.hpp"
#include "output_block.hpp"
#include "win32.hpp"
#include <memory>

namespace jkgm {
    namespace detail {
        using fb_s_type = specialization_t<win32_version<win32::unique_handle>>;
    }

    class file_output_block : public output_block {
    private:
        detail::fb_s_type fd;

    public:
        explicit file_output_block(fs::path const &filename);
        size_t write_some(span<char const> src) override;
        void set_position(size_t offset) override;
        size_t position() override;
    };

    class file_input_block : public input_block {
    private:
        detail::fb_s_type fd;

    public:
        explicit file_input_block(fs::path const &filename);
        size_t read_some(span<char> dest) override;
        void seek(int offset) override;
        void set_position(size_t offset) override;
        size_t position() override;
        size_t size() override;
    };

    std::unique_ptr<file_output_block> make_file_output_block(fs::path const &filename);
    std::unique_ptr<file_input_block> make_file_input_block(fs::path const &filename);
}
