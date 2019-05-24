# Editing configuration file (`jkgm.json`)

| Variable | Explanation | Syntax | Default value |
|----------------|------------------------------|--------|---------------|
| **resolution** | What resolution to output at. | ordered pair (`[ x, y ]`) | `[ 1024, 768 ]` |
| **internal_resolution** | What resolution to render at. Note that using a [custom resolution](https://pcgamingwiki.com/wiki/Glossary:Custom_resolution) is preferable when possible.| ordered pair, or `null` to use the same resolution as output | `null` |
| **fullscreen** | Select whether to use fullscreen or Windowed mode. | boolean<br/>(`true` for fullscreen, or `false` for windowed) |
| **hud_scale** | Upscale the HUD to avoid tiny HUD at high resolutions. Use whole integers for best results. | decimal<br/>(ex: 2.0 = 200% scaling) | Default: `1.0` |
| **max_anisotropy** | Level of [anisotropic filtering](https://en.wikipedia.org/wiki/Anisotropic_filtering) to use. Can be used even when `enable_texture_filtering` is set to false. | decimal (values taken from [ARB_texture_filter_anisotropic](https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_texture_filter_anisotropic.txt)): `1.0` to disable,<br/> `2.0` for 2x,<br/> `4.0` for 4x,<br/> `8.0` for 8x,<br/> `16.0` for 16x | Default: `2.0` |
| **antialiasing** | Type and degree of [antialiasing](https://en.wikipedia.org/wiki/Spatial_anti-aliasing) to use. | `null` or object (`{ "type": <type>, "samples": <integer> }`)<br/><br/>**Supported types:**<br/>`MSAA`: Multisample antialiasing smooths the edges of polygons only.<br/>`SSAA`: Supersample antialiasing smooths polygon edges and interiors, but is much more expensive than MSAA.<br/><br/>**Other fields:**<br/>`samples`: The number of antialiasing samples per pixel. Higher values may look better, but are more expensive.<br/>`min_sample_factor`: When used with MSAA, applies some amount of supersampling to polygon interiors. For example, 8xMSAA with a minimum sample factor of 0.25 will apply 2xSSAA to polygon interiors. This can be used to gain some antialiasing for polygon interiors without the full cost of SSAA.| Default: `null` |
| **enable_bloom** | Use [bloom](https://en.wikipedia.org/wiki/Bloom_(shader_effect)) effect for brighter lights. Requires compatible texture pack. | boolean | Default: `true` |
| **enable_ssao** | Use [screen-space ambient occlusion](https://en.wikipedia.org/wiki/Screen_space_ambient_occlusion) to subtly increase perceived depth in the image. | boolean | `true` |
| **enable_parallax** | Use [parallax occlusion mapping](https://en.wikipedia.org/wiki/Parallax_occlusion_mapping) to add depth to individual textures. Requires compatible texture pack. | boolean | `true` |
| **enable_texture_filtering** | Use [bilinear filtering](https://en.wikipedia.org/wiki/Bilinear_filtering) on textures, reducing texture aliasing whilst also making closeup textures blurry.  Disable to use nearest-neighbor filtering instead.<br/>Note that it does not affect [trilinear filtering](https://en.wikipedia.org/wiki/Trilinear_filtering), which is always used, or anisotropic filtering, which is controlled with `max_anisotropy`. | boolean | `true` |
| **enable_posterized_lighting** | Posterize the lighting, introducing noticeable [color banding](https://en.wikipedia.org/wiki/Colour_banding) that simulates the 8-bit lighting of the RenderDroid software renderer. | boolean | `false` |
| **command** | Name of *Jedi Knight* or *Mysteries of the Sith* executable to use. | `"<filename>"` | `"jk.exe"` |
| **data_path** | Path to material and shader patches.<br/>Accepts both absolute and relative [paths](https://en.wikipedia.org/wiki/Path_(computing)#MS-DOS/Microsoft_Windows_style). | `"<folderpath>"` | `"jkgm"` |
| **log_path** | Path to log file.<br/>Accepts both absolute and relative [paths](https://en.wikipedia.org/wiki/Path_(computing)#MS-DOS/Microsoft_Windows_style). | `"<folderpath>"`, or `null` to not output to a log | `null` |

<!-- No need to really have this, but I didn't want to remove it yet, in
## HUD Scaling
Recommend values for HUD scaling (based on 360p HUD size and Jedi Outcast/Academy HUD sizes): `2.0` for 720p, `3.0` for 1080p, `4.0` for 1440p, `6.0` for 2160p.
-->
