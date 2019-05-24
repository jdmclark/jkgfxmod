#include "renderer_screen.hpp"
#include "base/log.hpp"
#include "base/string_search.hpp"
#include "common/config.hpp"
#include "common/error_reporter.hpp"
#include "glad/gl.h"
#include "renderer_screen_basic.hpp"
#include "renderer_screen_msaa.hpp"

std::unique_ptr<jkgm::renderer_screen> jkgm::create_renderer_screen(config const *the_config,
                                                                    size<2, int> screen_res)
{
    if(the_config->antialiasing.has_value()) {
        auto aa_type = the_config->antialiasing->type;

        if(iequal(aa_type, std::string_view("msaa"))) {
            int msaa_samples = the_config->antialiasing->samples;
            std::optional<float> min_sample_factor;

            if(the_config->antialiasing->min_sample_factor > 0.0f) {
                if(GLAD_GL_ARB_sample_shading) {
                    min_sample_factor = the_config->antialiasing->min_sample_factor;
                }
                else {
                    LOG_WARNING("MSAA min_sample_factor is configured, but the graphics "
                                "hardware does not support it");
                    report_warning_message(
                        "The configuration specifies MSAA using a minimum sample factor, but your "
                        "graphics hardware does not support minimum sample factor. The specified "
                        "minimum sample factor will be ignored.\n\nDetails: GL_ARB_sample_shading "
                        "extension not available");
                }
            }

            return std::make_unique<renderer_screen_msaa>(
                screen_res, msaa_samples, min_sample_factor);
        }

        if(iequal(aa_type, std::string_view("ssaa"))) {
            int msaa_samples = the_config->antialiasing->samples;
            std::optional<float> min_sample_factor;

            if(GLAD_GL_ARB_sample_shading) {
                min_sample_factor = 1.0f;
            }
            else {
                LOG_WARNING("SSAA is configured, but the graphics hardware does not support "
                            "min_sample_factor");
                report_warning_message(
                    "The configuration specifies SSAA, but your graphics hardware does not support "
                    "it. MSAA will be used instead.\n\nDetails: GL_ARB_sample_shading extension "
                    "not available");
            }

            return std::make_unique<renderer_screen_msaa>(
                screen_res, msaa_samples, min_sample_factor);
        }

        LOG_WARNING("Unknown antialiasing type ignored: ", aa_type);
        report_warning_message(
            str(format("The configuration contains an unknown antialiasing type '",
                       aa_type,
                       "'. This session will not use antialiasing.")));
    }

    return std::make_unique<renderer_screen_basic>(screen_res);
}