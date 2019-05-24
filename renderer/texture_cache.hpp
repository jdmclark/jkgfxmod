#pragma once

#include "base/filesystem.hpp"
#include "base/md5.hpp"
#include "common/config.hpp"
#include "common/material.hpp"
#include "glutil/texture.hpp"
#include "math/size.hpp"
#include "renderer_fwd.hpp"
#include <memory>
#include <optional>

namespace jkgm {
    class texture_cache {
    public:
        virtual ~texture_cache() = default;

        virtual std::optional<material const *> get_replacement_material(md5 const &sig) = 0;

        virtual srgb_texture_id create_srgb_texture_from_buffer(size<2, int> const &dims,
                                                                span<char const> data) = 0;
        virtual srgb_texture_id get_srgb_texture_from_filename(fs::path const &file) = 0;
        virtual linear_texture_id get_linear_texture_from_filename(fs::path const &file) = 0;

        virtual void release_srgb_texture(srgb_texture_id id) = 0;
        virtual void release_linear_texture(linear_texture_id id) = 0;

        virtual gl::texture_view get_texture_handle(srgb_texture_id id) const = 0;
        virtual gl::texture_view get_texture_handle(linear_texture_id id) const = 0;
    };

    std::unique_ptr<texture_cache> create_texture_cache(config const *the_config);
}