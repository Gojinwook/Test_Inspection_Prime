namespace TeachingExecutor
{
    partial class Aouto_RegNoProc
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
            this.H_WindowMain = new HalconDotNet.HSmartWindowControl();
            this.panel_Left = new System.Windows.Forms.Panel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.but_Save = new System.Windows.Forms.Button();
            this.but_Cancel = new System.Windows.Forms.Button();
            this.but_Ok = new System.Windows.Forms.Button();
            this.GroupBoxCad = new System.Windows.Forms.GroupBox();
            this.checkBox_Fill = new System.Windows.Forms.CheckBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.track_Erosion_Coil = new System.Windows.Forms.TrackBar();
            this.track_Dilation_Coil = new System.Windows.Forms.TrackBar();
            this.track_Frame_Width = new System.Windows.Forms.TrackBar();
            this.OpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.SaveFileDialog = new System.Windows.Forms.SaveFileDialog();
            this.trackBar_Side = new System.Windows.Forms.TrackBar();
            this.label3 = new System.Windows.Forms.Label();
            this.trackBar_Central = new System.Windows.Forms.TrackBar();
            this.label5 = new System.Windows.Forms.Label();
            this.comboBox_Type_Unit = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.panel_Left.SuspendLayout();
            this.panel1.SuspendLayout();
            this.panel3.SuspendLayout();
            this.GroupBoxCad.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.track_Erosion_Coil)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.track_Dilation_Coil)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.track_Frame_Width)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_Side)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_Central)).BeginInit();
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
            this.panel1.Controls.Add(this.panel3);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 414);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(355, 74);
            this.panel1.TabIndex = 24;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.but_Save);
            this.panel3.Controls.Add(this.but_Cancel);
            this.panel3.Controls.Add(this.but_Ok);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel3.Location = new System.Drawing.Point(0, 15);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(355, 59);
            this.panel3.TabIndex = 23;
            // 
            // but_Save
            // 
            this.but_Save.Location = new System.Drawing.Point(21, 18);
            this.but_Save.Name = "but_Save";
            this.but_Save.Size = new System.Drawing.Size(114, 23);
            this.but_Save.TabIndex = 7;
            this.but_Save.Text = "Save RegNoProc";
            this.but_Save.UseVisualStyleBackColor = true;
            this.but_Save.Click += new System.EventHandler(this.but_Save_Click);
            // 
            // but_Cancel
            // 
            this.but_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.but_Cancel.Location = new System.Drawing.Point(151, 18);
            this.but_Cancel.Name = "but_Cancel";
            this.but_Cancel.Size = new System.Drawing.Size(75, 23);
            this.but_Cancel.TabIndex = 9;
            this.but_Cancel.Text = "Cancel";
            this.but_Cancel.UseVisualStyleBackColor = true;
            // 
            // but_Ok
            // 
            this.but_Ok.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.but_Ok.Location = new System.Drawing.Point(232, 18);
            this.but_Ok.Name = "but_Ok";
            this.but_Ok.Size = new System.Drawing.Size(91, 23);
            this.but_Ok.TabIndex = 8;
            this.but_Ok.Text = "Ok";
            this.but_Ok.UseVisualStyleBackColor = true;
            // 
            // GroupBoxCad
            // 
            this.GroupBoxCad.Controls.Add(this.comboBox_Type_Unit);
            this.GroupBoxCad.Controls.Add(this.checkBox_Fill);
            this.GroupBoxCad.Controls.Add(this.label4);
            this.GroupBoxCad.Controls.Add(this.label2);
            this.GroupBoxCad.Controls.Add(this.label5);
            this.GroupBoxCad.Controls.Add(this.label3);
            this.GroupBoxCad.Controls.Add(this.label6);
            this.GroupBoxCad.Controls.Add(this.label1);
            this.GroupBoxCad.Controls.Add(this.track_Erosion_Coil);
            this.GroupBoxCad.Controls.Add(this.track_Dilation_Coil);
            this.GroupBoxCad.Controls.Add(this.trackBar_Central);
            this.GroupBoxCad.Controls.Add(this.trackBar_Side);
            this.GroupBoxCad.Controls.Add(this.track_Frame_Width);
            this.GroupBoxCad.Dock = System.Windows.Forms.DockStyle.Top;
            this.GroupBoxCad.Location = new System.Drawing.Point(0, 0);
            this.GroupBoxCad.Name = "GroupBoxCad";
            this.GroupBoxCad.Size = new System.Drawing.Size(355, 414);
            this.GroupBoxCad.TabIndex = 20;
            this.GroupBoxCad.TabStop = false;
            this.GroupBoxCad.Text = "Setting up parameters";
            // 
            // checkBox_Fill
            // 
            this.checkBox_Fill.AutoSize = true;
            this.checkBox_Fill.Location = new System.Drawing.Point(124, 352);
            this.checkBox_Fill.Name = "checkBox_Fill";
            this.checkBox_Fill.Size = new System.Drawing.Size(120, 16);
            this.checkBox_Fill.TabIndex = 6;
            this.checkBox_Fill.Text = "Fill Reg_No_Proc";
            this.checkBox_Fill.UseVisualStyleBackColor = true;
            this.checkBox_Fill.CheckedChanged += new System.EventHandler(this.checkBox_Fill_CheckedChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(29, 305);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(80, 12);
            this.label4.TabIndex = 2;
            this.label4.Text = "Erosion_Coil:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(40, 255);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(78, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "Dilation_Coil:";
            this.label2.Click += new System.EventHandler(this.label2_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 72);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(99, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "Horizontal Width:";
            this.label1.Click += new System.EventHandler(this.label1_Click);
            // 
            // track_Erosion_Coil
            // 
            this.track_Erosion_Coil.Location = new System.Drawing.Point(115, 301);
            this.track_Erosion_Coil.Maximum = 200;
            this.track_Erosion_Coil.Minimum = 1;
            this.track_Erosion_Coil.Name = "track_Erosion_Coil";
            this.track_Erosion_Coil.Size = new System.Drawing.Size(234, 45);
            this.track_Erosion_Coil.TabIndex = 5;
            this.track_Erosion_Coil.Value = 1;
            this.track_Erosion_Coil.Scroll += new System.EventHandler(this.track_Frame_Width_Scroll);
            // 
            // track_Dilation_Coil
            // 
            this.track_Dilation_Coil.Location = new System.Drawing.Point(115, 250);
            this.track_Dilation_Coil.Maximum = 500;
            this.track_Dilation_Coil.Minimum = 5;
            this.track_Dilation_Coil.Name = "track_Dilation_Coil";
            this.track_Dilation_Coil.Size = new System.Drawing.Size(234, 45);
            this.track_Dilation_Coil.TabIndex = 4;
            this.track_Dilation_Coil.Value = 50;
            this.track_Dilation_Coil.Scroll += new System.EventHandler(this.track_Frame_Width_Scroll);
            // 
            // track_Frame_Width
            // 
            this.track_Frame_Width.Location = new System.Drawing.Point(115, 68);
            this.track_Frame_Width.Maximum = 500;
            this.track_Frame_Width.Minimum = 10;
            this.track_Frame_Width.Name = "track_Frame_Width";
            this.track_Frame_Width.Size = new System.Drawing.Size(234, 45);
            this.track_Frame_Width.TabIndex = 1;
            this.track_Frame_Width.Value = 50;
            this.track_Frame_Width.Scroll += new System.EventHandler(this.track_Frame_Width_Scroll);
            // 
            // OpenFileDialog
            // 
            this.OpenFileDialog.Filter = "Image files (*.jpg)|*.jpg|Image files (*.tif)|*.tif|All files (*.*)|*.*";
            this.OpenFileDialog.FilterIndex = 3;
            this.OpenFileDialog.RestoreDirectory = true;
            // 
            // trackBar_Side
            // 
            this.trackBar_Side.Location = new System.Drawing.Point(115, 119);
            this.trackBar_Side.Maximum = 500;
            this.trackBar_Side.Minimum = 10;
            this.trackBar_Side.Name = "trackBar_Side";
            this.trackBar_Side.Size = new System.Drawing.Size(234, 45);
            this.trackBar_Side.TabIndex = 2;
            this.trackBar_Side.Value = 50;
            this.trackBar_Side.Scroll += new System.EventHandler(this.track_Frame_Width_Scroll);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(50, 123);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(68, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "Side Width:";
            this.label3.Click += new System.EventHandler(this.label3_Click);
            // 
            // trackBar_Central
            // 
            this.trackBar_Central.Location = new System.Drawing.Point(115, 170);
            this.trackBar_Central.Maximum = 500;
            this.trackBar_Central.Minimum = 10;
            this.trackBar_Central.Name = "trackBar_Central";
            this.trackBar_Central.Size = new System.Drawing.Size(234, 45);
            this.trackBar_Central.TabIndex = 3;
            this.trackBar_Central.Value = 100;
            this.trackBar_Central.Scroll += new System.EventHandler(this.track_Frame_Width_Scroll);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(50, 174);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(83, 12);
            this.label5.TabIndex = 2;
            this.label5.Text = "Central Width:";
            this.label5.Click += new System.EventHandler(this.label5_Click);
            // 
            // comboBox_Type_Unit
            // 
            this.comboBox_Type_Unit.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Type_Unit.FormattingEnabled = true;
            this.comboBox_Type_Unit.Items.AddRange(new object[] {
            "Horizontal",
            "Vertical"});
            this.comboBox_Type_Unit.Location = new System.Drawing.Point(123, 33);
            this.comboBox_Type_Unit.Name = "comboBox_Type_Unit";
            this.comboBox_Type_Unit.Size = new System.Drawing.Size(121, 20);
            this.comboBox_Type_Unit.TabIndex = 0;
            this.comboBox_Type_Unit.SelectedIndexChanged += new System.EventHandler(this.comboBox_Type_Unit_SelectedIndexChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(60, 36);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(58, 12);
            this.label6.TabIndex = 2;
            this.label6.Text = "Unit type:";
            this.label6.Click += new System.EventHandler(this.label1_Click);
            // 
            // Aouto_RegNoProc
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1270, 832);
            this.Controls.Add(this.panel_Left);
            this.Controls.Add(this.H_WindowMain);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MinimizeBox = false;
            this.Name = "Aouto_RegNoProc";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Alignment Lite";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Alignment_FormClosing);
            this.Shown += new System.EventHandler(this.Alignment_Shown);
            this.panel_Left.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.GroupBoxCad.ResumeLayout(false);
            this.GroupBoxCad.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.track_Erosion_Coil)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.track_Dilation_Coil)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.track_Frame_Width)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_Side)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_Central)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private HalconDotNet.HSmartWindowControl H_WindowMain;
        private System.Windows.Forms.Panel panel_Left;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.GroupBox GroupBoxCad;
        private System.Windows.Forms.OpenFileDialog OpenFileDialog;
        private System.Windows.Forms.SaveFileDialog SaveFileDialog;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TrackBar track_Erosion_Coil;
        private System.Windows.Forms.TrackBar track_Dilation_Coil;
        private System.Windows.Forms.TrackBar track_Frame_Width;
        private System.Windows.Forms.CheckBox checkBox_Fill;
        private System.Windows.Forms.Button but_Cancel;
        private System.Windows.Forms.Button but_Ok;
        private System.Windows.Forms.Button but_Save;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TrackBar trackBar_Central;
        private System.Windows.Forms.TrackBar trackBar_Side;
        private System.Windows.Forms.ComboBox comboBox_Type_Unit;
        private System.Windows.Forms.Label label6;
    }
}