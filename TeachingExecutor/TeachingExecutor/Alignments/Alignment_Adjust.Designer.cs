namespace TeachingExecutor
{
    partial class Alignment_Adjust
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
            this.components = new System.ComponentModel.Container();
            this.but_Close = new System.Windows.Forms.Button();
            this.edAdjust = new System.Windows.Forms.Label();
            this.prgBar = new System.Windows.Forms.ProgressBar();
            this.ed_Log = new System.Windows.Forms.RichTextBox();
            this.timerThread = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // but_Close
            // 
            this.but_Close.Location = new System.Drawing.Point(458, 304);
            this.but_Close.Margin = new System.Windows.Forms.Padding(2);
            this.but_Close.Name = "but_Close";
            this.but_Close.Size = new System.Drawing.Size(70, 23);
            this.but_Close.TabIndex = 15;
            this.but_Close.Text = "Close";
            this.but_Close.UseVisualStyleBackColor = true;
            this.but_Close.Click += new System.EventHandler(this.butCancel_Click);
            // 
            // edAdjust
            // 
            this.edAdjust.BackColor = System.Drawing.Color.Transparent;
            this.edAdjust.Location = new System.Drawing.Point(11, 34);
            this.edAdjust.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.edAdjust.Name = "edAdjust";
            this.edAdjust.Size = new System.Drawing.Size(517, 17);
            this.edAdjust.TabIndex = 14;
            this.edAdjust.Text = "Teach FWM";
            this.edAdjust.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // prgBar
            // 
            this.prgBar.Location = new System.Drawing.Point(11, 11);
            this.prgBar.Margin = new System.Windows.Forms.Padding(2);
            this.prgBar.Name = "prgBar";
            this.prgBar.Size = new System.Drawing.Size(516, 19);
            this.prgBar.Step = 1;
            this.prgBar.TabIndex = 13;
            // 
            // ed_Log
            // 
            this.ed_Log.Location = new System.Drawing.Point(13, 54);
            this.ed_Log.Name = "ed_Log";
            this.ed_Log.Size = new System.Drawing.Size(515, 243);
            this.ed_Log.TabIndex = 17;
            this.ed_Log.Text = "";
            // 
            // timerThread
            // 
            this.timerThread.Tick += new System.EventHandler(this.timerThread_Tick);
            // 
            // Alignment_Adjust
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(538, 338);
            this.ControlBox = false;
            this.Controls.Add(this.ed_Log);
            this.Controls.Add(this.but_Close);
            this.Controls.Add(this.edAdjust);
            this.Controls.Add(this.prgBar);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Alignment_Adjust";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Alignment Adjust";
            this.Shown += new System.EventHandler(this.Alignment_Adjust_Shown);
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button but_Close;
        private System.Windows.Forms.Label edAdjust;
        private System.Windows.Forms.ProgressBar prgBar;
        private System.Windows.Forms.RichTextBox ed_Log;
        private System.Windows.Forms.Timer timerThread;
    }
}