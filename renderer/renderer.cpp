#include "renderer.hpp"
#include "base/file_stream.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"
#include "base/win32.hpp"
#include "d3d_impl.hpp"
#include "d3ddevice_impl.hpp"
#include "d3dexecutebuffer_impl.hpp"
#include "d3dviewport_impl.hpp"
#include "ddraw2_impl.hpp"
#include "ddraw_backbuffer_surface.hpp"
#include "ddraw_impl.hpp"
#include "ddraw_palette_impl.hpp"
#include "ddraw_phony_surface.hpp"
#include "ddraw_primary_surface.hpp"
#include "ddraw_sysmem_texture_surface.hpp"
#include "ddraw_vidmem_texture_surface.hpp"
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

    class post_model {
    public:
        gl::vertex_array vao;
        gl::buffer vb;
        gl::buffer ib;
        unsigned int num_indices = 6U;

        post_model()
        {
            gl::bind_vertex_array(vao);

            std::array<point<2, float>, 4> post_points{make_point(-1.0f, -1.0f),
                                                       make_point(1.0f, -1.0f),
                                                       make_point(-1.0f, 1.0f),
                                                       make_point(1.0f, 1.0f)};
            std::array<uint32_t, 6> post_indices{0, 1, 2, 2, 1, 3};

            gl::enable_vertex_attrib_array(0U);
            gl::bind_buffer(gl::buffer_bind_target::array, vb);
            gl::buffer_data(gl::buffer_bind_target::array,
                            make_span(post_points).as_const_bytes(),
                            gl::buffer_usage::static_draw);
            gl::vertex_attrib_pointer(
                /*index*/ 0,
                /*elements*/ 2,
                gl::vertex_element_type::float32,
                /*normalized*/ false);

            gl::bind_buffer(gl::buffer_bind_target::element_array, ib);
            gl::buffer_data(gl::buffer_bind_target::element_array,
                            make_span(post_indices).as_const_bytes(),
                            gl::buffer_usage::static_draw);
        }
    };

    class render_buffer {
    public:
        gl::framebuffer fbo;
        gl::texture tex;
        gl::renderbuffer rbo;
        box<2, int> viewport;

        explicit render_buffer(size<2, int> dims)
            : viewport(make_point(0, 0), dims)
        {
            constexpr int num_samples = 16;

            gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

            gl::bind_renderbuffer(rbo);
            gl::renderbuffer_storage_multisample(num_samples, gl::renderbuffer_format::depth, dims);

            gl::bind_texture(gl::texture_bind_target::texture_2d_multisample, tex);
            gl::tex_image_2d_multisample(gl::texture_bind_target::texture_2d_multisample,
                                         num_samples,
                                         gl::texture_internal_format::rgba32f,
                                         dims,
                                         /*fixed sample locations*/ true);
            gl::set_texture_max_level(gl::texture_bind_target::texture_2d_multisample, 0U);

            gl::framebuffer_renderbuffer(
                gl::framebuffer_bind_target::any, gl::framebuffer_attachment::depth, rbo);
            gl::framebuffer_texture(
                gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color0, tex, 0);

            gl::draw_buffers(gl::draw_buffer::color0);

            auto fbs = gl::check_framebuffer_status(gl::framebuffer_bind_target::any);
            if(gl::check_framebuffer_status(gl::framebuffer_bind_target::any) !=
               gl::framebuffer_status::complete) {
                gl::log_errors();
                LOG_ERROR("Failed to create render framebuffer: ", static_cast<int>(fbs));
            }

            gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
        }
    };

    class post_buffer {
    public:
        gl::framebuffer fbo;
        gl::texture tex;
        gl::renderbuffer rbo;
        box<2, int> viewport;

        explicit post_buffer(size<2, int> dims)
            : viewport(make_point(0, 0), dims)
        {
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

            gl::bind_renderbuffer(rbo);
            gl::renderbuffer_storage(gl::renderbuffer_format::depth, dims);

            gl::bind_texture(gl::texture_bind_target::texture_2d, tex);
            gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                             /*level*/ 0,
                             gl::texture_internal_format::rgba32f,
                             dims,
                             gl::texture_pixel_format::rgba,
                             gl::texture_pixel_type::float32,
                             span<char const>(nullptr, 0U));
            gl::set_texture_max_level(gl::texture_bind_target::texture_2d, 0U);
            gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::linear);
            gl::set_texture_min_filter(gl::texture_bind_target::texture_2d, gl::min_filter::linear);
            gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                                      gl::texture_direction::s,
                                      gl::texture_wrap_mode::clamp_to_edge);
            gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                                      gl::texture_direction::t,
                                      gl::texture_wrap_mode::clamp_to_edge);

            gl::framebuffer_renderbuffer(
                gl::framebuffer_bind_target::any, gl::framebuffer_attachment::depth, rbo);
            gl::framebuffer_texture(
                gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color0, tex, 0);

            gl::draw_buffers(gl::draw_buffer::color0);

            auto fbs = gl::check_framebuffer_status(gl::framebuffer_bind_target::any);
            if(gl::check_framebuffer_status(gl::framebuffer_bind_target::any) !=
               gl::framebuffer_status::complete) {
                gl::log_errors();
                LOG_ERROR("Failed to create render framebuffer: ", static_cast<int>(fbs));
            }

            gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
        }
    };

    class hdr_stack_em {
    public:
        post_buffer a;
        post_buffer b;
        int num_passes;
        float weight;

        hdr_stack_em(size<2, int> dims, int num_passes, float weight)
            : a(dims)
            , b(dims)
            , num_passes(num_passes)
            , weight(weight)
        {
        }
    };

    class hdr_stack {
    public:
        std::vector<hdr_stack_em> elements;

        hdr_stack()
        {
            elements.emplace_back(make_size(1024, 1024), /*passes*/ 2, /*weight*/ 1.0f);
            elements.emplace_back(make_size(512, 512), /*passes*/ 4, /*weight*/ 0.5f);
            elements.emplace_back(make_size(256, 256), /*passes*/ 8, /*weight*/ 0.25f);
            elements.emplace_back(make_size(128, 128), /*passes*/ 8, /*weight*/ 0.125f);
        }
    };

    struct opengl_state {
        gl::program menu_program;
        gl::program game_program;

        gl::program post_gauss7;
        gl::program post_low_pass;
        gl::program post_to_srgb;

        post_model postmdl;

        gl::texture menu_texture;
        std::vector<color_rgba8> menu_texture_data;

        render_buffer screen_renderbuffer;
        post_buffer screen_postbuffer1;
        post_buffer screen_postbuffer2;

        hdr_stack bloom_layers;

        opengl_state()
            : screen_renderbuffer(make_size(1920, 1440))
            , screen_postbuffer1(make_size(1920, 1440))
            , screen_postbuffer2(make_size(1920, 1440))
        {
            LOG_DEBUG("Loading OpenGL assets");

            link_program_from_files(
                "menu", &menu_program, "jkgm/shaders/menu.vert", "jkgm/shaders/menu.frag");
            link_program_from_files(
                "game", &game_program, "jkgm/shaders/game.vert", "jkgm/shaders/game.frag");
            link_program_from_files("post_gauss7",
                                    &post_gauss7,
                                    "jkgm/shaders/postprocess.vert",
                                    "jkgm/shaders/post_gauss7.frag");
            link_program_from_files("post_low_pass",
                                    &post_low_pass,
                                    "jkgm/shaders/postprocess.vert",
                                    "jkgm/shaders/post_low_pass.frag");
            link_program_from_files("post_to_srgb",
                                    &post_to_srgb,
                                    "jkgm/shaders/postprocess.vert",
                                    "jkgm/shaders/post_to_srgb.frag");

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
        DirectDraw_impl ddraw1;
        DirectDraw2_impl ddraw2;
        Direct3D_impl d3d1;
        Direct3DDevice_impl d3ddevice1;
        Direct3DViewport_impl d3dviewport1;

        DirectDraw_primary_surface_impl ddraw1_primary_surface;
        DirectDraw_backbuffer_surface_impl ddraw1_backbuffer_surface;

        std::vector<std::unique_ptr<DirectDrawPalette_impl>> ddraw1_palettes;
        std::vector<std::unique_ptr<DirectDraw_phony_surface_impl>> phony_surfaces;
        std::vector<std::unique_ptr<DirectDraw_sysmem_texture_surface_impl>>
            sysmem_texture_surfaces;
        std::vector<std::unique_ptr<DirectDraw_vidmem_texture_surface_impl>>
            vidmem_texture_surfaces;
        std::vector<std::unique_ptr<Direct3DExecuteBuffer_impl>> execute_buffers;

        HINSTANCE dll_instance;
        HWND hWnd;
        HDC hDC;
        HGLRC hGLRC;

        std::unique_ptr<opengl_state> ogs;

        char const *indexed_bitmap_source = nullptr;
        std::vector<color_rgba8> indexed_bitmap_colors;

    public:
        explicit renderer_impl(HINSTANCE dll_instance)
            : ddraw1(this)
            , ddraw2(this)
            , d3d1(this)
            , d3ddevice1(this)
            , d3dviewport1(this)
            , ddraw1_primary_surface(this)
            , ddraw1_backbuffer_surface(this)
            , dll_instance(dll_instance)
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
                                1440,
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

            gl::set_clear_color(solid(colors::black));
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            SwapBuffers(hDC);

            ogs = std::make_unique<opengl_state>();
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
            auto current_wnd_sz = make_size(1920, 1440);

            // Copy into post buffer
            gl::bind_framebuffer(gl::framebuffer_bind_target::read, ogs->screen_renderbuffer.fbo);
            gl::bind_framebuffer(gl::framebuffer_bind_target::draw, ogs->screen_postbuffer1.fbo);
            gl::blit_framebuffer(ogs->screen_renderbuffer.viewport,
                                 ogs->screen_postbuffer1.viewport,
                                 {gl::framebuffer_blit_buffer::color},
                                 gl::framebuffer_blit_filter::nearest);

            // Render low pass for bloom
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, ogs->screen_postbuffer2.fbo);
            gl::set_viewport(make_box(make_point(0, 0), current_wnd_sz));

            gl::set_clear_color(solid(colors::black));
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            gl::use_program(ogs->post_low_pass);

            gl::set_uniform_integer(gl::uniform_location_id(0), 0);

            gl::set_active_texture_unit(0);
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->screen_postbuffer1.tex);

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
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->screen_postbuffer1.tex);

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

                indexed_bitmap_colors[curr++] = to_discrete_color(srgb_to_linear(to_float_color(
                    color_rgba8(em.rgbRed, em.rgbGreen, em.rgbBlue, uint8_t(255U)))));
            }
        }

        void present_menu() override
        {
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

            gl::bind_vertex_array(ogs->postmdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

            end_frame();
        }

        void execute_game(IDirect3DExecuteBuffer *cmdbuf, IDirect3DViewport *vp) override
        {
            gl::enable(gl::capability::depth_test);
            gl::enable(gl::capability::blend);
            gl::set_blend_function(gl::blend_function::one,
                                   gl::blend_function::one_minus_source_alpha);
            gl::set_depth_function(gl::comparison_function::less);
            gl::use_program(ogs->game_program);
            gl::set_uniform_integer(gl::uniform_location_id(0), 0);

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
                            gl::bind_texture(
                                gl::texture_bind_target::texture_2d,
                                vidmem_texture_surfaces.at(payload->dwArg[0])->ogl_texture);
                            break;

                        // Silently ignore some useless commands
                        case D3DRENDERSTATE_ANTIALIAS:
                        case D3DRENDERSTATE_TEXTUREPERSPECTIVE:
                        case D3DRENDERSTATE_FILLMODE:
                        case D3DRENDERSTATE_SHADEMODE:
                        case D3DRENDERSTATE_MONOENABLE:
                        case D3DRENDERSTATE_TEXTUREMAPBLEND:
                        case D3DRENDERSTATE_TEXTUREMAG:
                        case D3DRENDERSTATE_TEXTUREMIN:
                        case D3DRENDERSTATE_SRCBLEND:
                        case D3DRENDERSTATE_DESTBLEND:
                        case D3DRENDERSTATE_CULLMODE:
                        case D3DRENDERSTATE_ZFUNC:
                        case D3DRENDERSTATE_ALPHAFUNC:
                        case D3DRENDERSTATE_DITHERENABLE:
                        case D3DRENDERSTATE_ALPHABLENDENABLE:
                        case D3DRENDERSTATE_FOGENABLE:
                        case D3DRENDERSTATE_SPECULARENABLE:
                        case D3DRENDERSTATE_SUBPIXEL:
                        case D3DRENDERSTATE_SUBPIXELX:
                        case D3DRENDERSTATE_STIPPLEDALPHA:
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

                        // HACK:
                        ::glBegin(GL_TRIANGLES);

                        auto r1 = float(RGBA_GETRED(v1.color)) / 255.0f;
                        auto g1 = float(RGBA_GETGREEN(v1.color)) / 255.0f;
                        auto b1 = float(RGBA_GETBLUE(v1.color)) / 255.0f;
                        auto a1 = float(RGBA_GETALPHA(v1.color)) / 255.0f;

                        ::glColor4f(r1, g1, b1, a1);
                        ::glTexCoord2f(v1.tu, v1.tv);
                        ::glVertex4f(v1.sx, v1.sy, v1.sz, v1.rhw);

                        auto r2 = float(RGBA_GETRED(v2.color)) / 255.0f;
                        auto g2 = float(RGBA_GETGREEN(v2.color)) / 255.0f;
                        auto b2 = float(RGBA_GETBLUE(v2.color)) / 255.0f;
                        auto a2 = float(RGBA_GETALPHA(v2.color)) / 255.0f;

                        ::glColor4f(r2, g2, b2, a2);
                        ::glTexCoord2f(v2.tu, v2.tv);
                        ::glVertex4f(v2.sx, v2.sy, v2.sz, v2.rhw);

                        auto r3 = float(RGBA_GETRED(v3.color)) / 255.0f;
                        auto g3 = float(RGBA_GETGREEN(v3.color)) / 255.0f;
                        auto b3 = float(RGBA_GETBLUE(v3.color)) / 255.0f;
                        auto a3 = float(RGBA_GETALPHA(v3.color)) / 255.0f;

                        ::glColor4f(r3, g3, b3, a3);
                        ::glTexCoord2f(v3.tu, v3.tv);
                        ::glVertex4f(v3.sx, v3.sy, v3.sz, v3.rhw);
                        ::glEnd();
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
            end_frame();
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
            return &ddraw1_primary_surface;
        }

        IDirectDrawSurface *get_directdraw_backbuffer_surface() override
        {
            return &ddraw1_backbuffer_surface;
        }

        IDirectDrawSurface *get_directdraw_phony_surface(DDSURFACEDESC desc,
                                                         std::string name) override
        {
            phony_surfaces.push_back(
                std::make_unique<DirectDraw_phony_surface_impl>(this, desc, std::move(name)));
            return phony_surfaces.back().get();
        }

        IDirectDrawSurface *get_directdraw_sysmem_texture_surface(DDSURFACEDESC desc) override
        {
            sysmem_texture_surfaces.push_back(
                std::make_unique<DirectDraw_sysmem_texture_surface_impl>(this, desc));
            return sysmem_texture_surfaces.back().get();
        }

        IDirectDrawSurface *get_directdraw_vidmem_texture_surface(DDSURFACEDESC desc) override
        {
            vidmem_texture_surfaces.push_back(
                std::make_unique<DirectDraw_vidmem_texture_surface_impl>(
                    this, desc, vidmem_texture_surfaces.size()));
            return vidmem_texture_surfaces.back().get();
        }

        IDirectDrawPalette *get_directdraw_palette(span<PALETTEENTRY const> entries) override
        {
            ddraw1_palettes.push_back(std::make_unique<DirectDrawPalette_impl>(this));

            auto *rv = ddraw1_palettes.back().get();
            std::copy(entries.begin(), entries.end(), rv->entries.begin());

            return rv;
        }

        IDirect3DExecuteBuffer *get_direct3dexecutebuffer(size_t bufsz) override
        {
            execute_buffers.push_back(std::make_unique<Direct3DExecuteBuffer_impl>(this, bufsz));
            return execute_buffers.back().get();
        }
    };
}

std::unique_ptr<jkgm::renderer> jkgm::create_renderer(HINSTANCE dll_instance)
{
    return std::make_unique<jkgm::renderer_impl>(dll_instance);
}