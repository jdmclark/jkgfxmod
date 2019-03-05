#include "renderer.hpp"
#include "base/file_stream.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"
#include "base/win32.hpp"
#include "glad/glad.h"
#include "glutil/buffer.hpp"
#include "glutil/gl.hpp"
#include "glutil/program.hpp"
#include "glutil/shader.hpp"
#include "glutil/texture.hpp"
#include "glutil/vertex_array.hpp"
#include "math/colors.hpp"
#include <Windows.h>
#include <chrono>

namespace jkgm {
    LRESULT CALLBACK renderer_wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    ATOM register_renderer_wndclass(HINSTANCE dll_instance)
    {
        WNDCLASSEX wc;
        ZeroMemory(&wc, sizeof(wc));

        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = renderer_wndproc;
        wc.hInstance = dll_instance;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpszClassName = L"wKernelAdvRenderer";

        return RegisterClassEx(&wc);
    }

    gl::shader compile_shader_from_file(fs::path const &filename, gl::shader_type type)
    {
        gl::shader rv(type);

        auto fs = make_file_input_stream(filename);
        memory_block mb;
        memory_output_block mob(&mb);
        fs->copy_to(&mob);

        gl::shader_source(rv, make_string_span(""), make_span(mb.str()));
        gl::compile_shader(rv);
        if(!gl::get_shader_compile_status(rv)) {
            LOG_ERROR(
                "Failed to compile ", filename.generic_string(), ": ", gl::get_shader_info_log(rv));
        }

        return rv;
    }

    void link_program_from_files(std::string const &name,
                                 gl::program *prog,
                                 fs::path const &vx,
                                 fs::path const &fg)
    {
        auto vx_shader = compile_shader_from_file(vx, gl::shader_type::vertex);
        auto fg_shader = compile_shader_from_file(fg, gl::shader_type::fragment);

        gl::attach_shader(*prog, vx_shader);
        gl::attach_shader(*prog, fg_shader);

        gl::link_program(*prog);

        if(!gl::get_program_link_status(*prog)) {
            LOG_ERROR("Failed to link program ", name, ": ", gl::get_program_info_log(*prog));
        }
    }

    struct opengl_state {
        gl::program menu_program;

        gl::vertex_array menu_vao;
        gl::buffer menu_vb;
        gl::buffer menu_ib;
        gl::texture menu_texture;

        std::vector<color_rgba8> menu_texture_data;

        opengl_state()
        {
            LOG_DEBUG("Loading OpenGL assets");

            link_program_from_files(
                "menu", &menu_program, "jkgm/shaders/menu.vert", "jkgm/shaders/menu.frag");

            gl::bind_vertex_array(menu_vao);

            std::array<point<2, float>, 4> menu_points{make_point(-1.0f, -1.0f),
                                                       make_point(1.0f, -1.0f),
                                                       make_point(-1.0f, 1.0f),
                                                       make_point(1.0f, 1.0f)};
            std::array<uint32_t, 6> menu_indices{0, 1, 2, 2, 1, 3};

            gl::enable_vertex_attrib_array(0U);
            gl::bind_buffer(gl::buffer_bind_target::array, menu_vb);
            gl::buffer_data(gl::buffer_bind_target::array,
                            make_span(menu_points).as_const_bytes(),
                            gl::buffer_usage::static_draw);
            gl::vertex_attrib_pointer(/*index*/ 0,
                                      /*elements*/ 2,
                                      gl::vertex_element_type::float32,
                                      /*normalized*/ false);

            gl::bind_buffer(gl::buffer_bind_target::element_array, menu_ib);
            gl::buffer_data(gl::buffer_bind_target::element_array,
                            make_span(menu_indices).as_const_bytes(),
                            gl::buffer_usage::static_draw);

            gl::bind_texture(gl::texture_bind_target::texture_2d, menu_texture);
            gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                             0,
                             gl::texture_internal_format::rgba,
                             make_size(1024, 1024),
                             gl::texture_pixel_format::bgra,
                             gl::texture_pixel_type::uint8,
                             make_span<char const>(nullptr, 0U));
            gl::set_texture_max_level(gl::texture_bind_target::texture_2d, 0U);
            gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::linear);

            menu_texture_data.resize(640 * 480, color_rgba8::zero());
        }
    };

    class renderer_impl : public renderer {
    private:
        HINSTANCE dll_instance;
        HWND hWnd;
        HDC hDC;
        HGLRC hGLRC;

        std::unique_ptr<opengl_state> ogs;

        char const *indexed_bitmap_source = nullptr;
        std::vector<color_rgba8> indexed_bitmap_colors;

        using timestamp_t = std::chrono::high_resolution_clock::time_point;
        timestamp_t prev_ticks;
        timestamp_t curr_ticks;
        double accumulator = 0.0;

    public:
        explicit renderer_impl(HINSTANCE dll_instance)
            : dll_instance(dll_instance)
        {
            indexed_bitmap_colors.resize(256, color_rgba8::zero());
            register_renderer_wndclass(dll_instance);
        }

        void initialize() override
        {
            hWnd = CreateWindow(L"wKernelAdvRenderer",
                                L"JkGfxMod",
                                WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                700,
                                300,
                                1920,
                                1080,
                                /*parent*/ NULL,
                                /*menu*/ NULL,
                                dll_instance,
                                /*param*/ NULL);

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

            gl::set_clear_color(solid(colors::cornflower_blue));
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            SwapBuffers(hDC);

            ogs = std::make_unique<opengl_state>();

            prev_ticks = std::chrono::high_resolution_clock::now();
            curr_ticks = prev_ticks;
        }

        void set_menu_source(char const *indexed_bitmap) override
        {
            indexed_bitmap_source = indexed_bitmap;
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

        void present_menu() override
        {
            // Cap menu presentation at 15 Hz
            constexpr double menu_update_interval = (1.0 / 60.0);

            prev_ticks = curr_ticks;
            curr_ticks = std::chrono::high_resolution_clock::now();
            auto elapsed_ticks = curr_ticks - prev_ticks;
            double elapsed = std::chrono::duration<double>(elapsed_ticks).count();

            accumulator += elapsed;
            if(accumulator < menu_update_interval) {
                //return;
            }

            accumulator = 0.0;

            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

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
            gl::use_program(ogs->menu_program);
            gl::set_uniform_integer(gl::uniform_location_id(0), 0);

            gl::bind_vertex_array(ogs->menu_vao);
            gl::draw_elements(gl::element_type::triangles, 6U, gl::index_type::uint32);

            SwapBuffers(hDC);
        }
    };
}

std::unique_ptr<jkgm::renderer> jkgm::create_renderer(HINSTANCE dll_instance)
{
    return std::make_unique<jkgm::renderer_impl>(dll_instance);
}