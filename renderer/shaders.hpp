#pragma once

#include "base/filesystem.hpp"
#include "base/span.hpp"
#include "glutil/program.hpp"
#include <vector>

namespace jkgm {
    class opengl_shader {
    protected:
        gl::program the_program;

        opengl_shader(std::string const &name,
                      fs::path const &vertex_part,
                      fs::path const &fragment_part);

    public:
        opengl_shader(opengl_shader const &) = delete;
        opengl_shader(opengl_shader &&) = delete;

        opengl_shader &operator=(opengl_shader const &) = delete;
        opengl_shader &operator=(opengl_shader &&) = delete;

        virtual ~opengl_shader() = default;

        void use_program();
    };

    namespace shaders {
        class menu_shader : public opengl_shader {
        private:
            gl::uniform_location_id tex;

        public:
            explicit menu_shader(fs::path const &data_root);

            void set_menu_sampler(int sampler);
        };

        class game_pass_shader : public opengl_shader {
        private:
            gl::uniform_location_id screen_resolution;

            gl::uniform_location_id features;

            gl::uniform_location_id albedo_map;
            gl::uniform_location_id albedo_factor;

            gl::uniform_location_id emissive_map;
            gl::uniform_location_id emissive_factor;

            gl::uniform_location_id alpha_cutoff;

            gl::uniform_location_id displacement_map;
            gl::uniform_location_id displacement_factor;

        protected:
            game_pass_shader(std::string const &name,
                             fs::path const &vertex_part,
                             fs::path const &fragment_part);

        public:
            void set_screen_resolution(size<2, float> res);

            void set_features(point<4, float> value);

            void set_albedo_map_sampler(int sampler);
            void set_albedo_factor(color factor);

            void set_emissive_map_sampler(int sampler);
            void set_emissive_factor(color_rgb factor);

            void set_alpha_cutoff(float cutoff);

            void set_displacement_map_sampler(int sampler);
            void set_displacement_factor(float factor);
        };

        class game_opaque_pass_shader : public game_pass_shader {
        public:
            explicit game_opaque_pass_shader(fs::path const &data_root);
        };

        class game_post_ssao_shader : public opengl_shader {
        public:
            static constexpr int num_kernel_samples = 16;

        private:
            gl::uniform_location_id normal_image;
            gl::uniform_location_id depth_image;
            gl::uniform_location_id noise_image;

            std::vector<gl::uniform_location_id> samples;

        public:
            explicit game_post_ssao_shader(fs::path const &data_root);

            void set_normal_sampler(int sampler);
            void set_depth_sampler(int sampler);
            void set_noise_sampler(int sampler);

            void set_samples(span<point<3, float> const> values);
        };

        class game_post_opaque_composite_shader : public opengl_shader {
        private:
            gl::uniform_location_id color_image;
            gl::uniform_location_id emissive_image;
            gl::uniform_location_id occlusion_image;

        public:
            explicit game_post_opaque_composite_shader(fs::path const &data_root);

            void set_color_sampler(int sampler);
            void set_emissive_sampler(int sampler);
            void set_occlusion_sampler(int sampler);
        };

        class game_transparency_pass_shader : public game_pass_shader {
        public:
            explicit game_transparency_pass_shader(fs::path const &data_root);
        };

        class post_box4_shader : public opengl_shader {
        private:
            gl::uniform_location_id fbuf_image;
            gl::uniform_location_id normal_image;
            gl::uniform_location_id depth_image;

        public:
            explicit post_box4_shader(fs::path const &data_root);

            void set_fbuf_sampler(int sampler);
            void set_normal_sampler(int sampler);
            void set_depth_sampler(int sampler);
        };

        class post_gauss7_shader : public opengl_shader {
        private:
            gl::uniform_location_id fbuf_image;
            gl::uniform_location_id fbuf_dimensions;
            gl::uniform_location_id blur_direction;

        public:
            explicit post_gauss7_shader(fs::path const &data_root);

            void set_fbuf_sampler(int sampler);
            void set_fbuf_dimensions(size<2, float> dims);
            void set_blur_direction(direction<2, float> dir);
        };

        class post_low_pass_shader : public opengl_shader {
        private:
            gl::uniform_location_id fbuf_image;

        public:
            explicit post_low_pass_shader(fs::path const &data_root);

            void set_fbuf_sampler(int sampler);
        };

        class post_to_srgb_shader : public opengl_shader {
        public:
            static constexpr int num_bloom_layers = 4;

        private:
            gl::uniform_location_id fbuf_image;
            std::vector<gl::uniform_location_id> bloom_fbuf;
            std::vector<gl::uniform_location_id> bloom_weight;

        public:
            explicit post_to_srgb_shader(fs::path const &data_root);

            void set_fbuf_sampler(int sampler);
            void set_bloom_samplers(span<int const> samplers);
            void set_bloom_weights(span<float const> weights);
        };
    }
}