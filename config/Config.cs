using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Text.Json;

namespace config
{
    abstract class AntialiasingConfig
    {
        public abstract void Write(Utf8JsonWriter doc);

        public virtual bool Equals(AntialiasingConfig other)
        {
            return false;
        }
    };

    class NullAAConfig : AntialiasingConfig
    {
        public NullAAConfig()
        {

        }

        public override void Write(Utf8JsonWriter doc)
        {
            doc.WriteNull("antialiasing");
        }

        public override bool Equals(AntialiasingConfig other)
        {
            return other is NullAAConfig;
        }
    }

    class MSAAConfig : AntialiasingConfig
    {
        public int Samples;
        public decimal MinSampleFactor;

        public MSAAConfig()
        {
            Samples = 2;
            MinSampleFactor = new decimal(0.0);
        }

        public MSAAConfig(int samples, decimal min_samp_factor)
        {
            Samples = samples;
            MinSampleFactor = min_samp_factor;
        }

        public override void Write(Utf8JsonWriter doc)
        {
            doc.WriteStartObject("antialiasing");
            doc.WriteString("type", "MSAA");
            doc.WriteNumber("samples", Samples);
            doc.WriteNumber("min_sample_factor", MinSampleFactor);
            doc.WriteEndObject();
        }

        public override bool Equals(AntialiasingConfig other)
        {
            if (other is MSAAConfig aaother)
            {
                return aaother.Samples == Samples && aaother.MinSampleFactor == MinSampleFactor;
            }

            return false;
        }
    }

    class SSAAConfig : AntialiasingConfig
    {
        public int Samples;

        public SSAAConfig()
        {
            Samples = 2;
        }

        public SSAAConfig(int samples)
        {
            Samples = samples;
        }

        public override void Write(Utf8JsonWriter doc)
        {
            doc.WriteStartObject("antialiasing");
            doc.WriteString("type", "SSAA");
            doc.WriteNumber("samples", Samples);
            doc.WriteEndObject();
        }

        public override bool Equals(AntialiasingConfig other)
        {
            if (other is SSAAConfig aaother)
            {
                return aaother.Samples == Samples;
            }

            return false;
        }
    }

    class Config
    {
        public Tuple<int, int> Resolution;
        public bool Fullscreen;

        public bool CorrectGameAspectRatio;
        public bool CorrectMenuAspectRatio;

        public decimal HudScale;

        public decimal MaxAnisotropy;
        public AntialiasingConfig Antialiasing;
        public bool EnableBloom;
        public bool EnableSSAO;
        public bool EnableParallax;
        public bool EnableTextureFiltering;
        public bool EnablePosterizedLighting;

        public string Command;
        public string DataPath;
        public string LogPath;

        public Config()
        {
            Resolution = new Tuple<int, int>(640, 480);
            Fullscreen = false;

            CorrectGameAspectRatio = false;
            CorrectMenuAspectRatio = true;

            HudScale = new decimal(1);

            MaxAnisotropy = new decimal(2);
            Antialiasing = new NullAAConfig();
            EnableBloom = true;
            EnableSSAO = true;
            EnableParallax = true;
            EnableTextureFiltering = true;
            EnablePosterizedLighting = false;

            Command = "jk.exe";
            DataPath = "jkgm";
            LogPath = null;
        }

        static JsonDocument LoadDocument(string config_path)
        {
            using (var f = File.OpenRead(config_path))
            {
                return JsonDocument.Parse(f);
            }
        }

        static AntialiasingConfig ParseAAConfig(JsonElement em)
        {
            if (em.ValueKind == JsonValueKind.Object)
            {
                JsonElement type_em;
                if (!em.TryGetProperty("type", out type_em))
                {
                    return new NullAAConfig();
                }

                int samp = 2;
                decimal samp_fac = new decimal(0);

                JsonElement samp_em;
                if (em.TryGetProperty("samples", out samp_em))
                {
                    samp = samp_em.GetInt32();
                }

                JsonElement samp_fac_em;
                if (em.TryGetProperty("min_sample_factor", out samp_fac_em))
                {
                    samp_fac = samp_fac_em.GetDecimal();
                }

                if (type_em.GetString().ToUpperInvariant() == "MSAA")
                {
                    return new MSAAConfig(samp, samp_fac);
                }

                if (type_em.GetString().ToUpperInvariant() == "SSAA")
                {
                    return new SSAAConfig(samp);
                }
            }

            return new NullAAConfig();
        }

        public void SaveToFile(string config_path)
        {
            using (var fs = File.Create(config_path))
            {
                using (var doc = new Utf8JsonWriter(fs, new JsonWriterOptions { Indented = true }))
                {
                    doc.WriteStartObject();

                    // Resolution
                    doc.WriteStartArray("resolution");
                    doc.WriteNumberValue(Resolution.Item1);
                    doc.WriteNumberValue(Resolution.Item2);
                    doc.WriteEndArray();

                    doc.WriteBoolean("fullscreen", Fullscreen);
                    doc.WriteBoolean("correct_game_aspect_ratio", CorrectGameAspectRatio);
                    doc.WriteBoolean("correct_menu_aspect_ratio", CorrectMenuAspectRatio);
                    doc.WriteNumber("hud_scale", HudScale);
                    doc.WriteNumber("max_anisotropy", MaxAnisotropy);

                    if (Antialiasing != null)
                    {
                        Antialiasing.Write(doc);
                    }

                    doc.WriteBoolean("enable_bloom", EnableBloom);
                    doc.WriteBoolean("enable_ssao", EnableSSAO);
                    doc.WriteBoolean("enable_parallax", EnableParallax);
                    doc.WriteBoolean("enable_texture_filtering", EnableTextureFiltering);
                    doc.WriteBoolean("enable_posterized_lighting", EnablePosterizedLighting);

                    doc.WriteString("command", Command);
                    doc.WriteString("data_path", DataPath);

                    if (LogPath != null)
                    {
                        doc.WriteString("log_path", LogPath);
                    }

                    doc.WriteEndObject();
                    doc.Flush();
                }
            }
        }

        public static Config LoadFromFile(string config_path)
        {
            Config rv = new Config();

            if (!File.Exists(config_path))
            {
                // Configuration file has not been created yet. Return the default config.
                return rv;
            }

            using (var doc = LoadDocument(config_path))
            {
                foreach (var em in doc.RootElement.EnumerateObject())
                {
                    switch (em.Name)
                    {
                        case "resolution":
                            if (em.Value.ValueKind == JsonValueKind.Array && em.Value.GetArrayLength() == 2)
                            {
                                var arr = em.Value.EnumerateArray();
                                rv.Resolution = new Tuple<int, int>(arr.ElementAt(0).GetInt32(), arr.ElementAt(1).GetInt32());
                            }
                            break;

                        case "fullscreen":
                            rv.Fullscreen = em.Value.GetBoolean();
                            break;

                        case "correct_game_aspect_ratio":
                            rv.CorrectGameAspectRatio = em.Value.GetBoolean();
                            break;

                        case "correct_menu_aspect_ratio":
                            rv.CorrectMenuAspectRatio = em.Value.GetBoolean();
                            break;

                        case "hud_scale":
                            rv.HudScale = em.Value.GetDecimal();
                            break;

                        case "max_anisotropy":
                            rv.MaxAnisotropy = em.Value.GetDecimal();
                            break;

                        case "antialiasing":
                            rv.Antialiasing = ParseAAConfig(em.Value);
                            break;

                        case "enable_bloom":
                            rv.EnableBloom = em.Value.GetBoolean();
                            break;

                        case "enable_ssao":
                            rv.EnableSSAO = em.Value.GetBoolean();
                            break;

                        case "enable_parallax":
                            rv.EnableParallax = em.Value.GetBoolean();
                            break;

                        case "enable_texture_filtering":
                            rv.EnableTextureFiltering = em.Value.GetBoolean();
                            break;

                        case "enable_posterized_lighting":
                            rv.EnablePosterizedLighting = em.Value.GetBoolean();
                            break;

                        case "command":
                            rv.Command = em.Value.GetString();
                            break;

                        case "data_path":
                            rv.DataPath = em.Value.GetString();
                            break;

                        case "log_path":
                            if (em.Value.ValueKind == JsonValueKind.Null)
                            {
                                rv.LogPath = null;
                            }
                            else
                            {
                                rv.LogPath = em.Value.GetString();
                            }
                            break;
                    }
                }
            }

            return rv;
        }
    }
}
