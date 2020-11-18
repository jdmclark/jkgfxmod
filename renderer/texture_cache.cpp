#include "texture_cache.hpp"
#include "base/file_block.hpp"
#include "base/file_stream.hpp"
#include "common/config.hpp"
#include "common/image.hpp"
#include "common/material_map.hpp"
#include <algorithm>
#include <numeric>
#include <random>

namespace jkgm {
    namespace {
        struct srgb_texture {
            size<2, int> dims;
            gl::texture handle;
            int refct = 0;
            std::optional<fs::path> origin_filename;

            explicit srgb_texture(size<2, int> dims)
                : dims(dims)
            {
            }
        };

        struct linear_texture {
            size<2, int> dims;
            gl::texture handle;
            int refct = 0;
            std::optional<fs::path> origin_filename;

            explicit linear_texture(size<2, int> dims)
                : dims(dims)
            {
            }
        };

        size_t dims_to_bytes(size<2, int> dims)
        {
            return static_cast<size_t>(volume(dims)) * 4;
        }

        class texture_cache_impl : public texture_cache {
        public:
            config const *the_config;
            material_map materials;

            std::vector<srgb_texture> srgb_textures;
            std::map<fs::path, size_t> file_to_srgb_texture_map;

            std::vector<linear_texture> linear_textures;
            std::map<fs::path, size_t> file_to_linear_texture_map;

            size_t current_capacity = 0U;

            explicit texture_cache_impl(config const *the_config)
                : the_config(the_config)
            {
                materials.create_map(fs::path(the_config->data_path) / "materials");

                size_t preload_capacity = the_config->vram_texture_preload_size * 1024U * 1024U;

                auto const &raw_materials = materials.get_materials();

                std::vector<size_t> shuffled_materials;
                shuffled_materials.resize(raw_materials.size());
                std::iota(shuffled_materials.begin(), shuffled_materials.end(), size_t(0U));

                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(shuffled_materials.begin(), shuffled_materials.end(), g);

                for(size_t mat_index : shuffled_materials) {
                    if(current_capacity >= preload_capacity) {
                        break;
                    }

                    auto const &mat = raw_materials.at(mat_index);

                    if(mat->albedo_map.has_value()) {
                        get_srgb_texture_from_filename(*mat->albedo_map);
                    }

                    if(mat->emissive_map.has_value()) {
                        get_srgb_texture_from_filename(*mat->emissive_map);
                    }

                    if(mat->displacement_map.has_value()) {
                        get_linear_texture_from_filename(*mat->displacement_map);
                    }
                }

                // Reset reference counts from preloaded textures
                for(auto &em : srgb_textures) {
                    em.refct = 0U;
                }

                for(auto &em : linear_textures) {
                    em.refct = 0U;
                }
            }

            std::optional<material const *> get_replacement_material(md5 const &sig) override
            {
                return materials.get_material(sig);
            }

            std::optional<srgb_texture_id> get_existing_free_srgb_texture(size<2, int> const &dims)
            {
                for(size_t i = 0; i < srgb_textures.size(); ++i) {
                    auto &em = srgb_textures[i];
                    if(em.refct <= 0 && em.dims == dims) {
                        // This texture is a match. Clean it up before returning.
                        if(em.origin_filename.has_value()) {
                            file_to_srgb_texture_map.erase(*em.origin_filename);
                        }

                        em.refct = 0;

                        return srgb_texture_id(i);
                    }
                }

                return std::nullopt;
            }

            srgb_texture_id create_srgb_texture_from_buffer(size<2, int> const &dims,
                                                            span<char const> data) override
            {
                auto existing_buf = get_existing_free_srgb_texture(dims);
                if(existing_buf.has_value()) {
                    // Matching texture already exists. Refill it.
                    auto &em = at(srgb_textures, *existing_buf);
                    gl::bind_texture(gl::texture_bind_target::texture_2d, em.handle);
                    gl::tex_sub_image_2d(gl::texture_bind_target::texture_2d,
                                         0,
                                         make_box(make_point(0, 0), dims),
                                         gl::texture_pixel_format::rgba,
                                         gl::texture_pixel_type::uint8,
                                         data);
                    gl::generate_mipmap(gl::texture_bind_target::texture_2d);

                    ++em.refct;
                    return *existing_buf;
                }

                // Create new texture
                srgb_texture_id rv(srgb_textures.size());
                srgb_textures.emplace_back(dims);
                current_capacity += dims_to_bytes(dims);

                auto &em = srgb_textures.back();
                em.refct = 1;

                gl::bind_texture(gl::texture_bind_target::texture_2d, em.handle);
                gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                                 /*level*/ 0,
                                 gl::texture_internal_format::srgb_a8,
                                 dims,
                                 gl::texture_pixel_format::rgba,
                                 gl::texture_pixel_type::uint8,
                                 data);
                gl::generate_mipmap(gl::texture_bind_target::texture_2d);
                gl::set_texture_max_anisotropy(gl::texture_bind_target::texture_2d,
                                               std::max(1.0f, the_config->max_anisotropy));
                if(the_config->enable_texture_filtering) {
                    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d,
                                               gl::mag_filter::linear);
                    gl::set_texture_min_filter(gl::texture_bind_target::texture_2d,
                                               gl::min_filter::linear_mipmap_linear);
                }
                else {
                    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d,
                                               gl::mag_filter::nearest);
                    gl::set_texture_min_filter(gl::texture_bind_target::texture_2d,
                                               gl::min_filter::nearest_mipmap_linear);
                }

                return rv;
            }

            srgb_texture_id get_srgb_texture_from_filename(fs::path const &file) override
            {
                auto it = file_to_srgb_texture_map.find(file);
                if(it != file_to_srgb_texture_map.end()) {
                    // Image file already loaded
                    srgb_texture_id rv(it->second);
                    ++at(srgb_textures, rv).refct;
                    return rv;
                }

                auto fs = make_file_input_block(file);
                auto img = load_image(fs.get());

                auto rv = create_srgb_texture_from_buffer(img->dimensions,
                                                          make_span(img->data).as_const_bytes());

                auto &em = at(srgb_textures, rv);
                em.origin_filename = file;
                file_to_srgb_texture_map.emplace(file, rv.get());

                return rv;
            }

            void release_srgb_texture(srgb_texture_id id) override
            {
                --at(srgb_textures, id).refct;
            }

            std::optional<linear_texture_id>
                get_existing_free_linear_texture(size<2, int> const &dims)
            {
                for(size_t i = 0; i < linear_textures.size(); ++i) {
                    auto &em = linear_textures[i];
                    if(em.refct <= 0 && em.dims == dims) {
                        // This texture is a match. Clean it up before returning.
                        if(em.origin_filename.has_value()) {
                            file_to_linear_texture_map.erase(*em.origin_filename);
                        }

                        em.refct = 0;

                        return linear_texture_id(i);
                    }
                }

                return std::nullopt;
            }

            linear_texture_id create_linear_texture_from_buffer(size<2, int> const &dims,
                                                                span<char const> data)
            {
                auto existing_buf = get_existing_free_linear_texture(dims);
                if(existing_buf.has_value()) {
                    // Matching texture already exists. Refill it.
                    auto &em = at(linear_textures, *existing_buf);
                    gl::bind_texture(gl::texture_bind_target::texture_2d, em.handle);
                    gl::tex_sub_image_2d(gl::texture_bind_target::texture_2d,
                                         0,
                                         make_box(make_point(0, 0), dims),
                                         gl::texture_pixel_format::rgba,
                                         gl::texture_pixel_type::uint8,
                                         data);
                    gl::generate_mipmap(gl::texture_bind_target::texture_2d);

                    ++em.refct;
                    return *existing_buf;
                }

                // Create new texture
                linear_texture_id rv(linear_textures.size());
                linear_textures.emplace_back(dims);
                current_capacity += dims_to_bytes(dims);

                auto &em = linear_textures.back();
                em.refct = 1;

                gl::bind_texture(gl::texture_bind_target::texture_2d, em.handle);
                gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                                 /*level*/ 0,
                                 gl::texture_internal_format::rgba,
                                 dims,
                                 gl::texture_pixel_format::rgba,
                                 gl::texture_pixel_type::uint8,
                                 data);
                gl::generate_mipmap(gl::texture_bind_target::texture_2d);
                gl::set_texture_max_anisotropy(gl::texture_bind_target::texture_2d,
                                               std::max(1.0f, the_config->max_anisotropy));
                if(the_config->enable_texture_filtering) {
                    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d,
                                               gl::mag_filter::linear);
                    gl::set_texture_min_filter(gl::texture_bind_target::texture_2d,
                                               gl::min_filter::linear_mipmap_linear);
                }
                else {
                    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d,
                                               gl::mag_filter::nearest);
                    gl::set_texture_min_filter(gl::texture_bind_target::texture_2d,
                                               gl::min_filter::nearest_mipmap_linear);
                }

                return rv;
            }

            linear_texture_id get_linear_texture_from_filename(fs::path const &file) override
            {
                auto it = file_to_linear_texture_map.find(file);
                if(it != file_to_linear_texture_map.end()) {
                    // Image file already loaded
                    linear_texture_id rv(it->second);
                    ++at(linear_textures, rv).refct;
                    return rv;
                }

                auto fs = make_file_input_block(file);
                auto img = load_image(fs.get());

                auto rv = create_linear_texture_from_buffer(img->dimensions,
                                                            make_span(img->data).as_const_bytes());

                auto &em = at(linear_textures, rv);
                em.origin_filename = file;
                file_to_linear_texture_map.emplace(file, rv.get());

                return rv;
            }

            void release_linear_texture(linear_texture_id id) override
            {
                --at(linear_textures, id).refct;
            }

            gl::texture_view get_texture_handle(srgb_texture_id id) const override
            {
                return at(srgb_textures, id).handle;
            }

            gl::texture_view get_texture_handle(linear_texture_id id) const override
            {
                return at(linear_textures, id).handle;
            }
        };
    }
}

std::unique_ptr<jkgm::texture_cache> jkgm::create_texture_cache(config const *the_config)
{
    return std::make_unique<jkgm::texture_cache_impl>(the_config);
}