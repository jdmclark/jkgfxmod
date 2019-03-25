#include "renderer.hpp"
#include "backbuffer_menu_surface.hpp"
#include "backbuffer_surface.hpp"
#include "base/file_block.hpp"
#include "base/file_stream.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"
#include "base/win32.hpp"
#include "common/image.hpp"
#include "common/material_map.hpp"
#include "d3d_impl.hpp"
#include "d3ddevice_impl.hpp"
#include "d3dviewport_impl.hpp"
#include "ddraw2_impl.hpp"
#include "ddraw_impl.hpp"
#include "ddrawpalette_impl.hpp"
#include "execute_buffer.hpp"
#include "glad/glad.h"
#include "glutil/buffer.hpp"
#include "glutil/framebuffer.hpp"
#include "glutil/gl.hpp"
#include "glutil/program.hpp"
#include "glutil/shader.hpp"
#include "glutil/texture.hpp"
#include "glutil/vertex_array.hpp"
#include "math/color_conv.hpp"
#include "math/colors.hpp"
#include "offscreen_menu_surface.hpp"
#include "offscreen_surface.hpp"
#include "opengl_state.hpp"
#include "primary_menu_surface.hpp"
#include "primary_surface.hpp"
#include "sysmem_texture.hpp"
#include "triangle_batch.hpp"
#include "vidmem_texture.hpp"
#include "zbuffer_surface.hpp"
#include <Windows.h>
#include <chrono>
#include <random>

namespace jkgm {
    static WNDPROC original_wkernel_wndproc = nullptr;
    static size<2, int> original_configured_screen_res = make_size(0, 0);

    LRESULT CALLBACK renderer_wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch(uMsg) {
        case WM_GETMINMAXINFO: {
            RECT r{0,
                   0,
                   get<x>(original_configured_screen_res),
                   get<y>(original_configured_screen_res)};
            AdjustWindowRectEx(&r,
                               GetWindowLongPtr(hWnd, GWL_STYLE),
                               /*bMenu*/ FALSE,
                               GetWindowLongPtr(hWnd, GWL_EXSTYLE));

            LPMINMAXINFO mmi = (LPMINMAXINFO)lParam;
            mmi->ptMaxSize.x = r.right - r.left;
            mmi->ptMaxSize.y = r.bottom - r.top;
            return 0;
        }

        case WM_WINDOWPOSCHANGING: {
            RECT r{0,
                   0,
                   get<x>(original_configured_screen_res),
                   get<y>(original_configured_screen_res)};
            AdjustWindowRectEx(&r,
                               GetWindowLongPtr(hWnd, GWL_STYLE),
                               /*bMenu*/ FALSE,
                               GetWindowLongPtr(hWnd, GWL_EXSTYLE));

            LPWINDOWPOS wp = (LPWINDOWPOS)lParam;
            wp->cx = r.right - r.left;
            wp->cy = r.bottom - r.top;
            return 0;
        }

        case WM_MOUSEMOVE: {
            // Scale the mouse position so JK thinks it's over the menu
            auto xPos = (int16_t)lParam;
            auto yPos = (int16_t)(lParam >> 16);

            xPos =
                (int16_t)((float)xPos * (640.0f / (float)get<x>(original_configured_screen_res)));
            yPos =
                (int16_t)((float)yPos * (480.0f / (float)get<y>(original_configured_screen_res)));

            lParam = (((LPARAM)yPos << 16) | (LPARAM)xPos);

            // Pass this message back to the original wndproc
            break;
        }
        }

        return CallWindowProc(original_wkernel_wndproc, hWnd, uMsg, wParam, lParam);
    }

    class renderer_impl : public renderer {
    private:
        config const *the_config;
        material_map materials;

        renderer_mode mode = renderer_mode::menu;
        size<2, int> conf_scr_res;
        size<2, float> conf_scr_res_f;

        DirectDraw_impl ddraw1;
        DirectDraw2_impl ddraw2;
        Direct3D_impl d3d1;
        Direct3DDevice_impl d3ddevice1;
        Direct3DViewport_impl d3dviewport1;

        primary_menu_surface ddraw1_primary_menu_surface;
        backbuffer_menu_surface ddraw1_backbuffer_menu_surface;
        primary_surface ddraw1_primary_surface;
        backbuffer_surface ddraw1_backbuffer_surface;
        zbuffer_surface ddraw1_zbuffer_surface;
        offscreen_surface ddraw1_offscreen_surface;
        offscreen_menu_surface ddraw1_offscreen_menu_surface;

        DirectDrawPalette_impl ddraw1_palette;

        std::vector<std::unique_ptr<sysmem_texture_surface>> sysmem_texture_surfaces;
        std::vector<std::unique_ptr<vidmem_texture_surface>> vidmem_texture_surfaces;
        std::vector<std::unique_ptr<execute_buffer>> execute_buffers;

        HINSTANCE dll_instance;
        HWND hWnd;
        HDC hDC;
        HGLRC hGLRC;

        std::unique_ptr<opengl_state> ogs;

        HGDIOBJ indexed_bitmap_dc = NULL;
        char const *indexed_bitmap_source = nullptr;
        std::vector<color_rgba8> indexed_bitmap_colors;

        double menu_accumulator = 0.0;

        using timestamp_t = std::chrono::high_resolution_clock::time_point;
        timestamp_t menu_prev_ticks;
        timestamp_t menu_curr_ticks;

        triangle_batch world_batch;
        sorted_triangle_batch world_transparent_batch;
        triangle_batch gun_batch;
        sorted_triangle_batch gun_transparent_batch;

        bool is_gun = false;
        bool is_transparent = false;
        triangle_batch *current_triangle_batch = &world_batch;

        material_instance_id current_material = material_instance_id(0U);

        std::vector<point<3, float>> ssao_kernel;

    public:
        explicit renderer_impl(HINSTANCE dll_instance, config const *the_config)
            : the_config(the_config)
            , conf_scr_res(std::get<0>(the_config->resolution), std::get<1>(the_config->resolution))
            , conf_scr_res_f(static_cast<size<2, float>>(conf_scr_res))
            , ddraw1(this)
            , ddraw2(this)
            , d3d1(this)
            , d3ddevice1(this)
            , d3dviewport1(this)
            , ddraw1_primary_menu_surface(this)
            , ddraw1_backbuffer_menu_surface(this, &ddraw1_primary_menu_surface)
            , ddraw1_primary_surface(this)
            , ddraw1_backbuffer_surface(this, conf_scr_res)
            , ddraw1_zbuffer_surface(this)
            , ddraw1_palette(this)
            , dll_instance(dll_instance)
        {
            indexed_bitmap_colors.resize(256, color_rgba8::zero());

            menu_prev_ticks = std::chrono::high_resolution_clock::now();
            menu_curr_ticks = menu_prev_ticks;

            std::uniform_real_distribution<float> ssao_sample_dist(0.0f, 1.0f);
            std::default_random_engine generator;

            ssao_kernel.reserve(16);
            for(size_t i = 0; i < 16; ++i) {
                float scale = (float)i / 16;
                scale *= scale;
                scale = lerp(0.1f, 1.0f, scale);
                ssao_kernel.push_back(
                    normalize(point<3, float>(ssao_sample_dist(generator) * 2.0f - 1.0f,
                                              ssao_sample_dist(generator) * 2.0f - 1.0f,
                                              ssao_sample_dist(generator))) *
                    ssao_sample_dist(generator) * scale);
            }
        }

        void set_renderer_mode(renderer_mode mode) override
        {
            this->mode = mode;
        }

        size<2, int> get_configured_screen_resolution() override
        {
            return conf_scr_res;
        }

        point<2, int> get_cursor_pos(point<2, int> real_pos) override
        {
            if(mode == renderer_mode::menu) {
                // Stretch the point into 640x480
                return make_point(
                    (int)((float)get<x>(real_pos) * (640.0f / (float)get<x>(conf_scr_res))),
                    (int)((float)get<y>(real_pos) * (480.0f / (float)get<y>(conf_scr_res))));
            }

            return real_pos;
        }

        void initialize(HWND parentWnd) override
        {
            materials.create_map("jkgm/materials");

            hWnd = parentWnd;

            original_wkernel_wndproc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
            SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)&renderer_wndproc);

            original_configured_screen_res = get_configured_screen_resolution();

            if(the_config->fullscreen) {
                DEVMODE dm;
                ZeroMemory(&dm, sizeof(dm));
                dm.dmSize = sizeof(dm);
                dm.dmPelsWidth = get<x>(conf_scr_res);
                dm.dmPelsHeight = get<y>(conf_scr_res);
                dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

                ChangeDisplaySettings(&dm, CDS_FULLSCREEN);

                MoveWindow(
                    hWnd, 0, 0, get<x>(conf_scr_res), get<y>(conf_scr_res), /*repaint*/ TRUE);
            }

            hDC = GetDC(hWnd);

            PIXELFORMATDESCRIPTOR pfd;
            ZeroMemory(&pfd, sizeof(pfd));

            pfd.nSize = sizeof(pfd);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.cColorBits = 32;
            pfd.cAlphaBits = 8;
            pfd.cDepthBits = 24;

            int pfdid = ChoosePixelFormat(hDC, &pfd);
            if(pfdid == 0) {
                LOG_ERROR("Renderer ChoosePixelFormat failed: ",
                          win32::win32_category().message(GetLastError()));
            }

            SetPixelFormat(hDC, pfdid, &pfd);

            hGLRC = wglCreateContext(hDC);
            wglMakeCurrent(hDC, hGLRC);

            if(!gladLoadGL()) {
                LOG_ERROR("Failed to load GLAD");
            }

            ShowWindow(hWnd, SW_SHOW);

            gl::set_clear_color(solid(colors::black));
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            SwapBuffers(hDC);

            ogs = std::make_unique<opengl_state>(conf_scr_res, the_config);
            begin_frame();
        }

        void begin_frame()
        {
            gl::set_active_texture_unit(0);
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, ogs->screen_renderbuffer.fbo);
            gl::set_viewport(ogs->screen_renderbuffer.viewport);
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});
        }

        void end_frame()
        {
            // Compose renderbuffer onto window:
            auto current_wnd_sz = get_configured_screen_resolution();

            // Render low pass for bloom
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, ogs->screen_postbuffer2.fbo);
            gl::set_viewport(make_box(make_point(0, 0), current_wnd_sz));

            gl::set_clear_color(solid(colors::black));
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            gl::use_program(ogs->post_low_pass);

            gl::set_uniform_integer(gl::uniform_location_id(0), 0);

            gl::set_active_texture_unit(0);
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->screen_renderbuffer.tex);

            gl::bind_vertex_array(ogs->postmdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

            // Blur and down sample:
            gl::set_active_texture_unit(0);

            gl::texture_view src_tx = ogs->screen_postbuffer2.tex;

            gl::use_program(ogs->post_gauss7);
            gl::set_uniform_integer(gl::uniform_location_id(0), 0);

            auto hdr_vp_size = static_cast<size<2, float>>(current_wnd_sz);
            float hdr_aspect_ratio = get<x>(hdr_vp_size) / get<y>(hdr_vp_size);

            for(auto &hdr_stack_em : ogs->bloom_layers.elements) {
                auto layer_vp_size = static_cast<size<2, float>>(hdr_stack_em.a.viewport.size());
                gl::set_uniform_vector(gl::uniform_location_id(1),
                                       make_size(get<x>(layer_vp_size) * hdr_aspect_ratio,
                                                 get<y>(layer_vp_size)));

                for(int i = 0; i < hdr_stack_em.num_passes; ++i) {
                    // Blur horizontally
                    gl::bind_framebuffer(gl::framebuffer_bind_target::any, hdr_stack_em.b.fbo);
                    gl::set_viewport(hdr_stack_em.b.viewport);

                    gl::set_clear_color(solid(colors::black));
                    gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

                    gl::set_uniform_vector(gl::uniform_location_id(2), make_direction(1.0f, 0.0f));
                    gl::bind_texture(gl::texture_bind_target::texture_2d, src_tx);
                    gl::draw_elements(gl::element_type::triangles,
                                      ogs->postmdl.num_indices,
                                      gl::index_type::uint32);

                    // Blur vertically
                    gl::bind_framebuffer(gl::framebuffer_bind_target::any, hdr_stack_em.a.fbo);

                    gl::set_clear_color(solid(colors::black));
                    gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

                    gl::set_uniform_vector(gl::uniform_location_id(2), make_direction(0.0f, 1.0f));
                    gl::bind_texture(gl::texture_bind_target::texture_2d, hdr_stack_em.b.tex);
                    gl::draw_elements(gl::element_type::triangles,
                                      ogs->postmdl.num_indices,
                                      gl::index_type::uint32);

                    // Set up next stage
                    src_tx = hdr_stack_em.a.tex;
                }
            }

            gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
            gl::set_viewport(make_box(make_point(0, 0), current_wnd_sz));

            gl::set_clear_color(solid(colors::black));
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});
            gl::disable(gl::capability::depth_test);
            gl::disable(gl::capability::cull_face);

            // Copy to front buffer while converting to srgb
            gl::use_program(ogs->post_to_srgb);

            gl::set_uniform_integer(gl::uniform_location_id(0), 0);

            gl::set_active_texture_unit(0);
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->screen_renderbuffer.tex);

            int curr_em = 1;
            for(auto &hdr_stack_em : ogs->bloom_layers.elements) {
                gl::set_uniform_integer(gl::uniform_location_id(curr_em), curr_em);
                gl::set_active_texture_unit(curr_em);
                gl::bind_texture(gl::texture_bind_target::texture_2d, hdr_stack_em.a.tex);
                ++curr_em;
            }

            curr_em = 5;
            for(auto &hdr_stack_em : ogs->bloom_layers.elements) {
                gl::set_uniform_float(gl::uniform_location_id(curr_em), hdr_stack_em.weight);
                ++curr_em;
            }

            gl::bind_vertex_array(ogs->postmdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

            SwapBuffers(hDC);

            begin_frame();
        }

        HRESULT enumerate_devices(LPDDENUMCALLBACKA cb, LPVOID lpContext) override
        {
            // Emit only a single device, the default system device
            std::string fullname = "JkGfxMod OpenGL Device";
            std::string shortname = "device";
            cb(NULL, &fullname[0], &shortname[0], lpContext);

            return DD_OK;
        }

        void set_menu_source(HGDIOBJ ho, char const *indexed_bitmap) override
        {
            indexed_bitmap_dc = ho;
            indexed_bitmap_source = indexed_bitmap;
        }

        void maybe_clear_menu_source(HGDIOBJ ho) override
        {
            if(ho == indexed_bitmap_dc) {
                indexed_bitmap_dc = NULL;
                indexed_bitmap_source = nullptr;
            }
        }

        void set_menu_palette(UINT start, span<RGBQUAD const> entries) override
        {
            UINT curr = start;
            for(auto const &em : entries) {
                if(curr > 255U) {
                    break;
                }

                indexed_bitmap_colors[curr++] =
                    color_rgba8(em.rgbRed, em.rgbGreen, em.rgbBlue, uint8_t(255U));
            }
        }

        void present_menu_gdi() override
        {
            if(!indexed_bitmap_source) {
                end_frame();
                return;
            }

            // Copy new data from menu source
            if(indexed_bitmap_source) {
                for(size_t idx = 0U; idx < ogs->menu_texture_data.size(); ++idx) {
                    uint8_t index = indexed_bitmap_source[idx];
                    ogs->menu_texture_data[idx] = indexed_bitmap_colors[index];
                }
            }

            // Blit texture data into texture
            gl::set_active_texture_unit(0);
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->menu_texture);
            gl::tex_sub_image_2d(gl::texture_bind_target::texture_2d,
                                 0,
                                 make_box(make_point(0, 0), make_point(640, 480)),
                                 gl::texture_pixel_format::rgba,
                                 gl::texture_pixel_type::uint8,
                                 make_span(ogs->menu_texture_data).as_const_bytes());

            // Render
            gl::enable(gl::capability::blend);
            gl::disable(gl::capability::depth_test);
            gl::use_program(ogs->menu_program);
            gl::set_uniform_integer(gl::uniform_location_id(0), 0);

            gl::bind_vertex_array(ogs->postmdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

            end_frame();
        }

        void present_menu_surface_body()
        {
            // Copy new data from menu source
            for(size_t idx = 0U; idx < ogs->menu_texture_data.size(); ++idx) {
                uint8_t index = ddraw1_primary_menu_surface.buffer[idx];
                auto &palent = ddraw1_palette.entries[index];
                ogs->menu_texture_data[idx] = ddraw1_palette.srgb_entries[index];
            }

            // Blit texture data into texture
            gl::set_active_texture_unit(0);
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->menu_texture);
            gl::tex_sub_image_2d(gl::texture_bind_target::texture_2d,
                                 0,
                                 make_box(make_point(0, 0), make_point(640, 480)),
                                 gl::texture_pixel_format::rgba,
                                 gl::texture_pixel_type::uint8,
                                 make_span(ogs->menu_texture_data).as_const_bytes());

            // Render
            gl::enable(gl::capability::blend);
            gl::disable(gl::capability::depth_test);
            gl::use_program(ogs->menu_program);
            gl::set_uniform_integer(gl::uniform_location_id(0), 0);

            gl::bind_vertex_array(ogs->postmdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

            end_frame();
        }

        void present_menu_surface_immediate() override
        {
            menu_prev_ticks = std::chrono::high_resolution_clock::now();
            menu_curr_ticks = menu_prev_ticks;
            menu_accumulator = 0.0;

            present_menu_surface_body();
        }

        void present_menu_surface_delayed() override
        {
            // Present menu, emulating a 60 Hz monitor
            menu_prev_ticks = menu_curr_ticks;
            menu_curr_ticks = std::chrono::high_resolution_clock::now();
            auto elapsed_ticks = menu_curr_ticks - menu_prev_ticks;
            double elapsed = std::chrono::duration<double>(elapsed_ticks).count();

            menu_accumulator += elapsed;
            if(menu_accumulator >= (1.0 / 60.0)) {
                menu_accumulator = 0.0;
                present_menu_surface_body();
            }
        }

        void update_hud_texture()
        {
            gl::enable(gl::capability::blend);
            gl::disable(gl::capability::depth_test);

            ZeroMemory(ogs->hud_texture_data.data(), ogs->hud_texture_data.size());

            for(size_t i = 0; i < ddraw1_backbuffer_surface.buffer.size(); ++i) {
                auto const &in_em = ddraw1_backbuffer_surface.buffer[i];

                // Convert from RGB565 to RGBA8888
                ogs->hud_texture_data[i] = rgb565_key_to_srgb_a8(
                    in_em, /*transparent?*/ in_em == ddraw1_backbuffer_surface.color_key);
            }

            // Blit texture data into texture
            gl::set_active_texture_unit(0);
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->hud_texture);
            gl::tex_sub_image_2d(gl::texture_bind_target::texture_2d,
                                 0,
                                 make_box(make_point(0, 0),
                                          make_point(get<x>(conf_scr_res), get<y>(conf_scr_res))),
                                 gl::texture_pixel_format::rgba,
                                 gl::texture_pixel_type::uint8,
                                 make_span(ogs->hud_texture_data).as_const_bytes());

            // Render
            gl::use_program(ogs->menu_program);
            gl::set_uniform_integer(gl::uniform_location_id(0), 0);

            gl::bind_vertex_array(ogs->postmdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

            for(auto &em : ddraw1_backbuffer_surface.buffer) {
                em = ddraw1_backbuffer_surface.color_key;
            }
        }

        void update_current_batch()
        {
            if(is_gun && is_transparent) {
                current_triangle_batch = &gun_transparent_batch;
            }
            else if(is_gun) {
                current_triangle_batch = &gun_batch;
            }
            else if(is_transparent) {
                current_triangle_batch = &world_transparent_batch;
            }
            else {
                current_triangle_batch = &world_batch;
            }
        }

        void bind_material(material_instance_id id, bool force_opaque)
        {
            if(id.get() == 0U) {
                // This is the default (untextured) material
                gl::set_active_texture_unit(1);
                gl::bind_texture(gl::texture_bind_target::texture_2d, gl::default_texture);
                gl::set_active_texture_unit(0);
                gl::bind_texture(gl::texture_bind_target::texture_2d, gl::default_texture);

                // Disable features
                gl::set_uniform_vector(gl::uniform_location_id(1), point<4, float>::zero());

                // Albedo factor
                gl::set_uniform_vector(gl::uniform_location_id(3), color::fill(1.0f));

                // Emissive factor
                gl::set_uniform_vector(gl::uniform_location_id(5), color_rgb::zero());

                // Alpha cutoff
                gl::set_uniform_float(gl::uniform_location_id(6), 0.0f);
            }
            else {
                auto const &mat = vidmem_texture_surfaces.at(id.get() - 1);

                gl::texture_view albedo_map = gl::default_texture;
                if(mat->albedo_map.has_value()) {
                    albedo_map = at(ogs->srgb_textures, *mat->albedo_map).handle;
                }

                gl::texture_view emissive_map = gl::default_texture;
                if(mat->emissive_map.has_value()) {
                    emissive_map = at(ogs->srgb_textures, *mat->emissive_map).handle;
                }

                gl::set_active_texture_unit(1);
                gl::bind_texture(gl::texture_bind_target::texture_2d, emissive_map);
                gl::set_active_texture_unit(0);
                gl::bind_texture(gl::texture_bind_target::texture_2d, albedo_map);

                // Enable features
                gl::set_uniform_vector(
                    gl::uniform_location_id(1),
                    make_point(mat->albedo_map.has_value() ? 1.0f : 0.0f,
                               mat->emissive_map.has_value() ? 1.0f : 0.0f,
                               ((mat->alpha_mode == material_alpha_mode::mask) && (!force_opaque))
                                   ? 1.0f
                                   : 0.0f,
                               /*unused*/ 0.0f));

                gl::set_uniform_vector(gl::uniform_location_id(3), mat->albedo_factor);
                gl::set_uniform_vector(gl::uniform_location_id(5), mat->emissive_factor);
                gl::set_uniform_float(gl::uniform_location_id(6), mat->alpha_cutoff);
            }

            current_material = id;
        }

        void draw_batch(triangle_batch const &tb, triangle_buffer_model *trimdl, bool force_opaque)
        {
            gl::bind_vertex_array(trimdl->vao);

            size_t curr_offset = 0U;
            size_t num_verts = 0U;

            bind_material(material_instance_id(0U), force_opaque);

            for(auto const &tri : tb) {
                if(current_material != tri.material) {
                    // Draw pending elements from previous material
                    if(num_verts > 0) {
                        gl::draw_arrays(gl::element_type::triangles, curr_offset, num_verts);

                        curr_offset += num_verts;
                        num_verts = 0U;
                    }

                    bind_material(tri.material, force_opaque);
                }

                num_verts += 3;
            }

            if(num_verts > 0) {
                gl::draw_arrays(gl::element_type::triangles, curr_offset, num_verts);

                curr_offset += num_verts;
                num_verts = 0U;
            }
        }

        void fill_buffer(triangle_batch const &tb, triangle_buffer_model *mdl)
        {
            mdl->maybe_grow_buffers(tb.capacity() * 3);

            int curr_vert = 0;
            for(auto const &tri : tb) {
                mdl->pos[curr_vert] = tri.v0.pos;
                mdl->texcoords[curr_vert] = tri.v0.texcoords;
                mdl->color[curr_vert] = tri.v0.color;
                mdl->normals[curr_vert] = tri.normal;

                ++curr_vert;

                mdl->pos[curr_vert] = tri.v1.pos;
                mdl->texcoords[curr_vert] = tri.v1.texcoords;
                mdl->color[curr_vert] = tri.v1.color;
                mdl->normals[curr_vert] = tri.normal;

                ++curr_vert;

                mdl->pos[curr_vert] = tri.v2.pos;
                mdl->texcoords[curr_vert] = tri.v2.texcoords;
                mdl->color[curr_vert] = tri.v2.color;
                mdl->normals[curr_vert] = tri.normal;

                ++curr_vert;
            }

            mdl->num_vertices = curr_vert;
            mdl->update_buffers();
        }

        void draw_game_opaque_into_gbuffer()
        {
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, ogs->gbuffer.fbo);
            gl::clear({gl::clear_flag::depth, gl::clear_flag::color});

            // Draw batches
            gl::disable(gl::capability::blend);
            gl::enable(gl::capability::depth_test);
            gl::set_depth_mask(true);
            gl::disable(gl::capability::cull_face);
            gl::set_face_cull_mode(gl::face_mode::front_and_back);
            gl::set_blend_function(gl::blend_function::one,
                                   gl::blend_function::one_minus_source_alpha);
            gl::set_depth_function(gl::comparison_function::less);

            gl::use_program(ogs->game_opaque_pass_program);

            gl::set_uniform_vector(gl::uniform_location_id(0),
                                   static_cast<size<2, float>>(conf_scr_res));
            gl::set_uniform_integer(gl::uniform_location_id(2), 0);
            gl::set_uniform_integer(gl::uniform_location_id(4), 1);

            // Draw first pass (opaque world geometry)
            draw_batch(world_batch, &ogs->world_trimdl, /*force opaque*/ true);

            // Draw second pass (transparent world geometry with alpha testing)
            draw_batch(
                world_transparent_batch, &ogs->world_transparent_trimdl, /*force opaque*/ true);

            // Draw fourth pass (opaque gun geometry)
            draw_batch(gun_batch, &ogs->gun_trimdl, /*force opaque*/ true);

            // Draw fifth pass (transparent gun geometry with alpha testing)
            draw_batch(gun_transparent_batch, &ogs->gun_transparent_trimdl, /*force opaque*/ true);
        }

        void draw_game_ssao_postprocess()
        {
            // Compute SSAO:
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, ogs->ssao_occlusionbuffer->fbo);
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            gl::use_program(ogs->game_post_ssao_program);
            gl::set_uniform_integer(gl::uniform_location_id(0), 0);
            gl::set_uniform_integer(gl::uniform_location_id(1), 1);

            for(size_t i = 0; i < ssao_kernel.size(); ++i) {
                gl::set_uniform_vector(gl::uniform_location_id(2 + i), ssao_kernel[i]);
            }

            gl::set_active_texture_unit(1);
            gl::bind_texture(gl::texture_bind_target::texture_2d, *ogs->ssao_noise_texture);

            gl::set_active_texture_unit(0);
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->gbuffer.depth_nrm_tex);

            gl::bind_vertex_array(ogs->postmdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

            // Blur SSAO:
            gl::use_program(ogs->post_gauss3);
            gl::set_uniform_integer(gl::uniform_location_id(0), 0);

            auto hdr_vp_size =
                static_cast<size<2, float>>(ogs->screen_renderbuffer.viewport.size());
            gl::set_uniform_vector(gl::uniform_location_id(1), hdr_vp_size);

            // - Horizontal:
            gl::set_uniform_vector(gl::uniform_location_id(2), make_direction(1.0f, 0.0f));

            gl::bind_framebuffer(gl::framebuffer_bind_target::any, ogs->screen_postbuffer1.fbo);
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->ssao_occlusionbuffer->tex);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

            // - Vertical:
            gl::set_uniform_vector(gl::uniform_location_id(2), make_direction(0.0f, 1.0f));

            gl::bind_framebuffer(gl::framebuffer_bind_target::any, ogs->ssao_occlusionbuffer->fbo);
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->screen_postbuffer1.tex);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);
        }

        void draw_game_opaque_composite()
        {
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, ogs->screen_renderbuffer.fbo);
            gl::clear({gl::clear_flag::color});
            gl::disable(gl::capability::depth_test);

            gl::use_program(ogs->game_post_opaque_composite_program);
            gl::set_uniform_integer(gl::uniform_location_id(0), 0);
            gl::set_uniform_integer(gl::uniform_location_id(1), 1);
            gl::set_uniform_integer(gl::uniform_location_id(2), 2);

            gl::set_active_texture_unit(2);
            if(the_config->enable_ssao) {
                gl::bind_texture(gl::texture_bind_target::texture_2d,
                                 ogs->ssao_occlusionbuffer->tex);
            }
            else {
                gl::bind_texture(gl::texture_bind_target::texture_2d, gl::default_texture);
            }

            gl::set_active_texture_unit(1);
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->gbuffer.emissive_tex);

            gl::set_active_texture_unit(0);
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->gbuffer.color_tex);

            gl::bind_vertex_array(ogs->postmdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);
        }

        void draw_game_gbuffer_pass()
        {
            draw_game_opaque_into_gbuffer();

            if(the_config->enable_ssao) {
                draw_game_ssao_postprocess();
            }

            draw_game_opaque_composite();
        }

        void draw_game_transparency_pass()
        {
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, ogs->screen_renderbuffer.fbo);

            // Draw batches
            gl::disable(gl::capability::blend);
            gl::enable(gl::capability::depth_test);
            gl::set_depth_mask(true);
            gl::disable(gl::capability::cull_face);
            gl::set_face_cull_mode(gl::face_mode::front_and_back);
            gl::set_blend_function(gl::blend_function::one,
                                   gl::blend_function::one_minus_source_alpha);
            gl::set_depth_function(gl::comparison_function::less);

            gl::use_program(ogs->game_transparency_pass_program);
            gl::set_uniform_vector(gl::uniform_location_id(0),
                                   static_cast<size<2, float>>(conf_scr_res));
            gl::set_uniform_integer(gl::uniform_location_id(2), 0);
            gl::set_uniform_integer(gl::uniform_location_id(4), 1);

            gl::set_active_texture_unit(0);

            // Draw third pass (transparent world geometry with alpha blending)
            gl::enable(gl::capability::blend);
            gl::set_depth_mask(false);
            draw_batch(
                world_transparent_batch, &ogs->world_transparent_trimdl, /*force opaque*/ false);

            // Redraw gun overlay after z-clear
            gl::set_depth_mask(true);
            gl::clear({gl::clear_flag::depth});

            draw_batch(gun_batch, &ogs->gun_trimdl, /*force opaque*/ true);
            draw_batch(gun_transparent_batch, &ogs->gun_transparent_trimdl, /*force opaque*/ true);

            // Draw gun transparency
            draw_batch(gun_transparent_batch,
                       &ogs->gun_transparent_trimdl, /*force opaque*/
                       false);

            gl::enable(gl::capability::depth_test);
            gl::enable(gl::capability::blend);
            gl::set_depth_mask(true);
        }

        void begin_game() override {}

        void end_game() override {}

        static point<4, float> d3dtl_to_point(size<2, float> const &screen_dims,
                                              D3DTLVERTEX const &p)
        {
            // Reassign w for full-screen overlay vertices
            float w = 1.0f;
            if(p.rhw != 0.0f) {
                w = 1.0f / p.rhw;
            }

            // Convert pretransformed vertex to phony view space
            return make_point(w * ((p.sx / (get<x>(screen_dims) * 0.5f)) - 1.0f),
                              w * ((-p.sy / (get<y>(screen_dims) * 0.5f)) + 1.0f),
                              w * (-p.sz),
                              w);
        }

        void execute_game(IDirect3DExecuteBuffer *cmdbuf, IDirect3DViewport *vp) override
        {
            D3DEXECUTEDATA ed;
            cmdbuf->GetExecuteData(&ed);

            D3DEXECUTEBUFFERDESC ebd;
            cmdbuf->Lock(&ebd);

            D3DVIEWPORT vpd;
            vp->GetViewport(&vpd);

            auto vertex_span =
                make_span((D3DTLVERTEX const *)((char const *)ebd.lpData + ed.dwVertexOffset),
                          ed.dwVertexCount);

            auto cmd_span = make_span((char const *)ebd.lpData + ed.dwInstructionOffset,
                                      ed.dwInstructionLength);

            while(!cmd_span.empty()) {
                D3DINSTRUCTION inst = *(D3DINSTRUCTION const *)cmd_span.data();
                cmd_span = cmd_span.subspan(sizeof(D3DINSTRUCTION), span_to_end);

                for(size_t i = 0; i < inst.wCount; ++i) {
                    switch(inst.bOpcode) {
                    case D3DOP_EXIT:
                        break;

                    case D3DOP_PROCESSVERTICES: {
                        auto const *payload = (D3DPROCESSVERTICES const *)cmd_span.data();
                        if(payload->dwFlags != D3DPROCESSVERTICES_COPY || payload->wStart != 0 ||
                           payload->wDest != 0) {
                            LOG_ERROR("Unimplemented process vertices opcode ignored: ",
                                      payload->dwFlags,
                                      " ",
                                      payload->dwCount,
                                      " ",
                                      payload->wStart,
                                      " ",
                                      payload->wDest);
                            abort();
                        }
                    } break;

                    case D3DOP_STATERENDER: {
                        auto const *payload = (D3DSTATE const *)cmd_span.data();
                        switch(payload->drstRenderStateType) {
                        case D3DRENDERSTATE_TEXTUREHANDLE:
                            current_material = material_instance_id((size_t)payload->dwArg[0]);
                            break;

                        // Silently ignore some useless commands
                        case D3DRENDERSTATE_ANTIALIAS:
                        case D3DRENDERSTATE_TEXTUREPERSPECTIVE:
                        case D3DRENDERSTATE_FILLMODE:
                        case D3DRENDERSTATE_TEXTUREMAG:
                        case D3DRENDERSTATE_TEXTUREMIN:
                        case D3DRENDERSTATE_SRCBLEND:
                        case D3DRENDERSTATE_WRAPU:
                        case D3DRENDERSTATE_WRAPV:
                        case D3DRENDERSTATE_DESTBLEND:
                        case D3DRENDERSTATE_ALPHAFUNC:
                        case D3DRENDERSTATE_DITHERENABLE:
                        case D3DRENDERSTATE_FOGENABLE:
                        case D3DRENDERSTATE_SUBPIXEL:
                        case D3DRENDERSTATE_SUBPIXELX:
                        case D3DRENDERSTATE_TEXTUREMAPBLEND:
                        case D3DRENDERSTATE_STIPPLEDALPHA:
                        case D3DRENDERSTATE_SHADEMODE:
                        case D3DRENDERSTATE_ZENABLE:
                        case D3DRENDERSTATE_SPECULARENABLE:
                        case D3DRENDERSTATE_ALPHATESTENABLE:
                        case D3DRENDERSTATE_CULLMODE:
                        case D3DRENDERSTATE_ZFUNC:
                        case D3DRENDERSTATE_MONOENABLE:
                            break;

                        case D3DRENDERSTATE_ALPHABLENDENABLE:
                            is_transparent = (payload->dwArg[0] != 0);
                            update_current_batch();
                            break;

                        case D3DRENDERSTATE_ZWRITEENABLE:
                            if(!payload->dwArg[0]) {
                                // ACTUALLY means drawing the weapon overlay.
                                is_gun = true;
                                update_current_batch();
                            }
                            break;

                        default:
                            LOG_WARNING("Ignored unknown state render opcode: ",
                                        (int)payload->dtstTransformStateType);
                            break;
                        }
                    } break;

                    case D3DOP_TRIANGLE: {
                        auto const *payload = (D3DTRIANGLE const *)cmd_span.data();

                        auto const &v1 = vertex_span.data()[payload->v1];
                        auto const &v2 = vertex_span.data()[payload->v2];
                        auto const &v3 = vertex_span.data()[payload->v3];

                        auto c1 = srgb_to_linear(
                            to_float_color(color_rgba8((uint8_t)RGBA_GETRED(v1.color),
                                                       (uint8_t)RGBA_GETGREEN(v1.color),
                                                       (uint8_t)RGBA_GETBLUE(v1.color),
                                                       (uint8_t)RGBA_GETALPHA(v1.color))));
                        auto c2 = srgb_to_linear(
                            to_float_color(color_rgba8((uint8_t)RGBA_GETRED(v2.color),
                                                       (uint8_t)RGBA_GETGREEN(v2.color),
                                                       (uint8_t)RGBA_GETBLUE(v2.color),
                                                       (uint8_t)RGBA_GETALPHA(v2.color))));
                        auto c3 = srgb_to_linear(
                            to_float_color(color_rgba8((uint8_t)RGBA_GETRED(v3.color),
                                                       (uint8_t)RGBA_GETGREEN(v3.color),
                                                       (uint8_t)RGBA_GETBLUE(v3.color),
                                                       (uint8_t)RGBA_GETALPHA(v3.color))));

                        current_triangle_batch->insert(
                            triangle(triangle_vertex(d3dtl_to_point(conf_scr_res_f, v1),
                                                     make_point(v1.tu, v1.tv),
                                                     extend(get<rgb>(c1) * get<a>(c1), get<a>(c1))),
                                     triangle_vertex(d3dtl_to_point(conf_scr_res_f, v2),
                                                     make_point(v2.tu, v2.tv),
                                                     extend(get<rgb>(c2) * get<a>(c2), get<a>(c2))),
                                     triangle_vertex(d3dtl_to_point(conf_scr_res_f, v3),
                                                     make_point(v3.tu, v3.tv),
                                                     extend(get<rgb>(c3) * get<a>(c3), get<a>(c3))),
                                     current_material));
                    } break;

                    default:
                        LOG_WARNING(
                            "Unimplemented execute buffer opcode ", inst.bOpcode, " was ignored");
                    }

                    cmd_span = cmd_span.subspan(inst.bSize, span_to_end);
                }
            }

            cmdbuf->Unlock();
        }

        void present_game() override
        {
            world_batch.sort();
            world_transparent_batch.sort();
            gun_batch.sort();
            gun_transparent_batch.sort();

            fill_buffer(world_batch, &ogs->world_trimdl);
            fill_buffer(world_transparent_batch, &ogs->world_transparent_trimdl);
            fill_buffer(gun_batch, &ogs->gun_trimdl);
            fill_buffer(gun_transparent_batch, &ogs->gun_transparent_trimdl);

            draw_game_gbuffer_pass();
            draw_game_transparency_pass();

            update_hud_texture();
            end_frame();

            // Reset state:
            is_gun = false;
            is_transparent = false;
            current_triangle_batch = &world_batch;
            current_material = material_instance_id(0U);

            world_batch.clear();
            world_transparent_batch.clear();
            gun_batch.clear();
            gun_transparent_batch.clear();
        }

        void depth_clear_game() override
        {
            // JK calls this once per frame, immediately after present.
            // Ignore: the framebuffer is already cleared elsewhere.
        }

        IDirectDraw *get_directdraw() override
        {
            return &ddraw1;
        }

        IDirectDraw2 *get_directdraw2() override
        {
            return &ddraw2;
        }

        IDirect3D *get_direct3d() override
        {
            return &d3d1;
        }

        IDirect3DDevice *get_direct3ddevice() override
        {
            return &d3ddevice1;
        }

        IDirect3DViewport *get_direct3dviewport() override
        {
            return &d3dviewport1;
        }

        IDirectDrawSurface *get_directdraw_primary_surface() override
        {
            if(mode == renderer_mode::menu) {
                return &ddraw1_primary_menu_surface;
            }

            return &ddraw1_primary_surface;
        }

        IDirectDrawSurface *get_directdraw_backbuffer_surface() override
        {
            return &ddraw1_backbuffer_surface;
        }

        IDirectDrawSurface *get_directdraw_backbuffer_menu_surface() override
        {
            return &ddraw1_backbuffer_menu_surface;
        }

        IDirectDrawSurface *get_directdraw_zbuffer_surface(DDSURFACEDESC const &desc) override
        {
            ddraw1_zbuffer_surface.sd = desc;
            return &ddraw1_zbuffer_surface;
        }

        IDirectDrawSurface *get_directdraw_offscreen_surface(DDSURFACEDESC const &desc) override
        {
            if(mode == renderer_mode::menu) {
                ddraw1_offscreen_menu_surface.set_surface_desc(desc);
                return &ddraw1_offscreen_menu_surface;
            }

            ddraw1_offscreen_surface.set_surface_desc(desc);
            return &ddraw1_offscreen_surface;
        }

        IDirectDrawSurface *
            get_directdraw_sysmem_texture_surface(DDSURFACEDESC const &desc) override
        {
            auto get_matching_buffer = [&] {
                size_t num_pixels = desc.dwWidth * desc.dwHeight;
                for(auto &tex : sysmem_texture_surfaces) {
                    if(tex->refct <= 0 && tex->num_pixels == num_pixels) {
                        return tex.get();
                    }
                }

                sysmem_texture_surfaces.push_back(
                    std::make_unique<sysmem_texture_surface>(num_pixels));
                return sysmem_texture_surfaces.back().get();
            };

            auto *rv = get_matching_buffer();
            rv->set_surface_desc(desc);
            rv->AddRef();
            return rv;
        }

        IDirectDrawSurface *
            get_directdraw_vidmem_texture_surface(DDSURFACEDESC const &desc) override
        {
            auto get_matching_buffer = [&] {
                for(auto &tex : vidmem_texture_surfaces) {
                    if(tex->refct <= 0) {
                        tex->refct = 0;
                        tex->clear();
                        return tex.get();
                    }
                }

                vidmem_texture_surfaces.push_back(std::make_unique<vidmem_texture_surface>(
                    this, material_instance_id(vidmem_texture_surfaces.size() + 1)));
                return vidmem_texture_surfaces.back().get();
            };

            auto *rv = get_matching_buffer();
            rv->set_surface_desc(desc);
            rv->AddRef();
            return rv;
        }

        IDirectDrawPalette *get_directdraw_palette(span<PALETTEENTRY const> entries) override
        {
            std::copy(entries.begin(), entries.end(), ddraw1_palette.entries.begin());
            ddraw1_palette.recompute_palette();
            return &ddraw1_palette;
        }

        IDirect3DExecuteBuffer *create_direct3dexecutebuffer(size_t bufsz) override
        {
            auto get_matching_buffer = [&] {
                // Look for expired execute buffer of the same size
                for(auto &em : execute_buffers) {
                    if(em->refct <= 0 && em->bufsz == bufsz) {
                        return em.get();
                    }
                }

                execute_buffers.push_back(std::make_unique<execute_buffer>(bufsz));
                return execute_buffers.back().get();
            };

            auto *rv = get_matching_buffer();
            rv->AddRef();
            return rv;
        }

        std::optional<material const *> get_replacement_material(md5 const &sig) override
        {
            return materials.get_material(sig);
        }

        std::optional<srgb_texture_id> get_existing_free_srgb_texture(size<2, int> const &dims)
        {
            for(size_t i = 0; i < ogs->srgb_textures.size(); ++i) {
                auto &em = ogs->srgb_textures[i];
                if(em.refct <= 0 && em.dims == dims) {
                    // This texture is a match. Clean it up before returning.
                    if(em.origin_filename.has_value()) {
                        ogs->file_to_srgb_texture_map.erase(*em.origin_filename);
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
                auto &em = at(ogs->srgb_textures, *existing_buf);
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
            srgb_texture_id rv(ogs->srgb_textures.size());
            ogs->srgb_textures.emplace_back(dims);

            auto &em = ogs->srgb_textures.back();
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
            gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::linear);
            gl::set_texture_min_filter(gl::texture_bind_target::texture_2d,
                                       gl::min_filter::linear_mipmap_linear);

            return rv;
        }

        srgb_texture_id get_srgb_texture_from_filename(fs::path const &file) override
        {
            auto it = ogs->file_to_srgb_texture_map.find(file);
            if(it != ogs->file_to_srgb_texture_map.end()) {
                // Image file already loaded
                srgb_texture_id rv(it->second);
                ++at(ogs->srgb_textures, rv).refct;
                return rv;
            }

            auto fs = make_file_input_block(file);
            auto img = load_image(fs.get());

            auto rv = create_srgb_texture_from_buffer(img->dimensions,
                                                      make_span(img->data).as_const_bytes());

            auto &em = at(ogs->srgb_textures, rv);
            em.origin_filename = file;
            ogs->file_to_srgb_texture_map.emplace(file, rv.get());

            return rv;
        }

        void release_srgb_texture(srgb_texture_id id) override
        {
            --at(ogs->srgb_textures, id).refct;
        }
    };
}

std::unique_ptr<jkgm::renderer> jkgm::create_renderer(HINSTANCE dll_instance,
                                                      config const *the_config)
{
    return std::make_unique<jkgm::renderer_impl>(dll_instance, the_config);
}