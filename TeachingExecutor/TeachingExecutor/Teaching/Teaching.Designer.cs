namespace TeachingExecutor
{
    partial class Teaching
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
            this.edTeachType = new System.Windows.Forms.Label();
            this.prgBarTeach = new System.Windows.Forms.ProgressBar();
            this.butCancel = new System.Windows.Forms.Button();
            this.butTeach = new System.Windows.Forms.Button();
            this.timerThread = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // edTeachType
            // 
            this.edTeachType.BackColor = System.Drawing.Color.Transparent;
            this.edTeachType.Location = new System.Drawing.Point(11, 34);
            this.edTeachType.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.edTeachType.Name = "edTeachType";
            this.edTeachType.Size = new System.Drawing.Size(517, 17);
            this.edTeachType.TabIndex = 4;
            this.edTeachType.Text = "Teach FWM";
            this.edTeachType.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // prgBarTeach
            // 
            this.prgBarTeach.Location = new System.Drawing.Point(11, 11);
            this.prgBarTeach.Margin = new System.Windows.Forms.Padding(2);
            this.prgBarTeach.Name = "prgBarTeach";
            this.prgBarTeach.Size = new System.Drawing.Size(516, 19);
            this.prgBarTeach.Step = 1;
            this.prgBarTeach.TabIndex = 3;
            // 
            // butCancel
            // 
            this.butCancel.Location = new System.Drawing.Point(377, 54);
            this.butCancel.Margin = new System.Windows.Forms.Padding(2);
            this.butCancel.Name = "butCancel";
            this.butCancel.Size = new System.Drawing.Size(70, 23);
            this.butCancel.TabIndex = 11;
            this.butCancel.Text = "Cnacel";
            this.butCancel.UseVisualStyleBackColor = true;
            this.butCancel.Click += new System.EventHandler(this.butCancel_Click);
            // 
            // butTeach
            // 
            this.butTeach.Location = new System.Drawing.Point(452, 54);
            this.butTeach.Name = "butTeach";
            this.butTeach.Size = new System.Drawing.Size(75, 23);
            this.butTeach.TabIndex = 12;
            this.butTeach.Text = "Teaching";
            this.butTeach.UseVisualStyleBackColor = true;
            this.butTeach.Click += new System.EventHandler(this.butTeach_Click);
            // 
            // timerThread
            // 
            this.timerThread.Tick += new System.EventHandler(this.timerThread_Tick);
            // 
            // Teaching
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(539, 87);
            this.Controls.Add(this.butTeach);
            this.Controls.Add(this.butCancel);
            this.Controls.Add(this.edTeachType);
            this.Controls.Add(this.prgBarTeach);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Teaching";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Teaching";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label edTeachType;
        private System.Windows.Forms.ProgressBar prgBarTeach;
        private System.Windows.Forms.Button butCancel;
        private System.Windows.Forms.Button butTeach;
        private System.Windows.Forms.Timer timerThread;
    }
}