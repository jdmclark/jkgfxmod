using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace config
{
    public partial class MainWindow : Form
    {
        Config cfg;

        List<AntialiasingConfig> default_antialiasing_types;
        List<Tuple<int, int>> default_resolutions;

        public MainWindow()
        {
            InitializeComponent();

            default_antialiasing_types = new List<AntialiasingConfig>
            {
                new NullAAConfig(),
                new MSAAConfig(2, new decimal(0)),
                new MSAAConfig(4, new decimal(0)),
                new MSAAConfig(8, new decimal(0)),
                new MSAAConfig(8, new decimal(0.25)),
                new MSAAConfig(8, new decimal(0.5)),
                new SSAAConfig(2),
                new SSAAConfig(4)
            };

            comboBoxAntialiasing.Items.Add("None");
            comboBoxAntialiasing.Items.Add("MSAA 2x");
            comboBoxAntialiasing.Items.Add("MSAA 4x");
            comboBoxAntialiasing.Items.Add("MSAA 8x");
            comboBoxAntialiasing.Items.Add("MSAA 8x SSAA 2x");
            comboBoxAntialiasing.Items.Add("MSAA 8x SSAA 4x");
            comboBoxAntialiasing.Items.Add("SSAA 2x");
            comboBoxAntialiasing.Items.Add("SSAA 4x");
            comboBoxAntialiasing.Items.Add("Custom");

            default_resolutions = new List<Tuple<int, int>>
            {
                new Tuple<int, int>(640, 480),
                new Tuple<int, int>(720, 480),
                new Tuple<int, int>(800, 600),
                new Tuple<int, int>(1024, 768),
                new Tuple<int, int>(1152, 864),
                new Tuple<int, int>(1176, 664),
                new Tuple<int, int>(1280, 720),
                new Tuple<int, int>(1280, 768),
                new Tuple<int, int>(1280, 800),
                new Tuple<int, int>(1280, 960),
                new Tuple<int, int>(1280, 1024),
                new Tuple<int, int>(1360, 768),
                new Tuple<int, int>(1366, 768),
                new Tuple<int, int>(1440, 900),
                new Tuple<int, int>(1600, 900),
                new Tuple<int, int>(1600, 1024),
                new Tuple<int, int>(1600, 1200),
                new Tuple<int, int>(1680, 1050),
                new Tuple<int, int>(1768, 992),
                new Tuple<int, int>(1920, 1080),
                new Tuple<int, int>(1920, 1200),
                new Tuple<int, int>(1920, 1440),
                new Tuple<int, int>(2048, 1536),
                new Tuple<int, int>(2560, 1440),
                new Tuple<int, int>(2560, 1600),
                new Tuple<int, int>(3840, 2160)
            };

            foreach (var em in default_resolutions)
            {
                comboBoxResolution.Items.Add(String.Format("{0}x{1}", em.Item1, em.Item2));
            }

            comboBoxResolution.Items.Add("Custom");

            cfg = Config.LoadFromFile("jkgm.json");
            UpdateStateFromConfig();
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            cfg.SaveToFile("jkgm.json");
            Close();
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void Popup_FormClosed(object sender, FormClosedEventArgs e)
        {
            UpdateStateFromConfig();
        }

        private void UpdateStateFromConfig()
        {
            // Set profile buttons
            if (cfg.EnableTextureFiltering && !cfg.EnablePosterizedLighting)
            {
                radioButtonModern.Checked = true;
            }
            else if (!cfg.EnableTextureFiltering && cfg.EnablePosterizedLighting)
            {
                radioButtonRetro.Checked = true;
            }
            else
            {
                radioButtonCustom.Checked = true;
            }

            // Set quality dropdown
            if (cfg.EnableBloom && cfg.EnableSSAO && cfg.EnableParallax)
            {
                comboBoxQuality.SelectedItem = "High";
            }
            else if (cfg.EnableBloom && !cfg.EnableSSAO && !cfg.EnableParallax)
            {
                comboBoxQuality.SelectedItem = "Medium";
            }
            else if (!cfg.EnableBloom && !cfg.EnableSSAO && !cfg.EnableParallax)
            {
                comboBoxQuality.SelectedItem = "Low";
            }
            else
            {
                comboBoxQuality.SelectedItem = "Custom";
            }

            // Set resolution dropdown
            comboBoxResolution.SelectedIndex = comboBoxResolution.Items.Count - 1;
            int index = 0;
            foreach (var em in default_resolutions)
            {
                if (em.Equals(cfg.Resolution))
                {
                    comboBoxResolution.SelectedIndex = index;
                    break;
                }

                ++index;
            }

            // Set full screen checkbox
            checkBoxFullscreen.Checked = cfg.Fullscreen;

            // Set antialiasing dropdown
            comboBoxAntialiasing.SelectedIndex = comboBoxAntialiasing.Items.Count - 1;
            if (cfg.Antialiasing == null)
            {
                comboBoxAntialiasing.SelectedIndex = 1;
            }
            else
            {
                index = 0;
                foreach (var em in default_antialiasing_types)
                {
                    if (em.Equals(cfg.Antialiasing))
                    {
                        comboBoxAntialiasing.SelectedIndex = index;
                        break;
                    }

                    ++index;
                }
            }

            numericUpDownAnisotropicFiltering.Value = cfg.MaxAnisotropy;
            numericUpDownHudScale.Value = cfg.HudScale;
            checkBoxMenuAspect.Checked = cfg.CorrectMenuAspectRatio;
            checkBoxGameAspect.Checked = cfg.CorrectGameAspectRatio;

            checkBoxWriteLog.Checked = (cfg.LogPath != null);
        }

        private void ProfileCheckedChanged(object sender, EventArgs e)
        {
            if (radioButtonModern.Checked)
            {
                cfg.EnableTextureFiltering = true;
                cfg.EnablePosterizedLighting = false;
            }
            else if (radioButtonRetro.Checked)
            {
                cfg.EnableTextureFiltering = false;
                cfg.EnablePosterizedLighting = true;
            }
        }

        private void comboBoxQuality_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (comboBoxQuality.SelectedItem)
            {
                case "High":
                    cfg.EnableBloom = true;
                    cfg.EnableSSAO = true;
                    cfg.EnableParallax = true;
                    break;

                case "Medium":
                    cfg.EnableBloom = true;
                    cfg.EnableSSAO = false;
                    cfg.EnableParallax = false;
                    break;

                case "Low":
                    cfg.EnableBloom = false;
                    cfg.EnableSSAO = false;
                    cfg.EnableParallax = false;
                    break;
            }
        }

        private void comboBoxResolution_SelectedIndexChanged(object sender, EventArgs e)
        {
            int si = comboBoxResolution.SelectedIndex;
            if (si > 0 && si < default_resolutions.Count)
            {
                cfg.Resolution = default_resolutions.ElementAt(si);
            }
        }

        private void checkBoxFullscreen_CheckedChanged(object sender, EventArgs e)
        {
            cfg.Fullscreen = checkBoxFullscreen.Checked;
        }

        private void comboBoxAntialiasing_SelectedIndexChanged(object sender, EventArgs e)
        {
            int si = comboBoxAntialiasing.SelectedIndex;
            if (si > 0 && si < default_antialiasing_types.Count)
            {
                cfg.Antialiasing = default_antialiasing_types.ElementAt(si);
            }
        }

        private void numericUpDownAnisotropicFiltering_ValueChanged(object sender, EventArgs e)
        {
            cfg.MaxAnisotropy = numericUpDownAnisotropicFiltering.Value;
        }

        private void numericUpDownHudScale_ValueChanged(object sender, EventArgs e)
        {
            cfg.HudScale = numericUpDownHudScale.Value;
        }

        private void checkBoxMenuAspect_CheckedChanged(object sender, EventArgs e)
        {
            cfg.CorrectMenuAspectRatio = checkBoxMenuAspect.Checked;
        }

        private void checkBoxGameAspect_CheckedChanged(object sender, EventArgs e)
        {
            cfg.CorrectGameAspectRatio = checkBoxGameAspect.Checked;
        }

        private void checkBoxWriteLog_CheckedChanged(object sender, EventArgs e)
        {
            if (cfg.LogPath == null && checkBoxWriteLog.Checked)
            {
                cfg.LogPath = "jkgm.log";
            }
            else if (!checkBoxWriteLog.Checked)
            {
                cfg.LogPath = null;
            }
        }
    }
}
