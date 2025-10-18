namespace TeachingExecutor
{
    partial class Alignment
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
            System.Windows.Forms.ListViewItem listViewItem6 = new System.Windows.Forms.ListViewItem(new string[] {
            "Center",
            "15000",
            "20000",
            "90.5"}, -1);
            System.Windows.Forms.ListViewItem listViewItem7 = new System.Windows.Forms.ListViewItem(new string[] {
            "Top Left",
            "10000",
            "10000",
            "100.0"}, -1);
            System.Windows.Forms.ListViewItem listViewItem8 = new System.Windows.Forms.ListViewItem(new string[] {
            "Top Right",
            "0",
            "0",
            "0.0"}, -1);
            System.Windows.Forms.ListViewItem listViewItem9 = new System.Windows.Forms.ListViewItem(new string[] {
            "Bottom Left",
            "0",
            "0",
            "0.0"}, -1);
            System.Windows.Forms.ListViewItem listViewItem10 = new System.Windows.Forms.ListViewItem(new string[] {
            "Bottom Right",
            "0",
            "0",
            "0.0"}, -1);
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Alignment));
            this.H_WindowMain = new HalconDotNet.HSmartWindowControl();
            this.panel_Left = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.groupBox_Adjust = new System.Windows.Forms.GroupBox();
            this.ed_Adj_Dilation = new System.Windows.Forms.NumericUpDown();
            this.label10 = new System.Windows.Forms.Label();
            this.ed_Adj_Erosion = new System.Windows.Forms.NumericUpDown();
            this.label9 = new System.Windows.Forms.Label();
            this.chBox_CAD_Adjust = new System.Windows.Forms.CheckBox();
            this.ButImSave = new System.Windows.Forms.Button();
            this.ButOpenFM = new System.Windows.Forms.Button();
            this.ButCombination = new System.Windows.Forms.Button();
            this.TabModelParams = new System.Windows.Forms.TabControl();
            this.TabPageModelsParams = new System.Windows.Forms.TabPage();
            this.cBoxFMMinContrast = new System.Windows.Forms.ComboBox();
            this.cBoxFMContrast = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.cBoxFMMetric = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.TabPageFindParams = new System.Windows.Forms.TabPage();
            this.label8 = new System.Windows.Forms.Label();
            this.cBoxInterpolation = new System.Windows.Forms.ComboBox();
            this.EditMinScore = new System.Windows.Forms.NumericUpDown();
            this.EditNumLevels2 = new System.Windows.Forms.NumericUpDown();
            this.EditNumLevels1 = new System.Windows.Forms.NumericUpDown();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.ButSaveFM = new System.Windows.Forms.Button();
            this.ButAlignment = new System.Windows.Forms.Button();
            this.GroupBoxFM = new System.Windows.Forms.GroupBox();
            this.ListViewFM = new System.Windows.Forms.ListView();
            this.columnHeader7 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader8 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader9 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader10 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.ToolStrip = new System.Windows.Forms.ToolStrip();
            this.ButAddROI = new System.Windows.Forms.ToolStripButton();
            this.ButCadReg = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.CBoxImageType = new System.Windows.Forms.ToolStripComboBox();
            this.CBoxFitType = new System.Windows.Forms.ToolStripComboBox();
            this.GroupBoxCad = new System.Windows.Forms.GroupBox();
            this.ButImageOpen = new System.Windows.Forms.Button();
            this.LabelImageSize = new System.Windows.Forms.Label();
            this.LabelCadSize = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.TextImageFile = new System.Windows.Forms.TextBox();
            this.ButCadOpen = new System.Windows.Forms.Button();
            this.TextCadFile = new System.Windows.Forms.TextBox();
            this.OpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.SaveFileDialog = new System.Windows.Forms.SaveFileDialog();
            this.label11 = new System.Windows.Forms.Label();
            this.ed_Adj_Bias = new System.Windows.Forms.NumericUpDown();
            this.panel_Left.SuspendLayout();
            this.panel3.SuspendLayout();
            this.groupBox_Adjust.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ed_Adj_Dilation)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ed_Adj_Erosion)).BeginInit();
            this.TabModelParams.SuspendLayout();
            this.TabPageModelsParams.SuspendLayout();
            this.TabPageFindParams.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.EditMinScore)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.EditNumLevels2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.EditNumLevels1)).BeginInit();
            this.GroupBoxFM.SuspendLayout();
            this.ToolStrip.SuspendLayout();
            this.GroupBoxCad.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ed_Adj_Bias)).BeginInit();
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
            this.H_WindowMain.Size = new System.Drawing.Size(1270, 909);
            this.H_WindowMain.TabIndex = 1;
            this.H_WindowMain.WindowSize = new System.Drawing.Size(1270, 909);
            this.H_WindowMain.Load += new System.EventHandler(this.H_WindowMain_Load);
            // 
            // panel_Left
            // 
            this.panel_Left.Controls.Add(this.panel3);
            this.panel_Left.Controls.Add(this.GroupBoxFM);
            this.panel_Left.Controls.Add(this.GroupBoxCad);
            this.panel_Left.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel_Left.Location = new System.Drawing.Point(0, 0);
            this.panel_Left.Margin = new System.Windows.Forms.Padding(2);
            this.panel_Left.Name = "panel_Left";
            this.panel_Left.Size = new System.Drawing.Size(355, 909);
            this.panel_Left.TabIndex = 2;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.groupBox_Adjust);
            this.panel3.Controls.Add(this.ButImSave);
            this.panel3.Controls.Add(this.ButOpenFM);
            this.panel3.Controls.Add(this.ButCombination);
            this.panel3.Controls.Add(this.TabModelParams);
            this.panel3.Controls.Add(this.ButSaveFM);
            this.panel3.Controls.Add(this.ButAlignment);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel3.Location = new System.Drawing.Point(0, 300);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(355, 450);
            this.panel3.TabIndex = 23;
            // 
            // groupBox_Adjust
            // 
            this.groupBox_Adjust.Controls.Add(this.ed_Adj_Bias);
            this.groupBox_Adjust.Controls.Add(this.label11);
            this.groupBox_Adjust.Controls.Add(this.ed_Adj_Dilation);
            this.groupBox_Adjust.Controls.Add(this.label10);
            this.groupBox_Adjust.Controls.Add(this.ed_Adj_Erosion);
            this.groupBox_Adjust.Controls.Add(this.label9);
            this.groupBox_Adjust.Controls.Add(this.chBox_CAD_Adjust);
            this.groupBox_Adjust.Location = new System.Drawing.Point(4, 154);
            this.groupBox_Adjust.Name = "groupBox_Adjust";
            this.groupBox_Adjust.Size = new System.Drawing.Size(347, 111);
            this.groupBox_Adjust.TabIndex = 28;
            this.groupBox_Adjust.TabStop = false;
            // 
            // ed_Adj_Dilation
            // 
            this.ed_Adj_Dilation.Location = new System.Drawing.Point(123, 49);
            this.ed_Adj_Dilation.Maximum = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.ed_Adj_Dilation.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.ed_Adj_Dilation.Name = "ed_Adj_Dilation";
            this.ed_Adj_Dilation.Size = new System.Drawing.Size(75, 21);
            this.ed_Adj_Dilation.TabIndex = 35;
            this.ed_Adj_Dilation.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.ed_Adj_Dilation.Value = new decimal(new int[] {
            15,
            0,
            0,
            0});
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(28, 53);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(89, 12);
            this.label10.TabIndex = 34;
            this.label10.Text = "Adjust Dilation:";
            // 
            // ed_Adj_Erosion
            // 
            this.ed_Adj_Erosion.Location = new System.Drawing.Point(123, 22);
            this.ed_Adj_Erosion.Maximum = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.ed_Adj_Erosion.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.ed_Adj_Erosion.Name = "ed_Adj_Erosion";
            this.ed_Adj_Erosion.Size = new System.Drawing.Size(75, 21);
            this.ed_Adj_Erosion.TabIndex = 35;
            this.ed_Adj_Erosion.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.ed_Adj_Erosion.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(26, 25);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(91, 12);
            this.label9.TabIndex = 34;
            this.label9.Text = "Adjust Erosion:";
            // 
            // chBox_CAD_Adjust
            // 
            this.chBox_CAD_Adjust.AutoSize = true;
            this.chBox_CAD_Adjust.Location = new System.Drawing.Point(9, 0);
            this.chBox_CAD_Adjust.Name = "chBox_CAD_Adjust";
            this.chBox_CAD_Adjust.Size = new System.Drawing.Size(214, 16);
            this.chBox_CAD_Adjust.TabIndex = 27;
            this.chBox_CAD_Adjust.Text = "Adjust the dimensions of the CAD";
            this.chBox_CAD_Adjust.UseVisualStyleBackColor = true;
            this.chBox_CAD_Adjust.CheckedChanged += new System.EventHandler(this.chBox_CAD_Adjust_CheckedChanged);
            // 
            // ButImSave
            // 
            this.ButImSave.Location = new System.Drawing.Point(236, 376);
            this.ButImSave.Name = "ButImSave";
            this.ButImSave.Size = new System.Drawing.Size(105, 23);
            this.ButImSave.TabIndex = 26;
            this.ButImSave.Text = "Save Image";
            this.ButImSave.UseVisualStyleBackColor = true;
            this.ButImSave.Click += new System.EventHandler(this.ButImSave_Click);
            // 
            // ButOpenFM
            // 
            this.ButOpenFM.Location = new System.Drawing.Point(13, 376);
            this.ButOpenFM.Name = "ButOpenFM";
            this.ButOpenFM.Size = new System.Drawing.Size(105, 23);
            this.ButOpenFM.TabIndex = 25;
            this.ButOpenFM.Text = "Open";
            this.ButOpenFM.UseVisualStyleBackColor = true;
            this.ButOpenFM.Click += new System.EventHandler(this.ButOpenFM_Click);
            // 
            // ButCombination
            // 
            this.ButCombination.Location = new System.Drawing.Point(180, 320);
            this.ButCombination.Name = "ButCombination";
            this.ButCombination.Size = new System.Drawing.Size(161, 43);
            this.ButCombination.TabIndex = 24;
            this.ButCombination.Text = "Combination";
            this.ButCombination.UseVisualStyleBackColor = true;
            this.ButCombination.Click += new System.EventHandler(this.ButCombination_Click);
            // 
            // TabModelParams
            // 
            this.TabModelParams.Controls.Add(this.TabPageModelsParams);
            this.TabModelParams.Controls.Add(this.TabPageFindParams);
            this.TabModelParams.Dock = System.Windows.Forms.DockStyle.Top;
            this.TabModelParams.Location = new System.Drawing.Point(0, 0);
            this.TabModelParams.Name = "TabModelParams";
            this.TabModelParams.SelectedIndex = 0;
            this.TabModelParams.Size = new System.Drawing.Size(355, 124);
            this.TabModelParams.TabIndex = 22;
            // 
            // TabPageModelsParams
            // 
            this.TabPageModelsParams.Controls.Add(this.cBoxFMMinContrast);
            this.TabPageModelsParams.Controls.Add(this.cBoxFMContrast);
            this.TabPageModelsParams.Controls.Add(this.label5);
            this.TabPageModelsParams.Controls.Add(this.cBoxFMMetric);
            this.TabPageModelsParams.Controls.Add(this.label4);
            this.TabPageModelsParams.Controls.Add(this.label3);
            this.TabPageModelsParams.Location = new System.Drawing.Point(4, 22);
            this.TabPageModelsParams.Name = "TabPageModelsParams";
            this.TabPageModelsParams.Padding = new System.Windows.Forms.Padding(3);
            this.TabPageModelsParams.Size = new System.Drawing.Size(347, 98);
            this.TabPageModelsParams.TabIndex = 0;
            this.TabPageModelsParams.Text = "Model params";
            this.TabPageModelsParams.UseVisualStyleBackColor = true;
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
            this.cBoxFMMinContrast.Location = new System.Drawing.Point(123, 38);
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
            this.cBoxFMContrast.Location = new System.Drawing.Point(123, 11);
            this.cBoxFMContrast.Name = "cBoxFMContrast";
            this.cBoxFMContrast.Size = new System.Drawing.Size(156, 20);
            this.cBoxFMContrast.TabIndex = 26;
            this.cBoxFMContrast.SelectedIndexChanged += new System.EventHandler(this.ModelParams_SelectedIndexChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(74, 68);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(44, 12);
            this.label5.TabIndex = 25;
            this.label5.Text = "Metric:";
            // 
            // cBoxFMMetric
            // 
            this.cBoxFMMetric.FormattingEnabled = true;
            this.cBoxFMMetric.Items.AddRange(new object[] {
            "ignore_color_polarity",
            "ignore_global_polarity",
            "ignore_local_polarity",
            "use_polarity"});
            this.cBoxFMMetric.Location = new System.Drawing.Point(123, 65);
            this.cBoxFMMetric.Name = "cBoxFMMetric";
            this.cBoxFMMetric.Size = new System.Drawing.Size(156, 20);
            this.cBoxFMMetric.TabIndex = 24;
            this.cBoxFMMetric.SelectedIndexChanged += new System.EventHandler(this.ModelParams_SelectedIndexChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(40, 42);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(77, 12);
            this.label4.TabIndex = 23;
            this.label4.Text = "MinContrast:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(61, 15);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(56, 12);
            this.label3.TabIndex = 22;
            this.label3.Text = "Contrast:";
            // 
            // TabPageFindParams
            // 
            this.TabPageFindParams.Controls.Add(this.label8);
            this.TabPageFindParams.Controls.Add(this.cBoxInterpolation);
            this.TabPageFindParams.Controls.Add(this.EditMinScore);
            this.TabPageFindParams.Controls.Add(this.EditNumLevels2);
            this.TabPageFindParams.Controls.Add(this.EditNumLevels1);
            this.TabPageFindParams.Controls.Add(this.label7);
            this.TabPageFindParams.Controls.Add(this.label6);
            this.TabPageFindParams.Location = new System.Drawing.Point(4, 22);
            this.TabPageFindParams.Name = "TabPageFindParams";
            this.TabPageFindParams.Padding = new System.Windows.Forms.Padding(3);
            this.TabPageFindParams.Size = new System.Drawing.Size(347, 98);
            this.TabPageFindParams.TabIndex = 1;
            this.TabPageFindParams.Text = "Find params";
            this.TabPageFindParams.UseVisualStyleBackColor = true;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(40, 67);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(77, 12);
            this.label8.TabIndex = 35;
            this.label8.Text = "Interpolation:";
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
            this.cBoxInterpolation.Location = new System.Drawing.Point(123, 64);
            this.cBoxInterpolation.Name = "cBoxInterpolation";
            this.cBoxInterpolation.Size = new System.Drawing.Size(156, 20);
            this.cBoxInterpolation.TabIndex = 34;
            this.cBoxInterpolation.SelectedIndexChanged += new System.EventHandler(this.ModelParams_SelectedIndexChanged);
            // 
            // EditMinScore
            // 
            this.EditMinScore.DecimalPlaces = 1;
            this.EditMinScore.Increment = new decimal(new int[] {
            2,
            0,
            0,
            65536});
            this.EditMinScore.Location = new System.Drawing.Point(123, 10);
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
            // EditNumLevels2
            // 
            this.EditNumLevels2.Location = new System.Drawing.Point(204, 37);
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
            // EditNumLevels1
            // 
            this.EditNumLevels1.Location = new System.Drawing.Point(123, 37);
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
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(54, 15);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(63, 12);
            this.label7.TabIndex = 29;
            this.label7.Text = "MinScore:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(44, 42);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(73, 12);
            this.label6.TabIndex = 27;
            this.label6.Text = "NumLevels:";
            // 
            // ButSaveFM
            // 
            this.ButSaveFM.Location = new System.Drawing.Point(124, 376);
            this.ButSaveFM.Name = "ButSaveFM";
            this.ButSaveFM.Size = new System.Drawing.Size(105, 23);
            this.ButSaveFM.TabIndex = 15;
            this.ButSaveFM.Text = "Save";
            this.ButSaveFM.UseVisualStyleBackColor = true;
            this.ButSaveFM.Click += new System.EventHandler(this.ButSaveFM_Click);
            // 
            // ButAlignment
            // 
            this.ButAlignment.Location = new System.Drawing.Point(13, 320);
            this.ButAlignment.Name = "ButAlignment";
            this.ButAlignment.Size = new System.Drawing.Size(161, 43);
            this.ButAlignment.TabIndex = 14;
            this.ButAlignment.Text = "Alignment";
            this.ButAlignment.UseVisualStyleBackColor = true;
            this.ButAlignment.Click += new System.EventHandler(this.ButAlignment_Click);
            // 
            // GroupBoxFM
            // 
            this.GroupBoxFM.Controls.Add(this.ListViewFM);
            this.GroupBoxFM.Controls.Add(this.ToolStrip);
            this.GroupBoxFM.Dock = System.Windows.Forms.DockStyle.Top;
            this.GroupBoxFM.Location = new System.Drawing.Point(0, 134);
            this.GroupBoxFM.Margin = new System.Windows.Forms.Padding(30);
            this.GroupBoxFM.Name = "GroupBoxFM";
            this.GroupBoxFM.Size = new System.Drawing.Size(355, 166);
            this.GroupBoxFM.TabIndex = 22;
            this.GroupBoxFM.TabStop = false;
            this.GroupBoxFM.Text = "Fiducial Markers";
            // 
            // ListViewFM
            // 
            this.ListViewFM.Activation = System.Windows.Forms.ItemActivation.OneClick;
            this.ListViewFM.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader7,
            this.columnHeader8,
            this.columnHeader9,
            this.columnHeader10});
            this.ListViewFM.Dock = System.Windows.Forms.DockStyle.Top;
            this.ListViewFM.FullRowSelect = true;
            this.ListViewFM.HideSelection = false;
            this.ListViewFM.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.ListViewFM.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem6,
            listViewItem7,
            listViewItem8,
            listViewItem9,
            listViewItem10});
            this.ListViewFM.Location = new System.Drawing.Point(3, 45);
            this.ListViewFM.MultiSelect = false;
            this.ListViewFM.Name = "ListViewFM";
            this.ListViewFM.Size = new System.Drawing.Size(349, 110);
            this.ListViewFM.TabIndex = 9;
            this.ListViewFM.UseCompatibleStateImageBehavior = false;
            this.ListViewFM.View = System.Windows.Forms.View.Details;
            this.ListViewFM.SelectedIndexChanged += new System.EventHandler(this.ListViewFM_SelectedIndexChanged);
            // 
            // columnHeader7
            // 
            this.columnHeader7.Text = "Name FM";
            this.columnHeader7.Width = 165;
            // 
            // columnHeader8
            // 
            this.columnHeader8.Text = "X (Col)";
            this.columnHeader8.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // columnHeader9
            // 
            this.columnHeader9.Text = "Y (Row)";
            this.columnHeader9.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // columnHeader10
            // 
            this.columnHeader10.Text = "Score";
            this.columnHeader10.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.columnHeader10.Width = 45;
            // 
            // ToolStrip
            // 
            this.ToolStrip.AutoSize = false;
            this.ToolStrip.ImageScalingSize = new System.Drawing.Size(23, 23);
            this.ToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ButAddROI,
            this.ButCadReg,
            this.toolStripSeparator2,
            this.CBoxImageType,
            this.CBoxFitType});
            this.ToolStrip.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.HorizontalStackWithOverflow;
            this.ToolStrip.Location = new System.Drawing.Point(3, 17);
            this.ToolStrip.Name = "ToolStrip";
            this.ToolStrip.Size = new System.Drawing.Size(349, 28);
            this.ToolStrip.TabIndex = 10;
            // 
            // ButAddROI
            // 
            this.ButAddROI.AutoSize = false;
            this.ButAddROI.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.ButAddROI.Image = ((System.Drawing.Image)(resources.GetObject("ButAddROI.Image")));
            this.ButAddROI.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.ButAddROI.Name = "ButAddROI";
            this.ButAddROI.Size = new System.Drawing.Size(25, 25);
            this.ButAddROI.ToolTipText = "Fiducial point regions";
            this.ButAddROI.Click += new System.EventHandler(this.ButAddROI_Click);
            // 
            // ButCadReg
            // 
            this.ButCadReg.AutoSize = false;
            this.ButCadReg.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.ButCadReg.Image = ((System.Drawing.Image)(resources.GetObject("ButCadReg.Image")));
            this.ButCadReg.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.ButCadReg.Name = "ButCadReg";
            this.ButCadReg.Size = new System.Drawing.Size(25, 25);
            this.ButCadReg.ToolTipText = "Display cad regions";
            this.ButCadReg.Click += new System.EventHandler(this.ButCadReg_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 28);
            // 
            // CBoxImageType
            // 
            this.CBoxImageType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.CBoxImageType.Items.AddRange(new object[] {
            "CAD Image",
            "Raw Image",
            "Combination Image",
            "Alignment Image"});
            this.CBoxImageType.Name = "CBoxImageType";
            this.CBoxImageType.Size = new System.Drawing.Size(130, 28);
            this.CBoxImageType.SelectedIndexChanged += new System.EventHandler(this.CBoxImageType_SelectedIndexChanged);
            // 
            // CBoxFitType
            // 
            this.CBoxFitType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.CBoxFitType.Items.AddRange(new object[] {
            "Fit to FM",
            "Not Fit"});
            this.CBoxFitType.Name = "CBoxFitType";
            this.CBoxFitType.Size = new System.Drawing.Size(100, 28);
            this.CBoxFitType.Tag = "";
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
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(11, 85);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(75, 12);
            this.label2.TabIndex = 17;
            this.label2.Text = "Cam Image:";
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
            // TextImageFile
            // 
            this.TextImageFile.Location = new System.Drawing.Point(85, 80);
            this.TextImageFile.Name = "TextImageFile";
            this.TextImageFile.ReadOnly = true;
            this.TextImageFile.Size = new System.Drawing.Size(223, 21);
            this.TextImageFile.TabIndex = 15;
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
            // TextCadFile
            // 
            this.TextCadFile.Location = new System.Drawing.Point(85, 30);
            this.TextCadFile.Name = "TextCadFile";
            this.TextCadFile.ReadOnly = true;
            this.TextCadFile.Size = new System.Drawing.Size(223, 21);
            this.TextCadFile.TabIndex = 13;
            // 
            // OpenFileDialog
            // 
            this.OpenFileDialog.Filter = "Image files (*.jpg)|*.jpg|Image files (*.tif)|*.tif|All files (*.*)|*.*";
            this.OpenFileDialog.FilterIndex = 3;
            this.OpenFileDialog.RestoreDirectory = true;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(83, 80);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(34, 12);
            this.label11.TabIndex = 34;
            this.label11.Text = "Bias:";
            // 
            // ed_Adj_Bias
            // 
            this.ed_Adj_Bias.Location = new System.Drawing.Point(123, 76);
            this.ed_Adj_Bias.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.ed_Adj_Bias.Minimum = new decimal(new int[] {
            5,
            0,
            0,
            -2147483648});
            this.ed_Adj_Bias.Name = "ed_Adj_Bias";
            this.ed_Adj_Bias.Size = new System.Drawing.Size(75, 21);
            this.ed_Adj_Bias.TabIndex = 35;
            this.ed_Adj_Bias.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.ed_Adj_Bias.Value = new decimal(new int[] {
            1,
            0,
            0,
            -2147483648});
            // 
            // Alignment
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1270, 909);
            this.Controls.Add(this.panel_Left);
            this.Controls.Add(this.H_WindowMain);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MinimizeBox = false;
            this.Name = "Alignment";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Alignment";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Alignment_FormClosing);
            this.Shown += new System.EventHandler(this.Alignment_Shown);
            this.panel_Left.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.groupBox_Adjust.ResumeLayout(false);
            this.groupBox_Adjust.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ed_Adj_Dilation)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ed_Adj_Erosion)).EndInit();
            this.TabModelParams.ResumeLayout(false);
            this.TabPageModelsParams.ResumeLayout(false);
            this.TabPageModelsParams.PerformLayout();
            this.TabPageFindParams.ResumeLayout(false);
            this.TabPageFindParams.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.EditMinScore)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.EditNumLevels2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.EditNumLevels1)).EndInit();
            this.GroupBoxFM.ResumeLayout(false);
            this.ToolStrip.ResumeLayout(false);
            this.ToolStrip.PerformLayout();
            this.GroupBoxCad.ResumeLayout(false);
            this.GroupBoxCad.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ed_Adj_Bias)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private HalconDotNet.HSmartWindowControl H_WindowMain;
        private System.Windows.Forms.Panel panel_Left;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Button ButImSave;
        private System.Windows.Forms.Button ButOpenFM;
        private System.Windows.Forms.Button ButCombination;
        private System.Windows.Forms.TabControl TabModelParams;
        private System.Windows.Forms.TabPage TabPageModelsParams;
        private System.Windows.Forms.ComboBox cBoxFMMinContrast;
        private System.Windows.Forms.ComboBox cBoxFMContrast;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox cBoxFMMetric;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TabPage TabPageFindParams;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox cBoxInterpolation;
        private System.Windows.Forms.NumericUpDown EditMinScore;
        private System.Windows.Forms.NumericUpDown EditNumLevels2;
        private System.Windows.Forms.NumericUpDown EditNumLevels1;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button ButSaveFM;
        private System.Windows.Forms.Button ButAlignment;
        private System.Windows.Forms.GroupBox GroupBoxFM;
        private System.Windows.Forms.ListView ListViewFM;
        private System.Windows.Forms.ColumnHeader columnHeader7;
        private System.Windows.Forms.ColumnHeader columnHeader8;
        private System.Windows.Forms.ColumnHeader columnHeader9;
        private System.Windows.Forms.ColumnHeader columnHeader10;
        private System.Windows.Forms.ToolStrip ToolStrip;
        private System.Windows.Forms.ToolStripButton ButAddROI;
        private System.Windows.Forms.ToolStripButton ButCadReg;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripComboBox CBoxImageType;
        private System.Windows.Forms.ToolStripComboBox CBoxFitType;
        private System.Windows.Forms.GroupBox GroupBoxCad;
        private System.Windows.Forms.Button ButImageOpen;
        private System.Windows.Forms.Label LabelImageSize;
        private System.Windows.Forms.Label LabelCadSize;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox TextImageFile;
        private System.Windows.Forms.Button ButCadOpen;
        private System.Windows.Forms.TextBox TextCadFile;
        private System.Windows.Forms.OpenFileDialog OpenFileDialog;
        private System.Windows.Forms.SaveFileDialog SaveFileDialog;
        private System.Windows.Forms.CheckBox chBox_CAD_Adjust;
        private System.Windows.Forms.GroupBox groupBox_Adjust;
        private System.Windows.Forms.NumericUpDown ed_Adj_Erosion;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.NumericUpDown ed_Adj_Dilation;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.NumericUpDown ed_Adj_Bias;
        private System.Windows.Forms.Label label11;
    }
}