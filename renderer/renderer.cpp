#include "renderer.hpp"
#include "backbuffer_menu_surface.hpp"
#include "backbuffer_surface.hpp"
#include "base/file_block.hpp"
#include "base/file_stream.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"
#include "base/win32.hpp"
#include "common/error_reporter.hpp"
#include "common/image.hpp"
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
#include "renderer_ao.hpp"
#include "renderer_compositor.hpp"
#include "renderer_screen.hpp"
#include "sysmem_texture.hpp"
#include "texture_cache.hpp"
#include "triangle_batch.hpp"
#include "vidmem_texture.hpp"
#include "zbuffer_surface.hpp"
#include <Windows.h>
#include <chrono>

namespace jkgm {
    static WNDPROC original_wkernel_wndproc = nullptr;
    static size<2, int> original_configured_screen_res = make_size(0, 0);
    static box<2, int> actual_menu_area = make_box(make_point(0, 0), make_size(0, 0));

    using wglCreateContextAttribsARB_type = HGLRC(WINAPI *)(HDC hDC,
                                                            HGLRC hShareContext,
                                                            int const *attribList);
    static wglCreateContextAttribsARB_type wglCreateContextAttribsARB = nullptr;

    using wglChoosePixelFormatARB_type = BOOL(WINAPI *)(HDC hDC,
                                                        const int *piAttribIList,
                                                        const FLOAT *pfAttribFList,
                                                        UINT nMaxFormats,
                                                        int *piFormats,
                                                        UINT *nNumFormats);
    static wglChoosePixelFormatARB_type wglChoosePixelFormatARB = nullptr;

    using wglSwapIntervalEXT_type = BOOL(WINAPI *)(int interval);
    static wglSwapIntervalEXT_type wglSwapIntervalEXT = nullptr;

    // See https://www.opengl.org/registry/specs/ARB/wgl_create_context.txt for all values
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

// See https://www.opengl.org/registry/specs/ARB/wgl_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023

#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_TYPE_RGBA_ARB 0x202B

    void init_wgl_extensions(HINSTANCE hInstance)
    {
        WNDCLASS dummy_class;
        ZeroMemory(&dummy_class, sizeof(WNDCLASS));
        dummy_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        dummy_class.lpfnWndProc = DefWindowProc;
        dummy_class.lpszClassName = L"kernel_wgl_ext_loader";

        if(!RegisterClass(&dummy_class)) {
            report_fatal_message(
                str(fmt("JkGfxMod could not initialize OpenGL.\n\nDetails: Failed to register "
                        "WGL extension loader window class (",
                        win32::win32_category().message(GetLastError()),
                        ")")));
        }

        HWND dummy_window = CreateWindowEx(0,
                                           L"kernel_wgl_ext_loader",
                                           L"KernelWglExtLoader",
                                           0,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           NULL,
                                           NULL,
                                           hInstance,
                                           NULL);

        if(!dummy_window) {
            report_fatal_message(str(fmt("JkGfxMod could not initialize OpenGL.\n\nDetails: "
                                         "Failed to create WGL extension loader window (",
                                         win32::win32_category().message(GetLastError()),
                                         ")")));
        }

        HDC hDC = GetDC(dummy_window);

        PIXELFORMATDESCRIPTOR pfd;
        ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        int pixel_format = ChoosePixelFormat(hDC, &pfd);
        SetPixelFormat(hDC, pixel_format, &pfd);

        HGLRC dummy_context = wglCreateContext(hDC);
        if(dummy_context == NULL) {
            report_fatal_message(str(fmt("JkGfxMod could not initialize OpenGL.\n\nDetails: "
                                         "Failed to create WGL extension loader context (",
                                         win32::win32_category().message(GetLastError()),
                                         ")")));
        }

        wglMakeCurrent(hDC, dummy_context);

        wglCreateContextAttribsARB =
            (wglCreateContextAttribsARB_type)wglGetProcAddress("wglCreateContextAttribsARB");
        if(wglCreateContextAttribsARB == NULL) {
            report_fatal_message(
                str(fmt("JkGfxMod could not initialize OpenGL.\n\nDetails: Failed to acquire "
                        "wglCreateContextAttribsARB extension (",
                        win32::win32_category().message(GetLastError()),
                        ")")));
        }

        wglChoosePixelFormatARB =
            (wglChoosePixelFormatARB_type)wglGetProcAddress("wglChoosePixelFormatARB");
        if(wglChoosePixelFormatARB == NULL) {
            report_fatal_message(str(fmt("JkGfxMod could not initialize OpenGL.\n\nDetails: "
                                         "Failed to acquire wglChoosePixelFormatARB extension (",
                                         win32::win32_category().message(GetLastError()),
                                         ")")));
        }

        wglSwapIntervalEXT = (wglSwapIntervalEXT_type)wglGetProcAddress("wglSwapIntervalEXT");
        if(wglSwapIntervalEXT == NULL) {
            report_fatal_message(str(fmt("JkGfxMod could not initialize OpenGL.\n\nDetails: "
                                         "Failed to acquire wglSwapIntervalEXT extension (",
                                         win32::win32_category().message(GetLastError()),
                                         ")")));
        }

        wglMakeCurrent(hDC, 0);
        wglDeleteContext(dummy_context);
        ReleaseDC(dummy_window, hDC);
        DestroyWindow(dummy_window);
    }

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

            auto d = actual_menu_area.size();
            auto wscale = 640.0f / static_cast<float>(get<x>(d));
            auto hscale = 480.0f / static_cast<float>(get<y>(d));

            xPos = (int16_t)((float)(xPos - get<x>(actual_menu_area.start)) * wscale);
            yPos = (int16_t)((float)(yPos - get<y>(actual_menu_area.start)) * hscale);

            lParam = (((LPARAM)yPos << 16) | (LPARAM)xPos);

            // Pass this message back to the original wndproc
            break;
        }
        }

        return CallWindowProc(original_wkernel_wndproc, hWnd, uMsg, wParam, lParam);
    }

    static size<2, int> make_internal_scr_res(config const *the_config)
    {
        if(the_config->correct_game_aspect_ratio) {
            // The game will be rendered in a best-fit virtual screen with a 4:3 aspect ratio
            constexpr float aspect = 4.0f / 3.0f;

            int w_if_h_fit = (int)((float)std::get<1>(the_config->resolution) * aspect);
            if(w_if_h_fit > std::get<0>(the_config->resolution)) {
                // Virtual screen is too tall. Fit to width instead.
                int h_if_w_fit = (int)((float)std::get<0>(the_config->resolution) / aspect);
                return make_size(std::get<0>(the_config->resolution), h_if_w_fit);
            }

            return make_size(w_if_h_fit, std::get<1>(the_config->resolution));
        }

        return make_size(std::get<0>(the_config->resolution), std::get<1>(the_config->resolution));
    }

    static size<2, float> make_internal_scr_res_scale_f(size<2, int> conf_scr_res)
    {
        return make_size(2.0f / (float)get<x>(conf_scr_res), 2.0f / (float)get<y>(conf_scr_res));
    }

    static box<2, int> make_internal_scr_area(size<2, int> conf_scr_res,
                                              size<2, int> internal_scr_res)
    {
        // Adjust actual display area to fit the physical display area
        float aspect = (float)get<x>(internal_scr_res) / (float)get<y>(internal_scr_res);

        int width_if_height_fit = (int)(aspect * (float)get<y>(conf_scr_res));
        if(width_if_height_fit <= get<x>(conf_scr_res)) {
            // Virtual screen is taller or as tall as the physical screen
            int leftpad = (get<x>(conf_scr_res) - width_if_height_fit) / 2;
            return make_box(make_point(leftpad, 0),
                            make_size(width_if_height_fit, get<y>(conf_scr_res)));
        }

        // Virtual screen is wider or as wide as the physical screen
        int height_if_width_fit = (int)((float)get<x>(conf_scr_res) / aspect);
        int toppad = (get<y>(conf_scr_res) - height_if_width_fit) / 2;
        return make_box(make_point(0, toppad),
                        make_size(get<x>(conf_scr_res), height_if_width_fit));
    }

    static box<2, int> make_internal_menu_area(size<2, int> conf_scr_res,
                                               size<2, int> internal_scr_res,
                                               bool correct_aspect_ratio)
    {
        if(!correct_aspect_ratio) {
            return make_internal_scr_area(conf_scr_res, internal_scr_res);
        }

        int menu_w = 0;
        int menu_h = 0;

        constexpr float aspect = 4.0f / 3.0f;
        int w_if_h_fit = (int)((float)get<y>(conf_scr_res) * aspect);
        if(w_if_h_fit > get<x>(conf_scr_res)) {
            // Menu is too tall. Fit to width instead.
            int h_if_w_fit = (int)((float)get<x>(conf_scr_res) / aspect);
            menu_w = get<x>(conf_scr_res);
            menu_h = h_if_w_fit;
        }
        else {
            menu_w = w_if_h_fit;
            menu_h = get<y>(conf_scr_res);
        }

        // Center the menu virtual screen in the physical screen
        int menu_off_x = (get<x>(conf_scr_res) - menu_w) / 2;
        int menu_off_y = (get<y>(conf_scr_res) - menu_h) / 2;

        return make_box(make_point(menu_off_x, menu_off_y), make_size(menu_w, menu_h));
    }

    static direction<2, float> make_internal_scr_offset_f(size<2, int> conf_scr_res,
                                                          box<2, int> actual_display_area)
    {
        float xoff =
            ((float)get<x>(actual_display_area.start) / (float)get<x>(conf_scr_res)) * 2.0f;
        float yoff =
            ((float)get<y>(actual_display_area.start) / (float)get<y>(conf_scr_res)) * 2.0f;

        return make_direction(xoff, yoff);
    }

    class renderer_impl : public renderer {
    private:
        config const *the_config;

        renderer_mode mode = renderer_mode::menu;
        size<2, int> conf_scr_res;

        size<2, int> internal_scr_res;
        size<2, float> internal_scr_res_f;
        size<2, float> internal_scr_res_scale_f;
        box<2, int> actual_display_area;
        box<2, int> actual_menu_area;
        direction<2, float> internal_scr_offset_f;

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
        offscreen_menu_surface ddraw1_offscreen_credit_surface;

        DirectDrawPalette_impl ddraw1_palette;

        std::vector<std::unique_ptr<sysmem_texture_surface>> sysmem_texture_surfaces;
        std::vector<std::unique_ptr<vidmem_texture_surface>> vidmem_texture_surfaces;
        std::vector<std::unique_ptr<execute_buffer>> execute_buffers;

        HINSTANCE dll_instance = NULL;
        HWND hWnd = NULL;
        HDC hDC = NULL;
        HGLRC hGLRC = NULL;

        std::unique_ptr<opengl_state> ogs;
        std::unique_ptr<renderer_compositor> frame_compositor;
        std::unique_ptr<renderer_ao> frame_ao;
        std::unique_ptr<renderer_screen> frame_screen;

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
        triangle_batch overlay_batch;

        bool is_gun = false;
        bool is_transparent = false;
        triangle_batch *current_triangle_batch = &world_batch;

        material_instance_id current_material = material_instance_id(0U);

    public:
        explicit renderer_impl(HINSTANCE dll_instance, config const *the_config)
            : the_config(the_config)
            , conf_scr_res(std::get<0>(the_config->resolution), std::get<1>(the_config->resolution))
            , internal_scr_res(make_internal_scr_res(the_config))
            , internal_scr_res_f(static_cast<size<2, float>>(internal_scr_res))
            , internal_scr_res_scale_f(make_internal_scr_res_scale_f(conf_scr_res))
            , actual_display_area(make_internal_scr_area(conf_scr_res, internal_scr_res))
            , actual_menu_area(make_internal_menu_area(conf_scr_res,
                                                       internal_scr_res,
                                                       the_config->correct_menu_aspect_ratio))
            , internal_scr_offset_f(make_internal_scr_offset_f(conf_scr_res, actual_display_area))
            , ddraw1(this)
            , ddraw2(this)
            , d3d1(this)
            , d3ddevice1(this)
            , d3dviewport1(this)
            , ddraw1_primary_menu_surface(this)
            , ddraw1_backbuffer_menu_surface(this, &ddraw1_primary_menu_surface)
            , ddraw1_primary_surface(this)
            , ddraw1_backbuffer_surface(this, internal_scr_res)
            , ddraw1_zbuffer_surface(this)
            , ddraw1_palette(this)
            , dll_instance(dll_instance)
        {
            indexed_bitmap_colors.resize(256, color_rgba8::zero());

            menu_prev_ticks = std::chrono::high_resolution_clock::now();
            menu_curr_ticks = menu_prev_ticks;
        }

        void set_renderer_mode(renderer_mode mode) override
        {
            this->mode = mode;
        }

        size<2, int> get_internal_screen_resolution() override
        {
            return internal_scr_res;
        }

        bool is_parallax_enabled() override
        {
            return the_config->enable_parallax;
        }

        point<2, int> get_cursor_pos(point<2, int> real_pos) override
        {
            if(mode == renderer_mode::menu) {
                auto d = actual_menu_area.size();
                auto wscale = 640.0f / static_cast<float>(get<x>(d));
                auto hscale = 480.0f / static_cast<float>(get<y>(d));

                // Stretch the point into 640x480
                return make_point(
                    (int)((float)(get<x>(real_pos) - get<x>(actual_menu_area.start)) * wscale),
                    (int)((float)(get<y>(real_pos) - get<y>(actual_menu_area.start)) * hscale));
            }

            return real_pos;
        }

        void initialize(HINSTANCE hInstance, HWND parentWnd) override
        {
            init_wgl_extensions(hInstance);

            hWnd = parentWnd;

            original_wkernel_wndproc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
            SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)&renderer_wndproc);

            original_configured_screen_res = conf_scr_res;
            jkgm::actual_menu_area = actual_menu_area;

            if(the_config->fullscreen) {
                DEVMODE dm;
                ZeroMemory(&dm, sizeof(dm));
                dm.dmSize = sizeof(dm);
                dm.dmPelsWidth = get<x>(conf_scr_res);
                dm.dmPelsHeight = get<y>(conf_scr_res);
                dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

                auto rv = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
                if(rv != DISP_CHANGE_SUCCESSFUL) {
                    report_fatal_message(str(fmt(
                        "JkGfxMod could not change the display to the configured resolution. This "
                        "may have happened because your graphics device does not support the "
                        "configured resolution, or due to an implementation bug in "
                        "JkGfxMod.\n\nDetails: ChangeDisplaySettings returned ",
                        rv)));
                }

                MoveWindow(
                    hWnd, 0, 0, get<x>(conf_scr_res), get<y>(conf_scr_res), /*repaint*/ TRUE);
            }

            hDC = GetDC(hWnd);

            std::vector<int> pfd_attribs{WGL_DRAW_TO_WINDOW_ARB,
                                         GL_TRUE,
                                         WGL_SUPPORT_OPENGL_ARB,
                                         GL_TRUE,
                                         WGL_DOUBLE_BUFFER_ARB,
                                         GL_TRUE,
                                         WGL_ACCELERATION_ARB,
                                         WGL_FULL_ACCELERATION_ARB,
                                         WGL_PIXEL_TYPE_ARB,
                                         WGL_TYPE_RGBA_ARB,
                                         WGL_COLOR_BITS_ARB,
                                         32,
                                         WGL_DEPTH_BITS_ARB,
                                         24,
                                         WGL_STENCIL_BITS_ARB,
                                         8,
                                         0};

            int pfdid;
            UINT num_formats;
            bool cpf_res = wglChoosePixelFormatARB(
                hDC, pfd_attribs.data(), nullptr, /*max formats*/ 1, &pfdid, &num_formats);
            if(!cpf_res) {
                report_fatal_message(
                    str(fmt("JkGfxMod could not initialize OpenGL.\n\nDetails: "
                            "Call to wglChoosePixelFormatARB failed unexpectedly (returned ",
                            GetLastError(),
                            ")")));
            }

            if(num_formats == 0) {
                report_fatal_message("JkGfxMod could not initialize OpenGL.\n\nDetails: Failed to "
                                     "find suitable pixel format.");
            }

            PIXELFORMATDESCRIPTOR pfd;
            DescribePixelFormat(hDC, pfdid, sizeof(pfd), &pfd);
            SetPixelFormat(hDC, pfdid, &pfd);

            std::vector<int> gl_attribs{WGL_CONTEXT_MAJOR_VERSION_ARB,
                                        3,
                                        WGL_CONTEXT_MINOR_VERSION_ARB,
                                        3,
                                        WGL_CONTEXT_PROFILE_MASK_ARB,
                                        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                                        0};
            hGLRC = wglCreateContextAttribsARB(hDC, NULL, gl_attribs.data());

            if(hGLRC == NULL) {
                report_fatal_message(str(fmt("JkGfxMod could not initialize OpenGL.\n\nDetails: "
                                             "Failed to create OpenGL context (returned ",
                                             GetLastError(),
                                             ")")));
            }

            wglMakeCurrent(hDC, hGLRC);

            int present_interval = the_config->enable_vsync ? 1 : 0;
            auto vsync_res = wglSwapIntervalEXT(present_interval);
            if(!vsync_res) {
                report_fatal_message(
                    str(fmt("JkGfxMod could not initialize OpenGL.\n\nDetails: Call to "
                            "wglSwapIntervalEXT failed unexpectedly (returned ",
                            GetLastError(),
                            ")")));
            }

            if(!gladLoadGL()) {
                report_fatal_message("JkGfxMod could not initialize OpenGL.\n\nDetails: Failed to "
                                     "load extensions (gladLoadGL)");
            }

            ShowWindow(hWnd, SW_SHOW);

            gl::enable(gl::capability::framebuffer_srgb);

            gl::set_clear_color(solid(colors::black));
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            SwapBuffers(hDC);

            ogs = std::make_unique<opengl_state>(
                conf_scr_res, internal_scr_res, actual_display_area, actual_menu_area, the_config);
            texture_cache = create_texture_cache(the_config);
            frame_compositor = create_renderer_compositor(the_config);
            frame_ao = create_renderer_ao(the_config, conf_scr_res);
            frame_screen = create_renderer_screen(the_config, conf_scr_res);
            begin_frame();
        }

        void begin_frame()
        {
            gl::set_active_texture_unit(0);
            frame_screen->begin_frame();
        }

        void end_frame()
        {
            // Compose renderbuffer onto window:
            frame_screen->end_frame();
            frame_compositor->end_frame(
                ogs.get(), conf_scr_res, frame_screen->get_resolved_final_texture());

            SwapBuffers(hDC);

            begin_frame();
        }

        void show_directplay_dialog() override
        {
            ShowWindow(hWnd, SW_HIDE);
        }

        void restore_from_directplay_dialog() override
        {
            ShowWindow(hWnd, SW_SHOW);
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

            ogs->menu_program.use_program();
            ogs->menu_program.set_menu_sampler(0);

            gl::bind_vertex_array(ogs->menumdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->menumdl.num_indices, gl::index_type::uint32);

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

            ogs->menu_program.use_program();
            ogs->menu_program.set_menu_sampler(0);

            gl::bind_vertex_array(ogs->menumdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->menumdl.num_indices, gl::index_type::uint32);

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

        static constexpr int hud_texture_num_slabs = 4;

        int curr_slab = 0;
        void update_hud_texture()
        {
            if(curr_slab == 0) {
                // Convert HUD bitmap into valid texture data once every 4 frames
                ZeroMemory(ogs->hud_texture_data.data(), ogs->hud_texture_data.size());

                for(size_t i = 0; i < ddraw1_backbuffer_surface.buffer.size(); ++i) {
                    auto const &in_em = ddraw1_backbuffer_surface.buffer[i];

                    // Convert from RGB565 to RGBA8888
                    ogs->hud_texture_data[i] = rgb565_key_to_srgb_a8(
                        in_em, /*transparent?*/ in_em == ddraw1_backbuffer_surface.color_key);
                }
            }

            // Blit 1/nslabs of the HUD texture data into GPU texture
            gl::set_active_texture_unit(0);
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->hud_texture_back);

            int slab_rows = get<y>(internal_scr_res) / hud_texture_num_slabs;
            int start_row = slab_rows * curr_slab;

            int stop_row = slab_rows * (curr_slab + 1);
            if(curr_slab >= (hud_texture_num_slabs - 1)) {
                stop_row = get<y>(internal_scr_res);
            }

            auto region =
                make_box(make_point(0, start_row), make_point(get<x>(internal_scr_res), stop_row));
            auto offset = start_row * get<x>(internal_scr_res);
            auto num_pixels = volume(region);

            gl::tex_sub_image_2d(
                gl::texture_bind_target::texture_2d,
                /*level*/ 0,
                region,
                gl::texture_pixel_format::rgba,
                gl::texture_pixel_type::uint8,
                make_span(ogs->hud_texture_data).subspan(offset, num_pixels).as_const_bytes());

            ++curr_slab;
            if(curr_slab >= hud_texture_num_slabs) {
                // Swap and present after all 4 slabs have been copied
                std::fill(ddraw1_backbuffer_surface.buffer.begin(),
                          ddraw1_backbuffer_surface.buffer.end(),
                          ddraw1_backbuffer_surface.color_key);

                std::swap(ogs->hud_texture_front, ogs->hud_texture_back);
                curr_slab = 0;
            }
        }

        void draw_hud()
        {
            gl::enable(gl::capability::blend);
            gl::disable(gl::capability::depth_test);

            gl::set_active_texture_unit(0);
            gl::bind_texture(gl::texture_bind_target::texture_2d, ogs->hud_texture_front);

            // Render
            ogs->menu_program.use_program();
            ogs->menu_program.set_menu_sampler(0);

            gl::bind_vertex_array(ogs->hudmdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->hudmdl.num_indices, gl::index_type::uint32);
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

        void bind_material(shaders::game_pass_shader *prog,
                           material_instance_id id,
                           bool force_opaque,
                           bool posterize_lighting)
        {
            if(id.get() == 0U) {
                // This is the default (untextured) material
                gl::set_active_texture_unit(2);
                gl::bind_texture(gl::texture_bind_target::texture_2d, gl::default_texture);
                gl::set_active_texture_unit(1);
                gl::bind_texture(gl::texture_bind_target::texture_2d, gl::default_texture);
                gl::set_active_texture_unit(0);
                gl::bind_texture(gl::texture_bind_target::texture_2d, gl::default_texture);

                // Enable features
                prog->set_features(make_point(/*has albedo map*/ 0.0f,
                                              /*has emissive map*/ 0.0f,
                                              /*alpha mode*/ 0.0f,
                                              (posterize_lighting ? 1.0f : 0.0f)));

                // Albedo factor
                prog->set_albedo_factor(color::fill(1.0f));

                // Emissive factor
                prog->set_emissive_factor(color_rgb::zero());

                // Displacement factor
                prog->set_displacement_factor(0.0f);

                // Alpha cutoff
                prog->set_alpha_cutoff(0.0f);
            }
            else {
                auto const &mat = vidmem_texture_surfaces.at(id.get() - 1);

                gl::texture_view albedo_map = gl::default_texture;
                if(mat->albedo_map.has_value()) {
                    albedo_map = texture_cache->get_texture_handle(*mat->albedo_map);
                }

                gl::texture_view emissive_map = gl::default_texture;
                if(mat->emissive_map.has_value()) {
                    emissive_map = texture_cache->get_texture_handle(*mat->emissive_map);
                }

                gl::texture_view displacement_map = gl::default_texture;
                if(mat->displacement_map.has_value()) {
                    displacement_map = texture_cache->get_texture_handle(*mat->displacement_map);
                }

                gl::set_active_texture_unit(2);
                gl::bind_texture(gl::texture_bind_target::texture_2d, displacement_map);
                gl::set_active_texture_unit(1);
                gl::bind_texture(gl::texture_bind_target::texture_2d, emissive_map);
                gl::set_active_texture_unit(0);
                gl::bind_texture(gl::texture_bind_target::texture_2d, albedo_map);

                // Enable features
                prog->set_features(make_point(
                    mat->albedo_map.has_value() ? 1.0f : 0.0f,
                    mat->emissive_map.has_value() ? 1.0f : 0.0f,
                    ((mat->alpha_mode == material_alpha_mode::mask) && (!force_opaque)) ? 1.0f
                                                                                        : 0.0f,
                    (posterize_lighting ? 1.0f : 0.0f)));

                prog->set_albedo_factor(mat->albedo_factor);
                prog->set_emissive_factor(mat->emissive_factor);
                prog->set_displacement_factor(mat->displacement_factor);
                prog->set_alpha_cutoff(mat->alpha_cutoff);
            }

            current_material = id;
        }

        void draw_batch(shaders::game_pass_shader *prog,
                        triangle_batch const &tb,
                        triangle_buffer_model *trimdl,
                        bool force_opaque,
                        bool posterize_lighting)
        {
            gl::bind_vertex_array(trimdl->vao);

            size_t curr_offset = 0U;
            size_t num_verts = 0U;

            bind_material(prog, material_instance_id(0U), force_opaque, posterize_lighting);

            for(auto const &tri : tb) {
                if(current_material != tri.material) {
                    // Draw pending elements from previous material
                    if(num_verts > 0) {
                        gl::draw_arrays(gl::element_type::triangles, curr_offset, num_verts);

                        curr_offset += num_verts;
                        num_verts = 0U;
                    }

                    bind_material(prog, tri.material, force_opaque, posterize_lighting);
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

            auto *vx = mdl->mmio.data();
            for(auto const &tri : tb) {
                vx->pos = tri.v0.pos;
                vx->texcoords = tri.v0.texcoords;
                vx->col = tri.v0.color;
                vx->normal = tri.normal;

                ++vx;

                vx->pos = tri.v1.pos;
                vx->texcoords = tri.v1.texcoords;
                vx->col = tri.v1.color;
                vx->normal = tri.normal;

                ++vx;

                vx->pos = tri.v2.pos;
                vx->texcoords = tri.v2.texcoords;
                vx->col = tri.v2.color;
                vx->normal = tri.normal;

                ++vx;
            }

            mdl->num_vertices = tb.size() * 3;
            mdl->update_buffers();
        }

        void draw_game_opaque_into_gbuffer(triangle_buffer_models *trimdl, bool posterize_lighting)
        {
            // Draw batches
            gl::disable(gl::capability::blend);
            gl::enable(gl::capability::depth_test);
            gl::set_depth_mask(true);
            gl::disable(gl::capability::cull_face);
            gl::set_face_cull_mode(gl::face_mode::front_and_back);
            gl::set_blend_function(gl::blend_function::one,
                                   gl::blend_function::one_minus_source_alpha);
            gl::set_depth_function(gl::comparison_function::less);

            ogs->game_opaque_pass_program.use_program();
            ogs->game_opaque_pass_program.set_screen_resolution(
                static_cast<size<2, float>>(conf_scr_res));
            ogs->game_opaque_pass_program.set_albedo_map_sampler(0);
            ogs->game_opaque_pass_program.set_emissive_map_sampler(1);
            ogs->game_opaque_pass_program.set_displacement_map_sampler(2);

            // Draw first pass (opaque world geometry)
            draw_batch(&ogs->game_opaque_pass_program,
                       world_batch,
                       &trimdl->world_trimdl,
                       /*force opaque*/ true,
                       posterize_lighting);

            // Draw second pass (transparent world geometry with alpha testing)
            draw_batch(&ogs->game_opaque_pass_program,
                       world_transparent_batch,
                       &trimdl->world_transparent_trimdl,
                       /*force opaque*/ true,
                       posterize_lighting);

            // Draw fourth pass (opaque gun geometry)
            draw_batch(&ogs->game_opaque_pass_program,
                       gun_batch,
                       &trimdl->gun_trimdl,
                       /*force opaque*/ true,
                       posterize_lighting);

            // Draw fifth pass (transparent gun geometry with alpha testing)
            draw_batch(&ogs->game_opaque_pass_program,
                       gun_transparent_batch,
                       &trimdl->gun_transparent_trimdl,
                       /*force opaque*/ true,
                       posterize_lighting);
        }

        void draw_game_opaque_composite()
        {
            gl::disable(gl::capability::depth_test);

            ogs->game_post_opaque_composite_program.use_program();
            ogs->game_post_opaque_composite_program.set_color_sampler(0);
            ogs->game_post_opaque_composite_program.set_emissive_sampler(1);
            ogs->game_post_opaque_composite_program.set_occlusion_sampler(2);

            gl::set_active_texture_unit(2);
            gl::bind_texture(gl::texture_bind_target::texture_2d, frame_ao->get_ssao_texture());

            gl::set_active_texture_unit(1);
            gl::bind_texture(gl::texture_bind_target::texture_2d,
                             frame_screen->get_resolved_emissive_texture());

            gl::set_active_texture_unit(0);
            gl::bind_texture(gl::texture_bind_target::texture_2d,
                             frame_screen->get_resolved_color_texture());

            gl::bind_vertex_array(ogs->postmdl.vao);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);
        }

        void draw_game_gbuffer_pass(triangle_buffer_models *trimdl, bool posterize_lighting)
        {
            frame_screen->begin_opaque_pass();
            draw_game_opaque_into_gbuffer(trimdl, posterize_lighting);
            frame_screen->end_opaque_pass();

            frame_ao->update_ssao_texture(ogs.get(),
                                          frame_screen->get_resolved_normal_texture(),
                                          frame_screen->get_resolved_depth_texture());

            frame_screen->begin_compose_opaque_pass();
            draw_game_opaque_composite();
            frame_screen->end_compose_opaque_pass();
        }

        void draw_game_transparency_pass(triangle_buffer_models *trimdl, bool posterize_lighting)
        {
            frame_screen->begin_transparency_pass();

            // Draw batches
            gl::disable(gl::capability::blend);
            gl::enable(gl::capability::depth_test);
            gl::set_depth_mask(true);
            gl::disable(gl::capability::cull_face);
            gl::set_face_cull_mode(gl::face_mode::front_and_back);
            gl::set_blend_function(gl::blend_function::one,
                                   gl::blend_function::one_minus_source_alpha);
            gl::set_depth_function(gl::comparison_function::less);

            ogs->game_transparency_pass_program.use_program();
            ogs->game_transparency_pass_program.set_screen_resolution(
                static_cast<size<2, float>>(conf_scr_res));
            ogs->game_transparency_pass_program.set_albedo_map_sampler(0);
            ogs->game_transparency_pass_program.set_emissive_map_sampler(1);
            ogs->game_transparency_pass_program.set_displacement_map_sampler(2);
            gl::set_active_texture_unit(0);

            // Draw third pass (transparent world geometry with alpha blending)
            gl::enable(gl::capability::blend);
            gl::set_depth_mask(false);
            draw_batch(&ogs->game_transparency_pass_program,
                       world_transparent_batch,
                       &trimdl->world_transparent_trimdl,
                       /*force opaque*/ false,
                       posterize_lighting);

            // Redraw gun overlay after z-clear
            gl::set_depth_mask(true);
            gl::clear({gl::clear_flag::depth});

            draw_batch(&ogs->game_transparency_pass_program,
                       gun_batch,
                       &trimdl->gun_trimdl,
                       /*force opaque*/ true,
                       posterize_lighting);
            draw_batch(&ogs->game_transparency_pass_program,
                       gun_transparent_batch,
                       &trimdl->gun_transparent_trimdl,
                       /*force opaque*/ true,
                       posterize_lighting);

            // Draw gun transparency
            draw_batch(&ogs->game_transparency_pass_program,
                       gun_transparent_batch,
                       &trimdl->gun_transparent_trimdl, /*force opaque*/
                       false,
                       posterize_lighting);

            // Draw overlay
            gl::disable(gl::capability::depth_test);
            gl::set_depth_mask(false);
            draw_batch(&ogs->game_transparency_pass_program,
                       overlay_batch,
                       &trimdl->overlay_trimdl,
                       /*force opaque*/ false,
                       posterize_lighting);

            gl::enable(gl::capability::depth_test);
            gl::enable(gl::capability::blend);
            gl::set_depth_mask(true);
        }

        void begin_game() override {}

        void end_game() override {}

        static point<4, float> d3dtl_to_point(size<2, float> const &scr_scale,
                                              direction<2, float> const &screen_offset,
                                              D3DTLVERTEX const &p)
        {
            // Reassign w for full-screen overlay vertices
            float w = 1.0f;
            if(p.rhw != 0.0f) {
                w = 1.0f / p.rhw;
            }

            // Convert pretransformed vertex to phony view space
            return make_point(w * ((p.sx * get<x>(scr_scale)) - 1.0f + get<x>(screen_offset)),
                              w * ((-p.sy * get<y>(scr_scale)) + 1.0f - get<y>(screen_offset)),
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
                            report_unimplemented_function(str(fmt("Process vertices opcode: ",
                                                                  payload->dwFlags,
                                                                  " ",
                                                                  payload->dwCount,
                                                                  " ",
                                                                  payload->wStart,
                                                                  " ",
                                                                  payload->wDest)));
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

                        auto sc1 = to_float_color(color_rgba8((uint8_t)RGBA_GETRED(v1.color),
                                                              (uint8_t)RGBA_GETGREEN(v1.color),
                                                              (uint8_t)RGBA_GETBLUE(v1.color),
                                                              (uint8_t)RGBA_GETALPHA(v1.color)));
                        auto sc2 = to_float_color(color_rgba8((uint8_t)RGBA_GETRED(v2.color),
                                                              (uint8_t)RGBA_GETGREEN(v2.color),
                                                              (uint8_t)RGBA_GETBLUE(v2.color),
                                                              (uint8_t)RGBA_GETALPHA(v2.color)));
                        auto sc3 = to_float_color(color_rgba8((uint8_t)RGBA_GETRED(v3.color),
                                                              (uint8_t)RGBA_GETGREEN(v3.color),
                                                              (uint8_t)RGBA_GETBLUE(v3.color),
                                                              (uint8_t)RGBA_GETALPHA(v3.color)));

                        auto c1 =
                            gamma18_to_linear(extend(get<rgb>(sc1) * get<a>(sc1), get<a>(sc1)));
                        auto c2 =
                            gamma18_to_linear(extend(get<rgb>(sc2) * get<a>(sc2), get<a>(sc2)));
                        auto c3 =
                            gamma18_to_linear(extend(get<rgb>(sc3) * get<a>(sc3), get<a>(sc3)));

                        // Check if the current triangle is part of an overlay
                        if(is_gun &&
                           almost_equal(make_point(v1.sx, v1.sy, v1.sz), point<3, float>::zero()) &&
                           almost_equal(v2.sx, get<x>(internal_scr_res_f)) &&
                           (almost_equal(v2.sy, 0.0f) ||
                            almost_equal(v2.sy, get<y>(internal_scr_res_f))) &&
                           almost_equal(v2.sz, 0.0f) &&
                           (almost_equal(v3.sx, 0.0f) ||
                            almost_equal(v3.sx, get<x>(internal_scr_res_f))) &&
                           almost_equal(v3.sy, get<y>(internal_scr_res_f)) &&
                           almost_equal(v3.sz, 0.0f)) {
                            // Check if the current triangle is part of an overlay
                            overlay_batch.insert(
                                triangle(triangle_vertex(d3dtl_to_point(internal_scr_res_scale_f,
                                                                        internal_scr_offset_f,
                                                                        v1),
                                                         make_point(v1.tu, v1.tv),
                                                         c1),
                                         triangle_vertex(d3dtl_to_point(internal_scr_res_scale_f,
                                                                        internal_scr_offset_f,
                                                                        v2),
                                                         make_point(v2.tu, v2.tv),
                                                         c2),
                                         triangle_vertex(d3dtl_to_point(internal_scr_res_scale_f,
                                                                        internal_scr_offset_f,
                                                                        v3),
                                                         make_point(v3.tu, v3.tv),
                                                         c3),
                                         current_material));
                        }
                        else {
                            current_triangle_batch->insert(
                                triangle(triangle_vertex(d3dtl_to_point(internal_scr_res_scale_f,
                                                                        internal_scr_offset_f,
                                                                        v1),
                                                         make_point(v1.tu, v1.tv),
                                                         c1),
                                         triangle_vertex(d3dtl_to_point(internal_scr_res_scale_f,
                                                                        internal_scr_offset_f,
                                                                        v2),
                                                         make_point(v2.tu, v2.tv),
                                                         c2),
                                         triangle_vertex(d3dtl_to_point(internal_scr_res_scale_f,
                                                                        internal_scr_offset_f,
                                                                        v3),
                                                         make_point(v3.tu, v3.tv),
                                                         c3),
                                         current_material));
                        }
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
            update_hud_texture();

            ogs->tribuf.swap_next();
            auto *trimdl = ogs->tribuf.get_current();

            world_batch.sort();
            world_transparent_batch.sort();
            gun_batch.sort();
            gun_transparent_batch.sort();
            overlay_batch.sort();

            fill_buffer(world_batch, &trimdl->world_trimdl);
            fill_buffer(world_transparent_batch, &trimdl->world_transparent_trimdl);
            fill_buffer(gun_batch, &trimdl->gun_trimdl);
            fill_buffer(gun_transparent_batch, &trimdl->gun_transparent_trimdl);
            fill_buffer(overlay_batch, &trimdl->overlay_trimdl);

            bool posterize_lighting = the_config->enable_posterized_lighting;
            draw_game_gbuffer_pass(trimdl, posterize_lighting);
            draw_game_transparency_pass(trimdl, posterize_lighting);

            draw_hud();

            // Reset state:
            is_gun = false;
            is_transparent = false;
            current_triangle_batch = &world_batch;
            current_material = material_instance_id(0U);

            world_batch.clear();
            world_transparent_batch.clear();
            gun_batch.clear();
            gun_transparent_batch.clear();
            overlay_batch.clear();
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
                if(desc.dwHeight == 64) {
                    // JK uses a 64-pixel tall secondary surface for drawing the credit scroll.
                    ddraw1_offscreen_credit_surface.set_surface_desc(desc);
                    return &ddraw1_offscreen_credit_surface;
                }

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
    };
}

std::unique_ptr<jkgm::renderer> jkgm::create_renderer(HINSTANCE dll_instance,
                                                      config const *the_config)
{
    return std::make_unique<jkgm::renderer_impl>(dll_instance, the_config);
}