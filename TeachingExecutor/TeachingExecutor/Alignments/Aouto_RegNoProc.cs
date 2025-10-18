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
    public partial class Aouto_RegNoProc : Form
    {
        HObject ho_RegNoProc;

        public HWindow mMainWindow;

        private HObject ho_Gi = null;
        public string InitialDirectory = "C:\\";

        public Aouto_RegNoProc()
        {
            InitializeComponent();
            mMainWindow = H_WindowMain.HalconWindow;
            comboBox_Type_Unit.SelectedIndex = 0;
        }
        
        private void Alignment_FormClosing(object sender, FormClosingEventArgs e)
        {
        }

        private void Alignment_Shown(object sender, EventArgs e)
        {
            OpenModel(InitialDirectory);
            track_Erosion_Coil.Value = 1;
            Set_Reg_No_Proc();

        }


        public void OpenModel(string root)
        {
            String file_name = String.Format("{0}\\Gi.tif", root);
            HOperatorSet.ReadImage(out ho_Gi, file_name);
            ho_Gi.DispObj(mMainWindow);
            H_WindowMain.SetFullImagePart();
        }

        private void ButOpen_Click(object sender, EventArgs e)
        {
            Button but = (Button)sender;

            OpenFileDialog.InitialDirectory = Utils.Reg_Get_Values("Current_Path_Save_Lite_Image", InitialDirectory);
            if (OpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                Utils.Reg_Set_Value("Current_Path_Save_Lite_Image", System.IO.Path.GetDirectoryName(OpenFileDialog.FileName));

                HTuple hv_file_name = OpenFileDialog.FileName;


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


        private void Set_Reg_No_Proc()
        {
            if (ho_Gi == null)
            {
                return;
            }

            HTuple hv_Midle_Line_Type = comboBox_Type_Unit.SelectedIndex;

            HTuple hv_Frame_Width = track_Frame_Width.Value;
            HTuple hv_Side_Width = trackBar_Side.Value;
            HTuple hv_Midle_Line_Width = trackBar_Central.Value;
            HTuple hv_Dilation_Coil = track_Dilation_Coil.Value;
            HTuple hv_Erosion_Coil = track_Erosion_Coil.Value;


            HOperatorSet.GetImageSize(ho_Gi, out HTuple hv_Im_W, out HTuple hv_Im_H);
            HOperatorSet.GetDomain(ho_Gi, out HObject ho_Gi_Domain);

            //--- Удаление рамки
            HOperatorSet.GenRectangle1(out HObject ho_Frame_I, hv_Frame_Width, hv_Side_Width, hv_Im_H - hv_Frame_Width, hv_Im_W - hv_Side_Width);
            HOperatorSet.Difference(ho_Gi_Domain, ho_Frame_I, out HObject ho_Frame);

            //--- Удаление средней линии
            HObject ho_Midle_Line;
            if (hv_Midle_Line_Type == 0)
            {
                HTuple hv_M_Pos = hv_Im_W / 2;
                HOperatorSet.GenRectangle1(out ho_Midle_Line, 0, hv_M_Pos - (hv_Midle_Line_Width / 2),
                    hv_Im_H, hv_M_Pos + (hv_Midle_Line_Width / 2));
            }
            else
            {
                HTuple hv_M_Pos = hv_Im_H / 2;
                HOperatorSet.GenRectangle1(out ho_Midle_Line, hv_M_Pos - (hv_Midle_Line_Width / 2),
                    0, hv_M_Pos + (hv_Midle_Line_Width / 2), hv_Im_W);
            }
            HOperatorSet.Union2(ho_Midle_Line, ho_Frame, out ho_Frame);

            //--- Удаление лишних объектов
            HOperatorSet.Threshold(ho_Gi, out HObject ho_Gi_Reg, 128, 255);
            {
                HObject ExpTmpOutVar_0;
                HOperatorSet.Difference(ho_Gi_Reg, ho_Frame, out ExpTmpOutVar_0);
                ho_Gi_Reg.Dispose();
                ho_Gi_Reg = ExpTmpOutVar_0;
            }
            {
                HObject ExpTmpOutVar_0;
                HOperatorSet.Connection(ho_Gi_Reg, out ExpTmpOutVar_0);
                ho_Gi_Reg.Dispose();
                ho_Gi_Reg = ExpTmpOutVar_0;
            }
            HOperatorSet.CountObj(ho_Gi_Reg, out HTuple hv_NumObj);


            //--- Поиск 2-х максимальных площадей объектов - Coils
            HTuple hv_Arr_Obj = new HTuple();
            HTuple end_val65 = hv_NumObj;
            HTuple step_val65 = 1;
            HObject ho_ObjSel;
            for (HTuple hv_I = 1; hv_I.Continue(end_val65, step_val65); hv_I = hv_I.TupleAdd(step_val65))
            {
                HOperatorSet.SelectObj(ho_Gi_Reg, out ho_ObjSel, hv_I);
                if (HDevWindowStack.IsOpen())
                {
                    HOperatorSet.DispObj(ho_ObjSel, HDevWindowStack.GetActive());
                }
                HOperatorSet.AreaCenter(ho_ObjSel, out HTuple hv_Area, out HTuple hv_Row, out HTuple hv_Col);
                if (hv_Arr_Obj == null)
                    hv_Arr_Obj = new HTuple();
                hv_Arr_Obj[new HTuple(hv_Arr_Obj.TupleLength())] = hv_Area;
            }
            HOperatorSet.TupleSortIndex(hv_Arr_Obj, out HTuple hv_Indices);
            HTuple hv_Coil_1 = (hv_Indices.TupleSelect((new HTuple(hv_Indices.TupleLength())) - 1)) + 1;
            HTuple hv_Coil_2 = (hv_Indices.TupleSelect((new HTuple(hv_Indices.TupleLength())) - 2)) + 1;

            //--- Расширение Coils, затем сужение
            HOperatorSet.SelectObj(ho_Gi_Reg, out ho_ObjSel, hv_Coil_1);
            HOperatorSet.DilationCircle(ho_ObjSel, out HObject ho_ObjSel_Dil, hv_Dilation_Coil);
            HOperatorSet.ErosionCircle(ho_ObjSel_Dil, out HObject ho_Obj_Coil_1, hv_Erosion_Coil);

            HOperatorSet.SelectObj(ho_Gi_Reg, out ho_ObjSel, hv_Coil_2);
            HOperatorSet.DilationCircle(ho_ObjSel, out ho_ObjSel_Dil, hv_Dilation_Coil);
            HOperatorSet.ErosionCircle(ho_ObjSel_Dil, out HObject ho_Obj_Coil_2, hv_Erosion_Coil);

            //--- Собираем RegNoProc
            {
                HObject ExpTmpOutVar_0;
                HOperatorSet.Difference(ho_Obj_Coil_1, ho_Frame, out ExpTmpOutVar_0);
                ho_Obj_Coil_1.Dispose();
                ho_Obj_Coil_1 = ExpTmpOutVar_0;
            }
            {
                HObject ExpTmpOutVar_0;
                HOperatorSet.Difference(ho_Obj_Coil_2, ho_Frame, out ExpTmpOutVar_0);
                ho_Obj_Coil_2.Dispose();
                ho_Obj_Coil_2 = ExpTmpOutVar_0;
            }
            HOperatorSet.Union2(ho_Obj_Coil_1, ho_Obj_Coil_2, out HObject ho_Obj_Coil);
            HOperatorSet.Difference(ho_Gi_Domain, ho_Obj_Coil, out ho_RegNoProc);



            ho_Gi.DispObj(mMainWindow);

            mMainWindow.SetDraw("margin");
            mMainWindow.SetLineWidth(3);

            mMainWindow.SetColor("red");
            ho_Frame.DispObj(mMainWindow);

            if (checkBox_Fill.Checked)
            {
                mMainWindow.SetDraw("fill");
            }
            mMainWindow.SetColor("green");
            ho_RegNoProc.DispObj(mMainWindow);
        }

        private void track_Frame_Width_Scroll(object sender, EventArgs e)
        {
            Set_Reg_No_Proc();
        }

        private void checkBox_Fill_CheckedChanged(object sender, EventArgs e)
        {
            Set_Reg_No_Proc();
        }

        private void but_Save_Click(object sender, EventArgs e)
        {
            HOperatorSet.WriteObject(ho_RegNoProc, InitialDirectory + "\\RegNoProc");

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void label5_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void comboBox_Type_Unit_SelectedIndexChanged(object sender, EventArgs e)
        {
            Set_Reg_No_Proc();
        }
    }
}
