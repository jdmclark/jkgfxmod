#pragma once

#include "input_block.hpp"
#include "output_block.hpp"
#include <string_view>
#include <vector>

namespace jkgm {
    class memory_block;

    class memory_input_block : public input_block {
    private:
        memory_block const *mf;
        size_t offset = 0;

    public:
        explicit memory_input_block(memory_block const *mf);

        size_t read_some(span<char> dest) override;

        void seek(int off) override;
        void set_position(size_t off) override;
        size_t position() override;

        size_t size() override;
    };

    class memory_output_block : public output_block {
    private:
        memory_block *mf;
        size_t offset = 0;

    public:
        explicit memory_output_block(memory_block *mf);

        size_t write_some(span<char const> src) override;
        void set_position(size_t off) override;
        size_t position() override;
    };

    class memory_block {
        friend class memory_input_block;
        friend class memory_output_block;

    private:
        std::vector<char> buffer;

    public:
        char const *data() const;
        size_t size() const;
        bool empty() const;

        std::string_view str() const;
        void str(std::string_view value);

        void clear();
    };
}
