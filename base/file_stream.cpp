#include "file_stream.hpp"
#include "buffered_input_stream.hpp"
#include "buffered_output_stream.hpp"
#include "file_block.hpp"

namespace {
    class file_output_stream : public jkgm::output_stream {
    private:
        jkgm::file_output_block fb;
        jkgm::buffered_output_stream fs;

    public:
        explicit file_output_stream(fs::path const &filename)
            : fb(filename)
            , fs(&fb)
        {
        }

        size_t write_some(jkgm::span<char const> src) override
        {
            return fs.write_some(src);
        }
    };

    class file_input_stream : public jkgm::input_stream {
    private:
        jkgm::file_input_block fb;
        jkgm::buffered_input_stream fs;

    public:
        explicit file_input_stream(fs::path const &filename)
            : fb(filename)
            , fs(&fb)
        {
        }

        size_t read_some(jkgm::span<char> dest) override
        {
            return fs.read_some(dest);
        }
    };
}

std::unique_ptr<jkgm::output_stream> jkgm::make_file_output_stream(fs::path const &filename)
{
    return std::make_unique<file_output_stream>(filename);
}

std::unique_ptr<jkgm::input_stream> jkgm::make_file_input_stream(fs::path const &filename)
{
    return std::make_unique<file_input_stream>(filename);
}
