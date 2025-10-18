using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using HalconDotNet;

namespace TeachingExecutor
{
    public class ProgressState
    {
        public int progress_pos;
        public int progress_max_pos;
        public string progress_state;

        public ProgressState(int pos, int max_pos, string state)
        {
            progress_pos = pos;
            progress_state = state;
            progress_max_pos = max_pos;
        }
    }

    public partial class Alignment_Adjust : Form
    {
        private Alignment parent;

        private int m_Adj_Erosion;
        private int m_Adj_Dilation;
        private int m_Adj_Bias;

        private int m_fmCount;

        private HTuple m_ho_GiWidth;
        private HTuple m_ho_GiHeight;
        public HObject ho_GiReg;
        public HObject ho_GiReg_Adj = null;

        private Thread mThread;

        public Alignment_Adjust(
            Alignment parent,
            int _Adj_Erosion,
            int _Adj_Dilation,
            int _m_Adj_Bias,
            int _fmCount,
            HTuple _ho_GiWidth,
            HTuple _ho_GiHeight,
            HObject _ho_GiReg
            )
        {
            m_Adj_Erosion = _Adj_Erosion;
            m_Adj_Dilation = _Adj_Dilation;
            m_Adj_Bias = _m_Adj_Bias;
            m_fmCount = _fmCount;
            m_ho_GiWidth = _ho_GiWidth;
            m_ho_GiHeight = _ho_GiHeight;
            ho_GiReg = _ho_GiReg;

            this.parent = parent;
            InitializeComponent();
        }

        private void Create_Model(HObject ho_Gi_Reg_Dil, int i_adj, ref HTuple hv_Min_Err, ref int best_dil, ref string str_state)
        {
            HOperatorSet.RegionToBin(ho_Gi_Reg_Dil, out HObject ho_Gi_Dil, 255, 0, m_ho_GiWidth, m_ho_GiHeight);

            // --- Create models ---------------------------
            for (int i = 1; i < m_fmCount + 1; i++)
            {
                Alignment.FiducialMarker fm = parent.mFM.listFM[i];
                parent.CreateFM(fm, ho_Gi_Dil);
            }

            bool ret = parent.Test_Alignment();
            if (ret)
            {
                Image_Diff(parent.ho_ImAlignment, ho_Gi_Dil, m_ho_GiWidth, m_ho_GiHeight, out HTuple hv_Err);

                if (hv_Min_Err > hv_Err)
                {
                    hv_Min_Err = hv_Err.Clone();
                    best_dil = i_adj;
                    ho_GiReg_Adj = ho_Gi_Reg_Dil.Clone();

                    str_state += ". Best change = " + hv_Min_Err;
                }
                else
                {
                    str_state += ". Change rejected = " + hv_Min_Err;
                }
            }
            else
            {
                str_state += ". Model formation error";
            }
        }

        public static void Image_Diff(HObject image1, HObject image2, int width, int height, out HTuple hv_Err)
        {
            HOperatorSet.TupleGenSequence(0, width - 1, 1, out HTuple hv_Cols);

            hv_Err = 0;
            HTuple end_val15 = height - 1;
            HTuple step_val15 = 1;
            for (HTuple hv_Row = 0; hv_Row.Continue(end_val15, step_val15); hv_Row = hv_Row.TupleAdd(step_val15))
            {
                HOperatorSet.TupleGenConst(width, hv_Row, out HTuple hv_Rows);

                HOperatorSet.GetGrayval(image1, hv_Rows, hv_Cols, out HTuple hv_Gray);
                HOperatorSet.GetGrayval(image2, hv_Rows, hv_Cols, out HTuple hv_Gray_D);
                HTuple hv_Gray_Test = hv_Gray_D - hv_Gray;
                HOperatorSet.TupleAbs(hv_Gray_Test, out hv_Gray_Test);
                HOperatorSet.TupleSum(hv_Gray_Test, out hv_Gray_Test);
                hv_Err = hv_Err + hv_Gray_Test;
            }
        }

        public void Adjust()
        {
            HOperatorSet.GenEmptyObj(out ho_GiReg_Adj);

            HTuple hv_Min_Err = Int32.MaxValue;
            int best_dil = 0;
            string str_state = "";
            int adj_count = m_Adj_Erosion + m_Adj_Dilation;

            HOperatorSet.GenEmptyObj(out HObject ho_Gi_Reg_Dil);

            HTuple hv_Adj_Erosion = (double)m_Adj_Erosion + 0.5;
            HTuple hv_Adj_Dilation = (double)m_Adj_Dilation + 0.5;
            HOperatorSet.ErosionCircle(ho_GiReg, out HObject ho_Gi_Reg_Ers, hv_Adj_Erosion);
            str_state = "Erosion = " + hv_Adj_Erosion;
            ProgressState progress = new ProgressState(0, adj_count, str_state);
            mProgressLayer.Enqueue(progress);

            for (int i_adj = 1; i_adj <= adj_count; i_adj++)
            {
                ho_Gi_Reg_Dil.Dispose();
                HOperatorSet.DilationCircle(ho_Gi_Reg_Ers, out ho_Gi_Reg_Dil, i_adj);
                str_state = "Dilation = " + i_adj;
                Create_Model(ho_Gi_Reg_Dil, i_adj, ref hv_Min_Err, ref best_dil, ref str_state);

                progress = new ProgressState(i_adj, adj_count, str_state);
                mProgressLayer.Enqueue(progress);
            }

            str_state = "Final Dilation = " + best_dil + ". Bias = " + m_Adj_Bias;

            ho_Gi_Reg_Dil.Dispose();
            best_dil += m_Adj_Bias;
            HOperatorSet.DilationCircle(ho_Gi_Reg_Ers, out ho_Gi_Reg_Dil, best_dil);
            hv_Min_Err = Int32.MaxValue;
            Create_Model(ho_Gi_Reg_Dil, best_dil, ref hv_Min_Err, ref best_dil, ref str_state);

            progress = new ProgressState(adj_count, adj_count, str_state);
            mProgressLayer.Enqueue(progress);
        }

        public Queue<ProgressState> mProgressLayer = new Queue<ProgressState>();
        private void Alignment_Adjust_Shown(object sender, EventArgs e)
        {
            ed_Log.Text = "";
            but_Close.Enabled = false;

            mThread = new Thread(() => Adjust());
            mThread.Name = "mThread_Alignment_Adjust";
            mThread.Start();
            mThread.IsBackground = true;
            timerThread.Start();
        }

        private void timerThread_Tick(object sender, EventArgs e)
        {
            ThreadState state = mThread.ThreadState;
            if (state == ThreadState.Stopped) 
            {
                timerThread.Stop();
                but_Close.Enabled = true;
            }

            if (mProgressLayer.Count > 0)
            {
                ProgressState progress = mProgressLayer.Peek();
                mProgressLayer.Dequeue();
                prgBar.Maximum = progress.progress_max_pos;
                prgBar.Value = progress.progress_pos;
                string str_state = progress.progress_state;

                ed_Log.AppendText(str_state + "\r\n");
                ed_Log.ScrollToCaret();
            }
        }

        private void butCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }
    }
}
