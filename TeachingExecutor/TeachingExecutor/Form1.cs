using HalconDotNet;
using System;
using System.IO;
using System.Windows.Forms;

namespace TeachingExecutor
{
    public partial class Form1 : Form
    {
        private OpenFileDialog openFileDialog = new OpenFileDialog();
        private string modelPath;

        public Form1(string[] args)
        {
            InitializeComponent();
            modelPath = args.Length > 0 ? args[0] : string.Empty;
            StartPosition = FormStartPosition.CenterScreen;
            //string message = "받은 명령줄 인자:\n" + string.Join("\n", _args);
            //MessageBox.Show(message, "C++에서 전달된 데이터", MessageBoxButtons.OK, MessageBoxIcon.Information);

        }

        private void butAlignment_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(modelPath))
            {
                LoadAlignment(modelPath);
            }
            else
            {
                openFileDialog.InitialDirectory = "";
                openFileDialog.Filter = "All files (*.*)|*.*";
                openFileDialog.FilterIndex = 0;
                openFileDialog.RestoreDirectory = true;
                openFileDialog.FileName = "";
                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    String root = System.IO.Path.GetDirectoryName(openFileDialog.FileName);
                    LoadAlignment(root);
                }
            }
        }

        private void butTeaching_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(modelPath))
            {
                LoadTeaching(modelPath);
            }
            else
            {
                openFileDialog.InitialDirectory = "";
                openFileDialog.Filter = "All files (*.*)|*.*";
                openFileDialog.FilterIndex = 0;
                openFileDialog.RestoreDirectory = true;
                openFileDialog.FileName = "";
                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    String root = System.IO.Path.GetDirectoryName(openFileDialog.FileName);
                    LoadTeaching(root);
                }
            }
        }

        private void butAlignmentLite_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(modelPath))
            {
                LoadAlignmentL(modelPath);
            }
            else
            {
                openFileDialog.InitialDirectory = Utils.Reg_Get_Values("Current_Path_Save_Lite", "");
                openFileDialog.Filter = "All files (*.*)|*.*";
                openFileDialog.FilterIndex = 0;
                openFileDialog.RestoreDirectory = true;
                openFileDialog.FileName = "";
                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    String root = System.IO.Path.GetDirectoryName(openFileDialog.FileName);
                    LoadAlignmentL(root);
                }
            }
        }

        private void LoadAlignmentL(string path)
        {
            AlignmentL alignmentL = new AlignmentL();
            alignmentL.InitialDirectory = path;

            HOperatorSet.ReadImage(out HObject ho_Gi, alignmentL.InitialDirectory + "\\Gi.tif");

            alignmentL.Set_GI("Gi.tif", ho_Gi);

            string file_raw_image = alignmentL.InitialDirectory + "\\RawImage.bmp";
            if (File.Exists(file_raw_image))
            {
                HOperatorSet.ReadImage(out HObject ho_Image, file_raw_image);
                alignmentL.Set_RawImage("RawImage.bmp", ho_Image);
            }
            else
            {
                alignmentL.Set_RawImage("Gi.tif", ho_Gi);
            }

            alignmentL.OpenFM(alignmentL.InitialDirectory);

            if (alignmentL.ShowDialog() == DialogResult.Cancel)
            {
                //return;
            }
        }

        private void LoadAlignment(string path)
        {
            Alignment alignment = new Alignment();
            alignment.InitialDirectory = path;

            HOperatorSet.ReadImage(out HObject ho_Gi, alignment.InitialDirectory + "\\Gi.tif");

            alignment.Set_GI("Gi.bin", ho_Gi);

            string file_raw_image = alignment.InitialDirectory + "\\RawImage.bmp";
            if (File.Exists(file_raw_image))
            {
                HOperatorSet.ReadImage(out HObject ho_Image, file_raw_image);
                alignment.Set_RawImage("RawImage.bmp", ho_Image);
            }

            alignment.OpenFM(alignment.InitialDirectory + "\\FM.fmk");

            if (alignment.ShowDialog() == DialogResult.Cancel)
            {
                //return;
            }
        }

        private void LoadTeaching(string path)
        {
            Teaching cTeaching = new Teaching(path);
            if (cTeaching.ShowDialog() == DialogResult.Cancel)
            {
                return;
            }
        }

        private void but_Reg_No_Proc_Click(object sender, EventArgs e)
        {
            Aouto_RegNoProc ren_no_proc = new Aouto_RegNoProc();

            if (!string.IsNullOrEmpty(modelPath))
            {
                ren_no_proc.InitialDirectory = modelPath;
            }
            else
            {
                openFileDialog.InitialDirectory = Utils.Reg_Get_Values("Current_Path_Save_Lite", "");
                openFileDialog.Filter = "All files (*.*)|*.*";
                openFileDialog.FilterIndex = 0;
                openFileDialog.RestoreDirectory = true;
                openFileDialog.FileName = "";
                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    String root = System.IO.Path.GetDirectoryName(openFileDialog.FileName);
                    ren_no_proc.InitialDirectory = root;
                }
            }

            if (ren_no_proc.ShowDialog() == DialogResult.Cancel)
            {





            }



        }
    }
}
