namespace TeachingExecutor
{
    partial class AlignmentL
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AlignmentL));
            this.H_WindowMain = new HalconDotNet.HSmartWindowControl();
            this.panel_Left = new System.Windows.Forms.Panel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.gBox_Test = new System.Windows.Forms.GroupBox();
            this.ButImSave = new System.Windows.Forms.Button();
            this.EdScope = new System.Windows.Forms.TextBox();
            this.ButCombination = new System.Windows.Forms.Button();
            this.label17 = new System.Windows.Forms.Label();
            this.panel3 = new System.Windows.Forms.Panel();
            this.ButOpenFM = new System.Windows.Forms.Button();
            this.ButSaveFM = new System.Windows.Forms.Button();
            this.GroupFindParams = new System.Windows.Forms.GroupBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.cBoxInterpolation = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.EditMinScore = new System.Windows.Forms.NumericUpDown();
            this.EditNumLevels1 = new System.Windows.Forms.NumericUpDown();
            this.EditNumLevels2 = new System.Windows.Forms.NumericUpDown();
            this.GroupModelsParams = new System.Windows.Forms.GroupBox();
            this.cBoxFMMinContrast = new System.Windows.Forms.ComboBox();
            this.cBoxFMContrast = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.cBoxFMMetric = new System.Windows.Forms.ComboBox();
            this.GroupBoxFML = new System.Windows.Forms.GroupBox();
            this.butUpDateFM_RNP = new System.Windows.Forms.Button();
            this.butUpdateFM = new System.Windows.Forms.Button();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.Ed_Dy = new System.Windows.Forms.TextBox();
            this.Ed_Dx = new System.Windows.Forms.TextBox();
            this.gBox_Display_Image = new System.Windows.Forms.GroupBox();
            this.But_Disp_ALIG = new System.Windows.Forms.RadioButton();
            this.But_Disp_RAW = new System.Windows.Forms.RadioButton();
            this.But_Disp_CAD = new System.Windows.Forms.RadioButton();
            this.chBox_ROI_Find = new System.Windows.Forms.CheckBox();
            this.chBox_ROI_FM = new System.Windows.Forms.CheckBox();
            this.chBox_Fit = new System.Windows.Forms.CheckBox();
            this.chBox_RegProc_Rect = new System.Windows.Forms.CheckBox();
            this.chBox_GiReg = new System.Windows.Forms.CheckBox();
            this.GroupBoxCad = new System.Windows.Forms.GroupBox();
            this.OpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.SaveFileDialog = new System.Windows.Forms.SaveFileDialog();
            this.ButImageOpen = new System.Windows.Forms.Button();
            this.LabelImageSize = new System.Windows.Forms.Label();
            this.TextImageFile = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.ButCadOpen = new System.Windows.Forms.Button();
            this.LabelCadSize = new System.Windows.Forms.Label();
            this.TextCadFile = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.panel_Left.SuspendLayout();
            this.panel1.SuspendLayout();
            this.gBox_Test.SuspendLayout();
            this.panel3.SuspendLayout();
            this.GroupFindParams.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.EditMinScore)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.EditNumLevels1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.EditNumLevels2)).BeginInit();
            this.GroupModelsParams.SuspendLayout();
            this.GroupBoxFML.SuspendLayout();
            this.gBox_Display_Image.SuspendLayout();
            this.GroupBoxCad.SuspendLayout();
            this.SuspendLayout();
            // 
            // H_WindowMain
            // 
            this.H_WindowMain.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.H_WindowMain.AutoValidate = System.Windows.Forms.AutoValidate.EnableAllowFocusChange;
            this.H_WindowMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.H_WindowMain.HDoubleClickToFitContent = false;
            this.H_WindowMain.HDrawingObjectsModifier = HalconDotNet.HSmartWindowControl.DrawingObjectsModifier.None;
            this.H_WindowMain.HImagePart = new System.Drawing.Rectangle(0, 0, 640, 480);
            this.H_WindowMain.HKeepAspectRatio = true;
            this.H_WindowMain.HMoveContent = true;
            this.H_WindowMain.HZoomContent = HalconDotNet.HSmartWindowControl.ZoomContent.WheelForwardZoomsIn;
            this.H_WindowMain.Location = new System.Drawing.Point(0, 0);
            this.H_WindowMain.Margin = new System.Windows.Forms.Padding(0);
            this.H_WindowMain.Name = "H_WindowMain";
            this.H_WindowMain.Size = new System.Drawing.Size(1270, 832);
            this.H_WindowMain.TabIndex = 1;
            this.H_WindowMain.WindowSize = new System.Drawing.Size(1270, 832);
            this.H_WindowMain.Load += new System.EventHandler(this.H_WindowMain_Load);
            // 
            // panel_Left
            // 
            this.panel_Left.Controls.Add(this.panel1);
            this.panel_Left.Controls.Add(this.gBox_Display_Image);
            this.panel_Left.Controls.Add(this.GroupBoxCad);
            this.panel_Left.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel_Left.Location = new System.Drawing.Point(0, 0);
            this.panel_Left.Margin = new System.Windows.Forms.Padding(2);
            this.panel_Left.Name = "panel_Left";
            this.panel_Left.Size = new System.Drawing.Size(355, 832);
            this.panel_Left.TabIndex = 2;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.gBox_Test);
            this.panel1.Controls.Add(this.panel3);
            this.panel1.Controls.Add(this.GroupFindParams);
            this.panel1.Controls.Add(this.GroupModelsParams);
            this.panel1.Controls.Add(this.GroupBoxFML);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 283);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(355, 537);
            this.panel1.TabIndex = 24;
            // 
            // gBox_Test
            // 
            this.gBox_Test.Controls.Add(this.ButImSave);
            this.gBox_Test.Controls.Add(this.EdScope);
            this.gBox_Test.Controls.Add(this.ButCombination);
            this.gBox_Test.Controls.Add(this.label17);
            this.gBox_Test.Dock = System.Windows.Forms.DockStyle.Top;
            this.gBox_Test.Location = new System.Drawing.Point(0, 367);
            this.gBox_Test.Name = "gBox_Test";
            this.gBox_Test.Size = new System.Drawing.Size(355, 104);
            this.gBox_Test.TabIndex = 3;
            this.gBox_Test.TabStop = false;
            this.gBox_Test.Text = "Test";
            // 
            // ButImSave
            // 
            this.ButImSave.Location = new System.Drawing.Point(46, 66);
            this.ButImSave.Name = "ButImSave";
            this.ButImSave.Size = new System.Drawing.Size(124, 23);
            this.ButImSave.TabIndex = 26;
            this.ButImSave.Text = "Save Image";
            this.ButImSave.UseVisualStyleBackColor = true;
            this.ButImSave.Click += new System.EventHandler(this.ButImSave_Click);
            // 
            // EdScope
            // 
            this.EdScope.Location = new System.Drawing.Point(225, 34);
            this.EdScope.Name = "EdScope";
            this.EdScope.Size = new System.Drawing.Size(61, 21);
            this.EdScope.TabIndex = 0;
            // 
            // ButCombination
            // 
            this.ButCombination.Location = new System.Drawing.Point(46, 22);
            this.ButCombination.Name = "ButCombination";
            this.ButCombination.Size = new System.Drawing.Size(124, 43);
            this.ButCombination.TabIndex = 24;
            this.ButCombination.Text = "Alignment Test";
            this.ButCombination.UseVisualStyleBackColor = true;
            this.ButCombination.Click += new System.EventHandler(this.ButCombination_Click);
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(177, 39);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(45, 12);
            this.label17.TabIndex = 1;
            this.label17.Text = "Scope:";
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.ButOpenFM);
            this.panel3.Controls.Add(this.ButSaveFM);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel3.Location = new System.Drawing.Point(0, 478);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(355, 59);
            this.panel3.TabIndex = 23;
            // 
            // ButOpenFM
            // 
            this.ButOpenFM.Location = new System.Drawing.Point(55, 17);
            this.ButOpenFM.Name = "ButOpenFM";
            this.ButOpenFM.Size = new System.Drawing.Size(105, 23);
            this.ButOpenFM.TabIndex = 25;
            this.ButOpenFM.Text = "Open Model";
            this.ButOpenFM.UseVisualStyleBackColor = true;
            this.ButOpenFM.Click += new System.EventHandler(this.ButOpenFM_Click);
            // 
            // ButSaveFM
            // 
            this.ButSaveFM.Location = new System.Drawing.Point(166, 17);
            this.ButSaveFM.Name = "ButSaveFM";
            this.ButSaveFM.Size = new System.Drawing.Size(129, 23);
            this.ButSaveFM.TabIndex = 15;
            this.ButSaveFM.Text = "Create and Save";
            this.ButSaveFM.UseVisualStyleBackColor = true;
            this.ButSaveFM.Click += new System.EventHandler(this.ButSaveFM_Click);
            // 
            // GroupFindParams
            // 
            this.GroupFindParams.Controls.Add(this.label8);
            this.GroupFindParams.Controls.Add(this.label7);
            this.GroupFindParams.Controls.Add(this.cBoxInterpolation);
            this.GroupFindParams.Controls.Add(this.label6);
            this.GroupFindParams.Controls.Add(this.EditMinScore);
            this.GroupFindParams.Controls.Add(this.EditNumLevels1);
            this.GroupFindParams.Controls.Add(this.EditNumLevels2);
            this.GroupFindParams.Dock = System.Windows.Forms.DockStyle.Top;
            this.GroupFindParams.Location = new System.Drawing.Point(0, 253);
            this.GroupFindParams.Name = "GroupFindParams";
            this.GroupFindParams.Size = new System.Drawing.Size(355, 114);
            this.GroupFindParams.TabIndex = 2;
            this.GroupFindParams.TabStop = false;
            this.GroupFindParams.Text = "Find params";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(46, 83);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(77, 12);
            this.label8.TabIndex = 35;
            this.label8.Text = "Interpolation:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(60, 31);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(63, 12);
            this.label7.TabIndex = 29;
            this.label7.Text = "MinScore:";
            // 
            // cBoxInterpolation
            // 
            this.cBoxInterpolation.FormattingEnabled = true;
            this.cBoxInterpolation.Items.AddRange(new object[] {
            "bicubic",
            "bilinear",
            "constant",
            "nearest_neighbor",
            "weighted"});
            this.cBoxInterpolation.Location = new System.Drawing.Point(129, 80);
            this.cBoxInterpolation.Name = "cBoxInterpolation";
            this.cBoxInterpolation.Size = new System.Drawing.Size(156, 20);
            this.cBoxInterpolation.TabIndex = 34;
            this.cBoxInterpolation.SelectedIndexChanged += new System.EventHandler(this.ModelParams_SelectedIndexChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(50, 58);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(73, 12);
            this.label6.TabIndex = 27;
            this.label6.Text = "NumLevels:";
            // 
            // EditMinScore
            // 
            this.EditMinScore.DecimalPlaces = 1;
            this.EditMinScore.Increment = new decimal(new int[] {
            2,
            0,
            0,
            65536});
            this.EditMinScore.Location = new System.Drawing.Point(129, 26);
            this.EditMinScore.Name = "EditMinScore";
            this.EditMinScore.Size = new System.Drawing.Size(75, 21);
            this.EditMinScore.TabIndex = 33;
            this.EditMinScore.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.EditMinScore.Value = new decimal(new int[] {
            5,
            0,
            0,
            65536});
            this.EditMinScore.ValueChanged += new System.EventHandler(this.FindParams_ValueChanged);
            // 
            // EditNumLevels1
            // 
            this.EditNumLevels1.Location = new System.Drawing.Point(129, 53);
            this.EditNumLevels1.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.EditNumLevels1.Name = "EditNumLevels1";
            this.EditNumLevels1.Size = new System.Drawing.Size(75, 21);
            this.EditNumLevels1.TabIndex = 31;
            this.EditNumLevels1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.EditNumLevels1.ValueChanged += new System.EventHandler(this.FindParams_ValueChanged);
            // 
            // EditNumLevels2
            // 
            this.EditNumLevels2.Location = new System.Drawing.Point(210, 53);
            this.EditNumLevels2.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.EditNumLevels2.Name = "EditNumLevels2";
            this.EditNumLevels2.Size = new System.Drawing.Size(75, 21);
            this.EditNumLevels2.TabIndex = 32;
            this.EditNumLevels2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.EditNumLevels2.ValueChanged += new System.EventHandler(this.FindParams_ValueChanged);
            // 
            // GroupModelsParams
            // 
            this.GroupModelsParams.Controls.Add(this.cBoxFMMinContrast);
            this.GroupModelsParams.Controls.Add(this.cBoxFMContrast);
            this.GroupModelsParams.Controls.Add(this.label3);
            this.GroupModelsParams.Controls.Add(this.label5);
            this.GroupModelsParams.Controls.Add(this.label4);
            this.GroupModelsParams.Controls.Add(this.cBoxFMMetric);
            this.GroupModelsParams.Dock = System.Windows.Forms.DockStyle.Top;
            this.GroupModelsParams.Location = new System.Drawing.Point(0, 145);
            this.GroupModelsParams.Name = "GroupModelsParams";
            this.GroupModelsParams.Size = new System.Drawing.Size(355, 108);
            this.GroupModelsParams.TabIndex = 1;
            this.GroupModelsParams.TabStop = false;
            this.GroupModelsParams.Text = "Model params";
            // 
            // cBoxFMMinContrast
            // 
            this.cBoxFMMinContrast.FormattingEnabled = true;
            this.cBoxFMMinContrast.Items.AddRange(new object[] {
            "auto",
            "1",
            "2",
            "3",
            "5",
            "7",
            "10",
            "20",
            "30",
            "40"});
            this.cBoxFMMinContrast.Location = new System.Drawing.Point(130, 51);
            this.cBoxFMMinContrast.Name = "cBoxFMMinContrast";
            this.cBoxFMMinContrast.Size = new System.Drawing.Size(156, 20);
            this.cBoxFMMinContrast.TabIndex = 27;
            this.cBoxFMMinContrast.SelectedIndexChanged += new System.EventHandler(this.ModelParams_SelectedIndexChanged);
            // 
            // cBoxFMContrast
            // 
            this.cBoxFMContrast.FormattingEnabled = true;
            this.cBoxFMContrast.Items.AddRange(new object[] {
            "10",
            "100",
            "120",
            "140",
            "160",
            "20",
            "30",
            "40",
            "60",
            "80",
            "auto",
            "auto_contrast",
            "auto_contrast_hyst",
            "auto_min_size"});
            this.cBoxFMContrast.Location = new System.Drawing.Point(130, 24);
            this.cBoxFMContrast.Name = "cBoxFMContrast";
            this.cBoxFMContrast.Size = new System.Drawing.Size(156, 20);
            this.cBoxFMContrast.TabIndex = 26;
            this.cBoxFMContrast.SelectedIndexChanged += new System.EventHandler(this.ModelParams_SelectedIndexChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(68, 28);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(56, 12);
            this.label3.TabIndex = 22;
            this.label3.Text = "Contrast:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(81, 81);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(44, 12);
            this.label5.TabIndex = 25;
            this.label5.Text = "Metric:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(47, 55);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(77, 12);
            this.label4.TabIndex = 23;
            this.label4.Text = "MinContrast:";
            // 
            // cBoxFMMetric
            // 
            this.cBoxFMMetric.FormattingEnabled = true;
            this.cBoxFMMetric.Items.AddRange(new object[] {
            "ignore_color_polarity",
            "ignore_global_polarity",
            "ignore_local_polarity",
            "use_polarity"});
            this.cBoxFMMetric.Location = new System.Drawing.Point(130, 78);
            this.cBoxFMMetric.Name = "cBoxFMMetric";
            this.cBoxFMMetric.Size = new System.Drawing.Size(156, 20);
            this.cBoxFMMetric.TabIndex = 24;
            this.cBoxFMMetric.SelectedIndexChanged += new System.EventHandler(this.ModelParams_SelectedIndexChanged);
            // 
            // GroupBoxFML
            // 
            this.GroupBoxFML.Controls.Add(this.butUpDateFM_RNP);
            this.GroupBoxFML.Controls.Add(this.butUpdateFM);
            this.GroupBoxFML.Controls.Add(this.label13);
            this.GroupBoxFML.Controls.Add(this.label12);
            this.GroupBoxFML.Controls.Add(this.Ed_Dy);
            this.GroupBoxFML.Controls.Add(this.Ed_Dx);
            this.GroupBoxFML.Dock = System.Windows.Forms.DockStyle.Top;
            this.GroupBoxFML.Location = new System.Drawing.Point(0, 0);
            this.GroupBoxFML.Name = "GroupBoxFML";
            this.GroupBoxFML.Size = new System.Drawing.Size(355, 145);
            this.GroupBoxFML.TabIndex = 0;
            this.GroupBoxFML.TabStop = false;
            this.GroupBoxFML.Text = "Fiducial Marker";
            // 
            // butUpDateFM_RNP
            // 
            this.butUpDateFM_RNP.Location = new System.Drawing.Point(83, 109);
            this.butUpDateFM_RNP.Name = "butUpDateFM_RNP";
            this.butUpDateFM_RNP.Size = new System.Drawing.Size(203, 21);
            this.butUpDateFM_RNP.TabIndex = 2;
            this.butUpDateFM_RNP.Text = "Update from RegProc Rectangle";
            this.butUpDateFM_RNP.UseVisualStyleBackColor = true;
            this.butUpDateFM_RNP.Click += new System.EventHandler(this.butUpDateFM_RNP_Click);
            // 
            // butUpdateFM
            // 
            this.butUpdateFM.Location = new System.Drawing.Point(82, 82);
            this.butUpdateFM.Name = "butUpdateFM";
            this.butUpdateFM.Size = new System.Drawing.Size(203, 21);
            this.butUpdateFM.TabIndex = 2;
            this.butUpdateFM.Text = "Update from Gi Domain";
            this.butUpdateFM.UseVisualStyleBackColor = true;
            this.butUpdateFM.Click += new System.EventHandler(this.butUpdateFM_Click);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(86, 51);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(56, 12);
            this.label13.TabIndex = 1;
            this.label13.Text = "Y (Row):";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(92, 24);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(50, 12);
            this.label12.TabIndex = 1;
            this.label12.Text = "X (Col):";
            // 
            // Ed_Dy
            // 
            this.Ed_Dy.Location = new System.Drawing.Point(143, 47);
            this.Ed_Dy.Name = "Ed_Dy";
            this.Ed_Dy.Size = new System.Drawing.Size(100, 21);
            this.Ed_Dy.TabIndex = 0;
            // 
            // Ed_Dx
            // 
            this.Ed_Dx.Location = new System.Drawing.Point(143, 20);
            this.Ed_Dx.Name = "Ed_Dx";
            this.Ed_Dx.Size = new System.Drawing.Size(100, 21);
            this.Ed_Dx.TabIndex = 0;
            // 
            // gBox_Display_Image
            // 
            this.gBox_Display_Image.Controls.Add(this.But_Disp_ALIG);
            this.gBox_Display_Image.Controls.Add(this.But_Disp_RAW);
            this.gBox_Display_Image.Controls.Add(this.But_Disp_CAD);
            this.gBox_Display_Image.Controls.Add(this.chBox_ROI_Find);
            this.gBox_Display_Image.Controls.Add(this.chBox_ROI_FM);
            this.gBox_Display_Image.Controls.Add(this.chBox_Fit);
            this.gBox_Display_Image.Controls.Add(this.chBox_RegProc_Rect);
            this.gBox_Display_Image.Controls.Add(this.chBox_GiReg);
            this.gBox_Display_Image.Dock = System.Windows.Forms.DockStyle.Top;
            this.gBox_Display_Image.Location = new System.Drawing.Point(0, 134);
            this.gBox_Display_Image.Margin = new System.Windows.Forms.Padding(30);
            this.gBox_Display_Image.Name = "gBox_Display_Image";
            this.gBox_Display_Image.Size = new System.Drawing.Size(355, 149);
            this.gBox_Display_Image.TabIndex = 22;
            this.gBox_Display_Image.TabStop = false;
            this.gBox_Display_Image.Text = "Display Image";
            // 
            // But_Disp_ALIG
            // 
            this.But_Disp_ALIG.AutoSize = true;
            this.But_Disp_ALIG.Location = new System.Drawing.Point(54, 73);
            this.But_Disp_ALIG.Name = "But_Disp_ALIG";
            this.But_Disp_ALIG.Size = new System.Drawing.Size(79, 16);
            this.But_Disp_ALIG.TabIndex = 0;
            this.But_Disp_ALIG.TabStop = true;
            this.But_Disp_ALIG.Text = "Alignment";
            this.But_Disp_ALIG.UseVisualStyleBackColor = true;
            this.But_Disp_ALIG.CheckedChanged += new System.EventHandler(this.But_Disp_ALIG_CheckedChanged);
            // 
            // But_Disp_RAW
            // 
            this.But_Disp_RAW.AutoSize = true;
            this.But_Disp_RAW.Location = new System.Drawing.Point(54, 51);
            this.But_Disp_RAW.Name = "But_Disp_RAW";
            this.But_Disp_RAW.Size = new System.Drawing.Size(49, 16);
            this.But_Disp_RAW.TabIndex = 0;
            this.But_Disp_RAW.TabStop = true;
            this.But_Disp_RAW.Text = "RAW";
            this.But_Disp_RAW.UseVisualStyleBackColor = true;
            this.But_Disp_RAW.CheckedChanged += new System.EventHandler(this.But_Disp_RAW_CheckedChanged);
            // 
            // But_Disp_CAD
            // 
            this.But_Disp_CAD.AutoSize = true;
            this.But_Disp_CAD.Location = new System.Drawing.Point(54, 29);
            this.But_Disp_CAD.Name = "But_Disp_CAD";
            this.But_Disp_CAD.Size = new System.Drawing.Size(48, 16);
            this.But_Disp_CAD.TabIndex = 0;
            this.But_Disp_CAD.TabStop = true;
            this.But_Disp_CAD.Text = "CAD";
            this.But_Disp_CAD.UseVisualStyleBackColor = true;
            this.But_Disp_CAD.CheckedChanged += new System.EventHandler(this.But_Disp_CAD_CheckedChanged);
            // 
            // chBox_ROI_Find
            // 
            this.chBox_ROI_Find.AutoSize = true;
            this.chBox_ROI_Find.Location = new System.Drawing.Point(179, 96);
            this.chBox_ROI_Find.Name = "chBox_ROI_Find";
            this.chBox_ROI_Find.Size = new System.Drawing.Size(120, 16);
            this.chBox_ROI_Find.TabIndex = 11;
            this.chBox_ROI_Find.Text = "Display ROI_Find";
            this.chBox_ROI_Find.UseVisualStyleBackColor = true;
            this.chBox_ROI_Find.CheckedChanged += new System.EventHandler(this.chBox_GiReg_CheckedChanged);
            // 
            // chBox_ROI_FM
            // 
            this.chBox_ROI_FM.AutoSize = true;
            this.chBox_ROI_FM.Location = new System.Drawing.Point(179, 74);
            this.chBox_ROI_FM.Name = "chBox_ROI_FM";
            this.chBox_ROI_FM.Size = new System.Drawing.Size(114, 16);
            this.chBox_ROI_FM.TabIndex = 11;
            this.chBox_ROI_FM.Text = "Display ROI_FM";
            this.chBox_ROI_FM.UseVisualStyleBackColor = true;
            this.chBox_ROI_FM.CheckedChanged += new System.EventHandler(this.chBox_GiReg_CheckedChanged);
            // 
            // chBox_Fit
            // 
            this.chBox_Fit.AutoSize = true;
            this.chBox_Fit.Location = new System.Drawing.Point(179, 118);
            this.chBox_Fit.Name = "chBox_Fit";
            this.chBox_Fit.Size = new System.Drawing.Size(76, 16);
            this.chBox_Fit.TabIndex = 11;
            this.chBox_Fit.Text = "Fit Image";
            this.chBox_Fit.UseVisualStyleBackColor = true;
            this.chBox_Fit.CheckedChanged += new System.EventHandler(this.chBox_GiReg_CheckedChanged);
            // 
            // chBox_RegProc_Rect
            // 
            this.chBox_RegProc_Rect.AutoSize = true;
            this.chBox_RegProc_Rect.Location = new System.Drawing.Point(179, 51);
            this.chBox_RegProc_Rect.Name = "chBox_RegProc_Rect";
            this.chBox_RegProc_Rect.Size = new System.Drawing.Size(147, 16);
            this.chBox_RegProc_Rect.TabIndex = 11;
            this.chBox_RegProc_Rect.Text = "Display RegProc Rect";
            this.chBox_RegProc_Rect.UseVisualStyleBackColor = true;
            this.chBox_RegProc_Rect.CheckedChanged += new System.EventHandler(this.chBox_GiReg_CheckedChanged);
            // 
            // chBox_GiReg
            // 
            this.chBox_GiReg.AutoSize = true;
            this.chBox_GiReg.Location = new System.Drawing.Point(179, 29);
            this.chBox_GiReg.Name = "chBox_GiReg";
            this.chBox_GiReg.Size = new System.Drawing.Size(121, 16);
            this.chBox_GiReg.TabIndex = 11;
            this.chBox_GiReg.Text = "Display Gi region";
            this.chBox_GiReg.UseVisualStyleBackColor = true;
            this.chBox_GiReg.CheckedChanged += new System.EventHandler(this.chBox_GiReg_CheckedChanged);
            // 
            // GroupBoxCad
            // 
            this.GroupBoxCad.Controls.Add(this.ButImageOpen);
            this.GroupBoxCad.Controls.Add(this.LabelImageSize);
            this.GroupBoxCad.Controls.Add(this.LabelCadSize);
            this.GroupBoxCad.Controls.Add(this.label2);
            this.GroupBoxCad.Controls.Add(this.label1);
            this.GroupBoxCad.Controls.Add(this.TextImageFile);
            this.GroupBoxCad.Controls.Add(this.ButCadOpen);
            this.GroupBoxCad.Controls.Add(this.TextCadFile);
            this.GroupBoxCad.Dock = System.Windows.Forms.DockStyle.Top;
            this.GroupBoxCad.Location = new System.Drawing.Point(0, 0);
            this.GroupBoxCad.Name = "GroupBoxCad";
            this.GroupBoxCad.Size = new System.Drawing.Size(355, 134);
            this.GroupBoxCad.TabIndex = 20;
            this.GroupBoxCad.TabStop = false;
            this.GroupBoxCad.Text = "Images";
            // 
            // OpenFileDialog
            // 
            this.OpenFileDialog.Filter = "Image files (*.jpg)|*.jpg|Image files (*.tif)|*.tif|All files (*.*)|*.*";
            this.OpenFileDialog.FilterIndex = 3;
            this.OpenFileDialog.RestoreDirectory = true;
            // 
            // ButImageOpen
            // 
            this.ButImageOpen.Image = ((System.Drawing.Image)(resources.GetObject("ButImageOpen.Image")));
            this.ButImageOpen.Location = new System.Drawing.Point(314, 78);
            this.ButImageOpen.Name = "ButImageOpen";
            this.ButImageOpen.Size = new System.Drawing.Size(27, 27);
            this.ButImageOpen.TabIndex = 20;
            this.ButImageOpen.UseVisualStyleBackColor = true;
            this.ButImageOpen.Click += new System.EventHandler(this.ButOpen_Click);
            // 
            // LabelImageSize
            // 
            this.LabelImageSize.AutoSize = true;
            this.LabelImageSize.Font = new System.Drawing.Font("Gulim", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LabelImageSize.Location = new System.Drawing.Point(141, 104);
            this.LabelImageSize.Name = "LabelImageSize";
            this.LabelImageSize.Size = new System.Drawing.Size(85, 11);
            this.LabelImageSize.TabIndex = 19;
            this.LabelImageSize.Text = "123034 x 15432";
            // 
            // TextImageFile
            // 
            this.TextImageFile.Location = new System.Drawing.Point(85, 80);
            this.TextImageFile.Name = "TextImageFile";
            this.TextImageFile.ReadOnly = true;
            this.TextImageFile.Size = new System.Drawing.Size(223, 21);
            this.TextImageFile.TabIndex = 15;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(11, 85);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(75, 12);
            this.label2.TabIndex = 17;
            this.label2.Text = "Cam Image:";
            // 
            // ButCadOpen
            // 
            this.ButCadOpen.Image = ((System.Drawing.Image)(resources.GetObject("ButCadOpen.Image")));
            this.ButCadOpen.Location = new System.Drawing.Point(314, 27);
            this.ButCadOpen.Name = "ButCadOpen";
            this.ButCadOpen.Size = new System.Drawing.Size(27, 27);
            this.ButCadOpen.TabIndex = 12;
            this.ButCadOpen.UseVisualStyleBackColor = true;
            this.ButCadOpen.Click += new System.EventHandler(this.ButOpen_Click);
            // 
            // LabelCadSize
            // 
            this.LabelCadSize.AutoSize = true;
            this.LabelCadSize.Font = new System.Drawing.Font("Gulim", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LabelCadSize.Location = new System.Drawing.Point(141, 54);
            this.LabelCadSize.Name = "LabelCadSize";
            this.LabelCadSize.Size = new System.Drawing.Size(85, 11);
            this.LabelCadSize.TabIndex = 18;
            this.LabelCadSize.Text = "123034 x 15432";
            // 
            // TextCadFile
            // 
            this.TextCadFile.Location = new System.Drawing.Point(85, 30);
            this.TextCadFile.Name = "TextCadFile";
            this.TextCadFile.ReadOnly = true;
            this.TextCadFile.Size = new System.Drawing.Size(223, 21);
            this.TextCadFile.TabIndex = 13;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(53, 35);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(34, 12);
            this.label1.TabIndex = 16;
            this.label1.Text = "CAD:";
            // 
            // AlignmentL
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1270, 832);
            this.Controls.Add(this.panel_Left);
            this.Controls.Add(this.H_WindowMain);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MinimizeBox = false;
            this.Name = "AlignmentL";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Alignment Lite";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Alignment_FormClosing);
            this.Shown += new System.EventHandler(this.Alignment_Shown);
            this.panel_Left.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.gBox_Test.ResumeLayout(false);
            this.gBox_Test.PerformLayout();
            this.panel3.ResumeLayout(false);
            this.GroupFindParams.ResumeLayout(false);
            this.GroupFindParams.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.EditMinScore)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.EditNumLevels1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.EditNumLevels2)).EndInit();
            this.GroupModelsParams.ResumeLayout(false);
            this.GroupModelsParams.PerformLayout();
            this.GroupBoxFML.ResumeLayout(false);
            this.GroupBoxFML.PerformLayout();
            this.gBox_Display_Image.ResumeLayout(false);
            this.gBox_Display_Image.PerformLayout();
            this.GroupBoxCad.ResumeLayout(false);
            this.GroupBoxCad.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private HalconDotNet.HSmartWindowControl H_WindowMain;
        private System.Windows.Forms.Panel panel_Left;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Button ButImSave;
        private System.Windows.Forms.Button ButOpenFM;
        private System.Windows.Forms.Button ButCombination;
        private System.Windows.Forms.ComboBox cBoxFMMinContrast;
        private System.Windows.Forms.ComboBox cBoxFMContrast;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox cBoxFMMetric;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox cBoxInterpolation;
        private System.Windows.Forms.NumericUpDown EditMinScore;
        private System.Windows.Forms.NumericUpDown EditNumLevels2;
        private System.Windows.Forms.NumericUpDown EditNumLevels1;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button ButSaveFM;
        private System.Windows.Forms.GroupBox GroupBoxCad;
        private System.Windows.Forms.OpenFileDialog OpenFileDialog;
        private System.Windows.Forms.SaveFileDialog SaveFileDialog;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.GroupBox gBox_Test;
        private System.Windows.Forms.TextBox EdScope;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.GroupBox GroupFindParams;
        private System.Windows.Forms.GroupBox GroupModelsParams;
        private System.Windows.Forms.GroupBox GroupBoxFML;
        private System.Windows.Forms.Button butUpdateFM;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox Ed_Dy;
        private System.Windows.Forms.TextBox Ed_Dx;
        private System.Windows.Forms.CheckBox chBox_GiReg;
        private System.Windows.Forms.CheckBox chBox_Fit;
        private System.Windows.Forms.GroupBox gBox_Display_Image;
        private System.Windows.Forms.RadioButton But_Disp_ALIG;
        private System.Windows.Forms.RadioButton But_Disp_RAW;
        private System.Windows.Forms.RadioButton But_Disp_CAD;
        private System.Windows.Forms.CheckBox chBox_ROI_FM;
        private System.Windows.Forms.CheckBox chBox_ROI_Find;
        private System.Windows.Forms.CheckBox chBox_RegProc_Rect;
        private System.Windows.Forms.Button butUpDateFM_RNP;
        private System.Windows.Forms.Button ButImageOpen;
        private System.Windows.Forms.Label LabelImageSize;
        private System.Windows.Forms.Label LabelCadSize;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox TextImageFile;
        private System.Windows.Forms.Button ButCadOpen;
        private System.Windows.Forms.TextBox TextCadFile;
    }
}