namespace config
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.buttonOK = new System.Windows.Forms.Button();
            this.buttonCancel = new System.Windows.Forms.Button();
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
            this.radioButtonRetro = new System.Windows.Forms.RadioButton();
            this.radioButtonModern = new System.Windows.Forms.RadioButton();
            this.radioButtonCustom = new System.Windows.Forms.RadioButton();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.checkBoxGameAspect = new System.Windows.Forms.CheckBox();
            this.checkBoxMenuAspect = new System.Windows.Forms.CheckBox();
            this.comboBoxAntialiasing = new System.Windows.Forms.ComboBox();
            this.numericUpDownAnisotropicFiltering = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.numericUpDownHudScale = new System.Windows.Forms.NumericUpDown();
            this.comboBoxResolution = new System.Windows.Forms.ComboBox();
            this.comboBoxQuality = new System.Windows.Forms.ComboBox();
            this.checkBoxFullscreen = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
            this.checkBoxWriteLog = new System.Windows.Forms.CheckBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.flowLayoutPanel1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tableLayoutPanel2.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownAnisotropicFiltering)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownHudScale)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.tableLayoutPanel3.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.SuspendLayout();
            // 
            // buttonOK
            // 
            this.buttonOK.Location = new System.Drawing.Point(479, 3);
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.Size = new System.Drawing.Size(75, 23);
            this.buttonOK.TabIndex = 0;
            this.buttonOK.Text = "OK";
            this.buttonOK.UseVisualStyleBackColor = true;
            this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
            // 
            // buttonCancel
            // 
            this.buttonCancel.Location = new System.Drawing.Point(560, 3);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(75, 23);
            this.buttonCancel.TabIndex = 2;
            this.buttonCancel.Text = "Cancel";
            this.buttonCancel.UseVisualStyleBackColor = true;
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.AutoSize = true;
            this.flowLayoutPanel1.Controls.Add(this.buttonCancel);
            this.flowLayoutPanel1.Controls.Add(this.buttonOK);
            this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.flowLayoutPanel1.FlowDirection = System.Windows.Forms.FlowDirection.RightToLeft;
            this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 657);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(638, 29);
            this.flowLayoutPanel1.TabIndex = 1;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.groupBox3);
            this.tabPage1.Controls.Add(this.groupBox2);
            this.tabPage1.Controls.Add(this.groupBox1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(630, 660);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Settings";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.tableLayoutPanel2);
            this.groupBox1.Location = new System.Drawing.Point(8, 6);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(614, 100);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Profile";
            // 
            // tableLayoutPanel2
            // 
            this.tableLayoutPanel2.ColumnCount = 3;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel2.Controls.Add(this.radioButtonCustom, 2, 0);
            this.tableLayoutPanel2.Controls.Add(this.radioButtonModern, 0, 0);
            this.tableLayoutPanel2.Controls.Add(this.radioButtonRetro, 1, 0);
            this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel2.Location = new System.Drawing.Point(3, 16);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 1;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel2.Size = new System.Drawing.Size(608, 81);
            this.tableLayoutPanel2.TabIndex = 3;
            // 
            // radioButtonRetro
            // 
            this.radioButtonRetro.Appearance = System.Windows.Forms.Appearance.Button;
            this.radioButtonRetro.AutoSize = true;
            this.radioButtonRetro.Dock = System.Windows.Forms.DockStyle.Fill;
            this.radioButtonRetro.Location = new System.Drawing.Point(205, 3);
            this.radioButtonRetro.Name = "radioButtonRetro";
            this.radioButtonRetro.Size = new System.Drawing.Size(196, 75);
            this.radioButtonRetro.TabIndex = 1;
            this.radioButtonRetro.TabStop = true;
            this.radioButtonRetro.Text = "Retro";
            this.radioButtonRetro.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.radioButtonRetro.UseVisualStyleBackColor = true;
            this.radioButtonRetro.CheckedChanged += new System.EventHandler(this.ProfileCheckedChanged);
            // 
            // radioButtonModern
            // 
            this.radioButtonModern.Appearance = System.Windows.Forms.Appearance.Button;
            this.radioButtonModern.AutoSize = true;
            this.radioButtonModern.Dock = System.Windows.Forms.DockStyle.Fill;
            this.radioButtonModern.Location = new System.Drawing.Point(3, 3);
            this.radioButtonModern.Name = "radioButtonModern";
            this.radioButtonModern.Size = new System.Drawing.Size(196, 75);
            this.radioButtonModern.TabIndex = 0;
            this.radioButtonModern.TabStop = true;
            this.radioButtonModern.Text = "Modern";
            this.radioButtonModern.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.radioButtonModern.UseVisualStyleBackColor = true;
            this.radioButtonModern.CheckedChanged += new System.EventHandler(this.ProfileCheckedChanged);
            // 
            // radioButtonCustom
            // 
            this.radioButtonCustom.Appearance = System.Windows.Forms.Appearance.Button;
            this.radioButtonCustom.AutoSize = true;
            this.radioButtonCustom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.radioButtonCustom.Location = new System.Drawing.Point(407, 3);
            this.radioButtonCustom.Name = "radioButtonCustom";
            this.radioButtonCustom.Size = new System.Drawing.Size(198, 75);
            this.radioButtonCustom.TabIndex = 2;
            this.radioButtonCustom.TabStop = true;
            this.radioButtonCustom.Text = "Custom";
            this.radioButtonCustom.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.radioButtonCustom.UseVisualStyleBackColor = true;
            this.radioButtonCustom.CheckedChanged += new System.EventHandler(this.ProfileCheckedChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.groupBox2.Controls.Add(this.tableLayoutPanel1);
            this.groupBox2.Location = new System.Drawing.Point(8, 112);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(614, 224);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Display";
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 75F));
            this.tableLayoutPanel1.Controls.Add(this.label3, 0, 6);
            this.tableLayoutPanel1.Controls.Add(this.label1, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.label2, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.checkBoxFullscreen, 1, 2);
            this.tableLayoutPanel1.Controls.Add(this.comboBoxQuality, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.comboBoxResolution, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.numericUpDownHudScale, 1, 6);
            this.tableLayoutPanel1.Controls.Add(this.label4, 0, 4);
            this.tableLayoutPanel1.Controls.Add(this.label5, 0, 5);
            this.tableLayoutPanel1.Controls.Add(this.numericUpDownAnisotropicFiltering, 1, 5);
            this.tableLayoutPanel1.Controls.Add(this.comboBoxAntialiasing, 1, 4);
            this.tableLayoutPanel1.Controls.Add(this.checkBoxMenuAspect, 1, 7);
            this.tableLayoutPanel1.Controls.Add(this.checkBoxGameAspect, 1, 8);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 16);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 9;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.Size = new System.Drawing.Size(608, 205);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // checkBoxGameAspect
            // 
            this.checkBoxGameAspect.AutoSize = true;
            this.checkBoxGameAspect.Location = new System.Drawing.Point(155, 182);
            this.checkBoxGameAspect.Name = "checkBoxGameAspect";
            this.checkBoxGameAspect.Size = new System.Drawing.Size(178, 17);
            this.checkBoxGameAspect.TabIndex = 16;
            this.checkBoxGameAspect.Text = "Use Original Game Aspect Ratio";
            this.checkBoxGameAspect.UseVisualStyleBackColor = true;
            this.checkBoxGameAspect.CheckedChanged += new System.EventHandler(this.checkBoxGameAspect_CheckedChanged);
            // 
            // checkBoxMenuAspect
            // 
            this.checkBoxMenuAspect.AutoSize = true;
            this.checkBoxMenuAspect.Location = new System.Drawing.Point(155, 159);
            this.checkBoxMenuAspect.Name = "checkBoxMenuAspect";
            this.checkBoxMenuAspect.Size = new System.Drawing.Size(133, 17);
            this.checkBoxMenuAspect.TabIndex = 15;
            this.checkBoxMenuAspect.Text = "Fix Menu Aspect Ratio";
            this.checkBoxMenuAspect.UseVisualStyleBackColor = true;
            this.checkBoxMenuAspect.CheckedChanged += new System.EventHandler(this.checkBoxMenuAspect_CheckedChanged);
            // 
            // comboBoxAntialiasing
            // 
            this.comboBoxAntialiasing.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxAntialiasing.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxAntialiasing.FormattingEnabled = true;
            this.comboBoxAntialiasing.Location = new System.Drawing.Point(155, 80);
            this.comboBoxAntialiasing.Name = "comboBoxAntialiasing";
            this.comboBoxAntialiasing.Size = new System.Drawing.Size(450, 21);
            this.comboBoxAntialiasing.TabIndex = 13;
            this.comboBoxAntialiasing.SelectedIndexChanged += new System.EventHandler(this.comboBoxAntialiasing_SelectedIndexChanged);
            // 
            // numericUpDownAnisotropicFiltering
            // 
            this.numericUpDownAnisotropicFiltering.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.numericUpDownAnisotropicFiltering.DecimalPlaces = 1;
            this.numericUpDownAnisotropicFiltering.Location = new System.Drawing.Point(155, 107);
            this.numericUpDownAnisotropicFiltering.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownAnisotropicFiltering.Name = "numericUpDownAnisotropicFiltering";
            this.numericUpDownAnisotropicFiltering.Size = new System.Drawing.Size(450, 20);
            this.numericUpDownAnisotropicFiltering.TabIndex = 14;
            this.numericUpDownAnisotropicFiltering.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownAnisotropicFiltering.ValueChanged += new System.EventHandler(this.numericUpDownAnisotropicFiltering_ValueChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label5.Location = new System.Drawing.Point(3, 104);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(146, 26);
            this.label5.TabIndex = 12;
            this.label5.Text = "Anisotropic Filtering";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label4.Location = new System.Drawing.Point(3, 77);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(146, 27);
            this.label4.TabIndex = 11;
            this.label4.Text = "Antialiasing";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // numericUpDownHudScale
            // 
            this.numericUpDownHudScale.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.numericUpDownHudScale.Location = new System.Drawing.Point(155, 133);
            this.numericUpDownHudScale.Maximum = new decimal(new int[] {
            16,
            0,
            0,
            0});
            this.numericUpDownHudScale.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownHudScale.Name = "numericUpDownHudScale";
            this.numericUpDownHudScale.Size = new System.Drawing.Size(450, 20);
            this.numericUpDownHudScale.TabIndex = 9;
            this.numericUpDownHudScale.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownHudScale.ValueChanged += new System.EventHandler(this.numericUpDownHudScale_ValueChanged);
            // 
            // comboBoxResolution
            // 
            this.comboBoxResolution.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxResolution.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxResolution.FormattingEnabled = true;
            this.comboBoxResolution.Location = new System.Drawing.Point(155, 30);
            this.comboBoxResolution.Name = "comboBoxResolution";
            this.comboBoxResolution.Size = new System.Drawing.Size(450, 21);
            this.comboBoxResolution.TabIndex = 7;
            this.comboBoxResolution.SelectedIndexChanged += new System.EventHandler(this.comboBoxResolution_SelectedIndexChanged);
            // 
            // comboBoxQuality
            // 
            this.comboBoxQuality.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxQuality.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxQuality.FormattingEnabled = true;
            this.comboBoxQuality.Items.AddRange(new object[] {
            "Low",
            "Medium",
            "High",
            "Custom"});
            this.comboBoxQuality.Location = new System.Drawing.Point(155, 3);
            this.comboBoxQuality.Name = "comboBoxQuality";
            this.comboBoxQuality.Size = new System.Drawing.Size(450, 21);
            this.comboBoxQuality.TabIndex = 6;
            this.comboBoxQuality.SelectedIndexChanged += new System.EventHandler(this.comboBoxQuality_SelectedIndexChanged);
            // 
            // checkBoxFullscreen
            // 
            this.checkBoxFullscreen.AutoSize = true;
            this.checkBoxFullscreen.Location = new System.Drawing.Point(155, 57);
            this.checkBoxFullscreen.Name = "checkBoxFullscreen";
            this.checkBoxFullscreen.Size = new System.Drawing.Size(79, 17);
            this.checkBoxFullscreen.TabIndex = 5;
            this.checkBoxFullscreen.Text = "Full Screen";
            this.checkBoxFullscreen.UseVisualStyleBackColor = true;
            this.checkBoxFullscreen.CheckedChanged += new System.EventHandler(this.checkBoxFullscreen_CheckedChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label2.Location = new System.Drawing.Point(3, 27);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(146, 27);
            this.label2.TabIndex = 3;
            this.label2.Text = "Resolution";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label1.Location = new System.Drawing.Point(3, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(146, 27);
            this.label1.TabIndex = 2;
            this.label1.Text = "Quality";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label3.Location = new System.Drawing.Point(3, 130);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(146, 26);
            this.label3.TabIndex = 10;
            this.label3.Text = "HUD Scale";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.tableLayoutPanel3);
            this.groupBox3.Location = new System.Drawing.Point(8, 342);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(614, 46);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Developer";
            // 
            // tableLayoutPanel3
            // 
            this.tableLayoutPanel3.ColumnCount = 2;
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 75F));
            this.tableLayoutPanel3.Controls.Add(this.checkBoxWriteLog, 1, 0);
            this.tableLayoutPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel3.Location = new System.Drawing.Point(3, 16);
            this.tableLayoutPanel3.Name = "tableLayoutPanel3";
            this.tableLayoutPanel3.RowCount = 1;
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel3.Size = new System.Drawing.Size(608, 27);
            this.tableLayoutPanel3.TabIndex = 0;
            // 
            // checkBoxWriteLog
            // 
            this.checkBoxWriteLog.AutoSize = true;
            this.checkBoxWriteLog.Location = new System.Drawing.Point(155, 3);
            this.checkBoxWriteLog.Name = "checkBoxWriteLog";
            this.checkBoxWriteLog.Size = new System.Drawing.Size(107, 17);
            this.checkBoxWriteLog.TabIndex = 0;
            this.checkBoxWriteLog.Text = "Save Debug Log";
            this.checkBoxWriteLog.UseVisualStyleBackColor = true;
            this.checkBoxWriteLog.CheckedChanged += new System.EventHandler(this.checkBoxWriteLog_CheckedChanged);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(638, 686);
            this.tabControl1.TabIndex = 0;
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(638, 686);
            this.Controls.Add(this.flowLayoutPanel1);
            this.Controls.Add(this.tabControl1);
            this.Name = "MainWindow";
            this.Text = "JkGfxMod Configuration";
            this.flowLayoutPanel1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.tableLayoutPanel2.ResumeLayout(false);
            this.tableLayoutPanel2.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownAnisotropicFiltering)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownHudScale)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.tableLayoutPanel3.ResumeLayout(false);
            this.tableLayoutPanel3.PerformLayout();
            this.tabControl1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.Button buttonCancel;
        private System.Windows.Forms.Button buttonOK;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
        private System.Windows.Forms.CheckBox checkBoxWriteLog;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.CheckBox checkBoxFullscreen;
        private System.Windows.Forms.ComboBox comboBoxQuality;
        private System.Windows.Forms.ComboBox comboBoxResolution;
        private System.Windows.Forms.NumericUpDown numericUpDownHudScale;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown numericUpDownAnisotropicFiltering;
        private System.Windows.Forms.ComboBox comboBoxAntialiasing;
        private System.Windows.Forms.CheckBox checkBoxMenuAspect;
        private System.Windows.Forms.CheckBox checkBoxGameAspect;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.RadioButton radioButtonCustom;
        private System.Windows.Forms.RadioButton radioButtonModern;
        private System.Windows.Forms.RadioButton radioButtonRetro;
        private System.Windows.Forms.TabControl tabControl1;
    }
}

