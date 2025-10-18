namespace TeachingExecutor
{
    partial class Form1
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.butAlignment = new System.Windows.Forms.Button();
            this.butTeaching = new System.Windows.Forms.Button();
            this.butAlignmentLite = new System.Windows.Forms.Button();
            this.but_Reg_No_Proc = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // butAlignment
            // 
            this.butAlignment.Location = new System.Drawing.Point(104, 47);
            this.butAlignment.Name = "butAlignment";
            this.butAlignment.Size = new System.Drawing.Size(120, 24);
            this.butAlignment.TabIndex = 0;
            this.butAlignment.TabStop = false;
            this.butAlignment.Text = "Alignment";
            this.butAlignment.UseVisualStyleBackColor = true;
            this.butAlignment.Click += new System.EventHandler(this.butAlignment_Click);
            // 
            // butTeaching
            // 
            this.butTeaching.Location = new System.Drawing.Point(104, 169);
            this.butTeaching.Name = "butTeaching";
            this.butTeaching.Size = new System.Drawing.Size(120, 40);
            this.butTeaching.TabIndex = 2;
            this.butTeaching.Text = "Teaching";
            this.butTeaching.UseVisualStyleBackColor = true;
            this.butTeaching.Click += new System.EventHandler(this.butTeaching_Click);
            // 
            // butAlignmentLite
            // 
            this.butAlignmentLite.Location = new System.Drawing.Point(104, 77);
            this.butAlignmentLite.Name = "butAlignmentLite";
            this.butAlignmentLite.Size = new System.Drawing.Size(120, 40);
            this.butAlignmentLite.TabIndex = 0;
            this.butAlignmentLite.Text = "Alignment Lite";
            this.butAlignmentLite.UseVisualStyleBackColor = true;
            this.butAlignmentLite.Click += new System.EventHandler(this.butAlignmentLite_Click);
            // 
            // but_Reg_No_Proc
            // 
            this.but_Reg_No_Proc.Location = new System.Drawing.Point(104, 123);
            this.but_Reg_No_Proc.Name = "but_Reg_No_Proc";
            this.but_Reg_No_Proc.Size = new System.Drawing.Size(120, 40);
            this.but_Reg_No_Proc.TabIndex = 1;
            this.but_Reg_No_Proc.Text = "Set RegNoProc";
            this.but_Reg_No_Proc.UseVisualStyleBackColor = true;
            this.but_Reg_No_Proc.Click += new System.EventHandler(this.but_Reg_No_Proc_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(319, 248);
            this.Controls.Add(this.but_Reg_No_Proc);
            this.Controls.Add(this.butTeaching);
            this.Controls.Add(this.butAlignmentLite);
            this.Controls.Add(this.butAlignment);
            this.Name = "Form1";
            this.Text = "TeachingExecutor";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button butAlignment;
        private System.Windows.Forms.Button butTeaching;
        private System.Windows.Forms.Button butAlignmentLite;
        private System.Windows.Forms.Button but_Reg_No_Proc;
    }
}

