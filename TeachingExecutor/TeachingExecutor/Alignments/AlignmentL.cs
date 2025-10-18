using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

using HalconDotNet;

namespace TeachingExecutor
{
    public partial class AlignmentL : Form
    {
        public enum FMType : ushort
        {
            fmCenter = 0,
            fmTopLeft = 1,
            fmTopRight = 2,
            fmBottomLeft = 3,
            fmBottomRight = 4
        }

        public enum Display_Image_Type : ushort
        {
            Display_CAD = 0,
            Display_RAW = 1,
            Display_ALIGNMENT = 2,
        }

        private const String COLOR_FM_RECT = "green";
        private const String COLOR_FM_FIND = "red";
        private const String COLOR_GI_REG = "orange";
        private const String COLOR_FM_FOUND = "firebrick";

        private const int VERSION_ALIGNMENT = 4;

        private Display_Image_Type m_Disp_Image_Type = Display_Image_Type.Display_CAD;


        [Serializable()]
        public class FiducialMarker
        {
            public FMType fmType { get; set; }

            public double score;

            // --- Params
            public double minScore { get; set; }
            public HTuple numLevel;
            public string interpolation;

            public string contrast;
            public string minContrast;
            public string metric;


            // --- Model
            public HObject ho_ROI_FM;
            public HObject ho_ROI_Find;

            public HTuple hv_ModelID;
            public HTuple hv_RefRow;
            public HTuple hv_RefCol;

            public FiducialMarker(FMType type)
            {
                fmType = type;

                ho_ROI_FM = null;
                ho_ROI_Find = null;

                hv_ModelID = 0;
                hv_RefRow = 0;
                hv_RefCol = 0;
                //HOperatorSet.TupleGenConst(5, 0, out hv_RefRow);
                //HOperatorSet.TupleGenConst(5, 0, out hv_RefCol);
                //HOperatorSet.TupleGenConst(5, 0, out hv_ModelID);

                minScore = 0.3;
                numLevel = 2;
                numLevel.Append(3);

                contrast = "auto";
                minContrast = "auto";
                metric = "use_polarity";
                interpolation = "constant";

                score = 0;
            }

        }

        // --- Objects -------------------------------------------------------------

        [Serializable()]
        public class FiducialMarkerList
        {
            //private String fileName { get; set; }
            //private bool saved { get; set; }
            public List<FiducialMarker> listFM = new List<FiducialMarker>();

            //public string contrast { get; set; }
            //public string minContrast { get; set; }
            //public string metric { get; set; }
        }

        public HWindow mMainWindow;

        private HObject ho_Gi = null;
        private HObject ho_Im = null;
        private HObject ho_RegProc_Rect = null;

        public HObject ho_ImAlignment = null;


        private HObject ho_GiReg = null;
        private HObject ho_GiReg_Adj = null;
        HTuple hv_GiWidth = null, hv_GiHeight = null;
        HTuple hv_ImWidth = null, hv_imHeight = null;


        private HDrawingObject ho_CurrRectFM = null;
        private HDrawingObject ho_CurrRectFind = null;

        public FiducialMarker m_FML = null;

        public string InitialDirectory;

        private bool m_Disp = true;
        public AlignmentL()
        {
            InitializeComponent();

            m_FML = new FiducialMarker((FMType)0);

            mMainWindow = H_WindowMain.HalconWindow;

            TextCadFile.Text = "";
            TextImageFile.Text = "";
            LabelCadSize.Text = "";
            LabelImageSize.Text = "";

            /// TODO: To params
            cBoxFMContrast.Text = "auto";
            cBoxFMMinContrast.SelectedIndex = 0;
            cBoxFMMetric.SelectedIndex = 3;

            //EditMinScore.Value = (decimal)0.3;
            //EditNumLevels1.Value = 2;
            //EditNumLevels2.Value = 3;

            cBoxInterpolation.SelectedIndex = 2;

            But_Disp_CAD.Checked = true;
            m_Disp_Image_Type = Display_Image_Type.Display_CAD;

            chBox_ROI_Find.Checked = true;
            chBox_ROI_FM.Checked = true;

            InitialDirectory = "C:\\WORK_CURR\\Programm\\AOI2\\DATA\\AOI_Compass\\";

        }

        private void SetXY(int idx)
        {
            FiducialMarker fm = m_FML;
            HOperatorSet.SmallestRectangle1(fm.ho_ROI_FM, out HTuple hv_Row1, out HTuple hv_Col1, out HTuple hv_Row2, out HTuple hv_Col2);
            int dt_X = (hv_Col2 - hv_Col1) / 2 + hv_Col1;
            int dt_Y = (hv_Row2 - hv_Row1) / 2 + hv_Row1;

            if (m_Disp)
            {
                Ed_Dx.Text = String.Format("{0}", dt_X);
                Ed_Dy.Text = String.Format("{0}", dt_Y);
            }

        }

        private void EventRectFM(HDrawingObject dobj, HWindow hwin, string type)
        {
            if ((type == "on_resize" || type == "on_drag"))
            {
                FiducialMarker fm = m_FML;

                HOperatorSet.GetDrawingObjectParams(ho_CurrRectFM, "row1", out HTuple hv_Row1);
                HOperatorSet.GetDrawingObjectParams(ho_CurrRectFM, "row2", out HTuple hv_Row2);
                HOperatorSet.GetDrawingObjectParams(ho_CurrRectFM, "column1", out HTuple hv_Col1);
                HOperatorSet.GetDrawingObjectParams(ho_CurrRectFM, "column2", out HTuple hv_Col2);

                HOperatorSet.GenRectangle1(out fm.ho_ROI_FM, hv_Row1, hv_Col1, hv_Row2, hv_Col2);

                SetXY(0);

                m_flag_SaveFM = false;
            }
        }
        private void EventRectFind(HDrawingObject dobj, HWindow hwin, string type)
        {
            if ((type == "on_resize" || type == "on_drag"))
            {
                FiducialMarker fm = m_FML;

                HOperatorSet.GetDrawingObjectParams(ho_CurrRectFind, "row1", out HTuple hv_Row1);
                HOperatorSet.GetDrawingObjectParams(ho_CurrRectFind, "row2", out HTuple hv_Row2);
                HOperatorSet.GetDrawingObjectParams(ho_CurrRectFind, "column1", out HTuple hv_Col1);
                HOperatorSet.GetDrawingObjectParams(ho_CurrRectFind, "column2", out HTuple hv_Col2);

                HOperatorSet.GenRectangle1(out fm.ho_ROI_Find, hv_Row1, hv_Col1, hv_Row2, hv_Col2);

                m_flag_SaveFM = false;
            }
        }

        private void ClearROI()
        {
            if (ho_CurrRectFM != null)
            {
                mMainWindow.DetachDrawingObjectFromWindow(ho_CurrRectFM);
                ho_CurrRectFM.Dispose();
                ho_CurrRectFM = null;
            }

            if (ho_CurrRectFind != null)
            {
                mMainWindow.DetachDrawingObjectFromWindow(ho_CurrRectFind);
                ho_CurrRectFind.Dispose();
                ho_CurrRectFind = null;
            }
        }

        /// <summary>
        /// Display the image and selected FMs
        /// </summary>
        private bool disp_GiReg_Type = false;
        private void DrawImageROI()
        {
            mMainWindow.ClearWindow();
            if (m_Disp_Image_Type == Display_Image_Type.Display_CAD && ho_Gi != null)
            {
                ho_Gi.DispObj(mMainWindow);
            }
            else if (m_Disp_Image_Type == Display_Image_Type.Display_RAW && ho_Im != null)
            {
                ho_Im.DispObj(mMainWindow);
            }
            else if (m_Disp_Image_Type == Display_Image_Type.Display_ALIGNMENT && ho_ImAlignment != null)
            {
                ho_ImAlignment.DispObj(mMainWindow);
            }
            else
            {
                //mainWindow.ClearWindow();
                return;
            }

            //if (ListViewFM.SelectedItems.Count > 0)
            {
                FiducialMarker fm = m_FML;

                ClearROI();

                mMainWindow.SetDraw("margin");

                HTuple hv_Row1 = null, hv_Col1 = null, hv_Row2 = null, hv_Col2 = null;
                if (ho_RegProc_Rect != null && chBox_RegProc_Rect.Checked)
                {
                    mMainWindow.SetLineWidth(1);
                    mMainWindow.SetColor("firebrick");

                    ho_RegProc_Rect.DispObj(mMainWindow);
                }
                if (fm.ho_ROI_Find != null && chBox_ROI_Find.Checked)
                {
                    HOperatorSet.SmallestRectangle1(fm.ho_ROI_Find, out hv_Row1, out hv_Col1, out hv_Row2, out hv_Col2);
                    ho_CurrRectFind = HDrawingObject.CreateDrawingObject(HDrawingObject.HDrawingObjectType.RECTANGLE1, hv_Row1, hv_Col1, hv_Row2, hv_Col2);

                    ho_CurrRectFind.SetDrawingObjectParams("color", COLOR_FM_FIND);
                    ho_CurrRectFind.OnResize(EventRectFind);
                    ho_CurrRectFind.OnDrag(EventRectFind);
                    mMainWindow.AttachDrawingObjectToWindow(ho_CurrRectFind);
                }
                if (fm.ho_ROI_FM != null && chBox_ROI_FM.Checked)
                {
                    HOperatorSet.SmallestRectangle1(fm.ho_ROI_FM, out hv_Row1, out hv_Col1, out hv_Row2, out hv_Col2);
                    ho_CurrRectFM = HDrawingObject.CreateDrawingObject(HDrawingObject.HDrawingObjectType.RECTANGLE1, hv_Row1, hv_Col1, hv_Row2, hv_Col2);

                    ho_CurrRectFM.SetDrawingObjectParams("color", COLOR_FM_RECT);
                    ho_CurrRectFM.OnResize(EventRectFM);
                    ho_CurrRectFM.OnDrag(EventRectFM);
                    mMainWindow.AttachDrawingObjectToWindow(ho_CurrRectFM);
                }
                if (chBox_GiReg.Checked && ho_GiReg != null)
                {
                    mMainWindow.SetLineWidth(1);
                    mMainWindow.SetColor("firebrick");

                    if (ho_GiReg_Adj != null && disp_GiReg_Type)
                    {
                        ho_GiReg_Adj.DispObj(mMainWindow);
                    }
                    else
                    {
                        ho_GiReg.DispObj(mMainWindow);
                    }
                    disp_GiReg_Type = !disp_GiReg_Type;
                }

                if (chBox_Fit.Checked && (fm.ho_ROI_FM != null || fm.ho_ROI_Find != null))
                {
                    int dt_X = (hv_Col2 - hv_Col1) / 2;
                    int dt_Y = (hv_Row2 - hv_Row1) / 2;
                    int X0 = hv_Col1 + dt_X;
                    int Y0 = hv_Row1 + dt_Y;

                    int dtX = dt_X + dt_X / 2;
                    int dtY = dt_Y + dt_Y / 2;
                    int dt = dtX > dtY ? dtX : dtY;
                    mMainWindow.SetPart(Y0 - dt, (X0 - dt), Y0 + dt, (X0 + dt));
                }

                fIndexChange = true;

                cBoxFMContrast.Text = fm.contrast;
                cBoxFMMinContrast.Text = fm.minContrast;
                cBoxFMMetric.Text = fm.metric;
                EditMinScore.Value = (decimal)fm.minScore;
                cBoxInterpolation.Text = fm.interpolation;

                if (fm.numLevel.Length > 1)
                {
                    EditNumLevels1.Value = (decimal)fm.numLevel[0];
                    EditNumLevels2.Value = (decimal)fm.numLevel[1];
                }
                else
                {
                    EditNumLevels1.Value = (decimal)fm.numLevel;
                    EditNumLevels2.Value = (decimal)fm.numLevel;
                }

                fIndexChange = false;
            }

        }

        private void CBoxImageType_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void ButAddROI_Click(object sender, EventArgs e)
        {
        }

        private void ButCadReg_Click(object sender, EventArgs e)
        {
        }



        // ------------------------------------------------------------
        // --- FM -----------------------------------------------------
        // ------------------------------------------------------------
        private void DispResult(HWindow window, HTuple color, HTuple hv_ModelID, HTuple hv_Row, HTuple hv_Col, HTuple hv_Angle)
        {
            if (!m_Disp)
            {
                return;
            }
            Utils.DevDisplayShapeMatchingResults(hv_ModelID, hv_Row, hv_Col, hv_Angle, 1, 1, 0,
                out HObject ho_ContoursAffinTrans);

            if (ho_ContoursAffinTrans == null)
                return;

            window.SetDraw("margin");
            window.SetLineWidth(2);
            window.SetColor(color);
            ho_ContoursAffinTrans.DispObj(window);

        }
        private void FindFM(FiducialMarker fm, HObject ho_Im_ROI,
            out HTuple hv_Row, out HTuple hv_Col, out HTuple hv_Angle, out HTuple hv_Score)
        {
            HOperatorSet.FindShapeModel(
                ho_Im_ROI, fm.hv_ModelID, 0, 0,
                0.1,
                1, 0.5, "least_squares",
                fm.numLevel, 0.9,
                out hv_Row, out hv_Col, out hv_Angle, out hv_Score);

            DispResult(mMainWindow, COLOR_FM_FOUND, fm.hv_ModelID, hv_Row, hv_Col, hv_Angle);

        }

        public void CreateFM(FiducialMarker fm, HObject ho_Gi_Set)
        {
            HOperatorSet.ReduceDomain(ho_Gi_Set, fm.ho_ROI_FM, out HObject ho_Gi_ROI);
            HOperatorSet.CreateShapeModel(ho_Gi_ROI,
                "auto", 0, 0, "auto", "auto",
                fm.metric,      //cBoxFMMetric.Text,
                fm.contrast,    //cBoxFMContrast.Text,
                fm.minContrast, //cBoxFMMinContrast.Text,
                out fm.hv_ModelID);

            FindFM(fm, ho_Gi_ROI, out HTuple hv_Row, out HTuple hv_Col, out HTuple hv_Angle, out HTuple hv_Score);

            DispResult(mMainWindow, COLOR_FM_FOUND, fm.hv_ModelID, hv_Row, hv_Col, hv_Angle);

            fm.hv_RefRow = hv_Row;
            fm.hv_RefCol = hv_Col;
        }

        private bool CheckFMROI()
        {
            if (m_FML.ho_ROI_FM == null)
            {
                return false;
            }
            return true;
        }

        private void ButCombination_Click(object sender, EventArgs e)
        {
            FiducialMarker fm = m_FML;
            try
            {
                if (fm.ho_ROI_Find != null)
                {
                    // --- Create model Combination ---------------------------
                    CreateFM(fm, ho_Gi);

                    // --- Find FM Combination --------------------------------
                    HOperatorSet.ReduceDomain(ho_Im, fm.ho_ROI_Find, out HObject ho_Img_ROI);
                    FindFM(fm, ho_Img_ROI, out HTuple hv_Row, out HTuple hv_Col, out HTuple hv_Angle, out HTuple hv_Score);

                    // --- Combination Image
                    HOperatorSet.HomMat2dIdentity(out HTuple hv_matrix_trans);
                    HOperatorSet.HomMat2dTranslate(
                        hv_matrix_trans,
                        fm.hv_RefRow - hv_Row,
                        fm.hv_RefCol - hv_Col,
                        out hv_matrix_trans);

                    if (ho_ImAlignment != null)
                    {
                        ho_ImAlignment.Dispose();
                    }
                    HOperatorSet.AffineTransImage(
                        ho_Im, out ho_ImAlignment, hv_matrix_trans,
                        fm.interpolation,
                        "true");

                    fm.score = hv_Score;
                    EdScope.Text = String.Format("{0:F3}", fm.score);

                    SetXY(0);

                    m_flag_SaveFM = false;
                }
            }
            catch (HalconException exc)
            {
                /// TODO: Log
                MessageBox.Show("Failed combination! HalconException: " + exc.GetErrorMessage(), "AlignmentL", MessageBoxButtons.OK, MessageBoxIcon.Error);

                return;
            }
        }


        public bool m_flag_SaveFM = false;
        public bool SaveFM()
        {
            SaveFileDialog.InitialDirectory = Utils.Reg_Get_Values("Current_Path_Save_Lite", InitialDirectory);
            SaveFileDialog.Filter = "FM files (*.fmk)|*.fmk|All files (*.*)|*.*";
            SaveFileDialog.FilterIndex = 0;
            SaveFileDialog.RestoreDirectory = true;
            SaveFileDialog.FileName = "FML";

            if (SaveFileDialog.ShowDialog() == DialogResult.OK)
            {
                String root = System.IO.Path.GetDirectoryName(SaveFileDialog.FileName);
                Utils.Reg_Set_Value("Current_Path_Save_Lite", root);

                CreateFM(m_FML, ho_Gi);

                BinaryFormatter bf = new BinaryFormatter();
                FileStream fs = new FileStream(String.Format("{0}//FML.fmk", root), FileMode.Create);
                bf.Serialize(fs, m_FML);
                fs.Flush();
                fs.Close();

                // --- Сохраним параметры, реперные точки и модели в Халкон формате
                String subDirParent = String.Format("{0}//Alig2//", root);
                String subDir = String.Format("{0}//Alig2//Lite//", root);
                Utils.FolderCheck(subDir);

                // Для совместимости со старой версией
                HOperatorSet.TupleGenConst(10, 0, out HTuple hv_Params);
                hv_Params[0] = 5;                       // Models_Count
                hv_Params[1] = 800;                     // Max_Bias1
                hv_Params[2] = 15;                      // Max_Bias2
                hv_Params[3] = 0.3;                     // Min_Scope
                hv_Params[4] = VERSION_ALIGNMENT;       // Version
                HOperatorSet.WriteTuple(hv_Params, subDir + "Alig_Param.tup");
                HOperatorSet.WriteTuple(hv_Params, subDirParent + "Alig_Param.tup");

                {
                    int i = 0;
                    subDir = String.Format("{0}//Alig2//Lite//FM_{1}//", root, i);
                    Utils.FolderCheck(subDir);

                    FiducialMarker fm = m_FML;

                    HOperatorSet.WriteShapeModel(fm.hv_ModelID, subDir + "ModelID.sbm");
                    HOperatorSet.WriteObject(fm.ho_ROI_FM, subDir + "ROI_FM");
                    HOperatorSet.WriteObject(fm.ho_ROI_Find, subDir + "ROI_Find");

                    HOperatorSet.TupleGenConst(10, 0, out hv_Params);
                    hv_Params[0] = fm.hv_RefCol;
                    hv_Params[1] = fm.hv_RefRow;
                    hv_Params[2] = fm.metric;
                    hv_Params[3] = fm.minScore;
                    if (fm.numLevel.Length > 1)
                    {
                        hv_Params[4] = fm.numLevel[0];
                        hv_Params[5] = fm.numLevel[1];
                    }
                    else
                    {
                        hv_Params[4] = fm.numLevel;
                        hv_Params[5] = fm.numLevel;
                    }
                    hv_Params[6] = fm.interpolation;
                    HOperatorSet.WriteTuple(hv_Params, subDir + "Params.tup");
                }

                m_flag_SaveFM = true;
                return true;
            }
            else
            {
                return false;
            }

        }
        private void ButSaveFM_Click(object sender, EventArgs e)
        {
            SaveFM();
        }


        public void OpenFM(string root)
        {
            String file_name_RNP = String.Format("{0}\\RegNoProc.hobj", root);
            HObject ho_RegNoProc = null;
            HOperatorSet.ReadObject(out ho_RegNoProc, file_name_RNP);
            HOperatorSet.GetDomain(ho_Gi, out HObject ho_Domain);
            HOperatorSet.Difference(ho_Domain, ho_RegNoProc, out HObject ho_RegProc);
            HOperatorSet.SmallestRectangle1(ho_RegProc, out HTuple row1, out HTuple col1, out HTuple row2, out HTuple col2);
            HOperatorSet.GenRectangle1(out ho_RegProc_Rect, row1, col1, row2, col2);


            String subDir = String.Format("{0}\\Alig2\\Lite\\", root);
            if (File.Exists(subDir + "Alig_Param.tup"))
            {
                HOperatorSet.ReadTuple(subDir + "Alig_Param.tup", out HTuple hv_Alig_Param);
                if (hv_Alig_Param[4] != VERSION_ALIGNMENT)
                {
                    return;
                }
            }


            UpdateFM(2);

            string file_name = root + "\\FML.fmk";
            if (File.Exists(file_name))
            {
                BinaryFormatter bf = new BinaryFormatter();
                FileStream fs = new FileStream(file_name, FileMode.Open);                   // Open file
                FiducialMarker fm = bf.Deserialize(fs) as FiducialMarker;                               // Deserialize file
                fs.Close();                                                                 // Close the filestream

                if (fm != null)
                {
                    m_FML = fm;
                    DrawImageROI();
                    SetXY(0);
                }
            }


            Utils.Reg_Set_Value("Current_Path_Save_Lite", System.IO.Path.GetDirectoryName(file_name));
        }

        private void ButOpenFM_Click(object sender, EventArgs e)
        {
            OpenFileDialog.InitialDirectory = Utils.Reg_Get_Values("Current_Path_Save_Lite", InitialDirectory);
            OpenFileDialog.Filter = "FM files (*.fmk)|*.fmk|All files (*.*)|*.*";
            OpenFileDialog.FilterIndex = 0;
            OpenFileDialog.RestoreDirectory = true;
            OpenFileDialog.FileName = "";

            if (OpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                OpenFM(OpenFileDialog.FileName);
            }
        }

        private void ModelParams_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!fIndexChange)
            {
                FiducialMarker fm = m_FML;

                fm.contrast = cBoxFMContrast.Text;
                fm.minContrast = cBoxFMMinContrast.Text;
                fm.metric = cBoxFMMetric.Text;
                fm.interpolation = cBoxInterpolation.Text;
            }
        }

        private void FindParams_ValueChanged(object sender, EventArgs e)
        {
            if (!fIndexChange)
            {
                FiducialMarker fm = m_FML;

                fm.minScore = (double)EditMinScore.Value;

                fm.numLevel = new HTuple();
                if (EditNumLevels1.Value == EditNumLevels2.Value)
                {
                    fm.numLevel = (int)EditNumLevels1.Value;
                }
                else
                {
                    fm.numLevel[0] = (int)EditNumLevels1.Value;
                    fm.numLevel[1] = (int)EditNumLevels2.Value;
                }
            }

            m_flag_SaveFM = false;
        }

        private bool fIndexChange = false;

        private void ButImSave_Click(object sender, EventArgs e)
        {
            SaveFileDialog.InitialDirectory = Utils.Reg_Get_Values("Current_Path_Save_Lite", InitialDirectory); ;
            SaveFileDialog.Filter = "All files (*.*)|*.*";
            SaveFileDialog.FilterIndex = 0;
            SaveFileDialog.RestoreDirectory = true;
            SaveFileDialog.FileName = "Image";

            if (SaveFileDialog.ShowDialog() == DialogResult.OK)
            {
                Utils.Reg_Set_Value("Current_Path_Save_Lite", System.IO.Path.GetDirectoryName(SaveFileDialog.FileName));

                HOperatorSet.WriteImage(ho_ImAlignment, "tiff", 0, SaveFileDialog.FileName);
            }

        }

        private void My_MouseWheel(object sender, MouseEventArgs e)
        {
            Point pt = H_WindowMain.Location;
            MouseEventArgs newe = new MouseEventArgs(e.Button, e.Clicks, e.X - pt.X, e.Y - pt.Y, e.Delta);
            H_WindowMain.HSmartWindowControl_MouseWheel(sender, newe);
        }

        private void H_WindowMain_Load(object sender, EventArgs e)
        {
            this.MouseWheel += My_MouseWheel;
        }

        private void ListViewFM_SelectedIndexChanged(object sender, EventArgs e)
        {
            ListView list = (ListView)sender;

            if (list.SelectedItems.Count > 0)
            {
                fIndexChange = true;
                DrawImageROI();
                fIndexChange = false;
            }
        }

        public void Set_GI(string name, HObject ho_Image)
        {
            ho_Gi = ho_Image;
            ho_GiReg_Adj = null;

            HOperatorSet.GetImageSize(ho_Gi, out hv_GiWidth, out hv_GiHeight);
            LabelCadSize.Text = hv_GiWidth + " X " + hv_GiHeight;

            HOperatorSet.Threshold(ho_Gi, out ho_GiReg, 125, 255);

            ho_Gi.DispObj(mMainWindow);
            H_WindowMain.SetFullImagePart();
            TextCadFile.Text = name;
        }

        private void Alignment_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (m_flag_SaveFM == true)
            {
                return;
            }

            System.Text.StringBuilder messageBoxCS = new System.Text.StringBuilder();
            messageBoxCS.AppendFormat("{0}", "The alignment model has not been saved!");
            messageBoxCS.AppendLine();
            messageBoxCS.AppendFormat("{0}", "Save the model?");
            messageBoxCS.AppendLine();

            DialogResult dialogResult = MessageBox.Show(messageBoxCS.ToString(), "Saving the alignment model", MessageBoxButtons.YesNoCancel);
            if (dialogResult == DialogResult.Yes)
            {
                bool saved = SaveFM();
                if (!saved)
                {
                    e.Cancel = true;
                }
            }
            else if (dialogResult == DialogResult.Cancel)
            {
                e.Cancel = true;
            }
        }

        private void Alignment_Shown(object sender, EventArgs e)
        {

        }

        private void chBox_CAD_Adjust_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox obj = (CheckBox)sender;
        }

        private void UpdateFM(int type)
        {
            if (type == 1)
            {
                HOperatorSet.GetDomain(ho_Gi, out HObject ho_Gi_Domain);
                HOperatorSet.SmallestRectangle1(ho_Gi_Domain, out HTuple hv_RowGi1, out HTuple hv_ColGi1, out HTuple hv_RowGi2, out HTuple hv_ColGi2);
                HOperatorSet.GenRectangle1(out m_FML.ho_ROI_FM, hv_RowGi1, hv_ColGi1, hv_RowGi2, hv_ColGi2);
            }
            else
            {
                m_FML.ho_ROI_FM = ho_RegProc_Rect;
            }


            HOperatorSet.GetDomain(ho_Im, out HObject ho_Im_Domain);
            HOperatorSet.SmallestRectangle1(ho_Im_Domain, out HTuple hv_RowIm1, out HTuple hv_ColIm1, out HTuple hv_RowIm2, out HTuple hv_ColIm2);
            HOperatorSet.GenRectangle1(out m_FML.ho_ROI_Find, hv_RowIm1, hv_ColIm1, hv_RowIm2, hv_ColIm2);

            DrawImageROI();

            SetXY(0);

            m_flag_SaveFM = false;
        }

        private void butUpdateFM_Click(object sender, EventArgs e)
        {
            UpdateFM(1);
        }

        private void chBox_GiReg_CheckedChanged(object sender, EventArgs e)
        {
            DrawImageROI();
        }

        private void But_Disp_CAD_CheckedChanged(object sender, EventArgs e)
        {
            m_Disp_Image_Type = Display_Image_Type.Display_CAD;
            DrawImageROI();
        }

        private void But_Disp_RAW_CheckedChanged(object sender, EventArgs e)
        {
            m_Disp_Image_Type = Display_Image_Type.Display_RAW;
            DrawImageROI();
        }

        private void But_Disp_ALIG_CheckedChanged(object sender, EventArgs e)
        {
            m_Disp_Image_Type = Display_Image_Type.Display_ALIGNMENT;
            DrawImageROI();
        }

        private void butUpDateFM_RNP_Click(object sender, EventArgs e)
        {
            UpdateFM(2);
        }

        public void Set_RawImage(string name, HObject ho_Image)
        {
            ho_Im = ho_Image;

            HOperatorSet.GetImageSize(ho_Im, out hv_ImWidth, out hv_imHeight);
            LabelImageSize.Text = hv_ImWidth + " X " + hv_imHeight;

            ho_Im.DispObj(mMainWindow);
            H_WindowMain.SetFullImagePart();
            TextImageFile.Text = name;
        }

        private void ButOpen_Click(object sender, EventArgs e)
        {
            Button but = (Button)sender;

            OpenFileDialog.InitialDirectory = Utils.Reg_Get_Values("Current_Path_Save_Lite_Image", InitialDirectory);
            if (OpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                Utils.Reg_Set_Value("Current_Path_Save_Lite_Image", System.IO.Path.GetDirectoryName(OpenFileDialog.FileName));

                HTuple hv_file_name = OpenFileDialog.FileName;

                if (but == ButCadOpen)
                {
                    //HOperatorSet.GenEmptyObj(out ho_Gi);
                    HOperatorSet.ReadImage(out HObject ho_Image, hv_file_name);

                    Set_GI(OpenFileDialog.SafeFileName, ho_Image);



                    //TextCadFile.Text = OpenFileDialog.SafeFileName;

                    //HOperatorSet.GetImageSize(ho_Gi, out hv_GiWidth, out hv_GiHeight);
                    //LabelCadSize.Text = hv_GiWidth + " X " + hv_GiHeight;

                    //HOperatorSet.Threshold(ho_Gi, out ho_GiReg, 125, 255);

                    //ho_Gi.DispObj(mMainWindow);
                    //H_WindowMain.SetFullImagePart();
                }
                else if (but == ButImageOpen)
                {
                    //HOperatorSet.GenEmptyObj(out ho_Im);
                    HOperatorSet.ReadImage(out HObject ho_Image, hv_file_name);

                    Set_RawImage(OpenFileDialog.SafeFileName, ho_Image);



                    //TextImageFile.Text = OpenFileDialog.SafeFileName;

                    //HOperatorSet.GetImageSize(ho_Im, out hv_ImWidth, out hv_imHeight);
                    //LabelImageSize.Text = hv_ImWidth + " X " + hv_imHeight;

                    //ho_Im.DispObj(mMainWindow);
                    //H_WindowMain.SetFullImagePart();
                }
            }
        }
    }
}
