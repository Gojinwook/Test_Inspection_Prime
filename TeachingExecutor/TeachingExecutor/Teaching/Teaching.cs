using System;
using System.Windows.Forms;
using System.Threading;
using HalconDotNet;
using System.IO;
using Teaching_DLL;
using System.Runtime.InteropServices;
using System.Collections;
using System.Collections.Generic;

namespace TeachingExecutor
{
    public partial class Teaching : Form
    {
        [DllImport("ALGORITHMSUB.dll")] static public extern IntPtr Create_Open_Short_2();
        [DllImport("ALGORITHMSUB.dll")] static public extern IntPtr Teaching_Open_Short_2(IntPtr op, int gray_gap, int buff_size, string root);

        private Thread mThread_Teaching;

        public HTuple m_hv_ModelPath;
        public HObject m_ho_Gi;
        public HObject m_ho_Im;
        public HObject m_ho_RegNoProc;

        public Teaching(string model_path)
        {
            InitializeComponent();

            m_hv_ModelPath = model_path + "\\";

            butTeach.Enabled = true;
        }

        private HTuple m_hv_MaxPRC = 100;
        private HTuple m_hv_TotalPRC = 0;
        private HTuple m_hv_CurrentOper = "";

        private void TeachingProc()
        {

            if (File.Exists(m_hv_ModelPath + "Gi.tif") == false)
            {
                MessageBox.Show("Error reading file - Gi.tif", "Teaching", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (File.Exists(m_hv_ModelPath + "Im.tif") == false)
            {
                HOperatorSet.ReadImage(out m_ho_Im, m_hv_ModelPath + "Gi.tif");
            }
            else
            {
                HOperatorSet.ReadImage(out m_ho_Im, m_hv_ModelPath + "Im.tif");
            }

            HOperatorSet.ReadImage(out m_ho_Gi, m_hv_ModelPath + "Gi.tif");

            HObject HWidthImageS, HSkeletonsLG;
                       
            HOperatorSet.ReadImage(out HWidthImageS, m_hv_ModelPath + "WidthImageS.tif");
            HOperatorSet.ReadObject(out HSkeletonsLG, m_hv_ModelPath + "SkeletonsLG.hobj");
            
            m_hv_MaxPRC = 100;
//            TeachingMainFWM(m_hv_ModelPath, m_ho_Gi, m_ho_Im, m_ho_RegNoProc);
            TeachingLowGrayOnly(m_hv_ModelPath, HSkeletonsLG, HWidthImageS);

            //TeachingMain_Test(out hv_Text, out hv_Max, out hv_Value);
        }

        private void butTeach_Click(object sender, EventArgs e)
        {
            mThread_Teaching = new Thread(() => TeachingProc());
            mThread_Teaching.Name = "mThread_Unit_Layer_Draw";
            mThread_Teaching.Start();
            mThread_Teaching.IsBackground = true;

            butTeach.Enabled = false;
            butCancel.Enabled = true;

            timerThread.Start();
        }
        private void butCancel_Click(object sender, EventArgs e)
        {
            mThread_Teaching.Abort();
        }

        private void timerThread_Tick(object sender, EventArgs e)
        {
            ThreadState state = mThread_Teaching.ThreadState;
            if (state == ThreadState.Stopped || state == ThreadState.Aborted) // ThreadState.Background
            {
                timerThread.Stop();

                System.Text.StringBuilder messageBoxCS = new System.Text.StringBuilder();
                if (state == ThreadState.Aborted)
                {
                    messageBoxCS.AppendFormat("{0}", "Teaching has stopped");
                    messageBoxCS.AppendLine();
                    messageBoxCS.AppendFormat("{0}", "Teaching data may be corrupted!");
                    messageBoxCS.AppendLine();
                }
                messageBoxCS.AppendFormat("{0}", "Finish model teaching?");
                messageBoxCS.AppendLine();

                DialogResult dialogResult = MessageBox.Show(messageBoxCS.ToString(), "Teaching", MessageBoxButtons.YesNo);
                if (dialogResult == DialogResult.Yes)
                {
                    DialogResult = DialogResult.OK;
                }
                butTeach.Enabled = true;
                butCancel.Enabled = true;

                m_hv_MaxPRC = 100;
                m_hv_CurrentOper = "";
                m_hv_TotalPRC = 0;
            }

            prgBarTeach.Maximum = m_hv_MaxPRC;
            edTeachType.Text = m_hv_CurrentOper;
            prgBarTeach.Value = m_hv_TotalPRC;
        }

        //private bool TeachingMain_Test(out HTuple hv_Text, out HTuple hv_Max, out HTuple hv_Value)
        //{
        //    hv_Value = 0;

        //    hv_Text = "Teach TEST FWM";
        //    hv_Max = 1000;
        //    for (int i = 0; i < 1000; i++)
        //    {
        //        Thread.Sleep(1);
        //        hv_Value = i;
        //    }

        //    return true;
        //}
        private bool TeachingMainFWM(HTuple hv_path, HObject ho_Gi, HObject ho_Im, HObject ho_RegNoProc)
        {
            // Local iconic variables 

            HObject ho_Circlembscc = null, ho_DomainG, ho_RegionCut;
            HObject ho_RegionG, ho_RegDontCare, ho_RegSMboundary, ho_RegProc;
            HObject ho_DK, ho_RegionFlash = null, ho_RegionTrace = null;
            HObject ho_RegionPolygon = null, ho_GI_Flash = null, ho_GI_Trace = null;
            HObject ho_Outer1 = null, ho_Outer2 = null, ho_InnerA = null;
            HObject ho_InnerB = null, ho_Inner1 = null, ho_Inner2 = null;
            HObject ho_CareReg1 = null, ho_CareReg2 = null, ho_DKu;
            HObject ho_Region = null, ho_RegionGS, ho_SkeletonG, ho_SkeletonGS;
            HObject ho_WidthImageP, ho_DirImageP, ho_WidthImageS, ho_DirImageS;
            HObject ho_SkeletonsFWMi, ho_SkeletonsG, ho_ImageIR, ho_RegionsPNi, ho_SkeletonsLG;
            HObject ho_RegionsPNSi, ho_SkeletonFWMi, ho_RegionsNFWM;
            HObject ho_BoundariesNFWM, ho_SkeletonsFWMS, ho_RegionsGND;
            HObject ho_RegionsEMPTY, ho_ImDisplacement, ho_SkeletonPartsP;
            HObject ho_EPNFWMremove, ho_SkeletonPartsPnoG, ho_ImageIR16;
            HObject ho_ImageIRS, ho_RegionMS, ho_SkeletonsFWMcut, ho_SkeletonsFWMScut;
            HObject ho_RegionsPNPi, ho_RegionsmainSNoCut, ho_SkelEP;
            HObject ho_RegForbidP = null, ho_SkeletonsFWMii, ho_SkeletonsFWM;
            HObject ho_RegionNFWM, ho_RegionFWM, ho_ImageChainLength;
            HObject ho_RegionsNFWMm, ho_SkelsAPver, ho_CircsAPver, ho_R = null;
            HObject ho_PC = null, ho_P1, ho_PCS, ho_P1S, ho_P2S, ho_DomainIm;
            HObject ho_RC, ho_RegionI, ho_RegionPH, ho_RegionIsl, ho_RegionISnoIslnoPh;
            HObject ho_RegionInoIslnoPh, ho_RegionIS, ho_SkeletonGnoR;
            HObject ho_RegionsFWM, ho_RegionFWMdil, ho_RegSkel, ho_RegSkeld;
            HObject ho_SkeletonI, ho_EndPointsI, ho_JuncPointsI, ho_SkelPointsI;
            HObject ho_RegionCheck, ho_RegionsShi, ho_RegionsOp, ho_RegionsOpCon;
            HObject ho_R2 = null, ho_RegionOpCon, ho_RegionsShu, ho_RegionsSh;
            HObject ho_Reg0005 = null, ho_RegionInoD, ho_RegionsInoDn;
            HObject ho_RegionsInoDnp, ho_RegionsInoD, ho_RegionsShm;
            HObject ho_RegionsInoDBi, ho_SkeletonG2, ho_RegionInoDC = null;
            HObject ho_RegionsInoDC = null, ho_ImagePN, ho_RegionInoDe;
            HObject ho_RegionsInoDB, ho_ImageIRBo = null, ho_RegionInoDSi;
            HObject ho_RegionInoDS, ho_RegionsInoDS, ho_RCo, ho_RegionDifference;
            HObject ho_ImIRBSi, ho_ImIRBPi, ho_ImIRBP2, ho_ImIRBS, ho_RegionsFWMSkel;
            HObject ho_ImICN, ho_ImageResult, ho_RegionIntersectionFI;
            HObject ho_ImageIRB = null, ho_RegionNFWMm, ho_RegionNFWMmdil;
            HObject ho_RegionInFWM, ho_RegionDifference2, ho_RegionD;
            HObject ho_ImCLCS, ho_RegsMB, ho_RegsSpur, ho_RegionFWMcl;
            HObject ho_RegionInoDnoFWM, ho_MHoImageirb = null, ho_MLocalImage = null;
            HObject ho_Imageir16 = null, ho_Skeletonsfwm = null, ho_sk = null;
            HObject ho_Rcont = null, ho_Pc = null, ho_R1 = null, ho_RegionLine = null;
            HObject ho_PINT1 = null, ho_Rcontu = null, ho_Rcontd = null, ho_Regmbcc = null;
            HObject ho_Imageirb = null, ho_Img0001 = null, ho_HoMmregmbs = null;
            HObject ho_HoMmregsps = null, ho_HoRegionsop = null, ho_HoRegionsshi = null;
            HObject ho_Reg0001 = null, ho_Img0005 = null, ho_Img0015 = null;
            HObject ho_Reg0015 = null, ho_Img0025 = null, ho_Reg0025 = null;
            HObject ho_Img0002 = null, ho_Reg0002 = null, ho_Img0003 = null;
            HObject ho_Reg0003 = null, ho_Img0010 = null, ho_Reg0010 = null;
            HObject ho_Img = null, ho_Reg = null, ho_CircleSP = null, ho_CircleSPU = null;
            HObject ho_CircleSPUC = null, ho_CirclesSP = null, ho_HoRegionsh = null;
            HObject ho_CSP = null, ho_RegionIntersection = null, ho_HoMmregspsU = null;
            HObject ho_HoMmregspsUC = null, ho_EndPointsF = null, ho_Circlembcc = null;
            HObject ho_Circlespcc = null, ho_AllRC = null, ho_Mmregmbss = null;
            HObject ho_Regmbscc = null, ho_bnd = null, ho_RCgc = null, ho_RegionOp = null;
            HObject ho_RegionSh = null, ho_Empty = null, ho_RegionShOpi = null;
            HObject ho_RegionShOp = null, ho_Rmbnfwm = null, ho_Rspnfwm = null;
            HObject ho_RegDefs = null, ho_RegMBcc = null, ho_RegSPcc = null;
            HObject ho_RegMBScc = null, ho_ImR = null, ho_ImageIlluminate = null;
            HObject ho_RegionInoDB, ho_RegionDifference1 = null, ho_ho_Rmbfarfwm = null;
            HObject ho_ho_RconnectedNFWMmb = null, ho_Regsmbnall = null;
            HObject ho_Regsspnall = null, ho_RMBnfwm = null, ho_RSPnfwm = null;
            HObject ho_ObjectSelected = null, ho_PP1 = null, ho_PP2 = null;
            HObject ho_RegionInoDr = null, ho_RegsMBNall = null, ho_RegsSPNall = null;
            HObject ho_RegionGi = null, ho_Regsspnnofwm = null, ho_RegionFWMPA = null;
            HObject ho_Regsmbnnofwm = null, ho_Rclfwm = null, ho_RegsMBNcnof = null;
            HObject ho_Rclfwmbutnfrm = null, ho_RconnectedNFWM = null, ho_Circle1 = null;
            HObject ho_CircleMB = null, ho_RegsSPNcnof = null, ho_Mmregmbs = null;
            HObject ho_RIntr = null, ho_CirclesMBcc = null, ho_CirclesSPcc = null;
            HObject ho_SkeletonInoD = null, ho_RegMBNc = null, ho_CirclesMBNc = null;
            HObject ho_CircleMBNc = null, ho_RegSPNc = null, ho_CirclesSPNc = null;
            HObject ho_CircleSPNc = null, ho_AllPoints = null, ho_RegionGu;
            HObject ho_EndPointsu = null, ho_RegionGB, ho_EndPoints, ho_Rectangles;
            HObject ho_Forb, ho_Rectanglesu, ho_RegionExpandedi = null;
            HObject ho_RegionExpanded, ho_RInod = null, ho_RInodFillUp = null;
            HObject ho_RHole = null, ho_RHoles = null, ho_Hole = null, ho_HoleB = null;
            HObject ho_Ps, ho_Psl, ho_Psls, ho_CirclesMin1, ho_RegionUnion;
            HObject ho_CirclesMin, ho_RInoDB = null, ho_Regionb, ho_Pss;
            HObject ho_CirclesMax, ho_CirclesSpur, ho_CirclesMB, ho_EndPointsFs;
            HObject ho_CFE, ho_RegionsEndFWM, ho_REndFWM = null, ho_EP = null;
            HObject ho_RCEndFWM = null, ho_RegsspnnofwmR = null, ho_CSPN;
            HObject ho_RegsSP, ho_MBreal, ho_CircleMBr, ho_SPreal, ho_CircleSPr;
            HObject ho_RInoD = null, ho_RegPDCV = null, ho_RegPDCH = null;
            HObject ho_RegionIii, ho_Regsp, ho_Regmb, ho_Regmbs, ho_Regsps;
            HObject ho_CircleSPi, ho_CircleMBi;

            // Local control variables 
            HTuple hv_autowidth = null, hv_EPexp = null;
            HTuple hv_minPSklen = null, hv_lpnum2 = null; // New paramtrs 2025/06/02

            HTuple hv_n = new HTuple(), hv_minWG = null, hv_minWGS = null;
            HTuple hv_wpl = null, hv_kum = null, hv_fwmhw = null, hv_parop = null;
            HTuple hv_GNDamin = null, hv_TO = null, hv_marginL = new HTuple();
            HTuple hv_marginR = new HTuple(), hv_marginU = new HTuple();
            HTuple hv_marginD = new HTuple(), hv_ovlL = new HTuple();
            HTuple hv_ovlR = new HTuple(), hv_gap = null, hv_jrad = null;
            HTuple hv_sm_factor = null, hv_lookaround = null, hv_groundpn = null;
            HTuple hv_groundgval = null, hv_useODB = null;
            HTuple hv_Row11 = null, hv_Column11 = null, hv_Row21 = null;
            HTuple hv_Column21 = null, hv_pathTeach = null, hv_pathTest = null;
            HTuple hv_pathN = null, hv_smoothCADcontours = null, hv_maxdirSP = null;
            HTuple hv_wFWM = null, hv_wFWMS = null, hv_wside = null;
            HTuple hv_wsideS = null, hv_eradNFWMremove = null, hv_eradMSe = null;
            HTuple hv_elengthend = null, hv_elengthnoend = null, hv_cutendMSS = null;
            HTuple hv_jpadd = null, hv_removeGNDfromFWM = null, hv_WidthG = null;
            HTuple hv_HeightG = null, hv_t1 = null, hv_AreaDK = null;
            HTuple hv_RowDK = null, hv_ColumnDK = null, hv_nDK = null;
            HTuple hv_IndicesDK = new HTuple(), hv_InvertedDK = new HTuple();
            HTuple hv_AIsSubset = new HTuple(), hv_Area3 = null, hv_RowDKu = null;
            HTuple hv_ColumnDKu = null, hv_nFWM = null, hv_nFWMS = null;
            HTuple hv_nNonFWM = null, hv_WG = null, hv_YC = null, hv_XC = null;
            HTuple hv_Y1 = null, hv_X1 = null, hv_Y2 = null, hv_X2 = null;
            HTuple hv_Pnum = null, hv_PnumR = null, hv_PregC = null;
            HTuple hv_PcontR = null, hv_PaddrR = null, hv_PaddrC = null;
            HTuple hv_PCinRnum = null, hv_PuseintP = null, hv_WGS = null;
            HTuple hv_YCS = null, hv_XCS = null, hv_Y1S = null, hv_X1S = null;
            HTuple hv_Y2S = null, hv_X2S = null, hv_PnumS = null, hv_PnumRS = null;
            HTuple hv_PregCS = null, hv_PcontRS = null, hv_PaddrRS = null;
            HTuple hv_PaddrCS = null, hv_PCinRnumS = null, hv_PuseintS = null;
            HTuple hv_SequenceIRB = null, hv_SequenceIRBS = null, hv_WGN1 = null;
            HTuple hv_YCN = null, hv_XCN = null, hv_Y1N = null, hv_X1N = null;
            HTuple hv_Y2N = null, hv_X2N = null, hv_PnumN = null, hv_PnumRN = null;
            HTuple hv_PregCN = null, hv_PcontRN1 = null, hv_PaddrRN = null;
            HTuple hv_PaddrCN = null, hv_PCinRnumN = null, hv_Pnum2 = null;
            HTuple hv_Pnum2S = null, hv_nFWMii = null, hv_nPNPi = null;
            HTuple hv_pathTeachT = null, hv_nGND = new HTuple(), hv_nEMPTY = null;
            HTuple hv_epr = null, hv_significantangleSP = null, hv_t2 = null;
            HTuple hv_t = null, hv_pathd = null, hv_WN = new HTuple();
            HTuple hv_pathIm = null, hv_thr = null, hv_thr2 = null;
            HTuple hv_fumax = null, hv_aislmax = null, hv_PHrmin = null;
            HTuple hv_PHamin = null, hv_Islrmin = null, hv_Islamin = null;
            HTuple hv_nIsl = null, hv_d1 = null, hv_s1 = null, hv_Width = null;
            HTuple hv_Height = null, hv_aphmax = null, hv_s2 = null;
            HTuple hv_s = null, hv_dilS = null, hv_ClS = null, hv_dilFWM = null;
            HTuple hv_s3 = null, hv_sprep = null, hv_ssh = null, hv_nOp = null;
            HTuple hv_s4 = null, hv_sop = null, hv_nShi = null, hv_nSh = null;
            HTuple hv_thrc = null, hv_nInoD = null, hv_lsz = null;
            HTuple hv_lstep = null, hv_expsize = null, hv_dil = null;
            HTuple hv_nEmpty = null, hv_nFWMSB = null, hv_nreg = null;
            HTuple hv_stseqS = null, hv_overfS = null, hv_g = null;
            HTuple hv_SequenceICN = null, hv_sz = null, hv_stseq = new HTuple();
            HTuple hv_overf = new HTuple(), hv_dmin = null, hv_dmax = null;
            HTuple hv_Rows = new HTuple(), hv_gMB = null, hv_gSpur = null;
            HTuple hv_dist = null, hv_nMBda = null, hv_nSpurda = null;
            HTuple hv_SPregs = null, hv_MBregs = null, hv_prcmb = null;
            HTuple hv_prcsp = null, hv_prcsh = null, hv_nAddrC = new HTuple();
            HTuple hv_gIRB = new HTuple(), hv_Wall = new HTuple();
            HTuple hv_Wallmin = new HTuple(), hv_pat0space1 = new HTuple();
            HTuple hv_Prc = new HTuple(), hv_Def = new HTuple(), hv_nc = new HTuple();
            HTuple hv_cbeg = new HTuple(), hv_k = new HTuple(), hv_W = new HTuple();
            HTuple hv_Wd = new HTuple(), hv_NINT = new HTuple(), hv_PINTx1 = new HTuple();
            HTuple hv_PINTy1 = new HTuple(), hv_PINTx2 = new HTuple();
            HTuple hv_PINTy2 = new HTuple(), hv_ncont = new HTuple();
            HTuple hv_ab = new HTuple(), hv_a = new HTuple(), hv_ae = new HTuple();
            HTuple hv_nP = new HTuple(), hv_numc = new HTuple(), hv_pnum = new HTuple();
            HTuple hv_pnum2 = new HTuple(), hv_Wmin = new HTuple();
            HTuple hv_Wch = new HTuple(), hv_Wchmin = new HTuple();
            HTuple hv_Prcmin = new HTuple(), hv_Wfilt = new HTuple();
            HTuple hv_Sh = new HTuple(), hv_Xc = new HTuple(), hv_Yc = new HTuple();
            HTuple hv_fwm = new HTuple(), hv_j = new HTuple(), hv_x1 = new HTuple();
            HTuple hv_y1 = new HTuple(), hv_x2 = new HTuple(), hv_y2 = new HTuple();
            HTuple hv_xc = new HTuple(), hv_yc = new HTuple(), hv_ix1 = new HTuple();
            HTuple hv_iy1 = new HTuple(), hv_ix2 = new HTuple(), hv_iy2 = new HTuple();
            HTuple hv_ixc = new HTuple(), hv_iyc = new HTuple(), hv_sub = new HTuple();
            HTuple hv_wg = new HTuple(), hv_clcmax = new HTuple();
            HTuple hv_PIntXS = new HTuple(), hv_PIntYS = new HTuple();
            HTuple hv_w = new HTuple(), hv_wmin = new HTuple(), hv_sh = new HTuple();
            HTuple hv_normint = new HTuple(), hv_merr = new HTuple();
            HTuple hv_wd = new HTuple(), hv_szmis = new HTuple(), hv_Defmis = new HTuple();
            HTuple hv_Defmin = new HTuple(), hv_Wfiltmin = new HTuple();
            HTuple hv_wmed = new HTuple(), hv_Defsh = new HTuple();
            HTuple hv_Wfiltsh = new HTuple(), hv_Wchsh = new HTuple();
            HTuple hv_Prcsh = new HTuple(), hv_FWG = new HTuple();
            HTuple hv_FWGsm = new HTuple(), hv_XValues2 = new HTuple();
            HTuple hv_WGsm = new HTuple(), hv_DefG = new HTuple();
            HTuple hv_WGfilt = new HTuple(), hv_WGch = new HTuple();
            HTuple hv_PrcG = new HTuple(), hv_wmean = new HTuple();
            HTuple hv_wgmean = new HTuple(), hv_dw = new HTuple();
            HTuple hv_RpcReal = new HTuple(), hv_maxdef = new HTuple();
            HTuple hv_Indices1 = new HTuple(), hv_maxPrc = new HTuple();
            HTuple hv_mindef = new HTuple(), hv_minPrc = new HTuple();
            HTuple hv_Rowssk = new HTuple(), hv_Columnssk = new HTuple();
            HTuple hv_Wold = new HTuple(), hv_FW = new HTuple(), hv_FWsm = new HTuple();
            HTuple hv_lF = new HTuple(), hv_DLen = new HTuple(), hv_Max = new HTuple();
            HTuple hv_WD = new HTuple(), hv_wdmed = new HTuple(), hv_wgmed = new HTuple();
            HTuple hv_prcgr = new HTuple(), hv_PWidthtuple = new HTuple();
            HTuple hv_PWidthtupleS = new HTuple(), hv_XValues = new HTuple();
            HTuple hv_WGav = new HTuple(), hv_l = new HTuple(), hv_WGA = new HTuple();
            HTuple hv_WGsmC = new HTuple(), hv_WS = new HTuple(), hv_WS2 = new HTuple();
            HTuple hv_DefD = new HTuple(), hv_WDfilt = new HTuple();
            HTuple hv_WDch = new HTuple(), hv_PrcD = new HTuple();
            HTuple hv_PWidth_tuple = new HTuple(), hv_Selected1 = new HTuple();
            HTuple hv_WmedExt = new HTuple(), hv_Selected3 = new HTuple();
            HTuple hv_Ymed = new HTuple(), hv_nAll = new HTuple();
            HTuple hv_Wr = new HTuple(), hv_Selected = new HTuple();
            HTuple hv_Dif = new HTuple(), hv_difmax = new HTuple();
            HTuple hv_difmin = new HTuple(), hv_pn = new HTuple();
            HTuple hv_SPregs1 = new HTuple(), hv_Wmedext = new HTuple();
            HTuple hv_WGM = new HTuple(), hv_XCM = new HTuple(), hv_YCM = new HTuple();
            HTuple hv_X1M = new HTuple(), hv_Y1M = new HTuple(), hv_X2M = new HTuple();
            HTuple hv_Y2M = new HTuple(), hv_PnumM = new HTuple();
            HTuple hv_PnumRM = new HTuple(), hv_PaddrRM = new HTuple();
            HTuple hv_PaddrCM = new HTuple(), hv_PregCM = new HTuple();
            HTuple hv_PCinRnumM = new HTuple(), hv_PcontRM = new HTuple();
            HTuple hv_Width1 = new HTuple(), hv_Height1 = new HTuple();
            HTuple hv_Widtho = new HTuple(), hv_Heighto = new HTuple();
            HTuple hv_WM = new HTuple(), hv_pathDef = new HTuple();
            HTuple hv_pathDefImage = new HTuple(), hv_pathDefObj = new HTuple();
            HTuple hv_nMBs = new HTuple(), hv_nSPs1 = new HTuple();
            HTuple hv_AreaSP = new HTuple(), hv_RowSP = new HTuple();
            HTuple hv_ColumnSP = new HTuple(), hv_rSP = new HTuple();
            HTuple hv_RSP = new HTuple(), hv_nSP = new HTuple(), hv_Row2 = new HTuple();
            HTuple hv_Column2 = new HTuple(), hv_nSPs2 = new HTuple();
            HTuple hv_nAddrCS = new HTuple(), hv_i = null, hv_r = new HTuple();
            HTuple hv_c = new HTuple(), hv_PINTxS1 = new HTuple();
            HTuple hv_PINTyS1 = new HTuple(), hv_PINTxS2 = new HTuple();
            HTuple hv_PINTyS2 = new HTuple(), hv_WminS = new HTuple();
            HTuple hv_PrcS = new HTuple(), hv_PIntX = new HTuple();
            HTuple hv_PIntY = new HTuple(), hv_szw = new HTuple();
            HTuple hv_PWidthS_tuple = new HTuple(), hv_medoS_tuple = new HTuple();
            HTuple hv_rnum = new HTuple(), hv_DefN = new HTuple();
            HTuple hv_cnum = new HTuple(), hv_rnum2 = new HTuple();
            HTuple hv_Columns = new HTuple(), hv_abeg = new HTuple();
            HTuple hv_aend = new HTuple(), hv_X1c = new HTuple(), hv_Y1c = new HTuple();
            HTuple hv_Y1s = new HTuple(), hv_X1s = new HTuple(), hv_Y2s = new HTuple();
            HTuple hv_X2s = new HTuple(), hv_rmin = new HTuple(), hv_cmin = new HTuple();
            HTuple hv_radmin = new HTuple(), hv_WminN = new HTuple();
            HTuple hv_jj = new HTuple(), hv_addr = new HTuple(), hv_wN = new HTuple();
            HTuple hv_wminN = new HTuple(), hv_FDev = new HTuple();
            HTuple hv_prcp = new HTuple(), hv_prcm = new HTuple();
            HTuple hv_mabs = new HTuple(), hv_pabs = new HTuple();
            HTuple hv_crad = new HTuple(), hv_dpath = new HTuple();
            HTuple hv_dname = new HTuple(), hv_dnameSp = new HTuple();
            HTuple hv_TuwC = new HTuple(), hv_TuwG = new HTuple();
            HTuple hv_Equal = new HTuple(), hv_Equale = new HTuple();
            HTuple hv_em = new HTuple(), hv_IndicesE = new HTuple();
            HTuple hv_Sub = new HTuple(), hv_Subu = new HTuple(), hv_Z = new HTuple();
            HTuple hv_Greater1 = new HTuple(), hv_refrad = new HTuple();
            HTuple hv_viathr = new HTuple(), hv_viamin = new HTuple();
            HTuple hv_viamax = new HTuple(), hv_viadisplmax = new HTuple();
            HTuple hv_Area2 = new HTuple(), hv_Row1 = new HTuple();
            HTuple hv_Column1 = new HTuple(), hv_dfwm = new HTuple();
            HTuple hv_Puseint_tuple = new HTuple(), hv_PWmin_tuple = new HTuple();
            HTuple hv_jm = new HTuple(), hv_w1 = new HTuple(), hv_w2 = new HTuple();
            HTuple hv_prc = new HTuple(), hv_Wt = new HTuple(), hv_Wmint = new HTuple();
            HTuple hv_PInt1y_tuple = new HTuple(), hv_PInt1x_tuple = new HTuple();
            HTuple hv_PInt2y_tuple = new HTuple(), hv_PInt2x_tuple = new HTuple();
            HTuple hv_Cy = null, hv_Cx = null, hv_PShift_tuple = new HTuple();
            HTuple hv_fwmpatype = new HTuple(), hv_nMBAll = new HTuple();
            HTuple hv_nMBNFWMnoFWM = new HTuple(), hv_nSPAll = new HTuple();
            HTuple hv_nSPNFWMnoFWM = new HTuple(), hv_expv = new HTuple();
            HTuple hv_N15 = new HTuple(), hv_N20 = new HTuple(), hv_MBNregnum_tuple = new HTuple();
            HTuple hv_nMBNcnof = new HTuple(), hv_nMBNall = new HTuple();
            HTuple hv_MBNregnum = new HTuple(), hv_ncontb = new HTuple();
            HTuple hv_Wrp = new HTuple(), hv_PInt1xr = new HTuple();
            HTuple hv_PInt1yr = new HTuple(), hv_nMB = new HTuple();
            HTuple hv_NMB = new HTuple(), hv_NSP = new HTuple(), hv_Prc2 = new HTuple();
            HTuple hv_Wg = new HTuple(), hv_Wfilt1 = new HTuple();
            HTuple hv_Wch1 = new HTuple(), hv_Prc3 = new HTuple();
            HTuple hv_DW = new HTuple(), hv_nEP = null, hv_A = null;
            HTuple hv_Row = null, hv_Col = null, hv_Hw = null, hv_Dir = null;
            HTuple hv_Ang = null, hv_Ones = null, hv_RowsC = new HTuple();
            HTuple hv_ColsC = new HTuple(), hv_n1 = new HTuple(), hv_Seq = new HTuple();
            HTuple hv_nConnected = new HTuple(), hv_nHoles = new HTuple();
            HTuple hv_n2 = new HTuple(), hv_CNum = null, hv_CAddr = null;
            HTuple hv_CSeq = null, hv_Displacement1 = null, hv_Displacement = null;
            HTuple hv_nPDev = null, hv_LPlus = null, hv_Greater = null;
            HTuple hv_Sel = null, hv_Cys = null, hv_Cxs = null, hv_LMin = null;
            HTuple hv_Less = null, hv_Cysl = null, hv_Cxsl = null;
            HTuple hv_nMin = null, hv_Area = null, hv_rc = null, hv_Rc = null;
            HTuple hv_Indices = null, hv_Inverted = null, hv_Dev = null;
            HTuple hv_nInoDB = null, hv_RowsInoDB = new HTuple(), hv_ColsInoDB = new HTuple();
            HTuple hv_t3 = null, hv_nMax = null, hv_RowsFE = null;
            HTuple hv_ColsFE = null, hv_nFE = null, hv_rfe = null;
            HTuple hv_RFE = null, hv_RowsEndFWM = new HTuple(), hv_ColsEndFWM = new HTuple();
            HTuple hv_Dp1 = new HTuple(), hv_Dp = new HTuple(), hv_DistanceMin = new HTuple();
            HTuple hv_DistanceMax = new HTuple(), hv_nSPN = null, hv_Area1 = null;
            HTuple hv_Column = null, hv_RSPN = null, hv_DisplacementA1 = null;
            HTuple hv_grayspaceav = null, hv_grayspacemax = null, hv_graypatav = null;
            HTuple hv_graypatmin = null, hv_expandMB = null, hv_expandSP = null;
            HTuple hv_marg = null, hv_prcA = null, hv_tmb = null, hv_nMBreal = null;
            HTuple hv_TR = null, hv_tsp = null, hv_nSPreal = null;
            HTuple hv_RowsInoD = new HTuple(), hv_ColsInoD = new HTuple();
            HTuple hv_nId = new HTuple(), hv_Rsel = new HTuple(), hv_Csel = new HTuple();
            HTuple hv_Grayval = new HTuple(), hv_Pn = new HTuple();
            HTuple hv_CPN = null, hv_orc = null, hv_orr = null, hv_nr = null;
            HTuple hv_AreaSPs = null, hv_RowSPs = null, hv_ColumnSPs = null;
            HTuple hv_AreaMBs = null, hv_RowMBs = null, hv_ColumnMBs = null;
            HTuple hv_elongLG = null;

            HTupleVector hvec_VectorCX = new HTupleVector(1);
            HTupleVector hvec_VectorCY = new HTupleVector(1), hvec_VaddrC = new HTupleVector(1);
            HTupleVector hvec_VnumC = new HTupleVector(1), hvec_VseqC = new HTupleVector(1);
            HTupleVector hvec_VpnC = new HTupleVector(1);

            {
                HOperatorSet.GenEmptyObj(out ho_Circlembscc);
                HOperatorSet.GenEmptyObj(out ho_DomainG);
                HOperatorSet.GenEmptyObj(out ho_RegionCut);
                HOperatorSet.GenEmptyObj(out ho_RegionG);
                HOperatorSet.GenEmptyObj(out ho_RegDontCare);
                HOperatorSet.GenEmptyObj(out ho_RegSMboundary);
                HOperatorSet.GenEmptyObj(out ho_RegProc);
                HOperatorSet.GenEmptyObj(out ho_DK);
                HOperatorSet.GenEmptyObj(out ho_RegionFlash);
                HOperatorSet.GenEmptyObj(out ho_RegionTrace);
                HOperatorSet.GenEmptyObj(out ho_RegionPolygon);
                HOperatorSet.GenEmptyObj(out ho_GI_Flash);
                HOperatorSet.GenEmptyObj(out ho_GI_Trace);
                HOperatorSet.GenEmptyObj(out ho_Outer1);
                HOperatorSet.GenEmptyObj(out ho_Outer2);
                HOperatorSet.GenEmptyObj(out ho_InnerA);
                HOperatorSet.GenEmptyObj(out ho_InnerB);
                HOperatorSet.GenEmptyObj(out ho_Inner1);
                HOperatorSet.GenEmptyObj(out ho_Inner2);
                HOperatorSet.GenEmptyObj(out ho_CareReg1);
                HOperatorSet.GenEmptyObj(out ho_CareReg2);
                HOperatorSet.GenEmptyObj(out ho_DKu);
                HOperatorSet.GenEmptyObj(out ho_Region);
                HOperatorSet.GenEmptyObj(out ho_RegionGS);
                HOperatorSet.GenEmptyObj(out ho_SkeletonG);
                HOperatorSet.GenEmptyObj(out ho_SkeletonGS);
                HOperatorSet.GenEmptyObj(out ho_WidthImageP);
                HOperatorSet.GenEmptyObj(out ho_DirImageP);
                HOperatorSet.GenEmptyObj(out ho_WidthImageS);
                HOperatorSet.GenEmptyObj(out ho_DirImageS);
                HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMi);
                HOperatorSet.GenEmptyObj(out ho_SkeletonsG);
                HOperatorSet.GenEmptyObj(out ho_ImageIR);
                HOperatorSet.GenEmptyObj(out ho_RegionsPNi);
                HOperatorSet.GenEmptyObj(out ho_RegionsPNSi);
                HOperatorSet.GenEmptyObj(out ho_SkeletonFWMi);
                HOperatorSet.GenEmptyObj(out ho_RegionsNFWM);
                HOperatorSet.GenEmptyObj(out ho_BoundariesNFWM);
                HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMS);
                HOperatorSet.GenEmptyObj(out ho_RegionsGND);
                HOperatorSet.GenEmptyObj(out ho_RegionsEMPTY);
                HOperatorSet.GenEmptyObj(out ho_ImDisplacement);
                HOperatorSet.GenEmptyObj(out ho_SkeletonPartsP);
                HOperatorSet.GenEmptyObj(out ho_EPNFWMremove);
                HOperatorSet.GenEmptyObj(out ho_SkeletonPartsPnoG);
                HOperatorSet.GenEmptyObj(out ho_ImageIR16);
                HOperatorSet.GenEmptyObj(out ho_ImageIRS);
                HOperatorSet.GenEmptyObj(out ho_RegionMS);
                HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMcut);
                HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMScut);
                HOperatorSet.GenEmptyObj(out ho_RegionsPNPi);
                HOperatorSet.GenEmptyObj(out ho_RegionsmainSNoCut);
                HOperatorSet.GenEmptyObj(out ho_SkelEP);
                HOperatorSet.GenEmptyObj(out ho_RegForbidP);
                HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMii);
                HOperatorSet.GenEmptyObj(out ho_SkeletonsFWM);
                HOperatorSet.GenEmptyObj(out ho_SkeletonsLG);
                HOperatorSet.GenEmptyObj(out ho_RegionNFWM);
                HOperatorSet.GenEmptyObj(out ho_RegionFWM);
                HOperatorSet.GenEmptyObj(out ho_ImageChainLength);
                HOperatorSet.GenEmptyObj(out ho_RegionsNFWMm);
                HOperatorSet.GenEmptyObj(out ho_SkelsAPver);
                HOperatorSet.GenEmptyObj(out ho_CircsAPver);
                HOperatorSet.GenEmptyObj(out ho_R);
                HOperatorSet.GenEmptyObj(out ho_PC);
                HOperatorSet.GenEmptyObj(out ho_P1);
                HOperatorSet.GenEmptyObj(out ho_PCS);
                HOperatorSet.GenEmptyObj(out ho_P1S);
                HOperatorSet.GenEmptyObj(out ho_P2S);
                HOperatorSet.GenEmptyObj(out ho_DomainIm);
                HOperatorSet.GenEmptyObj(out ho_RC);
                HOperatorSet.GenEmptyObj(out ho_RegionI);
                HOperatorSet.GenEmptyObj(out ho_RegionPH);
                HOperatorSet.GenEmptyObj(out ho_RegionIsl);
                HOperatorSet.GenEmptyObj(out ho_RegionISnoIslnoPh);
                HOperatorSet.GenEmptyObj(out ho_RegionInoIslnoPh);
                HOperatorSet.GenEmptyObj(out ho_RegionIS);
                HOperatorSet.GenEmptyObj(out ho_SkeletonGnoR);
                HOperatorSet.GenEmptyObj(out ho_RegionsFWM);
                HOperatorSet.GenEmptyObj(out ho_RegionFWMdil);
                HOperatorSet.GenEmptyObj(out ho_RegSkel);
                HOperatorSet.GenEmptyObj(out ho_RegSkeld);
                HOperatorSet.GenEmptyObj(out ho_SkeletonI);
                HOperatorSet.GenEmptyObj(out ho_EndPointsI);
                HOperatorSet.GenEmptyObj(out ho_JuncPointsI);
                HOperatorSet.GenEmptyObj(out ho_SkelPointsI);
                HOperatorSet.GenEmptyObj(out ho_RegionCheck);
                HOperatorSet.GenEmptyObj(out ho_RegionsShi);
                HOperatorSet.GenEmptyObj(out ho_RegionsOp);
                HOperatorSet.GenEmptyObj(out ho_RegionsOpCon);
                HOperatorSet.GenEmptyObj(out ho_R2);
                HOperatorSet.GenEmptyObj(out ho_RegionOpCon);
                HOperatorSet.GenEmptyObj(out ho_RegionsShu);
                HOperatorSet.GenEmptyObj(out ho_RegionsSh);
                HOperatorSet.GenEmptyObj(out ho_Reg0005);
                HOperatorSet.GenEmptyObj(out ho_RegionInoD);
                HOperatorSet.GenEmptyObj(out ho_RegionsInoDn);
                HOperatorSet.GenEmptyObj(out ho_RegionsInoDnp);
                HOperatorSet.GenEmptyObj(out ho_RegionsInoD);
                HOperatorSet.GenEmptyObj(out ho_RegionsShm);
                HOperatorSet.GenEmptyObj(out ho_RegionsInoDBi);
                HOperatorSet.GenEmptyObj(out ho_SkeletonG2);
                HOperatorSet.GenEmptyObj(out ho_RegionInoDC);
                HOperatorSet.GenEmptyObj(out ho_RegionsInoDC);
                HOperatorSet.GenEmptyObj(out ho_ImagePN);
                HOperatorSet.GenEmptyObj(out ho_RegionInoDe);
                HOperatorSet.GenEmptyObj(out ho_RegionsInoDB);
                HOperatorSet.GenEmptyObj(out ho_ImageIRBo);
                HOperatorSet.GenEmptyObj(out ho_RegionInoDSi);
                HOperatorSet.GenEmptyObj(out ho_RegionInoDS);
                HOperatorSet.GenEmptyObj(out ho_RegionsInoDS);
                HOperatorSet.GenEmptyObj(out ho_RCo);
                HOperatorSet.GenEmptyObj(out ho_RegionDifference);
                HOperatorSet.GenEmptyObj(out ho_ImIRBSi);
                HOperatorSet.GenEmptyObj(out ho_ImIRBPi);
                HOperatorSet.GenEmptyObj(out ho_ImIRBP2);
                HOperatorSet.GenEmptyObj(out ho_ImIRBS);
                HOperatorSet.GenEmptyObj(out ho_RegionsFWMSkel);
                HOperatorSet.GenEmptyObj(out ho_ImICN);
                HOperatorSet.GenEmptyObj(out ho_ImageResult);
                HOperatorSet.GenEmptyObj(out ho_RegionIntersectionFI);
                HOperatorSet.GenEmptyObj(out ho_ImageIRB);
                HOperatorSet.GenEmptyObj(out ho_RegionNFWMm);
                HOperatorSet.GenEmptyObj(out ho_RegionNFWMmdil);
                HOperatorSet.GenEmptyObj(out ho_RegionInFWM);
                HOperatorSet.GenEmptyObj(out ho_RegionDifference2);
                HOperatorSet.GenEmptyObj(out ho_RegionD);
                HOperatorSet.GenEmptyObj(out ho_ImCLCS);
                HOperatorSet.GenEmptyObj(out ho_RegsMB);
                HOperatorSet.GenEmptyObj(out ho_RegsSpur);
                HOperatorSet.GenEmptyObj(out ho_RegionFWMcl);
                HOperatorSet.GenEmptyObj(out ho_RegionInoDnoFWM);
                HOperatorSet.GenEmptyObj(out ho_MHoImageirb);
                HOperatorSet.GenEmptyObj(out ho_MLocalImage);
                HOperatorSet.GenEmptyObj(out ho_Imageir16);
                HOperatorSet.GenEmptyObj(out ho_Skeletonsfwm);
                HOperatorSet.GenEmptyObj(out ho_sk);
                HOperatorSet.GenEmptyObj(out ho_Rcont);
                HOperatorSet.GenEmptyObj(out ho_Pc);
                HOperatorSet.GenEmptyObj(out ho_R1);
                HOperatorSet.GenEmptyObj(out ho_RegionLine);
                HOperatorSet.GenEmptyObj(out ho_PINT1);
                HOperatorSet.GenEmptyObj(out ho_Rcontu);
                HOperatorSet.GenEmptyObj(out ho_Rcontd);
                HOperatorSet.GenEmptyObj(out ho_Regmbcc);
                HOperatorSet.GenEmptyObj(out ho_Imageirb);
                HOperatorSet.GenEmptyObj(out ho_Img0001);
                HOperatorSet.GenEmptyObj(out ho_HoMmregmbs);
                HOperatorSet.GenEmptyObj(out ho_HoMmregsps);
                HOperatorSet.GenEmptyObj(out ho_HoRegionsop);
                HOperatorSet.GenEmptyObj(out ho_HoRegionsshi);
                HOperatorSet.GenEmptyObj(out ho_Reg0001);
                HOperatorSet.GenEmptyObj(out ho_Img0005);
                HOperatorSet.GenEmptyObj(out ho_Img0015);
                HOperatorSet.GenEmptyObj(out ho_Reg0015);
                HOperatorSet.GenEmptyObj(out ho_Img0025);
                HOperatorSet.GenEmptyObj(out ho_Reg0025);
                HOperatorSet.GenEmptyObj(out ho_Img0002);
                HOperatorSet.GenEmptyObj(out ho_Reg0002);
                HOperatorSet.GenEmptyObj(out ho_Img0003);
                HOperatorSet.GenEmptyObj(out ho_Reg0003);
                HOperatorSet.GenEmptyObj(out ho_Img0010);
                HOperatorSet.GenEmptyObj(out ho_Reg0010);
                HOperatorSet.GenEmptyObj(out ho_Img);
                HOperatorSet.GenEmptyObj(out ho_Reg);
                HOperatorSet.GenEmptyObj(out ho_CircleSP);
                HOperatorSet.GenEmptyObj(out ho_CircleSPU);
                HOperatorSet.GenEmptyObj(out ho_CircleSPUC);
                HOperatorSet.GenEmptyObj(out ho_CirclesSP);
                HOperatorSet.GenEmptyObj(out ho_HoRegionsh);
                HOperatorSet.GenEmptyObj(out ho_CSP);
                HOperatorSet.GenEmptyObj(out ho_RegionIntersection);
                HOperatorSet.GenEmptyObj(out ho_HoMmregspsU);
                HOperatorSet.GenEmptyObj(out ho_HoMmregspsUC);
                HOperatorSet.GenEmptyObj(out ho_EndPointsF);
                HOperatorSet.GenEmptyObj(out ho_Circlembcc);
                HOperatorSet.GenEmptyObj(out ho_Circlespcc);
                HOperatorSet.GenEmptyObj(out ho_AllRC);
                HOperatorSet.GenEmptyObj(out ho_Mmregmbss);
                HOperatorSet.GenEmptyObj(out ho_Regmbscc);
                HOperatorSet.GenEmptyObj(out ho_bnd);
                HOperatorSet.GenEmptyObj(out ho_RCgc);
                HOperatorSet.GenEmptyObj(out ho_RegionOp);
                HOperatorSet.GenEmptyObj(out ho_RegionSh);
                HOperatorSet.GenEmptyObj(out ho_Empty);
                HOperatorSet.GenEmptyObj(out ho_RegionShOpi);
                HOperatorSet.GenEmptyObj(out ho_RegionShOp);
                HOperatorSet.GenEmptyObj(out ho_Rmbnfwm);
                HOperatorSet.GenEmptyObj(out ho_Rspnfwm);
                HOperatorSet.GenEmptyObj(out ho_RegDefs);
                HOperatorSet.GenEmptyObj(out ho_RegMBcc);
                HOperatorSet.GenEmptyObj(out ho_RegSPcc);
                HOperatorSet.GenEmptyObj(out ho_RegMBScc);
                HOperatorSet.GenEmptyObj(out ho_ImR);
                HOperatorSet.GenEmptyObj(out ho_ImageIlluminate);
                HOperatorSet.GenEmptyObj(out ho_RegionInoDB);
                HOperatorSet.GenEmptyObj(out ho_RegionDifference1);
                HOperatorSet.GenEmptyObj(out ho_ho_Rmbfarfwm);
                HOperatorSet.GenEmptyObj(out ho_ho_RconnectedNFWMmb);
                HOperatorSet.GenEmptyObj(out ho_Regsmbnall);
                HOperatorSet.GenEmptyObj(out ho_Regsspnall);
                HOperatorSet.GenEmptyObj(out ho_RMBnfwm);
                HOperatorSet.GenEmptyObj(out ho_RSPnfwm);
                HOperatorSet.GenEmptyObj(out ho_ObjectSelected);
                HOperatorSet.GenEmptyObj(out ho_PP1);
                HOperatorSet.GenEmptyObj(out ho_PP2);
                HOperatorSet.GenEmptyObj(out ho_RegionInoDr);
                HOperatorSet.GenEmptyObj(out ho_RegsMBNall);
                HOperatorSet.GenEmptyObj(out ho_RegsSPNall);
                HOperatorSet.GenEmptyObj(out ho_RegionGi);
                HOperatorSet.GenEmptyObj(out ho_Regsspnnofwm);
                HOperatorSet.GenEmptyObj(out ho_RegionFWMPA);
                HOperatorSet.GenEmptyObj(out ho_Regsmbnnofwm);
                HOperatorSet.GenEmptyObj(out ho_Rclfwm);
                HOperatorSet.GenEmptyObj(out ho_RegsMBNcnof);
                HOperatorSet.GenEmptyObj(out ho_Rclfwmbutnfrm);
                HOperatorSet.GenEmptyObj(out ho_RconnectedNFWM);
                HOperatorSet.GenEmptyObj(out ho_Circle1);
                HOperatorSet.GenEmptyObj(out ho_CircleMB);
                HOperatorSet.GenEmptyObj(out ho_RegsSPNcnof);
                HOperatorSet.GenEmptyObj(out ho_Mmregmbs);
                HOperatorSet.GenEmptyObj(out ho_RIntr);
                HOperatorSet.GenEmptyObj(out ho_CirclesMBcc);
                HOperatorSet.GenEmptyObj(out ho_CirclesSPcc);
                HOperatorSet.GenEmptyObj(out ho_SkeletonInoD);
                HOperatorSet.GenEmptyObj(out ho_RegMBNc);
                HOperatorSet.GenEmptyObj(out ho_CirclesMBNc);
                HOperatorSet.GenEmptyObj(out ho_CircleMBNc);
                HOperatorSet.GenEmptyObj(out ho_RegSPNc);
                HOperatorSet.GenEmptyObj(out ho_CirclesSPNc);
                HOperatorSet.GenEmptyObj(out ho_CircleSPNc);
                HOperatorSet.GenEmptyObj(out ho_AllPoints);
                HOperatorSet.GenEmptyObj(out ho_RegionGu);
                HOperatorSet.GenEmptyObj(out ho_EndPointsu);
                HOperatorSet.GenEmptyObj(out ho_RegionGB);
                HOperatorSet.GenEmptyObj(out ho_EndPoints);
                HOperatorSet.GenEmptyObj(out ho_Rectangles);
                HOperatorSet.GenEmptyObj(out ho_Forb);
                HOperatorSet.GenEmptyObj(out ho_Rectanglesu);
                HOperatorSet.GenEmptyObj(out ho_RegionExpandedi);
                HOperatorSet.GenEmptyObj(out ho_RegionExpanded);
                HOperatorSet.GenEmptyObj(out ho_RInod);
                HOperatorSet.GenEmptyObj(out ho_RInodFillUp);
                HOperatorSet.GenEmptyObj(out ho_RHole);
                HOperatorSet.GenEmptyObj(out ho_RHoles);
                HOperatorSet.GenEmptyObj(out ho_Hole);
                HOperatorSet.GenEmptyObj(out ho_HoleB);
                HOperatorSet.GenEmptyObj(out ho_Ps);
                HOperatorSet.GenEmptyObj(out ho_Psl);
                HOperatorSet.GenEmptyObj(out ho_Psls);
                HOperatorSet.GenEmptyObj(out ho_CirclesMin1);
                HOperatorSet.GenEmptyObj(out ho_RegionUnion);
                HOperatorSet.GenEmptyObj(out ho_CirclesMin);
                HOperatorSet.GenEmptyObj(out ho_RInoDB);
                HOperatorSet.GenEmptyObj(out ho_Regionb);
                HOperatorSet.GenEmptyObj(out ho_Pss);
                HOperatorSet.GenEmptyObj(out ho_CirclesMax);
                HOperatorSet.GenEmptyObj(out ho_CirclesSpur);
                HOperatorSet.GenEmptyObj(out ho_CirclesMB);
                HOperatorSet.GenEmptyObj(out ho_EndPointsFs);
                HOperatorSet.GenEmptyObj(out ho_CFE);
                HOperatorSet.GenEmptyObj(out ho_RegionsEndFWM);
                HOperatorSet.GenEmptyObj(out ho_REndFWM);
                HOperatorSet.GenEmptyObj(out ho_EP);
                HOperatorSet.GenEmptyObj(out ho_RCEndFWM);
                HOperatorSet.GenEmptyObj(out ho_RegsspnnofwmR);
                HOperatorSet.GenEmptyObj(out ho_CSPN);
                HOperatorSet.GenEmptyObj(out ho_RegsSP);
                HOperatorSet.GenEmptyObj(out ho_MBreal);
                HOperatorSet.GenEmptyObj(out ho_CircleMBr);
                HOperatorSet.GenEmptyObj(out ho_SPreal);
                HOperatorSet.GenEmptyObj(out ho_CircleSPr);
                HOperatorSet.GenEmptyObj(out ho_RInoD);
                HOperatorSet.GenEmptyObj(out ho_RegPDCV);
                HOperatorSet.GenEmptyObj(out ho_RegPDCH);
                HOperatorSet.GenEmptyObj(out ho_RegionIii);
                HOperatorSet.GenEmptyObj(out ho_Regsp);
                HOperatorSet.GenEmptyObj(out ho_Regmb);
                HOperatorSet.GenEmptyObj(out ho_Regmbs);
                HOperatorSet.GenEmptyObj(out ho_Regsps);
                HOperatorSet.GenEmptyObj(out ho_CircleSPi);
                HOperatorSet.GenEmptyObj(out ho_CircleMBi);

            }

            {
                ho_RegionCut.Dispose();
                ho_RegionG.Dispose();
                ho_RegionGS.Dispose();
                ho_SkeletonG.Dispose();
                ho_SkeletonGS.Dispose();
                ho_WidthImageP.Dispose();
                ho_DirImageP.Dispose();
                ho_WidthImageS.Dispose();
                ho_DirImageS.Dispose();
                ho_SkeletonsG.Dispose();
                ho_SkeletonsLG.Dispose();
                ho_ImageIR.Dispose();
                ho_RegionsPNi.Dispose();
                ho_RegionsPNSi.Dispose();
                ho_RegionsNFWM.Dispose();
                ho_BoundariesNFWM.Dispose();
                ho_SkeletonFWMi.Dispose();
                ho_SkeletonsFWMi.Dispose();
                ho_SkeletonsFWMcut.Dispose();
                ho_SkeletonsFWMS.Dispose();
                ho_SkeletonsFWMScut.Dispose();
                ho_RegionsGND.Dispose();
                ho_RegionsEMPTY.Dispose();
                ho_ImDisplacement.Dispose();
                ho_SkeletonG.Dispose();
                ho_SkeletonPartsP.Dispose();
                ho_EPNFWMremove.Dispose();
                ho_SkeletonPartsPnoG.Dispose();
                ho_ImageIR16.Dispose();
                ho_ImageIRS.Dispose();
                ho_RegionMS.Dispose();
                ho_RegionsPNPi.Dispose();
                ho_RegionsmainSNoCut.Dispose();
                ho_SkelEP.Dispose();
            }

            // ---------------------------------------------------------- 
            hv_pathTeach = hv_path + "Teach\\";
            Utils.FolderCheck(hv_pathTeach);
            hv_pathTeachT = hv_pathTeach + "Tuples\\";
            Utils.FolderCheck(hv_pathTeachT);

            HTuple hv_SetDefault = 1;
            try
            {
                HTuple hv_Path_Params = hv_path + "TeachPars.ser";
                LoadPars1(hv_Path_Params, hv_SetDefault,
                        out hv_GNDamin,
                        out hv_fwmhw, out hv_parop, out hv_gap, out hv_jrad,
                        out hv_groundpn, out hv_groundgval, out hv_sm_factor, out hv_lookaround,
                        out hv_wpl, out hv_smoothCADcontours, out hv_maxdirSP,
                        out hv_wFWM, out hv_wside, out hv_wFWMS, out hv_wsideS,
                        out hv_eradMSe, out hv_eradNFWMremove, out hv_elengthend,
                        out hv_elengthnoend, out hv_jpadd, out hv_removeGNDfromFWM,
                        out hv_cutendMSS, out hv_minWGS, out hv_minWG, out hv_useODB,
                        out hv_autowidth, out hv_EPexp, out hv_elongLG, // out HTuple hv_TeachPars,
                        out hv_minPSklen, out hv_lpnum2 // New paramtrs 2025/06/02
                        );
            }
            catch (HalconException exc)
            {
                /// TODO: Log
                //Console.WriteLine(exc.GetErrorMessage());
                MessageBox.Show("Error loading teaching parameters file! Training parameters. Check for the presence of the TeachPars.ser file in the Model folder. HalconException: " + exc.GetErrorMessage(), "LoadPars1", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            // ---------------------------------------------------------- 

            //*********  with Wg measured by same line sensor 2020/06/30
            // PARAMETERS *
            //202290317 fixed very big open problem
            //********************************************************
            // sensor width extension   11221122

            //n := 101
            //kn := n/log(n)
            //** Comp 17.5, LG 13.5
            //hv_minWG = 4;
            //wpl := 11.5
            //hv_wpl = 15.5;
            //***  um/pix
            hv_kum = 1.0;
            //*** FWM half width
            //hv_fwmhw = 12.5;
            //* opening
            //hv_parop = 2.5;

            //***groung regions ****
            //GNDamin := 900000
            //hv_GNDamin = 410000;


            //*********************************

            //**** part of FOV selection *****
            hv_TO = 1;
            //**** Marging selection *****
            switch (hv_TO.I)
            {
                case 1:
                    hv_marginL = 0;
                    hv_marginR = 100;
                    hv_marginU = 100;
                    hv_marginD = 100;
                    break;
                case 2:
                    hv_marginL = 100;
                    hv_marginR = 100;
                    hv_marginU = 100;
                    hv_marginD = 100;
                    break;
                case 3:
                    hv_marginL = 100;
                    hv_marginR = 0;
                    hv_marginU = 100;
                    hv_marginD = 100;
                    break;
            }



            if ((int)(new HTuple(hv_TO.TupleEqual(1))) != 0)
            {
                hv_ovlL = 0;
            }
            else
            {
                hv_ovlL = 100;
            }


            if ((int)(new HTuple(hv_TO.TupleEqual(3))) != 0)
            {
                hv_ovlR = 0;
            }
            else
            {
                hv_ovlR = 100;
            }

            //ovlR := 100
            //ovl := 0
            //*** for skeletons *******
            //hv_gap = 1;
            //hv_jrad = 10;
            //*************  for (Non-FWM) Buiulding MSN array
            //hv_sm_factor = 7;
            //hv_lookaround = 7;
            //*** for ImageIRB prep.
            //hv_groundpn = 1;
            //hv_groundgval = 3;

            //useODB:=3 for coils
            //hv_useODB = 0;
            //________path to saving data__________________*

            //** path for FWM


            //hv_pathTest = hv_path + "Test/";
            //ho_Gi.Dispose();
            //HOperatorSet.ReadImage(out ho_Gi, hv_path + "Gi.tif");

            //** path for Non-FWM
            hv_pathN = hv_path.Clone();
            //*** smoothening all CAD contours to make it similar with real pattern
            //**LG
            //hv_smoothCADcontours = 11;
            //**old Compass
            //smoothCADcontours := 51
            //**new Compass
            //smoothCADcontours := 41
            //read_object (Circlembscc, 'E:/PATTERN/CircleMBScc.hobj')
            //write_object (Circlembscc, 'E:/PATTERN/CircleMBScc1.hobj')

            //**** Parameters ********
            //************************
            //hv_maxdirSP = 40;
            //***  LG
            //hv_wFWM = new HTuple();
            //hv_wFWM[0] = 14; //23;
            //hv_wFWM[1] = 19; //30;
            //hv_wFWMS = 13; // 25;
            //hv_wside = 8;
            //hv_wsideS = 5;
            //hv_wside = 6;
            //hv_wsideS = 3;


            //hv_smoothCADcontours = 1;
            //hv_wside = 5;
            //hv_wsideS = 3;


            //hv_eradNFWMremove = 9;
            //hv_eradMSe = 9;
            //* elongation lengths for MS
            //hv_elengthend = -20;
            //hv_elengthnoend = -15;
            //* cutend MSS
            //hv_cutendMSS = 20;
            //* add to JP rad. for JP neibourhood removal from FWM
            //hv_jpadd = 3;
            //*
            //hv_removeGNDfromFWM = 0;
            //hv_elongLG = 10;

            //______________________________________________*

            //HOperatorSet.ReadObject(out ho_DK, m_hv_ModelPath + "DK.hobj");


            HOperatorSet.GetImageSize(ho_Gi, out hv_WidthG, out hv_HeightG);
            ho_DomainG.Dispose();
            HOperatorSet.GetDomain(ho_Gi, out ho_DomainG);
            //read_object (Ri, path+'R.hobj')
            ho_RegionG.Dispose();
            HOperatorSet.Threshold(ho_Gi, out ho_RegionG, 128, 255);

            ho_RegDontCare.Dispose();
            HOperatorSet.GenEmptyObj(out ho_RegDontCare);

            ho_RegSMboundary.Dispose();
            HOperatorSet.GenEmptyObj(out ho_RegSMboundary);


            ho_RegionFlash.Dispose();
            HOperatorSet.GenEmptyRegion(out ho_RegionFlash);
            ho_RegionTrace.Dispose();
            HOperatorSet.GenEmptyRegion(out ho_RegionTrace);
            ho_RegionPolygon.Dispose();
            HOperatorSet.GenEmptyRegion(out ho_RegionPolygon);

            HObject ho_SkeletonGd;



            try
            {
                //TeachingMain125(
                //    ho_RegNoProc, ho_Gi, ho_Im, ho_RegionFlash, ho_RegionTrace,

                //    out ho_RegionG, out ho_RegionGS, out ho_SkeletonG, out ho_SkeletonGS,
                //    out ho_WidthImageP, out ho_DirImageP, out ho_WidthImageS, out ho_DirImageS,
                //    out ho_SkeletonsG, out ho_ImageIR, out ho_RegionsPNi, out ho_RegionsPNSi,
                //    out ho_RegionsNFWM, out ho_BoundariesNFWM, out ho_SkeletonFWMi, out ho_SkeletonsFWMi,
                //    out ho_SkeletonsFWMcut, out ho_SkeletonsFWMS, out ho_SkeletonsFWMScut,
                //    out ho_RegionsGND, out ho_RegionsEMPTY, out ho_ImDisplacement, out ho_SkeletonGd,
                //    out ho_SkeletonPartsP, out ho_EPNFWMremove, out ho_SkeletonPartsPnoG, out ho_ImageIR16,
                //    out ho_ImageIRS, out ho_RegionMS, out ho_RegionsPNPi, out ho_RegionsmainSNoCut,
                //    out ho_SkelEP, out ho_RegionCut, 

                //    hv_GNDamin, hv_path, hv_fwmhw, hv_parop, hv_WidthG, hv_HeightG,
                //    hv_jrad, hv_gap, hv_groundpn, hv_groundgval, hv_sm_factor, hv_lookaround,
                //    hv_wpl, hv_smoothCADcontours, hv_maxdirSP, hv_wFWM, hv_wside, hv_wFWMS,
                //    hv_wsideS, hv_eradMSe, hv_eradNFWMremove, hv_elengthend, hv_elengthnoend,
                //    hv_jpadd, hv_removeGNDfromFWM, hv_cutendMSS, hv_minWG, hv_minWG, hv_useODB,
                //    hv_autowidth, out hv_nFWM, out hv_nFWMS, 

                //    out hv_nNonFWM, out hv_WG, out hv_YC,
                //    out hv_XC, out hv_Y1, out hv_X1, out hv_Y2, out hv_X2, out hv_Pnum, out hv_Pnum2,
                //    out hv_PnumR, out hv_PregC, out hv_PcontR, out hv_PaddrR, out hv_PaddrC,
                //    out hv_PCinRnum, out hv_PuseintP, out hv_WGS, out hv_YCS, out hv_XCS, out hv_Y1S,
                //    out hv_X1S, out hv_Y2S, out hv_X2S, out hv_PnumS, out hv_Pnum2S, out hv_PnumRS,
                //    out hv_PregCS, out hv_PcontRS, out hv_PaddrRS, out hv_PaddrCS, out hv_PCinRnumS,
                //    out hv_PuseintS, out hv_SequenceIRB, out hv_SequenceIRBS, out hv_WGN1,
                //    out hv_YCN, out hv_XCN, out hv_Y1N, out hv_X1N, out hv_Y2N, out hv_X2N,
                //    out hv_PnumN, out hv_PnumRN, out hv_PregCN, out hv_PcontRN1, out hv_PaddrRN,
                //    out hv_PaddrCN, out hv_PCinRnumN, out m_hv_TotalPRC, out m_hv_CurrentOper);

                //TeachingMain126(
                //    ho_RegNoProc, ho_Gi, ho_Im, ho_RegionFlash, ho_RegionTrace,

                //    out ho_RegionG, out ho_RegionGS, out ho_SkeletonG, out ho_SkeletonGS,
                //    out ho_WidthImageP, out ho_DirImageP, out ho_WidthImageS, out ho_DirImageS,
                //    out ho_SkeletonsG, out ho_ImageIR, out ho_RegionsPNi, out ho_RegionsPNSi,
                //    out ho_RegionsNFWM, out ho_BoundariesNFWM, out ho_SkeletonFWMi, out ho_SkeletonsFWMi,
                //    out ho_SkeletonsFWMcut, out ho_SkeletonsFWMS, out ho_SkeletonsFWMScut,
                //    out ho_RegionsGND, out ho_RegionsEMPTY, out ho_ImDisplacement, out ho_SkeletonGd,
                //    out ho_SkeletonPartsP, out ho_EPNFWMremove, out ho_SkeletonPartsPnoG, out ho_ImageIR16,
                //    out ho_ImageIRS, out ho_RegionMS, out ho_RegionsPNPi, out ho_RegionsmainSNoCut,
                //    out ho_SkelEP, out ho_RegionCut,

                //    hv_GNDamin, hv_path, hv_fwmhw, hv_parop, hv_WidthG, hv_HeightG,
                //    hv_jrad, hv_gap, hv_groundpn, hv_groundgval, hv_sm_factor, hv_lookaround,
                //    hv_wpl, hv_smoothCADcontours, hv_maxdirSP, hv_wFWM, hv_wside, hv_wFWMS,
                //    hv_wsideS, hv_eradMSe, hv_eradNFWMremove, hv_elengthend, hv_elengthnoend,
                //    hv_jpadd, hv_removeGNDfromFWM, hv_cutendMSS, hv_minWG, hv_minWG, hv_useODB,
                //    hv_autowidth, hv_EPexp, 

                //    out hv_nFWM, out hv_nFWMS,
                //    out hv_nNonFWM, out hv_WG, out hv_YC,
                //    out hv_XC, out hv_Y1, out hv_X1, out hv_Y2, out hv_X2, out hv_Pnum, out hv_Pnum2,
                //    out hv_PnumR, out hv_PregC, out hv_PcontR, out hv_PaddrR, out hv_PaddrC,
                //    out hv_PCinRnum, out hv_PuseintP, out hv_WGS, out hv_YCS, out hv_XCS, out hv_Y1S,
                //    out hv_X1S, out hv_Y2S, out hv_X2S, out hv_PnumS, out hv_Pnum2S, out hv_PnumRS,
                //    out hv_PregCS, out hv_PcontRS, out hv_PaddrRS, out hv_PaddrCS, out hv_PCinRnumS,
                //    out hv_PuseintS, out hv_SequenceIRB, out hv_SequenceIRBS, out hv_WGN1,
                //    out hv_YCN, out hv_XCN, out hv_Y1N, out hv_X1N, out hv_Y2N, out hv_X2N,
                //    out hv_PnumN, out hv_PnumRN, out hv_PregCN, out hv_PcontRN1, out hv_PaddrRN,
                //    out hv_PaddrCN, out hv_PCinRnumN, out m_hv_TotalPRC, out m_hv_CurrentOper);

                Teaching_DLL.Teaching_DLL teach = new Teaching_DLL.Teaching_DLL();

                //TeachingMain128(
                teach.Start_Teaching(
                    ho_RegNoProc, ho_Gi, ho_Im, ho_RegionFlash, ho_RegionTrace,

                    out ho_RegionG, out ho_RegionGS, out ho_SkeletonG, out ho_SkeletonGS,
                    out ho_WidthImageP, out ho_DirImageP, out ho_WidthImageS, out ho_DirImageS,
                    out ho_SkeletonsG, out ho_ImageIR, out ho_RegionsPNi, out ho_RegionsPNSi,
                    out ho_RegionsNFWM, out ho_BoundariesNFWM, out ho_SkeletonFWMi, out ho_SkeletonsFWMi,
                    out ho_SkeletonsFWMcut, out ho_SkeletonsFWMS, out ho_SkeletonsLG, out ho_SkeletonsFWMScut,
                    out ho_RegionsGND, out ho_RegionsEMPTY, out ho_ImDisplacement, out ho_SkeletonGd,
                    out ho_SkeletonPartsP, out ho_EPNFWMremove, out ho_SkeletonPartsPnoG, out ho_ImageIR16,
                    out ho_ImageIRS, out ho_RegionMS, out ho_RegionsPNPi, out ho_RegionsmainSNoCut,
                    out ho_SkelEP, out ho_RegionCut,

                    hv_GNDamin, hv_path, hv_fwmhw, hv_parop, hv_WidthG, hv_HeightG,
                    hv_jrad, hv_gap, hv_groundpn, hv_groundgval, hv_sm_factor, hv_lookaround,
                    hv_wpl, hv_smoothCADcontours, hv_maxdirSP, hv_wFWM, hv_wside, hv_wFWMS,
                    hv_wsideS, hv_eradMSe, hv_eradNFWMremove, hv_elengthend, hv_elengthnoend,
                    hv_jpadd, hv_removeGNDfromFWM, hv_cutendMSS, hv_minWGS, hv_minWG, hv_useODB,
                    hv_autowidth, hv_EPexp,

                    hv_minPSklen, hv_lpnum2, // New paramtrs 2025/06/02                    

                    hv_elongLG,
                    out hv_nFWM, out hv_nFWMS,
                    out hv_nNonFWM, out hv_WG, out hv_YC,
                    out hv_XC, out hv_Y1, out hv_X1, out hv_Y2, out hv_X2, out hv_Pnum, out hv_Pnum2,
                    out hv_PnumR, out hv_PregC, out hv_PcontR, out hv_PaddrR, out hv_PaddrC,
                    out hv_PCinRnum, out hv_PuseintP, out hv_WGS, out hv_YCS, out hv_XCS, out hv_Y1S,
                    out hv_X1S, out hv_Y2S, out hv_X2S, out hv_PnumS, out hv_Pnum2S, out hv_PnumRS,
                    out hv_PregCS, out hv_PcontRS, out hv_PaddrRS, out hv_PaddrCS, out hv_PCinRnumS,
                    out hv_PuseintS, out hv_SequenceIRB, out hv_SequenceIRBS, out hv_WGN1,
                    out hv_YCN, out hv_XCN, out hv_Y1N, out hv_X1N, out hv_Y2N, out hv_X2N,
                    out hv_PnumN, out hv_PnumRN, out hv_PregCN, out hv_PcontRN1, out hv_PaddrRN,
                    out hv_PaddrCN, out hv_PCinRnumN, out m_hv_TotalPRC, out m_hv_CurrentOper);
            }
            catch (HalconException exc)
            {
                /// TODO: Log
                //Console.WriteLine(exc.GetErrorMessage());
                MessageBox.Show("Failed TeachingMain125! HalconException: " + exc.GetErrorMessage(), "TeachingMain125", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            // ----------------------------------------------------------------------------------


            ho_RegForbidP.Dispose();
            ho_RegForbidP = ho_SkeletonGS.CopyObj(1, -1);

            ho_SkeletonsFWMii.Dispose();
            HOperatorSet.SelectShape(ho_SkeletonsFWMi, out ho_SkeletonsFWMii, "area", "and",
                150, 99999);
            ho_SkeletonsFWM.Dispose();
            HOperatorSet.Union1(ho_SkeletonsFWMii, out ho_SkeletonsFWM);
            HOperatorSet.SetSystem("neighborhood", 8);
            ho_SkeletonsFWMii.Dispose();
            HOperatorSet.Connection(ho_SkeletonsFWM, out ho_SkeletonsFWMii);
            //expand_region (SkeletonsFWMii, RegForbidP, RegionsPNPi, 100, 'image')
            HOperatorSet.CountObj(ho_SkeletonsFWMii, out hv_nFWMii);
            HOperatorSet.CountObj(ho_RegionsPNPi, out hv_nPNPi);

            ho_RegionNFWM.Dispose();
            HOperatorSet.Union1(ho_RegionsNFWM, out ho_RegionNFWM);
            ho_RegionFWM.Dispose();
            HOperatorSet.Difference(ho_RegionG, ho_RegionNFWM, out ho_RegionFWM);
            //write_object (RegionFWM, path+'RegionFWM.hobj')
            // stop(...); only in hdevelop
            //expand_region (SkeletonsFWM, RegForbidP, RegionsPNPi, 35, 'image')
            //expand_region (SkeletonsFWMi, RegForbidP, RegionsPNPi, 35, 'image')

            //*** Saving the Teaching data  *****
            //pathTeach := path+'Teach/'
            //** Teach IN
            //write_object (RegNoProc, pathTeach+'RegNoProc')
            //write_object (RegionCut, pathTeach+'RectangleCut')
            //,Gi,Im,RegionFlash,RegionTrace,  *** Teach obj. OUT
            HOperatorSet.WriteImage(ho_Gi, "tiff", 0, hv_pathTeach + "Gi");
            ho_ImageChainLength.Dispose();
            HOperatorSet.GenImageConst(out ho_ImageChainLength, "uint2", hv_WidthG, hv_HeightG);
            HOperatorSet.WriteImage(ho_ImageChainLength, "tiff", 0, hv_pathTeach + "ImCLC.tif");
            HOperatorSet.WriteImage(ho_WidthImageP, "tiff", 0, hv_pathTeach + "WidthImageP");
            HOperatorSet.WriteImage(ho_DirImageP, "tiff", 0, hv_pathTeach + "DirImageP");
            HOperatorSet.WriteImage(ho_WidthImageS, "tiff", 0, hv_pathTeach + "WidthImageS");
            HOperatorSet.WriteImage(ho_DirImageS, "tiff", 0, hv_pathTeach + "DirImageS");

            HOperatorSet.WriteObject(ho_RegionG, hv_pathTeach + "RegionG");
            HOperatorSet.WriteObject(ho_RegionGS, hv_pathTeach + "RegionGS");
            HOperatorSet.WriteObject(ho_SkeletonG, hv_pathTeach + "SkeletonG");
            HOperatorSet.WriteObject(ho_SkeletonGS, hv_pathTeach + "SkeletonGS");

            HOperatorSet.WriteObject(ho_RegionsGND, hv_pathTeach + "RegionsGND");
            HOperatorSet.WriteObject(ho_RegionsEMPTY, hv_pathTeach + "RegionsEMPTY");
            HOperatorSet.WriteObject(ho_RegionFWM, hv_pathTeach + "RegionFWM");

            HOperatorSet.WriteObject(ho_RegionsPNSi, hv_pathTeach + "RegionsPNSi");
            HOperatorSet.WriteObject(ho_RegionsPNPi, hv_pathTeach + "RegionsPNPi");

            HOperatorSet.WriteObject(ho_SkeletonsFWMi, hv_pathTeach + "SkeletonsFWM");
            HOperatorSet.WriteObject(ho_SkeletonsLG, hv_pathTeach + "SkeletonsLG");
            HOperatorSet.WriteObject(ho_SkeletonsFWMS, hv_pathTeach + "SkeletonsFWMS");
            HOperatorSet.WriteObject(ho_SkeletonsFWMcut, hv_pathTeach + "SkeletonsFWMcut");
            HOperatorSet.WriteObject(ho_SkeletonsFWMScut, hv_pathTeach + "SkeletonsFWMScut");
            HOperatorSet.WriteObject(m_ho_RegNoProc, hv_pathTeach + "RegNoProc");
            HOperatorSet.WriteObject(ho_RegionCut, hv_pathTeach + "RegionCut");
            HOperatorSet.WriteObject(ho_SkelEP, hv_pathTeach + "SkelSEP");


            //********
            //write_object (SkeletonsG, pathTeach+'SkeletonsG')
            //write_object (SkeletonsFWMi, pathTeach+'SkeletonsFWM')
            //write_image (ImageIR, 'tiff', 0, pathTeach+'ImageIR')
            //write_object (RegionsPNi, pathTeach+'RegionsPNi')





            //write_image (ImDisplacement, 'tiff', 0, pathTeach+'ImDisplacement')
            //write_object (SkeletonPartsP, pathTeach+'SkeletonPartsP')
            //write_image (ImageIR16, 'tiff', 0, pathTeach+'ImageIR16')
            //write_image (ImageIRS, 'tiff', 0, pathTeach+'ImageIRS')
            //write_object (RegionMS, pathTeach+'RegionMS')
            // stop(...); only in hdevelop

            //** Teach tuple OUT (FWM only)
            HOperatorSet.WriteTuple(hv_WG, hv_pathTeachT + "WG.tup");
            HOperatorSet.WriteTuple(hv_XC, hv_pathTeachT + "XC.tup");
            HOperatorSet.WriteTuple(hv_YC, hv_pathTeachT + "YC.tup");
            HOperatorSet.WriteTuple(hv_X1, hv_pathTeachT + "X1.tup");
            HOperatorSet.WriteTuple(hv_Y1, hv_pathTeachT + "Y1.tup");
            HOperatorSet.WriteTuple(hv_X2, hv_pathTeachT + "X2.tup");
            HOperatorSet.WriteTuple(hv_Y2, hv_pathTeachT + "Y2.tup");
            HOperatorSet.WriteTuple(hv_Pnum, hv_pathTeachT + "Pnum.tup");
            HOperatorSet.WriteTuple(hv_Pnum2, hv_pathTeachT + "Pnum2.tup");
            HOperatorSet.WriteTuple(hv_PnumR, hv_pathTeachT + "PnumR.tup");
            HOperatorSet.WriteTuple(hv_PaddrR, hv_pathTeachT + "PaddrR.tup");
            HOperatorSet.WriteTuple(hv_PaddrC, hv_pathTeachT + "PaddrC.tup");
            HOperatorSet.WriteTuple(hv_PregC, hv_pathTeachT + "PregC.tup");
            HOperatorSet.WriteTuple(hv_PCinRnum, hv_pathTeachT + "PCinRnum.tup");
            HOperatorSet.WriteTuple(hv_PcontR, hv_pathTeachT + "PcontR.tup");


            HOperatorSet.WriteTuple(hv_PuseintP, hv_pathTeachT + "PuseintP.tup");
            HOperatorSet.WriteTuple(hv_WG, hv_pathTeach + "WG.tup");

            //** Teach tuple OUT (FWMS only)
            HOperatorSet.WriteTuple(hv_WGS, hv_pathTeachT + "WGS.tup");
            HOperatorSet.WriteTuple(hv_XCS, hv_pathTeachT + "XCS.tup");
            HOperatorSet.WriteTuple(hv_YCS, hv_pathTeachT + "YCS.tup");
            HOperatorSet.WriteTuple(hv_X1S, hv_pathTeachT + "X1S.tup");
            HOperatorSet.WriteTuple(hv_Y1S, hv_pathTeachT + "Y1S.tup");
            HOperatorSet.WriteTuple(hv_X2S, hv_pathTeachT + "X2S.tup");
            HOperatorSet.WriteTuple(hv_Y2S, hv_pathTeachT + "Y2S.tup");
            HOperatorSet.WriteTuple(hv_PnumS, hv_pathTeachT + "PnumS.tup");
            HOperatorSet.WriteTuple(hv_Pnum2S, hv_pathTeachT + "Pnum2S.tup");
            HOperatorSet.WriteTuple(hv_PnumRS, hv_pathTeachT + "PnumRS.tup");
            HOperatorSet.WriteTuple(hv_PaddrRS, hv_pathTeachT + "PaddrRS.tup");
            HOperatorSet.WriteTuple(hv_PaddrCS, hv_pathTeachT + "PaddrCS.tup");
            HOperatorSet.WriteTuple(hv_PregCS, hv_pathTeachT + "PregCS.tup");
            HOperatorSet.WriteTuple(hv_PCinRnumS, hv_pathTeachT + "PCinRnumS.tup");
            HOperatorSet.WriteTuple(hv_PcontRS, hv_pathTeachT + "PcontRS.tup");

            // ----------------------------------------------------------------------------------

            string bin_Path = hv_pathTeachT + "bin";
            if (Directory.Exists(bin_Path))
            {
                string[] files = Directory.GetFiles(bin_Path, "*.dat");
                for (int dir_pos = 0; dir_pos < files.Length; dir_pos++)
                {
                    File.Delete(files[dir_pos]);
                }
            }

            // ----------------------------------------------------------------------------------

            HTuple hv_Win = 20;
            //HTuple hv_WinD2 = hv_Win / 2;
            HTuple hv_SensRatio = 2.5;

            Teach_Slice(ho_SkeletonsLG, ho_WidthImageS, hv_Win, hv_SensRatio, hv_pathTeachT, "S",
                out HTuple hv_Pointer_Skel, out HTuple hv_Pointer_Sens, out HTuple hv_Rows_Sens, out HTuple hv_Cols_Sens, out HTuple hv_Pointer_Pos,
                out m_hv_CurrentOper, out m_hv_TotalPRC, out m_hv_MaxPRC);


            Teach_Slice(ho_SkeletonsFWMi, ho_WidthImageP, hv_Win, hv_SensRatio, hv_pathTeachT, "P",
                out HTuple hv_Pointer_Skel_P, out HTuple hv_Pointer_Sens_P, out HTuple hv_Rows_Sens_P, out HTuple hv_Cols_Sens_P, out HTuple hv_Pointer_Pos_P,
                out m_hv_CurrentOper, out m_hv_TotalPRC, out m_hv_MaxPRC);

            IntPtr open_short = Create_Open_Short_2();

            Teaching_Open_Short_2(open_short, 60, 1000000, m_hv_ModelPath);

            return true;
        }

        private bool TeachingLowGrayOnly(HTuple hv_path, HObject ho_SkeletonsLG, HObject ho_WidthImageS)
        {
            HTuple hv_pathTeach = null, hv_pathTeachT = null;

            hv_pathTeach = hv_path + "Teach\\";
            Utils.FolderCheck(hv_pathTeach);
            hv_pathTeachT = hv_pathTeach + "Tuples\\";
            Utils.FolderCheck(hv_pathTeachT);

            // ----------------------------------------------------------------------------------

            string bin_Path = hv_pathTeachT + "bin";
            if (Directory.Exists(bin_Path))
            {
                string[] files = Directory.GetFiles(bin_Path, "*.dat");
                for (int dir_pos = 0; dir_pos < files.Length; dir_pos++)
                {
                    File.Delete(files[dir_pos]);
                }
            }

            // ----------------------------------------------------------------------------------

            HTuple hv_Win = 20;
            //HTuple hv_WinD2 = hv_Win / 2;
            HTuple hv_SensRatio = 2.5;

            Teach_Slice(ho_SkeletonsLG, ho_WidthImageS, hv_Win, hv_SensRatio, hv_pathTeachT, "S",
                out HTuple hv_Pointer_Skel, out HTuple hv_Pointer_Sens, out HTuple hv_Rows_Sens, out HTuple hv_Cols_Sens, out HTuple hv_Pointer_Pos,
                out m_hv_CurrentOper, out m_hv_TotalPRC, out m_hv_MaxPRC);

            return true;
        }


        // --------------------------------------------------------------------------
        // --- Функции Халкон -------------------------------------------------------
        // --------------------------------------------------------------------------


        public void WSelbyHist(HTuple hv_PrcWGi, HTuple hv_wmax, HTuple hv_prcmin, HTuple hv_numP,
              out HTuple hv_WminPa)
        {
            // Local iconic variables 

            // Local control variables 

            HTuple hv_FunctionWGi = null, hv_SmoothedFunctionWGi = null;
            HTuple hv_Min = null, hv_Max = null, hv_Imax1 = null, hv_Imax2 = null;
            HTuple hv_Ymax1 = null, hv_Ymax2 = null, hv_Ymax = null;
            HTuple hv_nm = null, hv_i = null, hv_j = null, hv_w = new HTuple();
            HTuple hv_y = new HTuple();
            // Initialize local and output iconic variables 
            HOperatorSet.CreateFunct1dArray(hv_PrcWGi, out hv_FunctionWGi);
            HOperatorSet.SmoothFunct1dMean(hv_FunctionWGi, 1, 3, out hv_SmoothedFunctionWGi);
            HOperatorSet.LocalMinMaxFunct1d(hv_SmoothedFunctionWGi, "plateaus_center", "true",
                out hv_Min, out hv_Max);
            hv_Imax1 = hv_Max.TupleInt();
            hv_Imax2 = (hv_Max.TupleInt()) + 1;
            HOperatorSet.GetYValueFunct1d(hv_FunctionWGi, hv_Imax1, "constant", out hv_Ymax1);
            HOperatorSet.GetYValueFunct1d(hv_FunctionWGi, hv_Imax2, "constant", out hv_Ymax2);
            hv_Ymax = hv_Ymax1.TupleMax2(hv_Ymax2);

            HOperatorSet.TupleLength(hv_Ymax, out hv_nm);
            hv_WminPa = new HTuple();
            hv_i = 0;
            HTuple end_val12 = hv_nm - 1;
            HTuple step_val12 = 1;
            for (hv_j = 0; hv_j.Continue(end_val12, step_val12); hv_j = hv_j.TupleAdd(step_val12))
            {
                hv_w = hv_Max.TupleSelect(hv_j);
                if ((int)(new HTuple(hv_w.TupleGreater(hv_wmax))) != 0)
                {
                    break;
                }
                hv_y = hv_Ymax.TupleSelect(hv_j);
                if ((int)(new HTuple(hv_y.TupleLess(hv_prcmin))) != 0)
                {
                    continue;
                }
                if (hv_WminPa == null)
                    hv_WminPa = new HTuple();
                hv_WminPa[hv_i] = ((hv_Max.TupleSelect(hv_j))).TupleRound();
                hv_i = hv_i + 1;
                if ((int)(new HTuple(hv_i.TupleGreaterEqual(hv_numP))) != 0)
                {
                    break;
                }
            }

            return;
        }

        public void dev_update_off()
        {

            // Initialize local and output iconic variables 
            //This procedure sets different update settings to 'off'.
            //This is useful to get the best performance and reduce overhead.
            //
            // dev_update_pc(...); only in hdevelop
            // dev_update_var(...); only in hdevelop
            // dev_update_window(...); only in hdevelop

            return;
        }

        public void MaesureSmoothing(HObject ho_RegionsG, HObject ho_RegionI, out HTuple hv_smoothCADcontours)
        {



            // Local iconic variables 

            HObject ho_RegionsGsm = null, ho_RegionGsm = null;
            HObject ho_RegionDifsm = null;

            // Local control variables 

            HTuple hv_difm = null, hv_sm = null, hv_AreaDifsm = new HTuple();
            HTuple hv_Row2 = new HTuple(), hv_Column2 = new HTuple();
            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_RegionsGsm);
            HOperatorSet.GenEmptyObj(out ho_RegionGsm);
            HOperatorSet.GenEmptyObj(out ho_RegionDifsm);
            try
            {
                hv_difm = 100000000000;
                for (hv_sm = 3; (int)hv_sm <= 21; hv_sm = (int)hv_sm + 2)
                {
                    ho_RegionsGsm.Dispose();
                    SmoothingRegion(ho_RegionsG, out ho_RegionsGsm, hv_sm);
                    ho_RegionGsm.Dispose();
                    HOperatorSet.Union1(ho_RegionsGsm, out ho_RegionGsm);
                    ho_RegionDifsm.Dispose();
                    HOperatorSet.SymmDifference(ho_RegionGsm, ho_RegionI, out ho_RegionDifsm);
                    HOperatorSet.AreaCenter(ho_RegionDifsm, out hv_AreaDifsm, out hv_Row2, out hv_Column2);
                    if ((int)(new HTuple(hv_AreaDifsm.TupleLessEqual(hv_difm))) != 0)
                    {
                        hv_difm = hv_AreaDifsm.Clone();
                    }
                    else
                    {
                        break;
                    }
                }
                hv_smoothCADcontours = hv_sm - 2;
                ho_RegionsGsm.Dispose();
                ho_RegionGsm.Dispose();
                ho_RegionDifsm.Dispose();

                return;
            }
            catch (HalconException HDevExpDefaultException)
            {
                ho_RegionsGsm.Dispose();
                ho_RegionGsm.Dispose();
                ho_RegionDifsm.Dispose();

                throw HDevExpDefaultException;
            }
        }

        public void SmoothingRegion(HObject ho_RegionsG, out HObject ho_RegionsGsm, HTuple hv_smooth)
        {




            // Stack for temporary objects 
            HObject[] OTemp = new HObject[20];

            // Local iconic variables 

            HObject ho_RegGsel = null, ho_Contours = null;
            HObject ho_SmoothedContours = null, ho_Regions = null, ho_Rs = null;
            HObject ho_Rsmoothedj = null, ho_Rinside = null, ho_Rsn = null;

            // Local control variables 

            HTuple hv_nG = null, hv_j = new HTuple(), hv_a = new HTuple();
            HTuple hv_Row = new HTuple(), hv_Column = new HTuple();
            HTuple hv_Angles = new HTuple(), hv_nR = new HTuple();
            HTuple hv_k = new HTuple();
            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_RegionsGsm);
            HOperatorSet.GenEmptyObj(out ho_RegGsel);
            HOperatorSet.GenEmptyObj(out ho_Contours);
            HOperatorSet.GenEmptyObj(out ho_SmoothedContours);
            HOperatorSet.GenEmptyObj(out ho_Regions);
            HOperatorSet.GenEmptyObj(out ho_Rs);
            HOperatorSet.GenEmptyObj(out ho_Rsmoothedj);
            HOperatorSet.GenEmptyObj(out ho_Rinside);
            HOperatorSet.GenEmptyObj(out ho_Rsn);
            //*** smoothing CAD regions ******
            HOperatorSet.CountObj(ho_RegionsG, out hv_nG);

            //smooth := 11
            if ((int)(new HTuple(hv_smooth.TupleGreaterEqual(3))) != 0)
            {
                ho_RegionsGsm.Dispose();
                HOperatorSet.GenEmptyObj(out ho_RegionsGsm);
                HTuple end_val6 = hv_nG;
                HTuple step_val6 = 1;
                for (hv_j = 1; hv_j.Continue(end_val6, step_val6); hv_j = hv_j.TupleAdd(step_val6))
                {
                    //if (j==1235)
                    //stop ()
                    //endif
                    ho_RegGsel.Dispose();
                    HOperatorSet.SelectObj(ho_RegionsG, out ho_RegGsel, hv_j);
                    HOperatorSet.AreaCenter(ho_RegGsel, out hv_a, out hv_Row, out hv_Column);
                    if ((int)(new HTuple(hv_a.TupleLess(10))) != 0)
                    {
                        continue;
                    }

                    ho_Contours.Dispose();
                    HOperatorSet.GenContourRegionXld(ho_RegGsel, out ho_Contours, "border_holes");
                    ho_SmoothedContours.Dispose();
                    HOperatorSet.SmoothContoursXld(ho_Contours, out ho_SmoothedContours, hv_smooth);
                    HOperatorSet.GetContourAngleXld(ho_SmoothedContours, "abs", "range", 3, out hv_Angles);
                    ho_Regions.Dispose();
                    HOperatorSet.GenRegionContourXld(ho_SmoothedContours, out ho_Regions, "filled");
                    HOperatorSet.CountObj(ho_Regions, out hv_nR);
                    ho_Rs.Dispose();
                    HOperatorSet.SelectObj(ho_Regions, out ho_Rs, 1);
                    if ((int)(new HTuple(hv_nR.TupleEqual(1))) != 0)
                    {
                        ho_Rsmoothedj.Dispose();
                        ho_Rsmoothedj = ho_Rs.CopyObj(1, -1);
                    }
                    else
                    {
                        ho_Rinside.Dispose();
                        HOperatorSet.GenEmptyObj(out ho_Rinside);
                        HTuple end_val26 = hv_nR;
                        HTuple step_val26 = 1;
                        for (hv_k = 2; hv_k.Continue(end_val26, step_val26); hv_k = hv_k.TupleAdd(step_val26))
                        {
                            ho_Rsn.Dispose();
                            HOperatorSet.SelectObj(ho_Regions, out ho_Rsn, hv_k);
                            {
                                HObject ExpTmpOutVar_0;
                                HOperatorSet.ConcatObj(ho_Rinside, ho_Rsn, out ExpTmpOutVar_0);
                                ho_Rinside.Dispose();
                                ho_Rinside = ExpTmpOutVar_0;
                            }
                        }
                        ho_Rsmoothedj.Dispose();
                        HOperatorSet.Difference(ho_Rs, ho_Rinside, out ho_Rsmoothedj);
                    }
                    {
                        HObject ExpTmpOutVar_0;
                        HOperatorSet.ConcatObj(ho_RegionsGsm, ho_Rsmoothedj, out ExpTmpOutVar_0);
                        ho_RegionsGsm.Dispose();
                        ho_RegionsGsm = ExpTmpOutVar_0;
                    }
                    //*         else



                }
            }
            else
            {
                ho_RegionsGsm.Dispose();
                ho_RegionsGsm = ho_RegionsG.CopyObj(1, -1);
            }
            ho_RegGsel.Dispose();
            ho_Contours.Dispose();
            ho_SmoothedContours.Dispose();
            ho_Regions.Dispose();
            ho_Rs.Dispose();
            ho_Rsmoothedj.Dispose();
            ho_Rinside.Dispose();
            ho_Rsn.Dispose();

            return;
        }

        public void PatternWidthDirImages(HObject ho_RegionG, HObject ho_SkeletonG, out HObject ho_WidthImageP,
            out HObject ho_DirImageP, out HObject ho_AngleImageP, HTuple hv_WidthG, HTuple hv_HeightG)
        {




            // Local iconic variables 

            HObject ho_DistanceImageGi, ho_DistanceImageG;
            HObject ho_WidthImageP4, ho_ImageResult, ho_SkContoursG;
            HObject ho_SelectedContourG = null, ho_RegionSCG = null;

            // Local control variables 

            HTuple hv_nSkCG = null, hv_k = null, hv_np = new HTuple();
            HTuple hv_RowG = new HTuple(), hv_ColG = new HTuple();
            HTuple hv_lookaround = new HTuple(), hv_AnglesG = new HTuple();
            HTuple hv_nA = new HTuple(), hv_AnglesG1 = new HTuple();
            HTuple hv_AnglesG2 = new HTuple(), hv_AnglesG3 = new HTuple();
            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_WidthImageP);
            HOperatorSet.GenEmptyObj(out ho_DirImageP);
            HOperatorSet.GenEmptyObj(out ho_AngleImageP);
            HOperatorSet.GenEmptyObj(out ho_DistanceImageGi);
            HOperatorSet.GenEmptyObj(out ho_DistanceImageG);
            HOperatorSet.GenEmptyObj(out ho_WidthImageP4);
            HOperatorSet.GenEmptyObj(out ho_ImageResult);
            HOperatorSet.GenEmptyObj(out ho_SkContoursG);
            HOperatorSet.GenEmptyObj(out ho_SelectedContourG);
            HOperatorSet.GenEmptyObj(out ho_RegionSCG);
            //** Width image for Pattern
            ho_DistanceImageGi.Dispose();
            HOperatorSet.DistanceTransform(ho_RegionG, out ho_DistanceImageGi, "euclidean",
                "true", hv_WidthG, hv_HeightG);

            //reduce_domain (DistanceImageG, SkeletonG, ImageReduced)
            ho_WidthImageP4.Dispose();
            HOperatorSet.ScaleImage(ho_DistanceImageGi, out ho_WidthImageP4, 2, -1);
            ho_DistanceImageG.Dispose();
            HOperatorSet.MeanImage(ho_WidthImageP4, out ho_DistanceImageG, 3, 3);

            ho_WidthImageP.Dispose();
            HOperatorSet.ConvertImageType(ho_DistanceImageG, out ho_WidthImageP, "byte");
            ho_ImageResult.Dispose();
            HOperatorSet.ConvolImage(ho_WidthImageP, out ho_ImageResult, "lowpas_3_3", "mirrored");
            //**
            //------- Dir image for Pattern
            ho_DirImageP.Dispose();
            HOperatorSet.CopyImage(ho_WidthImageP, out ho_DirImageP);
            ho_SkContoursG.Dispose();
            HOperatorSet.GenContoursSkeletonXld(ho_SkeletonG, out ho_SkContoursG, 10, "filter");
            //dev_update_off ()
            ho_AngleImageP.Dispose();
            HOperatorSet.GenImageConst(out ho_AngleImageP, "real", hv_WidthG, hv_HeightG);
            HOperatorSet.CountObj(ho_SkContoursG, out hv_nSkCG);
            HTuple end_val16 = hv_nSkCG;
            HTuple step_val16 = 1;
            for (hv_k = 1; hv_k.Continue(end_val16, step_val16); hv_k = hv_k.TupleAdd(step_val16))
            {
                //if (k==1452)
                //stop ()
                //endif
                ho_SelectedContourG.Dispose();
                HOperatorSet.SelectObj(ho_SkContoursG, out ho_SelectedContourG, hv_k);
                HOperatorSet.ContourPointNumXld(ho_SelectedContourG, out hv_np);
                HOperatorSet.GetContourXld(ho_SelectedContourG, out hv_RowG, out hv_ColG);
                ho_RegionSCG.Dispose();
                HOperatorSet.GenRegionPoints(out ho_RegionSCG, hv_RowG, hv_ColG);
                if ((int)(new HTuple(hv_np.TupleLess(21))) != 0)
                {
                    hv_lookaround = 1;
                }
                else
                {
                    hv_lookaround = 9;
                }
                HOperatorSet.GetContourAngleXld(ho_SelectedContourG, "abs", "regress", hv_lookaround,
                    out hv_AnglesG);
                HOperatorSet.TupleLength(hv_AnglesG, out hv_nA);

                HOperatorSet.SetGrayval(ho_AngleImageP, hv_RowG.TupleSelect(0), hv_ColG.TupleSelect(
                    0), (hv_AnglesG.TupleSelect(0)) - 3.14159);
                HOperatorSet.SetGrayval(ho_AngleImageP, hv_RowG.TupleSelect(1), hv_ColG.TupleSelect(
                    1), (hv_AnglesG.TupleSelect(1)) - 3.14159);
                HOperatorSet.SetGrayval(ho_AngleImageP, hv_RowG.TupleSelect(2), hv_ColG.TupleSelect(
                    2), (hv_AnglesG.TupleSelect(2)) - 3.14159);
                HOperatorSet.SetGrayval(ho_AngleImageP, hv_RowG.TupleSelect(hv_nA - 1), hv_ColG.TupleSelect(
                    hv_nA - 1), hv_AnglesG.TupleSelect(hv_nA - 1));
                HOperatorSet.SetGrayval(ho_AngleImageP, hv_RowG.TupleSelect(hv_nA - 2), hv_ColG.TupleSelect(
                    hv_nA - 2), hv_AnglesG.TupleSelect(hv_nA - 2));
                HOperatorSet.SetGrayval(ho_AngleImageP, hv_RowG.TupleSelect(hv_nA - 3), hv_ColG.TupleSelect(
                    hv_nA - 3), hv_AnglesG.TupleSelect(hv_nA - 3));
                //

                HOperatorSet.TupleAdd(hv_AnglesG, (new HTuple(90)).TupleRad(), out hv_AnglesG1);
                HOperatorSet.TupleFmod(hv_AnglesG1, (new HTuple(180)).TupleRad(), out hv_AnglesG2);
                HOperatorSet.TupleMult(hv_AnglesG2, 256.0 / ((new HTuple(180)).TupleRad()), out hv_AnglesG3);
                HOperatorSet.SetGrayval(ho_DirImageP, hv_RowG, hv_ColG, hv_AnglesG3);
            }

            ho_DistanceImageGi.Dispose();
            ho_DistanceImageG.Dispose();
            ho_WidthImageP4.Dispose();
            ho_ImageResult.Dispose();
            ho_SkContoursG.Dispose();
            ho_SelectedContourG.Dispose();
            ho_RegionSCG.Dispose();

            return;
        }


        public void SpaceWidthDirImages(HObject ho_RegionGS, HObject ho_SkeletonGS, out HObject ho_WidthImageS,
            out HObject ho_DirImageS, HTuple hv_WidthG, HTuple hv_HeightG)
        {




            // Local iconic variables 

            HObject ho_DistanceImageGSi, ho_DistanceImageGS;
            HObject ho_WidthImageP4, ho_SkContoursGS, ho_SelectedContourGS = null;
            HObject ho_RegionSCG = null;

            // Local control variables 

            HTuple hv_nSkCGS = null, hv_k = null, hv_np = new HTuple();
            HTuple hv_RowGS = new HTuple(), hv_ColGS = new HTuple();
            HTuple hv_lookaround = new HTuple(), hv_AnglesG = new HTuple();
            HTuple hv_AnglesG1 = new HTuple(), hv_AnglesG2 = new HTuple();
            HTuple hv_AnglesG3 = new HTuple();
            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_WidthImageS);
            HOperatorSet.GenEmptyObj(out ho_DirImageS);
            HOperatorSet.GenEmptyObj(out ho_DistanceImageGSi);
            HOperatorSet.GenEmptyObj(out ho_DistanceImageGS);
            HOperatorSet.GenEmptyObj(out ho_WidthImageP4);
            HOperatorSet.GenEmptyObj(out ho_SkContoursGS);
            HOperatorSet.GenEmptyObj(out ho_SelectedContourGS);
            HOperatorSet.GenEmptyObj(out ho_RegionSCG);
            try
            {
                //** Width image for Pattern
                ho_DistanceImageGSi.Dispose();
                HOperatorSet.DistanceTransform(ho_RegionGS, out ho_DistanceImageGSi, "euclidean",
                    "true", hv_WidthG, hv_HeightG);
                ho_DistanceImageGS.Dispose();
                HOperatorSet.MeanImage(ho_DistanceImageGSi, out ho_DistanceImageGS, 3, 3);
                //reduce_domain (DistanceImageG, SkeletonGS, ImageReduced)
                ho_WidthImageP4.Dispose();
                HOperatorSet.ScaleImage(ho_DistanceImageGS, out ho_WidthImageP4, 2, -1);
                ho_WidthImageS.Dispose();
                HOperatorSet.ConvertImageType(ho_WidthImageP4, out ho_WidthImageS, "byte");
                //**
                //------- Dir image for Pattern
                ho_DirImageS.Dispose();
                HOperatorSet.CopyImage(ho_WidthImageS, out ho_DirImageS);
                ho_SkContoursGS.Dispose();
                HOperatorSet.GenContoursSkeletonXld(ho_SkeletonGS, out ho_SkContoursGS, 10,
                    "filter");
                //dev_update_off ()

                HOperatorSet.CountObj(ho_SkContoursGS, out hv_nSkCGS);
                HTuple end_val13 = hv_nSkCGS;
                HTuple step_val13 = 1;
                for (hv_k = 1; hv_k.Continue(end_val13, step_val13); hv_k = hv_k.TupleAdd(step_val13))
                {
                    ho_SelectedContourGS.Dispose();
                    HOperatorSet.SelectObj(ho_SkContoursGS, out ho_SelectedContourGS, hv_k);
                    HOperatorSet.ContourPointNumXld(ho_SelectedContourGS, out hv_np);
                    HOperatorSet.GetContourXld(ho_SelectedContourGS, out hv_RowGS, out hv_ColGS);
                    ho_RegionSCG.Dispose();
                    HOperatorSet.GenRegionPoints(out ho_RegionSCG, hv_RowGS, hv_ColGS);
                    if ((int)(new HTuple(hv_np.TupleLess(21))) != 0)
                    {
                        hv_lookaround = 1;
                    }
                    else
                    {
                        hv_lookaround = 3;
                    }
                    HOperatorSet.GetContourAngleXld(ho_SelectedContourGS, "abs", "regress", hv_lookaround,
                        out hv_AnglesG);
                    HOperatorSet.TupleAdd(hv_AnglesG, (new HTuple(90)).TupleRad(), out hv_AnglesG1);
                    HOperatorSet.TupleFmod(hv_AnglesG1, (new HTuple(180)).TupleRad(), out hv_AnglesG2);
                    HOperatorSet.TupleMult(hv_AnglesG2, 256.0 / ((new HTuple(180)).TupleRad()),
                        out hv_AnglesG3);
                    HOperatorSet.SetGrayval(ho_DirImageS, hv_RowGS, hv_ColGS, hv_AnglesG3);
                }
                ho_DistanceImageGSi.Dispose();
                ho_DistanceImageGS.Dispose();
                ho_WidthImageP4.Dispose();
                ho_SkContoursGS.Dispose();
                ho_SelectedContourGS.Dispose();
                ho_RegionSCG.Dispose();

                return;
            }
            catch (HalconException HDevExpDefaultException)
            {
                ho_DistanceImageGSi.Dispose();
                ho_DistanceImageGS.Dispose();
                ho_WidthImageP4.Dispose();
                ho_SkContoursGS.Dispose();
                ho_SelectedContourGS.Dispose();
                ho_RegionSCG.Dispose();

                throw HDevExpDefaultException;
            }
        }

        public void FindWires3(HObject ho_RegionG, HObject ho_SkeletonG, HObject ho_RegionFlash,
              HObject ho_RegionTrace, HObject ho_DirImageP, HObject ho_WidthImageP, HObject ho_RegionsPNi,
              out HObject ho_SkeletonsWire, out HObject ho_Rnoint, HTuple hv_lTrmin, HTuple hv_nWr,
              HTuple hv_wpl, HTuple hv_sgap, out HTuple hv_nTr)
        {




            // Stack for temporary objects 
            HObject[] OTemp = new HObject[20];

            // Local iconic variables 

            HObject ho_RegionFlashDl, ho_SkeletonGP, ho_SkeletonTr;
            HObject ho_SkeletonTrnoFlDl, ho_SkeletonsTrnoFlDl, ho_Wires;
            HObject ho_WiresC, ho_Tr = null, ho_ContourTr = null, ho_WiC = null;
            HObject ho_RegionsLines = null, ho_RegionPNi = null, ho_RegionGiPNi = null;
            HObject ho_RegionGiPNiB = null, ho_RegionLine = null, ho_RegionIntersection2 = null;
            HObject ho_P2s = null, ho_RegionP1 = null, ho_RegionP2 = null;
            HObject ho_RegionPC = null, ho_RegionP = null;

            // Local control variables 

            HTuple hv_nWrOut = null, hv_j = null, hv_lTr = new HTuple();
            HTuple hv_numnz = null, hv_Rw = new HTuple(), hv_Cw = new HTuple();
            HTuple hv_pN = new HTuple(), hv_Dirs = new HTuple(), hv_Widths = new HTuple();
            HTuple hv_Dirsi = new HTuple(), hv_nD = new HTuple(), hv_jjj = new HTuple();
            HTuple hv_d = new HTuple(), hv_WG = new HTuple(), hv_Dir = new HTuple();
            HTuple hv_Ang = new HTuple(), hv_Y1 = new HTuple(), hv_X1 = new HTuple();
            HTuple hv_Y2 = new HTuple(), hv_X2 = new HTuple(), hv_YC = new HTuple();
            HTuple hv_XC = new HTuple(), hv_k = new HTuple(), hv_D = new HTuple();
            HTuple hv_n2s = new HTuple(), hv_A2 = new HTuple(), hv_Row2 = new HTuple();
            HTuple hv_Column2 = new HTuple(), hv_Db = new HTuple();
            HTuple hv_Histo = new HTuple(), hv_BinSize = new HTuple();
            HTuple hv_Indices = new HTuple(), hv_Inverted = new HTuple();
            HTuple hv_dmedb = new HTuple(), hv_DS = new HTuple(), hv_dmax = new HTuple();
            HTuple hv_dmin = new HTuple(), hv_dd = new HTuple(), hv_ddmed = new HTuple();
            HTuple hv_k1 = new HTuple(), hv_De = new HTuple(), hv_dmede = new HTuple();
            HTuple hv_k2 = new HTuple(), hv_Dm = new HTuple(), hv_X1m = new HTuple();
            HTuple hv_Y1m = new HTuple(), hv_X2m = new HTuple(), hv_Y2m = new HTuple();
            HTuple hv_XCm = new HTuple(), hv_YCm = new HTuple();
            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_SkeletonsWire);
            HOperatorSet.GenEmptyObj(out ho_Rnoint);
            HOperatorSet.GenEmptyObj(out ho_RegionFlashDl);
            HOperatorSet.GenEmptyObj(out ho_SkeletonGP);
            HOperatorSet.GenEmptyObj(out ho_SkeletonTr);
            HOperatorSet.GenEmptyObj(out ho_SkeletonTrnoFlDl);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsTrnoFlDl);
            HOperatorSet.GenEmptyObj(out ho_Wires);
            HOperatorSet.GenEmptyObj(out ho_WiresC);
            HOperatorSet.GenEmptyObj(out ho_Tr);
            HOperatorSet.GenEmptyObj(out ho_ContourTr);
            HOperatorSet.GenEmptyObj(out ho_WiC);
            HOperatorSet.GenEmptyObj(out ho_RegionsLines);
            HOperatorSet.GenEmptyObj(out ho_RegionPNi);
            HOperatorSet.GenEmptyObj(out ho_RegionGiPNi);
            HOperatorSet.GenEmptyObj(out ho_RegionGiPNiB);
            HOperatorSet.GenEmptyObj(out ho_RegionLine);
            HOperatorSet.GenEmptyObj(out ho_RegionIntersection2);
            HOperatorSet.GenEmptyObj(out ho_P2s);
            HOperatorSet.GenEmptyObj(out ho_RegionP1);
            HOperatorSet.GenEmptyObj(out ho_RegionP2);
            HOperatorSet.GenEmptyObj(out ho_RegionPC);
            HOperatorSet.GenEmptyObj(out ho_RegionP);
            try
            {
                hv_nWrOut = hv_nWr.Clone();
                ho_RegionFlashDl.Dispose();
                HOperatorSet.DilationCircle(ho_RegionFlash, out ho_RegionFlashDl, 2.5);
                //skeleton (RegionG, SkeletonG)
                HOperatorSet.SetSystem("neighborhood", 8);

                ho_SkeletonGP.Dispose();
                HOperatorSet.Pruning(ho_SkeletonG, out ho_SkeletonGP, 2);

                //symm_difference (SkeletonGP, SkeletonG, RegionDifference2)
                ho_SkeletonTr.Dispose();
                HOperatorSet.Intersection(ho_SkeletonGP, ho_RegionTrace, out ho_SkeletonTr);
                ho_SkeletonTrnoFlDl.Dispose();
                HOperatorSet.Difference(ho_SkeletonTr, ho_RegionFlashDl, out ho_SkeletonTrnoFlDl
                    );
                ho_SkeletonsTrnoFlDl.Dispose();
                HOperatorSet.Connection(ho_SkeletonTrnoFlDl, out ho_SkeletonsTrnoFlDl);
                HOperatorSet.CountObj(ho_SkeletonsTrnoFlDl, out hv_nTr);

                ho_Wires.Dispose();
                HOperatorSet.GenEmptyObj(out ho_Wires);
                ho_WiresC.Dispose();
                HOperatorSet.GenEmptyObj(out ho_WiresC);
                HTuple end_val15 = hv_nTr;
                HTuple step_val15 = 1;
                for (hv_j = 1; hv_j.Continue(end_val15, step_val15); hv_j = hv_j.TupleAdd(step_val15))
                {
                    ho_Tr.Dispose();
                    HOperatorSet.SelectObj(ho_SkeletonsTrnoFlDl, out ho_Tr, hv_j);
                    ho_ContourTr.Dispose();
                    HOperatorSet.GenContoursSkeletonXld(ho_Tr, out ho_ContourTr, 1, "filter");
                    HOperatorSet.LengthXld(ho_ContourTr, out hv_lTr);
                    if ((int)(new HTuple(hv_lTr.TupleLess(hv_lTrmin))) != 0)
                    {
                        continue;
                    }
                    {
                        HObject ExpTmpOutVar_0;
                        HOperatorSet.ConcatObj(ho_Wires, ho_Tr, out ExpTmpOutVar_0);
                        ho_Wires.Dispose();
                        ho_Wires = ExpTmpOutVar_0;
                    }
                    {
                        HObject ExpTmpOutVar_0;
                        HOperatorSet.ConcatObj(ho_WiresC, ho_ContourTr, out ExpTmpOutVar_0);
                        ho_WiresC.Dispose();
                        ho_WiresC = ExpTmpOutVar_0;
                    }
                    hv_nWrOut = hv_nWrOut + 1;
                }
                if (HDevWindowStack.IsOpen())
                {
                    HOperatorSet.DispText(HDevWindowStack.GetActive(), "Traces examined", "window",
                        "top", "left", "black", new HTuple(), new HTuple());
                }

                hv_numnz = 0;
                ho_Rnoint.Dispose();
                HOperatorSet.GenEmptyObj(out ho_Rnoint);
                ho_SkeletonsWire.Dispose();
                HOperatorSet.GenEmptyObj(out ho_SkeletonsWire);
                HTuple end_val31 = hv_nWrOut;
                HTuple step_val31 = 1;
                for (hv_j = 1; hv_j.Continue(end_val31, step_val31); hv_j = hv_j.TupleAdd(step_val31))
                {
                    //j := 1896
                    //if (j==1386)
                    //stop ()
                    //endif
                    ho_WiC.Dispose();
                    HOperatorSet.SelectObj(ho_WiresC, out ho_WiC, hv_j);
                    HOperatorSet.GetContourXld(ho_WiC, out hv_Rw, out hv_Cw);
                    HOperatorSet.TupleLength(hv_Rw, out hv_pN);
                    if ((int)(new HTuple(hv_pN.TupleLess(50))) != 0)
                    {
                        continue;
                    }
                    HOperatorSet.GetGrayval(ho_DirImageP, hv_Rw, hv_Cw, out hv_Dirs);
                    HOperatorSet.GetGrayval(ho_WidthImageP, hv_Rw, hv_Cw, out hv_Widths);
                    hv_Dirsi = hv_Dirs.Clone();
                    HOperatorSet.TupleLength(hv_Dirs, out hv_nD);
                    HTuple end_val46 = hv_nD - 2;
                    HTuple step_val46 = 1;
                    for (hv_jjj = 0; hv_jjj.Continue(end_val46, step_val46); hv_jjj = hv_jjj.TupleAdd(step_val46))
                    {
                        //if (jjj==641)
                        //stop ()
                        //endif
                        //jjj := nD-6
                        hv_d = (hv_Dirsi.TupleSelect(hv_jjj + 1)) - (hv_Dirsi.TupleSelect(hv_jjj + 0));
                        if ((int)(new HTuple(hv_d.TupleGreater(128))) != 0)
                        {
                            hv_d = hv_d - 255;
                        }
                        else if ((int)(new HTuple(hv_d.TupleLess(-128))) != 0)
                        {
                            hv_d = hv_d + 255;
                        }
                        if (hv_Dirs == null)
                            hv_Dirs = new HTuple();
                        hv_Dirs[hv_jjj + 1] = (hv_Dirs.TupleSelect(hv_jjj + 0)) + hv_d;
                    }
                    HOperatorSet.GetGrayval(ho_WidthImageP, hv_Rw, hv_Cw, out hv_WG);
                    HOperatorSet.GetGrayval(ho_DirImageP, hv_Rw, hv_Cw, out hv_Dir);
                    hv_Ang = ((hv_Dir * 1.0) * 3.1415926) / 256.0;
                    hv_Y1 = hv_Rw - ((hv_Ang.TupleSin()) * (((hv_WG / 2) + hv_wpl) - hv_sgap));
                    hv_X1 = hv_Cw + ((hv_Ang.TupleCos()) * (((hv_WG / 2) + hv_wpl) - hv_sgap));
                    hv_Y2 = hv_Rw + ((hv_Ang.TupleSin()) * (((hv_WG / 2) + hv_wpl) - hv_sgap));
                    hv_X2 = hv_Cw - ((hv_Ang.TupleCos()) * (((hv_WG / 2) + hv_wpl) - hv_sgap));
                    hv_YC = hv_Rw.Clone();
                    hv_XC = hv_Cw.Clone();

                    hv_k = hv_nD - 1;
                    ho_RegionsLines.Dispose();
                    HOperatorSet.GenRegionLine(out ho_RegionsLines, hv_Y1.TupleSelect(hv_k),
                        hv_X1.TupleSelect(hv_k), hv_Y2.TupleSelect(hv_k), hv_X2.TupleSelect(hv_k));

                    ho_RegionPNi.Dispose();
                    HOperatorSet.SelectRegionPoint(ho_RegionsPNi, out ho_RegionPNi, hv_YC.TupleSelect(
                        hv_nD / 2), hv_XC.TupleSelect(hv_nD / 2));
                    ho_RegionGiPNi.Dispose();
                    HOperatorSet.Intersection(ho_RegionG, ho_RegionPNi, out ho_RegionGiPNi);
                    ho_RegionGiPNiB.Dispose();
                    HOperatorSet.Boundary(ho_RegionGiPNi, out ho_RegionGiPNiB, "inner");
                    if (HDevWindowStack.IsOpen())
                    {
                        //dev_disp_text ('MS built', 'window', 'top', 'left', 'black', [], [])
                    }
                    hv_D = new HTuple();
                    HTuple end_val77 = hv_nD - 1;
                    HTuple step_val77 = 1;
                    for (hv_k = 0; hv_k.Continue(end_val77, step_val77); hv_k = hv_k.TupleAdd(step_val77))
                    {
                        ho_RegionLine.Dispose();
                        HOperatorSet.GenRegionLine(out ho_RegionLine, hv_Y1.TupleSelect(hv_k),
                            hv_X1.TupleSelect(hv_k), hv_Y2.TupleSelect(hv_k), hv_X2.TupleSelect(
                            hv_k));
                        ho_RegionIntersection2.Dispose();
                        HOperatorSet.Intersection(ho_RegionGiPNiB, ho_RegionLine, out ho_RegionIntersection2
                            );
                        ho_P2s.Dispose();
                        HOperatorSet.Connection(ho_RegionIntersection2, out ho_P2s);
                        HOperatorSet.CountObj(ho_P2s, out hv_n2s);
                        if ((int)(new HTuple(hv_n2s.TupleNotEqual(2))) != 0)
                        {
                            //stop ()
                            hv_d = 100;
                        }
                        else
                        {
                            HOperatorSet.AreaCenter(ho_P2s, out hv_A2, out hv_Row2, out hv_Column2);
                            HOperatorSet.DistancePp(hv_Row2.TupleSelect(0), hv_Column2.TupleSelect(
                                0), hv_Row2.TupleSelect(1), hv_Column2.TupleSelect(1), out hv_d);

                        }
                        HOperatorSet.TupleConcat(hv_D, hv_d, out hv_D);
                    }
                    if (HDevWindowStack.IsOpen())
                    {
                        //dev_disp_text ('Dist. measured', 'window', 'top', 'left', 'black', [], [])
                    }
                    hv_k = 0;
                    HOperatorSet.TupleSelectRange(hv_D, 0, (hv_nD / 2) - 1, out hv_Db);
                    HOperatorSet.TupleHistoRange(hv_Db, 0, 100, 50, out hv_Histo, out hv_BinSize);
                    HOperatorSet.TupleSortIndex(hv_Histo, out hv_Indices);
                    HOperatorSet.TupleInverse(hv_Indices, out hv_Inverted);
                    hv_dmedb = (hv_Inverted.TupleSelect(0)) * 2;
                    do
                    {
                        HOperatorSet.TupleSelectRange(hv_D, hv_k, hv_k + 5, out hv_DS);
                        HOperatorSet.TupleMax(hv_DS, out hv_dmax);
                        HOperatorSet.TupleMin(hv_DS, out hv_dmin);
                        hv_dd = hv_dmax - hv_dmin;
                        hv_ddmed = ((((hv_dmedb - hv_dmax)).TupleAbs())).TupleMax2(((hv_dmedb - hv_dmin)).TupleAbs()
                            );
                        hv_k = hv_k + 1;
                    }
                    while ((int)((new HTuple((new HTuple(hv_dd.TupleLess(3))).TupleAnd(new HTuple(hv_ddmed.TupleLess(
                        5))))).TupleAnd(new HTuple(hv_dmax.TupleNotEqual(100)))) == 0);
                    //    *until (dd<3 and ddmed < 5 and dmax!=100 or k>nD-6) 
                    hv_k1 = hv_k.Clone();
                    //stop ()
                    hv_k = 0;
                    //tuple_select_range (D, nD-db-1, nD-db, De)
                    //tuple_median (De, dmede)
                    HOperatorSet.TupleSelectRange(hv_D, hv_nD / 2, hv_nD - 1, out hv_De);
                    HOperatorSet.TupleHistoRange(hv_De, 0, 100, 50, out hv_Histo, out hv_BinSize);
                    HOperatorSet.TupleSortIndex(hv_Histo, out hv_Indices);
                    HOperatorSet.TupleInverse(hv_Indices, out hv_Inverted);
                    hv_dmede = (hv_Inverted.TupleSelect(0)) * 2;
                    do
                    {

                        HOperatorSet.TupleSelectRange(hv_D, hv_nD - (hv_k + 6), (hv_nD - hv_k) - 1, out hv_DS);
                        HOperatorSet.TupleMax(hv_DS, out hv_dmax);
                        HOperatorSet.TupleMin(hv_DS, out hv_dmin);
                        hv_dd = hv_dmax - hv_dmin;

                        hv_ddmed = ((((hv_dmede - hv_dmax)).TupleAbs())).TupleMax2(((hv_dmede - hv_dmin)).TupleAbs()
                            );
                        hv_k = hv_k + 1;
                    }
                    while ((int)((new HTuple((new HTuple(hv_dd.TupleLess(3))).TupleAnd(new HTuple(hv_ddmed.TupleLess(
                        5))))).TupleAnd(new HTuple(hv_dmax.TupleNotEqual(100)))) == 0);
                    hv_k2 = hv_nD - hv_k;
                    //stop ()
                    HOperatorSet.TupleSelectRange(hv_D, hv_k1, hv_k2, out hv_Dm);
                    HOperatorSet.TupleSelectRange(hv_X1, hv_k1, hv_k2, out hv_X1m);
                    HOperatorSet.TupleSelectRange(hv_Y1, hv_k1, hv_k2, out hv_Y1m);
                    HOperatorSet.TupleSelectRange(hv_X2, hv_k1, hv_k2, out hv_X2m);
                    HOperatorSet.TupleSelectRange(hv_Y2, hv_k1, hv_k2, out hv_Y2m);
                    HOperatorSet.TupleSelectRange(hv_XC, hv_k1, hv_k2, out hv_XCm);
                    HOperatorSet.TupleSelectRange(hv_YC, hv_k1, hv_k2, out hv_YCm);
                    //stop ()
                    ho_RegionP1.Dispose();
                    HOperatorSet.GenRegionPoints(out ho_RegionP1, hv_Y1m, hv_X1m);
                    ho_RegionP2.Dispose();
                    HOperatorSet.GenRegionPoints(out ho_RegionP2, hv_Y2m, hv_X2m);
                    ho_RegionPC.Dispose();
                    HOperatorSet.GenRegionPoints(out ho_RegionPC, hv_YCm, hv_XCm);
                    ho_RegionP.Dispose();
                    HOperatorSet.Union2(ho_RegionP1, ho_RegionP2, out ho_RegionP);
                    if (HDevWindowStack.IsOpen())
                    {
                        HOperatorSet.DispObj(ho_RegionPC, HDevWindowStack.GetActive());
                    }
                    //*** finding corresponding RegionPNi ****


                    //intersection (RegionP, RegionGiPNi, RegionIntersection)

                    //area_center (RegionIntersection, a, r3, c3)
                    //if (a>1)


                    //intersection (RegionsLines, RegionIntersection, RegionIntersection1)
                    //area_center (RegionIntersection1, Area, Row3, Column3)
                    //tuple_not (Area, Not)
                    //tuple_select_mask (X1, Not, X1s)
                    //tuple_select_mask (Y1, Not, Y1s)
                    //tuple_select_mask (X2, Not, X2s)
                    //tuple_select_mask (Y2, Not, Y2s)
                    //tuple_select_mask (XC, Not, XCs)
                    //tuple_select_mask (YC, Not, YCs)

                    //gen_region_points (RegionPCs, YCs, XCs)
                    //connection (RegionPCs, RegionPsCs)
                    //select_shape (RegionPsCs, SelectedRegions, 'area', 'and', 15, 99999999)
                    //union1 (SelectedRegions, SelectedRegion)
                    //concat_obj (SkeletonsWire, SelectedRegion, SkeletonsWire)

                    //numnz := numnz+1

                    //else
                    {
                        HObject ExpTmpOutVar_0;
                        HOperatorSet.ConcatObj(ho_SkeletonsWire, ho_RegionPC, out ExpTmpOutVar_0
                            );
                        ho_SkeletonsWire.Dispose();
                        ho_SkeletonsWire = ExpTmpOutVar_0;
                    }
                    //endif
                    //connection (RegionIntersection, RegionsIntersection)
                    //count_obj (RegionsIntersection, nInt)
                    //for jj := 1 to nInt by 1
                    //select_obj (RegionsIntersection, ObjectSelected1, jj)
                    //get_region_points (ObjectSelected1, Rows1, Columns1)
                    //tuple_length (Rows1, l)
                    //select_region_point (RegionsLines, Reg, Rows1[l/2], Columns1[l/2])
                    //stop ()
                    //endfor
                    //concat_obj (Rnoint, RegionIntersection, Rnoint)
                    //area_center (RegionIntersection, a, Row2, Column2)
                    //if (a>0)
                    //numnz := numnz+1
                    //endif
                    //union1 (RegionsLines, RegionLines)
                    //stop ()
                }
                ho_RegionFlashDl.Dispose();
                ho_SkeletonGP.Dispose();
                ho_SkeletonTr.Dispose();
                ho_SkeletonTrnoFlDl.Dispose();
                ho_SkeletonsTrnoFlDl.Dispose();
                ho_Wires.Dispose();
                ho_WiresC.Dispose();
                ho_Tr.Dispose();
                ho_ContourTr.Dispose();
                ho_WiC.Dispose();
                ho_RegionsLines.Dispose();
                ho_RegionPNi.Dispose();
                ho_RegionGiPNi.Dispose();
                ho_RegionGiPNiB.Dispose();
                ho_RegionLine.Dispose();
                ho_RegionIntersection2.Dispose();
                ho_P2s.Dispose();
                ho_RegionP1.Dispose();
                ho_RegionP2.Dispose();
                ho_RegionPC.Dispose();
                ho_RegionP.Dispose();

                return;
            }
            catch (HalconException HDevExpDefaultException)
            {
                ho_RegionFlashDl.Dispose();
                ho_SkeletonGP.Dispose();
                ho_SkeletonTr.Dispose();
                ho_SkeletonTrnoFlDl.Dispose();
                ho_SkeletonsTrnoFlDl.Dispose();
                ho_Wires.Dispose();
                ho_WiresC.Dispose();
                ho_Tr.Dispose();
                ho_ContourTr.Dispose();
                ho_WiC.Dispose();
                ho_RegionsLines.Dispose();
                ho_RegionPNi.Dispose();
                ho_RegionGiPNi.Dispose();
                ho_RegionGiPNiB.Dispose();
                ho_RegionLine.Dispose();
                ho_RegionIntersection2.Dispose();
                ho_P2s.Dispose();
                ho_RegionP1.Dispose();
                ho_RegionP2.Dispose();
                ho_RegionPC.Dispose();
                ho_RegionP.Dispose();

                throw HDevExpDefaultException;
            }
        }

        public void ExtendedSkeletonWFMP5(HObject ho_RegionG, HObject ho_SkeletonsFWM,
              HObject ho_SkeletonG, HObject ho_WidthImageP, HObject ho_DirImageP, out HObject ho_SkeletonParts,
              out HObject ho_WidthImagePout, out HObject ho_DirImagePout, HTuple hv_elengthend,
              HTuple hv_elengthnoend, HTuple hv_jpadd)
        {

            // Stack for temporary objects 
            HObject[] OTemp = new HObject[20];

            // Local iconic variables 

            HObject ho_SkeletonFWM, ho_EndPoints, ho_JuncPoints;
            HObject ho_EndPointsd, ho_EndPointd, ho_Domain, ho_JuncPointsC;
            HObject ho_CircJP, ho_CircJPu, ho_SelectedSkeletonG = null;
            HObject ho_SelectedSkeletonsG = null, ho_SSG = null, ho_SelectedContourG = null;
            HObject ho_RegionSCG = null, ho_ER1 = null, ho_ER2 = null, ho_Circle = null;
            HObject ho_RegionIntersection = null, ho_Rectangle21 = null;
            HObject ho_Rectangle22 = null;

            // Local control variables 

            HTuple hv_A = null, hv_Row = null, hv_Column = null;
            HTuple hv_Indices = null, hv_Sorted = null, hv_nFWM = null;
            HTuple hv_nrec = null, hv_nsk = null, hv_AJP = null, hv_RowsJP = null;
            HTuple hv_ColsJP = null, hv_Width = null, hv_Height = null;
            HTuple hv_rm = null, hv_cm = null, hv_WJP = null, hv_k = null;
            HTuple hv_nSSG = new HTuple(), hv_kk = new HTuple(), hv_a = new HTuple();
            HTuple hv_r = new HTuple(), hv_c = new HTuple(), hv_np = new HTuple();
            HTuple hv_RowG = new HTuple(), hv_ColG = new HTuple();
            HTuple hv_lookaround = new HTuple(), hv_AnglesG = new HTuple();
            HTuple hv_nA = new HTuple(), hv_IsSubset = new HTuple();
            HTuple hv_row = new HTuple(), hv_col = new HTuple(), hv_R21 = new HTuple();
            HTuple hv_C21 = new HTuple(), hv_n21 = new HTuple(), hv_ang = new HTuple();
            HTuple hv_dir21 = new HTuple(), hv_Dir21 = new HTuple();
            HTuple hv_R22 = new HTuple(), hv_C22 = new HTuple(), hv_n22 = new HTuple();
            HTuple hv_dir22 = new HTuple(), hv_Dir22 = new HTuple();
            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_SkeletonParts);
            HOperatorSet.GenEmptyObj(out ho_WidthImagePout);
            HOperatorSet.GenEmptyObj(out ho_DirImagePout);
            HOperatorSet.GenEmptyObj(out ho_SkeletonFWM);
            HOperatorSet.GenEmptyObj(out ho_EndPoints);
            HOperatorSet.GenEmptyObj(out ho_JuncPoints);
            HOperatorSet.GenEmptyObj(out ho_EndPointsd);
            HOperatorSet.GenEmptyObj(out ho_EndPointd);
            HOperatorSet.GenEmptyObj(out ho_Domain);
            HOperatorSet.GenEmptyObj(out ho_JuncPointsC);
            HOperatorSet.GenEmptyObj(out ho_CircJP);
            HOperatorSet.GenEmptyObj(out ho_CircJPu);
            HOperatorSet.GenEmptyObj(out ho_SelectedSkeletonG);
            HOperatorSet.GenEmptyObj(out ho_SelectedSkeletonsG);
            HOperatorSet.GenEmptyObj(out ho_SSG);
            HOperatorSet.GenEmptyObj(out ho_SelectedContourG);
            HOperatorSet.GenEmptyObj(out ho_RegionSCG);
            HOperatorSet.GenEmptyObj(out ho_ER1);
            HOperatorSet.GenEmptyObj(out ho_ER2);
            HOperatorSet.GenEmptyObj(out ho_Circle);
            HOperatorSet.GenEmptyObj(out ho_RegionIntersection);
            HOperatorSet.GenEmptyObj(out ho_Rectangle21);
            HOperatorSet.GenEmptyObj(out ho_Rectangle22);
            try
            {
                //** ExtendedSkeletonWFMP5 v1.0, 11.03.2023
                // 230313 - added skel. parts having multiple regions
                // 230223 - fixed possible non-equal numbers of SkeletonsFWM and SkContoursG
                ho_SkeletonFWM.Dispose();
                HOperatorSet.Union1(ho_SkeletonsFWM, out ho_SkeletonFWM);
                //junctions_skeleton (SkeletonFWM, EndPoints, JuncPoints)
                ho_EndPoints.Dispose(); ho_JuncPoints.Dispose();
                HOperatorSet.JunctionsSkeleton(ho_SkeletonG, out ho_EndPoints, out ho_JuncPoints);
                //dilation_circle (EndPoints, EndPointsd, 1.5)
                ho_EndPointsd.Dispose();
                HOperatorSet.DilationRectangle1(ho_EndPoints, out ho_EndPointsd, 2.5, 2.5);
                ho_EndPointd.Dispose();
                HOperatorSet.Union1(ho_EndPointsd, out ho_EndPointd);
                //gen_contours_skeleton_xld (SkeletonsFWM, SkContoursG, 1, 'filter')
                HOperatorSet.AreaCenter(ho_SkeletonsFWM, out hv_A, out hv_Row, out hv_Column);
                HOperatorSet.TupleSortIndex(hv_Row, out hv_Indices);
                HOperatorSet.TupleSort(hv_A, out hv_Sorted);
                //dev_update_off ()
                //gen_image_const (SkeletonParts, 'real', WidthG, HeightG)
                //count_obj (SkContoursG, nSkCG)
                HOperatorSet.CountObj(ho_SkeletonsFWM, out hv_nFWM);
                ho_SkeletonParts.Dispose();
                HOperatorSet.GenEmptyObj(out ho_SkeletonParts);
                ho_Domain.Dispose();
                HOperatorSet.GetDomain(ho_WidthImageP, out ho_Domain);
                hv_nrec = 0;
                hv_nsk = 0;
                ho_JuncPointsC.Dispose();
                HOperatorSet.Connection(ho_JuncPoints, out ho_JuncPointsC);
                HOperatorSet.AreaCenter(ho_JuncPointsC, out hv_AJP, out hv_RowsJP, out hv_ColsJP);
                HOperatorSet.GetImageSize(ho_DirImageP, out hv_Width, out hv_Height);

                HOperatorSet.TupleMax(hv_RowsJP, out hv_rm);
                HOperatorSet.TupleMax(hv_ColsJP, out hv_cm);
                if ((int)((new HTuple(hv_rm.TupleGreaterEqual(hv_Height))).TupleOr(new HTuple(hv_cm.TupleGreaterEqual(
                    hv_Width)))) != 0)
                {
                    // stop(...); only in hdevelop
                }
                HOperatorSet.GetGrayval(ho_WidthImageP, hv_RowsJP, hv_ColsJP, out hv_WJP);
                ho_CircJP.Dispose();
                HOperatorSet.GenCircle(out ho_CircJP, hv_RowsJP, hv_ColsJP, (hv_WJP / 2) + hv_jpadd);
                ho_CircJPu.Dispose();
                HOperatorSet.Union1(ho_CircJP, out ho_CircJPu);
                //get_region_points (JuncPoints, RowsJP, ColsJP)
                HTuple end_val33 = hv_nFWM;
                HTuple step_val33 = 1;
                for (hv_k = 1; hv_k.Continue(end_val33, step_val33); hv_k = hv_k.TupleAdd(step_val33))
                {
                    //if (k==65)
                    //stop ()
                    //endif

                    ho_SelectedSkeletonG.Dispose();
                    HOperatorSet.SelectObj(ho_SkeletonsFWM, out ho_SelectedSkeletonG, hv_k);
                    ho_SelectedSkeletonsG.Dispose();
                    HOperatorSet.Connection(ho_SelectedSkeletonG, out ho_SelectedSkeletonsG);
                    HOperatorSet.CountObj(ho_SelectedSkeletonsG, out hv_nSSG);
                    HTuple end_val41 = hv_nSSG;
                    HTuple step_val41 = 1;
                    for (hv_kk = 1; hv_kk.Continue(end_val41, step_val41); hv_kk = hv_kk.TupleAdd(step_val41))
                    {
                        ho_SSG.Dispose();
                        HOperatorSet.SelectObj(ho_SelectedSkeletonsG, out ho_SSG, hv_kk);
                        HOperatorSet.AreaCenter(ho_SSG, out hv_a, out hv_r, out hv_c);
                        if ((int)(new HTuple(hv_a.TupleLess(10))) != 0)
                        {
                            continue;
                        }
                        //select_obj (SkContoursG, SelectedContourG, k)
                        ho_SelectedContourG.Dispose();
                        HOperatorSet.GenContoursSkeletonXld(ho_SSG, out ho_SelectedContourG, 1,
                            "filter");

                        //if (r>600 and r<700 and c>10600 and c<10700)
                        //stop ()
                        //endif
                        //if (r>2020 and r<2050 and c>10800 and c<11000)
                        //stop ()
                        //endif
                        HOperatorSet.ContourPointNumXld(ho_SelectedContourG, out hv_np);
                        HOperatorSet.GetContourXld(ho_SelectedContourG, out hv_RowG, out hv_ColG);

                        ho_RegionSCG.Dispose();
                        HOperatorSet.GenRegionPoints(out ho_RegionSCG, hv_RowG, hv_ColG);
                        if ((int)(new HTuple(hv_np.TupleLess(21))) != 0)
                        {
                            hv_lookaround = 1;
                        }
                        else
                        {
                            hv_lookaround = 3;
                        }

                        HOperatorSet.GetContourAngleXld(ho_SelectedContourG, "abs", "regress",
                            hv_lookaround, out hv_AnglesG);
                        HOperatorSet.TupleLength(hv_AnglesG, out hv_nA);

                        ho_ER1.Dispose();
                        HOperatorSet.GenRegionPoints(out ho_ER1, hv_RowG.TupleSelect(0), hv_ColG.TupleSelect(
                            0));
                        ho_ER2.Dispose();
                        HOperatorSet.GenRegionPoints(out ho_ER2, hv_RowG.TupleSelect(hv_nA - 1),
                            hv_ColG.TupleSelect(hv_nA - 1));
                        if ((int)(new HTuple(hv_a.TupleGreater(0))) != 0)
                        {
                            HOperatorSet.TestSubsetRegion(ho_ER1, ho_EndPointd, out hv_IsSubset);
                            //            *if(IsSubset==1)
                            if ((int)(new HTuple(hv_IsSubset.TupleLessEqual(1))) != 0)
                            {
                                ho_Circle.Dispose();
                                HOperatorSet.GenCircle(out ho_Circle, hv_RowG.TupleSelect(0), hv_ColG.TupleSelect(
                                    0), hv_elengthnoend.TupleAbs());
                                ho_RegionIntersection.Dispose();
                                HOperatorSet.Intersection(ho_Circle, ho_SkeletonG, out ho_RegionIntersection
                                    );
                                if ((int)(new HTuple(hv_elengthnoend.TupleGreater(0))) != 0)
                                {
                                    {
                                        HObject ExpTmpOutVar_0;
                                        HOperatorSet.Union2(ho_SelectedSkeletonG, ho_RegionIntersection,
                                            out ExpTmpOutVar_0);
                                        ho_SelectedSkeletonG.Dispose();
                                        ho_SelectedSkeletonG = ExpTmpOutVar_0;
                                    }
                                }
                                else
                                {
                                    {
                                        HObject ExpTmpOutVar_0;
                                        HOperatorSet.Difference(ho_SelectedSkeletonG, ho_RegionIntersection,
                                            out ExpTmpOutVar_0);
                                        ho_SelectedSkeletonG.Dispose();
                                        ho_SelectedSkeletonG = ExpTmpOutVar_0;
                                    }
                                }
                                hv_nsk = hv_nsk + 1;
                            }
                            else
                            {
                                hv_row = (((hv_RowG.TupleSelect(1)) - (hv_elengthend * ((((hv_AnglesG.TupleSelect(
                                    1)) - 3.14159)).TupleSin())))).TupleRound();
                                hv_col = (((hv_ColG.TupleSelect(1)) + (hv_elengthend * ((((hv_AnglesG.TupleSelect(
                                    1)) - 3.14159)).TupleCos())))).TupleRound();
                                ho_Rectangle21.Dispose();
                                HOperatorSet.GenRectangle2(out ho_Rectangle21, hv_row, hv_col, (hv_AnglesG.TupleSelect(
                                    1)) - 3.14159, hv_elengthend, 0.25);
                                {
                                    HObject ExpTmpOutVar_0;
                                    HOperatorSet.Intersection(ho_Rectangle21, ho_Domain, out ExpTmpOutVar_0
                                        );
                                    ho_Rectangle21.Dispose();
                                    ho_Rectangle21 = ExpTmpOutVar_0;
                                }
                                {
                                    HObject ExpTmpOutVar_0;
                                    HOperatorSet.Union2(ho_SelectedSkeletonG, ho_Rectangle21, out ExpTmpOutVar_0
                                        );
                                    ho_SelectedSkeletonG.Dispose();
                                    ho_SelectedSkeletonG = ExpTmpOutVar_0;
                                }
                                HOperatorSet.GetRegionPoints(ho_Rectangle21, out hv_R21, out hv_C21);
                                HOperatorSet.TupleLength(hv_R21, out hv_n21);
                                hv_ang = hv_AnglesG.TupleSelect(1);
                                hv_dir21 = ((hv_ang - 1.57) * 256) / 3.14159;
                                HOperatorSet.TupleGenConst(hv_n21, hv_dir21, out hv_Dir21);
                                HOperatorSet.TupleMax(hv_R21, out hv_rm);
                                HOperatorSet.TupleMax(hv_C21, out hv_cm);
                                if ((int)((new HTuple(hv_rm.TupleGreaterEqual(hv_Height))).TupleOr(
                                    new HTuple(hv_cm.TupleGreaterEqual(hv_Width)))) != 0)
                                {
                                    // stop(...); only in hdevelop
                                }
                                HOperatorSet.SetGrayval(ho_DirImageP, hv_R21, hv_C21, hv_Dir21);
                                hv_nrec = hv_nrec + 1;
                            }

                            HOperatorSet.TestSubsetRegion(ho_ER2, ho_EndPointd, out hv_IsSubset);
                            if ((int)(new HTuple(hv_IsSubset.TupleLessEqual(1))) != 0)
                            {
                                ho_Circle.Dispose();
                                HOperatorSet.GenCircle(out ho_Circle, hv_RowG.TupleSelect(hv_nA - 1),
                                    hv_ColG.TupleSelect(hv_nA - 1), hv_elengthnoend.TupleAbs());
                                ho_RegionIntersection.Dispose();
                                HOperatorSet.Intersection(ho_Circle, ho_SkeletonG, out ho_RegionIntersection
                                    );
                                if ((int)(new HTuple(hv_elengthnoend.TupleGreater(0))) != 0)
                                {
                                    {
                                        HObject ExpTmpOutVar_0;
                                        HOperatorSet.Union2(ho_SelectedSkeletonG, ho_RegionIntersection,
                                            out ExpTmpOutVar_0);
                                        ho_SelectedSkeletonG.Dispose();
                                        ho_SelectedSkeletonG = ExpTmpOutVar_0;
                                    }
                                }
                                else
                                {
                                    {
                                        HObject ExpTmpOutVar_0;
                                        HOperatorSet.Difference(ho_SelectedSkeletonG, ho_RegionIntersection,
                                            out ExpTmpOutVar_0);
                                        ho_SelectedSkeletonG.Dispose();
                                        ho_SelectedSkeletonG = ExpTmpOutVar_0;
                                    }
                                }

                                hv_nsk = hv_nsk + 1;
                            }
                            else
                            {
                                hv_row = (((hv_RowG.TupleSelect(hv_nA - 2)) - (hv_elengthend * (((hv_AnglesG.TupleSelect(
                                    hv_nA - 2))).TupleSin())))).TupleRound();
                                hv_col = (((hv_ColG.TupleSelect(hv_nA - 2)) + (hv_elengthend * (((hv_AnglesG.TupleSelect(
                                    hv_nA - 2))).TupleCos())))).TupleRound();
                                ho_Rectangle22.Dispose();
                                HOperatorSet.GenRectangle2(out ho_Rectangle22, hv_row, hv_col, (hv_AnglesG.TupleSelect(
                                    hv_nA - 2)) - 3.14159, hv_elengthend, 0.01);
                                //gen_rectangle2 (Rectangle22, RowG[nA-2]-elengthend*sin(AnglesG[nA-2]), ColG[nA-2]+elengthend*cos(AnglesG[nA-2]), AnglesG[nA-2], elengthend, 0.25)
                                {
                                    HObject ExpTmpOutVar_0;
                                    HOperatorSet.Intersection(ho_Rectangle22, ho_Domain, out ExpTmpOutVar_0
                                        );
                                    ho_Rectangle22.Dispose();
                                    ho_Rectangle22 = ExpTmpOutVar_0;
                                }
                                {
                                    HObject ExpTmpOutVar_0;
                                    HOperatorSet.Union2(ho_SelectedSkeletonG, ho_Rectangle22, out ExpTmpOutVar_0
                                        );
                                    ho_SelectedSkeletonG.Dispose();
                                    ho_SelectedSkeletonG = ExpTmpOutVar_0;
                                }
                                HOperatorSet.GetRegionPoints(ho_Rectangle22, out hv_R22, out hv_C22);
                                HOperatorSet.TupleLength(hv_R22, out hv_n22);
                                hv_ang = hv_AnglesG.TupleSelect(hv_nA - 2);
                                hv_dir22 = ((hv_ang - 1.57) * 256) / 3.14159;
                                HOperatorSet.TupleGenConst(hv_n22, hv_dir22, out hv_Dir22);
                                HOperatorSet.TupleMax(hv_R22, out hv_rm);
                                HOperatorSet.TupleMax(hv_C22, out hv_cm);
                                if ((int)((new HTuple(hv_rm.TupleGreaterEqual(hv_Height))).TupleOr(
                                    new HTuple(hv_cm.TupleGreaterEqual(hv_Width)))) != 0)
                                {
                                    // stop(...); only in hdevelop
                                }
                                HOperatorSet.SetGrayval(ho_DirImageP, hv_R22, hv_C22, hv_Dir22);
                                hv_nrec = hv_nrec + 1;
                            }
                        }
                        {
                            HObject ExpTmpOutVar_0;
                            HOperatorSet.ConcatObj(ho_SkeletonParts, ho_SelectedSkeletonG, out ExpTmpOutVar_0
                                );
                            ho_SkeletonParts.Dispose();
                            ho_SkeletonParts = ExpTmpOutVar_0;
                        }
                        //get_region_points (SelectedSkeletonG, Rows, Columns)
                        //get_grayval (WidthImageP, Rows, Columns, WR)
                        //tuple_median (WR, wrm)
                        //tuple_min (WR, wrm)
                    }
                }
                {
                    HObject ExpTmpOutVar_0;
                    HOperatorSet.Difference(ho_SkeletonParts, ho_CircJPu, out ExpTmpOutVar_0);
                    ho_SkeletonParts.Dispose();
                    ho_SkeletonParts = ExpTmpOutVar_0;
                }
                ho_SkeletonFWM.Dispose();
                ho_EndPoints.Dispose();
                ho_JuncPoints.Dispose();
                ho_EndPointsd.Dispose();
                ho_EndPointd.Dispose();
                ho_Domain.Dispose();
                ho_JuncPointsC.Dispose();
                ho_CircJP.Dispose();
                ho_CircJPu.Dispose();
                ho_SelectedSkeletonG.Dispose();
                ho_SelectedSkeletonsG.Dispose();
                ho_SSG.Dispose();
                ho_SelectedContourG.Dispose();
                ho_RegionSCG.Dispose();
                ho_ER1.Dispose();
                ho_ER2.Dispose();
                ho_Circle.Dispose();
                ho_RegionIntersection.Dispose();
                ho_Rectangle21.Dispose();
                ho_Rectangle22.Dispose();

                return;
            }
            catch (HalconException HDevExpDefaultException)
            {
                ho_SkeletonFWM.Dispose();
                ho_EndPoints.Dispose();
                ho_JuncPoints.Dispose();
                ho_EndPointsd.Dispose();
                ho_EndPointd.Dispose();
                ho_Domain.Dispose();
                ho_JuncPointsC.Dispose();
                ho_CircJP.Dispose();
                ho_CircJPu.Dispose();
                ho_SelectedSkeletonG.Dispose();
                ho_SelectedSkeletonsG.Dispose();
                ho_SSG.Dispose();
                ho_SelectedContourG.Dispose();
                ho_RegionSCG.Dispose();
                ho_ER1.Dispose();
                ho_ER2.Dispose();
                ho_Circle.Dispose();
                ho_RegionIntersection.Dispose();
                ho_Rectangle21.Dispose();
                ho_Rectangle22.Dispose();

                throw HDevExpDefaultException;
            }
        }

        public void MSNBuildNonFWM1(HObject ho_RegionG, HObject ho_SkeletonPartsP, HObject ho_WidthImageP,
              HObject ho_DirImageP, HObject ho_RegionGS, HObject ho_ImageIR, out HObject ho_SkeletonFWM,
              out HObject ho_RegionsNFWM, out HObject ho_RegionsNFWMBcut, HTuple hv_sm_factor,
              HTuple hv_lookaround, HTuple hv_wpl, HTuple hv_wgntype, out HTuple hv_PnumN,
              out HTuple hv_PnumRN, out HTuple hv_Y1N, out HTuple hv_X1N, out HTuple hv_Y2N,
              out HTuple hv_X2N, out HTuple hv_PaddrCN, out HTuple hv_PaddrRN, out HTuple hv_WN,
              out HTuple hv_WGN, out HTuple hv_PregCN, out HTuple hv_PCinRnumN, out HTuple hv_PcontRN)
        {




            // Stack for temporary objects 
            HObject[] OTemp = new HObject[20];

            // Local iconic variables 

            HObject ho_RegionGu, ho_EndPointsu, ho_JuncPoints;
            HObject ho_RegionGB, ho_EndPoints, ho_Rectangles, ho_Forb;
            HObject ho_Rectanglesu, ho_RegionExpandedi, ho_RegionFWM;
            HObject ho_RegionDifference2, ho_RegionNFWM, ho_RegionsNFWMB;
            HObject ho_RegionNFWMBcut, ho_Dom, ho_ContoursAll, ho_RegsAll;
            HObject ho_Obj = null, ho_Contours1 = null, ho_Contours = null;
            HObject ho_ContoursSmi = null, ho_ContoursSm = null, ho_Contour = null;
            HObject ho_SkeletonRCgc = null;

            // Local control variables 

            HTuple hv_nEP = null, hv_A = null, hv_Row = null;
            HTuple hv_Col = null, hv_Hw = null, hv_Dir = null, hv_Ang = null;
            HTuple hv_n = new HTuple(), hv_Ones = null, hv_Area1 = null;
            HTuple hv_Row2 = null, hv_Column = null, hv_nNFWMcad = null;
            HTuple hv_RowN = null, hv_ColN = null, hv_pnum = null;
            HTuple hv_paddrR = null, hv_paddrC = null, hv_pcontn = null;
            HTuple hv_indcn = null, hv_AngN = null, hv_PlenN = null;
            HTuple hv_j = null, hv_nCont = new HTuple(), hv_nCo = new HTuple();
            HTuple hv_nnco = new HTuple(), hv_jj = new HTuple(), hv_pN = new HTuple();
            HTuple hv_r = new HTuple(), hv_c = new HTuple(), hv_gv = new HTuple();
            HTuple hv_J = new HTuple(), hv_AnglesG = new HTuple();
            HTuple hv_AnglesG1 = new HTuple(), hv_Y1t = new HTuple();
            HTuple hv_X1t = new HTuple(), hv_Y1n = new HTuple(), hv_X1n = new HTuple();
            HTuple hv_Y2n = new HTuple(), hv_X2n = new HTuple(), hv_Pnum = new HTuple();
            HTuple hv_IsInside = new HTuple(), hv_IsInside1 = new HTuple();
            HTuple hv_IsInside2 = new HTuple(), hv_IsInside3 = new HTuple();
            HTuple hv_IsInside4 = new HTuple(), hv_IsInside5 = new HTuple();
            HTuple hv_Yt = new HTuple(), hv_Xt = new HTuple();
            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_SkeletonFWM);
            HOperatorSet.GenEmptyObj(out ho_RegionsNFWM);
            HOperatorSet.GenEmptyObj(out ho_RegionsNFWMBcut);
            HOperatorSet.GenEmptyObj(out ho_RegionGu);
            HOperatorSet.GenEmptyObj(out ho_EndPointsu);
            HOperatorSet.GenEmptyObj(out ho_JuncPoints);
            HOperatorSet.GenEmptyObj(out ho_RegionGB);
            HOperatorSet.GenEmptyObj(out ho_EndPoints);
            HOperatorSet.GenEmptyObj(out ho_Rectangles);
            HOperatorSet.GenEmptyObj(out ho_Forb);
            HOperatorSet.GenEmptyObj(out ho_Rectanglesu);
            HOperatorSet.GenEmptyObj(out ho_RegionExpandedi);
            HOperatorSet.GenEmptyObj(out ho_RegionFWM);
            HOperatorSet.GenEmptyObj(out ho_RegionDifference2);
            HOperatorSet.GenEmptyObj(out ho_RegionNFWM);
            HOperatorSet.GenEmptyObj(out ho_RegionsNFWMB);
            HOperatorSet.GenEmptyObj(out ho_RegionNFWMBcut);
            HOperatorSet.GenEmptyObj(out ho_Dom);
            HOperatorSet.GenEmptyObj(out ho_ContoursAll);
            HOperatorSet.GenEmptyObj(out ho_RegsAll);
            HOperatorSet.GenEmptyObj(out ho_Obj);
            HOperatorSet.GenEmptyObj(out ho_Contours1);
            HOperatorSet.GenEmptyObj(out ho_Contours);
            HOperatorSet.GenEmptyObj(out ho_ContoursSmi);
            HOperatorSet.GenEmptyObj(out ho_ContoursSm);
            HOperatorSet.GenEmptyObj(out ho_Contour);
            HOperatorSet.GenEmptyObj(out ho_SkeletonRCgc);
            hv_PnumRN = new HTuple();
            try
            {
                ho_RegionGu.Dispose();
                HOperatorSet.Union1(ho_RegionG, out ho_RegionGu);
                ho_SkeletonFWM.Dispose();
                HOperatorSet.Union1(ho_SkeletonPartsP, out ho_SkeletonFWM);
                ho_EndPointsu.Dispose(); ho_JuncPoints.Dispose();
                HOperatorSet.JunctionsSkeleton(ho_SkeletonFWM, out ho_EndPointsu, out ho_JuncPoints
                    );
                ho_RegionGB.Dispose();
                HOperatorSet.Boundary(ho_RegionGu, out ho_RegionGB, "outer");
                ho_EndPoints.Dispose();
                HOperatorSet.Connection(ho_EndPointsu, out ho_EndPoints);
                HOperatorSet.CountObj(ho_EndPoints, out hv_nEP);
                HOperatorSet.AreaCenter(ho_EndPoints, out hv_A, out hv_Row, out hv_Col);
                HOperatorSet.GetGrayval(ho_WidthImageP, hv_Row, hv_Col, out hv_Hw);
                HOperatorSet.GetGrayval(ho_DirImageP, hv_Row, hv_Col, out hv_Dir);
                hv_Ang = (hv_Dir * ((new HTuple(180)).TupleRad())) / 256.0;
                //tuple_length (Row, n)
                HOperatorSet.TupleGenConst(hv_nEP, 2, out hv_Ones);

                ho_Rectangles.Dispose();
                HOperatorSet.GenRectangle2(out ho_Rectangles, hv_Row, hv_Col, hv_Ang, (hv_Hw / 2) + 7,
                    hv_Ones);
                ho_Forb.Dispose();
                HOperatorSet.Union2(ho_RegionGS, ho_Rectangles, out ho_Forb);
                ho_Rectanglesu.Dispose();
                HOperatorSet.Union1(ho_Rectangles, out ho_Rectanglesu);
                //expand_region (SkeletonFWM, Forb, RegionExpandedi, 'maximal', 'image')
                ho_RegionExpandedi.Dispose();
                HOperatorSet.ExpandRegion(ho_SkeletonFWM, ho_Forb, out ho_RegionExpandedi,
                    50, "image");
                //opening_circle (RegionExpandedi, RegionExpanded, 1.0)
                ho_RegionFWM.Dispose();
                HOperatorSet.DilationCircle(ho_RegionExpandedi, out ho_RegionFWM, 1.0);
                ho_RegionDifference2.Dispose();
                HOperatorSet.Difference(ho_RegionG, ho_RegionFWM, out ho_RegionDifference2);
                ho_RegionNFWM.Dispose();
                HOperatorSet.ClosingCircle(ho_RegionDifference2, out ho_RegionNFWM, 2.5);
                ho_RegionsNFWM.Dispose();
                HOperatorSet.Connection(ho_RegionNFWM, out ho_RegionsNFWM);
                ho_RegionsNFWMB.Dispose();
                HOperatorSet.Boundary(ho_RegionsNFWM, out ho_RegionsNFWMB, "outer");
                ho_RegionsNFWMBcut.Dispose();
                HOperatorSet.Difference(ho_RegionsNFWMB, ho_RegionG, out ho_RegionsNFWMBcut
                    );
                ho_RegionNFWMBcut.Dispose();
                HOperatorSet.Union1(ho_RegionsNFWMBcut, out ho_RegionNFWMBcut);
                HOperatorSet.AreaCenter(ho_RegionNFWMBcut, out hv_Area1, out hv_Row2, out hv_Column);
                HOperatorSet.CountObj(ho_RegionsNFWMBcut, out hv_nNFWMcad);
                //*** MSBuildNonFWM8 220726 ****
                //* corrected for -1
                //220111 fixed bug in inside/outside detection (lines 109-114)
                hv_RowN = new HTuple();
                hv_ColN = new HTuple();
                hv_PnumN = new HTuple();
                hv_pnum = new HTuple();
                hv_WN = new HTuple();
                //** address for each region (1 or several contours)
                hv_PaddrRN = 0;
                //** address for each contour
                hv_PaddrCN = new HTuple();
                //*** number of contours per region
                //PregN := []
                //* Region number corresponding to contour
                hv_PregCN = new HTuple();
                //* number of countours per region
                hv_PCinRnumN = new HTuple();
                //* 1st Contour number corresponding to region
                hv_PcontRN = new HTuple();
                //* current Region address
                hv_paddrR = 0;
                //* current Contour address
                hv_paddrC = 0;
                hv_pcontn = 1;
                hv_indcn = 0;
                hv_AngN = new HTuple();
                hv_PlenN = new HTuple();
                hv_Y1N = new HTuple();
                hv_X1N = new HTuple();
                hv_Y2N = new HTuple();
                hv_X2N = new HTuple();
                hv_WGN = new HTuple();

                ho_Dom.Dispose();
                HOperatorSet.GetDomain(ho_ImageIR, out ho_Dom);

                //count_obj (BoundariesNFWM, nNonWFM)

                ho_ContoursAll.Dispose();
                HOperatorSet.GenEmptyObj(out ho_ContoursAll);
                ho_RegsAll.Dispose();
                HOperatorSet.GenEmptyObj(out ho_RegsAll);
                //RegionsNFWM := RegionsNFWMBcut
                HTuple end_val69 = hv_nNFWMcad;
                HTuple step_val69 = 1;
                for (hv_j = 1; hv_j.Continue(end_val69, step_val69); hv_j = hv_j.TupleAdd(step_val69))
                {
                    ho_Obj.Dispose();
                    HOperatorSet.SelectObj(ho_RegionsNFWMBcut, out ho_Obj, hv_j);
                    //select_obj (RegionsNFWM, ObjR, j)
                    //inner_circle (RCgc, rmin, cmin, radmin)
                    ho_Contours1.Dispose();
                    HOperatorSet.GenContoursSkeletonXld(ho_Obj, out ho_Contours1, 1, "filter");
                    HOperatorSet.CountObj(ho_Contours1, out hv_nCont);
                    ho_Contours.Dispose();
                    HOperatorSet.UnionAdjacentContoursXld(ho_Contours1, out ho_Contours, 3, 1,
                        "attr_forget");
                    ho_ContoursSmi.Dispose();
                    HOperatorSet.SmoothContoursXld(ho_Contours, out ho_ContoursSmi, hv_sm_factor);
                    ho_ContoursSm.Dispose();
                    HOperatorSet.SelectContoursXld(ho_ContoursSmi, out ho_ContoursSm, "contour_length",
                        30.0, 20000000.0, -0.5, 0.5);
                    HOperatorSet.CountObj(ho_ContoursSm, out hv_nCo);
                    HOperatorSet.TupleConcat(hv_PCinRnumN, hv_nCo, out hv_PCinRnumN);
                    //** contours of each non-FWM region ( 1 or several)
                    hv_nnco = 0;
                    HOperatorSet.TupleConcat(hv_PcontRN, hv_pcontn, out hv_PcontRN);
                    hv_pcontn = hv_pcontn + hv_nCo;
                    HTuple end_val84 = hv_nCo;
                    HTuple step_val84 = 1;
                    for (hv_jj = 1; hv_jj.Continue(end_val84, step_val84); hv_jj = hv_jj.TupleAdd(step_val84))
                    {
                        ho_Contour.Dispose();
                        HOperatorSet.SelectObj(ho_ContoursSm, out ho_Contour, hv_jj);
                        HOperatorSet.GetContourXld(ho_Contour, out hv_Row, out hv_Col);
                        HOperatorSet.TupleLength(hv_Row, out hv_pN);
                        //*             if(pN>30)
                        hv_nnco = hv_nnco + 1;

                        {
                            HObject ExpTmpOutVar_0;
                            HOperatorSet.ConcatObj(ho_ContoursAll, ho_Contour, out ExpTmpOutVar_0);
                            ho_ContoursAll.Dispose();
                            ho_ContoursAll = ExpTmpOutVar_0;
                        }
                        ho_SkeletonRCgc.Dispose();
                        HOperatorSet.Skeleton(ho_Obj, out ho_SkeletonRCgc);
                        {
                            HObject ExpTmpOutVar_0;
                            HOperatorSet.Intersection(ho_SkeletonRCgc, ho_Dom, out ExpTmpOutVar_0);
                            ho_SkeletonRCgc.Dispose();
                            ho_SkeletonRCgc = ExpTmpOutVar_0;
                        }
                        HOperatorSet.GetRegionPoints(ho_SkeletonRCgc, out hv_r, out hv_c);
                        //tuple_max (c, cm)
                        HOperatorSet.GetGrayval(ho_ImageIR, hv_r, hv_c, out hv_gv);
                        HOperatorSet.TupleMedian(hv_gv, out hv_pnum);
                        //tuple_gen_const (pN, radmin*2., W)
                        HOperatorSet.TupleGenConst(hv_pN, hv_j, out hv_J);
                        HOperatorSet.TupleConcat(hv_RowN, hv_Row, out hv_RowN);
                        HOperatorSet.TupleConcat(hv_ColN, hv_Col, out hv_ColN);

                        //tuple_concat (WN, W, WN)
                        HOperatorSet.TupleConcat(hv_PaddrCN, hv_paddrC, out hv_PaddrCN);
                        hv_indcn = hv_indcn + 1;
                        HOperatorSet.TupleConcat(hv_PregCN, hv_j, out hv_PregCN);
                        hv_paddrC = hv_paddrC + hv_pN;
                        HOperatorSet.GetContourAngleXld(ho_Contour, "abs", "regress", hv_lookaround,
                            out hv_AnglesG);
                        HOperatorSet.TupleAdd(hv_AnglesG, (new HTuple(90)).TupleRad(), out hv_AnglesG1);
                        hv_Ang = hv_AnglesG1.Clone();
                        HOperatorSet.TupleConcat(hv_AngN, hv_Ang, out hv_AngN);

                        hv_Y1t = hv_Row - ((hv_Ang.TupleSin()) * 3);
                        hv_X1t = hv_Col + ((hv_Ang.TupleCos()) * 3);

                        hv_Y1n = hv_Row - ((hv_Ang.TupleSin()) * hv_wpl);
                        hv_X1n = hv_Col + ((hv_Ang.TupleCos()) * hv_wpl);
                        hv_Y2n = hv_Row + ((hv_Ang.TupleSin()) * hv_wpl);
                        hv_X2n = hv_Col - ((hv_Ang.TupleCos()) * hv_wpl);

                        HOperatorSet.TupleGenConst(hv_pN, hv_pnum, out hv_Pnum);
                        HOperatorSet.TupleConcat(hv_PnumN, hv_Pnum, out hv_PnumN);

                        //*** swapping P1 and P2 if necessary
                        //test_region_point (RegionsNFWM, Y1t[pN/2], X1t[pN/2], IsInside)
                        hv_n = hv_pN / 6;
                        HOperatorSet.TestRegionPoint(ho_RegionsNFWM, hv_Y1t.TupleSelect(hv_n),
                            hv_X1t.TupleSelect(hv_n), out hv_IsInside1);
                        HOperatorSet.TestRegionPoint(ho_RegionsNFWM, hv_Y1t.TupleSelect(2 * hv_n),
                            hv_X1t.TupleSelect(2 * hv_n), out hv_IsInside2);
                        HOperatorSet.TestRegionPoint(ho_RegionsNFWM, hv_Y1t.TupleSelect(3 * hv_n),
                            hv_X1t.TupleSelect(3 * hv_n), out hv_IsInside3);
                        HOperatorSet.TestRegionPoint(ho_RegionsNFWM, hv_Y1t.TupleSelect(4 * hv_n),
                            hv_X1t.TupleSelect(4 * hv_n), out hv_IsInside4);
                        HOperatorSet.TestRegionPoint(ho_RegionsNFWM, hv_Y1t.TupleSelect(5 * hv_n),
                            hv_X1t.TupleSelect(5 * hv_n), out hv_IsInside5);
                        HOperatorSet.TupleMedian(((((((hv_IsInside1.TupleConcat(hv_IsInside2))).TupleConcat(
                            hv_IsInside3))).TupleConcat(hv_IsInside4))).TupleConcat(hv_IsInside5),
                            out hv_IsInside);
                        if ((int)(hv_IsInside) != 0)
                        {
                            hv_Yt = hv_Y1n.Clone();
                            hv_Xt = hv_X1n.Clone();
                            hv_X1n = hv_X2n.Clone();
                            hv_Y1n = hv_Y2n.Clone();
                            hv_X2n = hv_Xt.Clone();
                            hv_Y2n = hv_Yt.Clone();
                        }
                        //***
                        if (HDevWindowStack.IsOpen())
                        {
                            HOperatorSet.SetColor(HDevWindowStack.GetActive(), "green");
                        }
                        //                get_window_attr(
                        //disp_line (H1C65EF90020, Y1n, X1n, Y2n, X2n)
                        //distance_pp (Y1n, X1n, Y2n, X2n, D)
                        //tuple_gen_const (pN, 1, Ones)
                        //gen_rectangle2 (Reg2, (Y1n+Y2n)/2., (X1n+X2n)/2., Ang, D/2., Ones)
                        //gen_region_points (RC, (Y1n+Y2n)/2., (X1n+X2n)/2.)
                        //intersection (Reg2, SkelN, Ps)
                        //union1 (SkelN, SN)
                        //distance_pr (SN, (Y1n+Y2n)/2., (X1n+X2n)/2., Dmin, Dmax)


                        HOperatorSet.TupleConcat(hv_Y1N, hv_Y1n, out hv_Y1N);
                        HOperatorSet.TupleConcat(hv_X1N, hv_X1n, out hv_X1N);
                        HOperatorSet.TupleConcat(hv_Y2N, hv_Y2n, out hv_Y2N);
                        HOperatorSet.TupleConcat(hv_X2N, hv_X2n, out hv_X2N);
                        //if (wgntype==0)
                        //tuple_concat (WGN, Dmin, WGN)
                        //else
                        //tuple_gen_const (pN, radmin, Wgn)
                        //tuple_concat (WGN, Wgn, WGN)
                        //endif
                    }
                    HOperatorSet.TupleConcat(hv_PaddrRN, hv_paddrC, out hv_PaddrRN);
                }
                ho_RegionGu.Dispose();
                ho_EndPointsu.Dispose();
                ho_JuncPoints.Dispose();
                ho_RegionGB.Dispose();
                ho_EndPoints.Dispose();
                ho_Rectangles.Dispose();
                ho_Forb.Dispose();
                ho_Rectanglesu.Dispose();
                ho_RegionExpandedi.Dispose();
                ho_RegionFWM.Dispose();
                ho_RegionDifference2.Dispose();
                ho_RegionNFWM.Dispose();
                ho_RegionsNFWMB.Dispose();
                ho_RegionNFWMBcut.Dispose();
                ho_Dom.Dispose();
                ho_ContoursAll.Dispose();
                ho_RegsAll.Dispose();
                ho_Obj.Dispose();
                ho_Contours1.Dispose();
                ho_Contours.Dispose();
                ho_ContoursSmi.Dispose();
                ho_ContoursSm.Dispose();
                ho_Contour.Dispose();
                ho_SkeletonRCgc.Dispose();

                return;
            }
            catch (HalconException HDevExpDefaultException)
            {
                ho_RegionGu.Dispose();
                ho_EndPointsu.Dispose();
                ho_JuncPoints.Dispose();
                ho_RegionGB.Dispose();
                ho_EndPoints.Dispose();
                ho_Rectangles.Dispose();
                ho_Forb.Dispose();
                ho_Rectanglesu.Dispose();
                ho_RegionExpandedi.Dispose();
                ho_RegionFWM.Dispose();
                ho_RegionDifference2.Dispose();
                ho_RegionNFWM.Dispose();
                ho_RegionsNFWMB.Dispose();
                ho_RegionNFWMBcut.Dispose();
                ho_Dom.Dispose();
                ho_ContoursAll.Dispose();
                ho_RegsAll.Dispose();
                ho_Obj.Dispose();
                ho_Contours1.Dispose();
                ho_Contours.Dispose();
                ho_ContoursSmi.Dispose();
                ho_ContoursSm.Dispose();
                ho_Contour.Dispose();
                ho_SkeletonRCgc.Dispose();

                throw HDevExpDefaultException;
            }
        }

        public void MSBuildFWM52(HObject ho_SkeletonsGThin, HObject ho_WidthImageP, HObject ho_DirImageP,
            HObject ho_ImageIR, HTuple hv_wpl, HTuple hv_notusedint, out HTuple hv_Pnum,
            out HTuple hv_Pnum2, out HTuple hv_PnumR, out HTuple hv_Y1, out HTuple hv_X1,
            out HTuple hv_Y2, out HTuple hv_X2, out HTuple hv_PaddrC, out HTuple hv_PaddrR,
            out HTuple hv_YC, out HTuple hv_XC, out HTuple hv_WG, out HTuple hv_PregC, out HTuple hv_PCinRnum,
            out HTuple hv_PcontR, out HTuple hv_Puseint)
        {




            // Local iconic variables 

            HObject ho_ObjectSelected1 = null, ho_Contours = null;
            HObject ho_Contour = null;

            // Local control variables 

            HTuple hv_RowSkTh = null, hv_ColSkTh = null;
            HTuple hv_paddrC = null, hv_paddrR = null, hv_pcont = null;
            HTuple hv_Notused = null, hv_Beg = null, hv_nSGThin = null;
            HTuple hv_j = null, hv_a = new HTuple(), hv_r = new HTuple();
            HTuple hv_c = new HTuple(), hv_pnumr = new HTuple(), hv_npreg = new HTuple();
            HTuple hv_R = new HTuple(), hv_C = new HTuple(), hv_nsp = new HTuple();
            HTuple hv_PnValr = new HTuple(), hv_nC = new HTuple();
            HTuple hv_jj = new HTuple(), hv_PnVal = new HTuple(), hv_pN = new HTuple();
            HTuple hv_Pui = new HTuple(), hv_End = new HTuple(), hv_Dirs = new HTuple();
            HTuple hv_Dirsi = new HTuple(), hv_nD = new HTuple(), hv_jjj = new HTuple();
            HTuple hv_d = new HTuple(), hv_FDirs = new HTuple(), hv_FDirsi = new HTuple();
            HTuple hv_Derivative = new HTuple(), hv_YMin = new HTuple();
            HTuple hv_YMax = new HTuple(), hv_Pn = new HTuple(), hv_nPnum = null;
            HTuple hv_pnump = null, hv_pnumc = new HTuple(), hv_Dir = null;
            HTuple hv_Ang = null, hv_K = null;
            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_ObjectSelected1);
            HOperatorSet.GenEmptyObj(out ho_Contours);
            HOperatorSet.GenEmptyObj(out ho_Contour);
            try
            {
                //*** MSBuildFWM52
                //*** 2024.02.19 - Pnum2 near changes of Pnum
                //*** 2022.06.17 - added Puseint - used intersection points tuple
                //***
                hv_RowSkTh = new HTuple();
                hv_ColSkTh = new HTuple();
                //* pnum corresponding to current MS
                hv_Pnum = new HTuple();
                //* pnum corresponding to current MS near cuts
                hv_Pnum2 = new HTuple();
                //* pnum corresponding to region
                hv_PnumR = new HTuple();
                //* start address of a contour
                hv_PaddrC = new HTuple();
                //* start address for region
                hv_PaddrR = new HTuple();
                //* Region number corresponding to contour
                hv_PregC = new HTuple();
                //* number of countours per region
                hv_PCinRnum = new HTuple();
                //* 1st Contour number corresponding to region
                hv_PcontR = new HTuple();
                //* saving intersect. points for current MS ==1, not saving ==0
                hv_Puseint = new HTuple();
                hv_paddrC = 0;
                hv_paddrR = 0;
                hv_pcont = 0;
                HOperatorSet.TupleGenConst(hv_notusedint, 0, out hv_Notused);
                HOperatorSet.TupleGenSequence(0, hv_notusedint - 1, 1, out hv_Beg);

                HOperatorSet.CountObj(ho_SkeletonsGThin, out hv_nSGThin);
                HTuple end_val31 = hv_nSGThin;
                HTuple step_val31 = 1;
                for (hv_j = 1; hv_j.Continue(end_val31, step_val31); hv_j = hv_j.TupleAdd(step_val31))
                {
                    //if (j==871)
                    //stop ()
                    //endif

                    //j := 871
                    ho_ObjectSelected1.Dispose();
                    HOperatorSet.SelectObj(ho_SkeletonsGThin, out ho_ObjectSelected1, hv_j);
                    HOperatorSet.AreaCenter(ho_ObjectSelected1, out hv_a, out hv_r, out hv_c);
                    //*** empty regions (no skeleton FWM inside of RegiondPNi[j]
                    hv_pnumr = -1;
                    //RowSkTh := 0
                    //ColSkTh := 0
                    hv_npreg = 0;
                    if ((int)(new HTuple(hv_a.TupleGreater(0))) != 0)
                    {
                        //*** non-empty regions
                        HOperatorSet.GetRegionPoints(ho_ObjectSelected1, out hv_R, out hv_C);
                        HOperatorSet.TupleLength(hv_R, out hv_nsp);

                        HOperatorSet.GetGrayval(ho_ImageIR, hv_R, hv_C, out hv_PnValr);
                        HOperatorSet.TupleMedian(hv_PnValr, out hv_pnumr);
                        //tuple_max (Val, vmax)
                        //if (vmax!=j)
                        //stop ()
                        //endif

                        //* contour is used to avoid points discontinuation ***
                        ho_Contours.Dispose();
                        HOperatorSet.GenContoursSkeletonXld(ho_ObjectSelected1, out ho_Contours,
                            1, "filter");
                        HOperatorSet.CountObj(ho_Contours, out hv_nC);
                        HOperatorSet.TupleConcat(hv_PCinRnum, hv_nC, out hv_PCinRnum);

                        HOperatorSet.TupleConcat(hv_PcontR, hv_pcont, out hv_PcontR);
                        hv_pcont = hv_pcont + hv_nC;

                        HTuple end_val64 = hv_nC;
                        HTuple step_val64 = 1;
                        for (hv_jj = 1; hv_jj.Continue(end_val64, step_val64); hv_jj = hv_jj.TupleAdd(step_val64))
                        {
                            ho_Contour.Dispose();
                            HOperatorSet.SelectObj(ho_Contours, out ho_Contour, hv_jj);
                            HOperatorSet.GetContourXld(ho_Contour, out hv_R, out hv_C);
                            HOperatorSet.GetGrayval(ho_ImageIR, hv_R, hv_C, out hv_PnVal);

                            //get_grayval (ImageIR, R, C, Val)
                            //tuple_median (Val, pnum)
                            HOperatorSet.TupleLength(hv_R, out hv_pN);

                            HOperatorSet.TupleGenConst(hv_pN, 1, out hv_Pui);
                            if ((int)(new HTuple(hv_pN.TupleGreaterEqual(hv_notusedint * 2))) != 0)
                            {
                                HOperatorSet.TupleGenSequence(hv_pN - hv_notusedint, hv_pN - 1, 1, out hv_End);
                                HOperatorSet.TupleReplace(hv_Pui, hv_Beg, hv_Notused, out hv_Pui);
                                HOperatorSet.TupleReplace(hv_Pui, hv_End, hv_Notused, out hv_Pui);
                            }

                            //*** can't skip any contour, numberings don't match!!!
                            //if (pN < 100)
                            //continue
                            //endif
                            HOperatorSet.TupleConcat(hv_PaddrC, hv_paddrC, out hv_PaddrC);
                            //pcont := pcont+1
                            hv_paddrC = hv_paddrC + hv_pN;
                            HOperatorSet.TupleConcat(hv_PregC, hv_j, out hv_PregC);
                            //if (pN>100)
                            HOperatorSet.TupleConcat(hv_RowSkTh, hv_R, out hv_RowSkTh);
                            HOperatorSet.TupleConcat(hv_ColSkTh, hv_C, out hv_ColSkTh);
                            HOperatorSet.GetGrayval(ho_DirImageP, hv_R, hv_C, out hv_Dirs);
                            hv_Dirsi = hv_Dirs.Clone();
                            HOperatorSet.TupleLength(hv_Dirs, out hv_nD);
                            HTuple end_val94 = hv_nD - 2;
                            HTuple step_val94 = 1;
                            for (hv_jjj = 0; hv_jjj.Continue(end_val94, step_val94); hv_jjj = hv_jjj.TupleAdd(step_val94))
                            {
                                //if (jjj==641)
                                //stop ()
                                //endif
                                hv_d = (hv_Dirsi.TupleSelect(hv_jjj + 1)) - (hv_Dirsi.TupleSelect(hv_jjj + 0));
                                if ((int)(new HTuple(hv_d.TupleGreater(128))) != 0)
                                {
                                    hv_d = hv_d - 255;
                                }
                                else if ((int)(new HTuple(hv_d.TupleLess(-128))) != 0)
                                {
                                    hv_d = hv_d + 255;
                                }
                                if (hv_Dirs == null)
                                    hv_Dirs = new HTuple();
                                hv_Dirs[hv_jjj + 1] = (hv_Dirs.TupleSelect(hv_jjj + 0)) + hv_d;
                            }
                            //endif
                            hv_npreg = hv_npreg + hv_pN;
                            HOperatorSet.CreateFunct1dArray(hv_Dirs, out hv_FDirs);
                            HOperatorSet.CreateFunct1dArray(hv_Dirsi, out hv_FDirsi);
                            HOperatorSet.DerivateFunct1d(hv_FDirs, "first", out hv_Derivative);
                            HOperatorSet.YRangeFunct1d(hv_Derivative, out hv_YMin, out hv_YMax);
                            if ((int)((new HTuple(hv_YMin.TupleLess(-20))).TupleOr(new HTuple(hv_YMax.TupleGreater(
                                20)))) != 0)
                            {
                                //stop ()
                            }
                            if (HDevWindowStack.IsOpen())
                            {
                                HOperatorSet.DispObj(ho_Contour, HDevWindowStack.GetActive());
                            }
                            HOperatorSet.TupleConcat(hv_Puseint, hv_Pui, out hv_Puseint);
                            HOperatorSet.TupleConcat(hv_Pnum, hv_PnVal, out hv_Pnum);
                        }
                    }
                    else
                    {
                        HOperatorSet.TupleConcat(hv_PCinRnum, 0, out hv_PCinRnum);
                        HOperatorSet.TupleConcat(hv_PcontR, -1, out hv_PcontR);
                        //tuple_concat (PaddrC, -1, PaddrC)
                    }
                    HOperatorSet.TupleGenConst(hv_npreg, hv_pnumr, out hv_Pn);
                    //tuple_concat (Pnum, Pn, Pnum)
                    HOperatorSet.TupleConcat(hv_PnumR, hv_pnumr, out hv_PnumR);
                    HOperatorSet.TupleConcat(hv_PaddrR, hv_paddrR, out hv_PaddrR);

                    hv_paddrR = hv_paddrR + hv_npreg;
                }

                //***** making Pnum2
                HOperatorSet.TupleLength(hv_Pnum, out hv_nPnum);

                HTuple end_val135 = hv_nPnum - 1;
                HTuple step_val135 = 1;
                for (hv_j = 0; hv_j.Continue(end_val135, step_val135); hv_j = hv_j.TupleAdd(step_val135))
                {
                    if (hv_Pnum2 == null)
                        hv_Pnum2 = new HTuple();
                    hv_Pnum2[hv_j] = -1;
                }
                hv_pnump = hv_Pnum.TupleSelect(0);
                HTuple end_val139 = hv_nPnum - 1;
                HTuple step_val139 = 1;
                for (hv_j = 20; hv_j.Continue(end_val139, step_val139); hv_j = hv_j.TupleAdd(step_val139))
                {
                    hv_pnumc = hv_Pnum.TupleSelect(hv_j);
                    if ((int)(new HTuple(hv_pnumc.TupleNotEqual(hv_pnump))) != 0)
                    {
                        for (hv_jj = 1; (int)hv_jj <= 20; hv_jj = (int)hv_jj + 1)
                        {
                            if (hv_Pnum2 == null)
                                hv_Pnum2 = new HTuple();
                            hv_Pnum2[hv_j - hv_jj] = hv_pnumc;
                            if (hv_Pnum2 == null)
                                hv_Pnum2 = new HTuple();
                            hv_Pnum2[hv_j + hv_jj] = hv_pnump;
                        }
                        if (hv_Pnum2 == null)
                            hv_Pnum2 = new HTuple();
                        hv_Pnum2[hv_j] = hv_pnump;
                    }
                    hv_pnump = hv_pnumc.Clone();
                }
                //** test ***
                //sj := 1100
                //pnum := PnumR[sj-1]
                //a := PaddrR[sj-1]
                //pnum1 := Pnum[a]
                //**************
                HOperatorSet.GetGrayval(ho_WidthImageP, hv_RowSkTh, hv_ColSkTh, out hv_WG);
                HOperatorSet.GetGrayval(ho_DirImageP, hv_RowSkTh, hv_ColSkTh, out hv_Dir);
                hv_Ang = ((hv_Dir * 1.0) * 3.1415926) / 256.0;
                hv_K = 1.0;
                hv_Y1 = hv_RowSkTh - ((hv_Ang.TupleSin()) * ((hv_WG / hv_K) + hv_wpl));
                hv_X1 = hv_ColSkTh + ((hv_Ang.TupleCos()) * ((hv_WG / hv_K) + hv_wpl));
                hv_Y2 = hv_RowSkTh + ((hv_Ang.TupleSin()) * ((hv_WG / hv_K) + hv_wpl));
                hv_X2 = hv_ColSkTh - ((hv_Ang.TupleCos()) * ((hv_WG / hv_K) + hv_wpl));
                hv_YC = hv_RowSkTh.Clone();
                hv_XC = hv_ColSkTh.Clone();

                //gen_region_points (P2, Y2, X2)
                ho_ObjectSelected1.Dispose();
                ho_Contours.Dispose();
                ho_Contour.Dispose();

                return;
            }
            catch (HalconException HDevExpDefaultException)
            {
                ho_ObjectSelected1.Dispose();
                ho_Contours.Dispose();
                ho_Contour.Dispose();

                throw HDevExpDefaultException;
            }
        }

        public void FindWidth(HTuple hv_WGi, HTuple hv_wside, out HTuple hv_Width)
        {



            // Local iconic variables 

            // Local control variables 

            HTuple hv_nWGi = null, hv_minWGi = null, hv_maxWGi = null;
            HTuple hv_HistoGi = null, hv_BinSize = null, hv_PrcWGi = null;
            HTuple hv_nP = null, hv_FPrcWGi = null, hv_MinPrcWG = null;
            HTuple hv_MaxPrcWG = null, hv_YMinPrcWG = null, hv_YMaxPrcWG = null;
            HTuple hv_nMax = null, hv_nMin = null, hv_Seq = null, hv_Indices = null;
            HTuple hv_Sorted = null, hv_Inverted1 = null, hv_Inverted = null;
            HTuple hv_j = null, hv_y = new HTuple(), hv_MaxIndi = null;
            HTuple hv_nMaxInd = null, hv_MaxInd = null, hv_jj = new HTuple();
            HTuple hv_curMaxInd = new HTuple(), hv_prevMaxInd = new HTuple();
            HTuple hv_difind = new HTuple(), hv_nextMaxInd = new HTuple();
            // Initialize local and output iconic variables 
            HOperatorSet.TupleLength(hv_WGi, out hv_nWGi);
            HOperatorSet.TupleMin(hv_WGi, out hv_minWGi);
            HOperatorSet.TupleMax(hv_WGi, out hv_maxWGi);
            HOperatorSet.TupleHistoRange(hv_WGi, 0, hv_maxWGi, 1 * (hv_maxWGi + 1), out hv_HistoGi,
                out hv_BinSize);
            hv_PrcWGi = (hv_HistoGi * 100.0) / hv_nWGi;
            HOperatorSet.TupleLength(hv_PrcWGi, out hv_nP);
            HOperatorSet.CreateFunct1dArray(hv_PrcWGi, out hv_FPrcWGi);

            HOperatorSet.LocalMinMaxFunct1d(hv_FPrcWGi, "plateaus_center", "true", out hv_MinPrcWG,
                out hv_MaxPrcWG);
            HOperatorSet.GetYValueFunct1d(hv_FPrcWGi, hv_MinPrcWG, "constant", out hv_YMinPrcWG);
            HOperatorSet.GetYValueFunct1d(hv_FPrcWGi, hv_MaxPrcWG, "constant", out hv_YMaxPrcWG);
            HOperatorSet.TupleLength(hv_YMaxPrcWG, out hv_nMax);
            HOperatorSet.TupleLength(hv_YMinPrcWG, out hv_nMin);

            HOperatorSet.TupleGenSequence(0, hv_nP - 1, 1, out hv_Seq);
            HOperatorSet.TupleSortIndex(hv_PrcWGi, out hv_Indices);
            HOperatorSet.TupleSort(hv_PrcWGi, out hv_Sorted);
            HOperatorSet.TupleInverse(hv_Sorted, out hv_Inverted1);
            HOperatorSet.TupleInverse(hv_Indices, out hv_Inverted);
            HTuple end_val19 = hv_nP - 1;
            HTuple step_val19 = 1;
            for (hv_j = 0; hv_j.Continue(end_val19, step_val19); hv_j = hv_j.TupleAdd(step_val19))
            {
                hv_y = hv_PrcWGi.TupleSelect(hv_Inverted.TupleSelect(hv_j));
                if ((int)(new HTuple(hv_y.TupleLess(10))) != 0)
                {
                    break;
                }
            }
            hv_Width = new HTuple();
            HOperatorSet.TupleSelectRange(hv_Inverted, 0, hv_j - 1, out hv_MaxIndi);
            HOperatorSet.TupleLength(hv_MaxIndi, out hv_nMaxInd);
            HOperatorSet.TupleSort(hv_MaxIndi, out hv_MaxInd);
            if ((int)(new HTuple(hv_nMaxInd.TupleEqual(1))) != 0)
            {
                hv_Width = hv_MaxInd.Clone();
            }
            else
            {
                HTuple end_val32 = hv_nMaxInd - 1;
                HTuple step_val32 = 1;
                for (hv_j = 1; hv_j.Continue(end_val32, step_val32); hv_j = hv_j.TupleAdd(step_val32))
                {
                    //jj := j
                    hv_curMaxInd = hv_MaxInd.TupleSelect(hv_j);
                    hv_prevMaxInd = hv_MaxInd.TupleSelect(hv_j - 1);
                    hv_difind = ((hv_curMaxInd - hv_prevMaxInd)).TupleAbs();
                    if ((int)(new HTuple(hv_difind.TupleLess(hv_wside))) != 0)
                    {
                        hv_curMaxInd = (hv_curMaxInd + hv_prevMaxInd) / 2;
                        HTuple end_val39 = hv_nMaxInd - 1;
                        HTuple step_val39 = 1;
                        for (hv_jj = hv_j + 1; hv_jj.Continue(end_val39, step_val39); hv_jj = hv_jj.TupleAdd(step_val39))
                        {
                            hv_nextMaxInd = hv_MaxInd.TupleSelect(hv_jj);
                            hv_difind = ((hv_curMaxInd - hv_nextMaxInd)).TupleAbs();
                            if ((int)(new HTuple(hv_difind.TupleLess(hv_wside))) != 0)
                            {
                                hv_curMaxInd = (hv_curMaxInd + hv_nextMaxInd) / 2;
                                hv_j = hv_jj.Clone();
                            }
                            else
                            {
                                //Width[j-1] := curMaxInd
                                HOperatorSet.TupleConcat(hv_Width, hv_curMaxInd, out hv_Width);
                                break;
                            }
                        }
                    }
                    //Width[j-1] := curMaxInd
                }
                HOperatorSet.TupleConcat(hv_Width, hv_curMaxInd, out hv_Width);
            }


            return;
        }


        public void TeachingMain128(HObject ho_RegNoProc, HObject ho_Gi, HObject ho_Im,
            HObject ho_RegionFlash, HObject ho_RegionTrace, out HObject ho_RegionG, out HObject ho_RegionGS,
            out HObject ho_SkeletonG, out HObject ho_SkeletonGS, out HObject ho_WidthImageP,
            out HObject ho_DirImageP, out HObject ho_WidthImageS, out HObject ho_DirImageS,
            out HObject ho_SkeletonsG, out HObject ho_ImageIR, out HObject ho_RegionsPNi,
            out HObject ho_RegionsPNSi, out HObject ho_RegionsNFWM, out HObject ho_BoundariesNFWM,
            out HObject ho_SkeletonFWM, out HObject ho_SkeletonsFWM, out HObject ho_SkeletonsFWMcut,
            out HObject ho_SkeletonsFWMS, out HObject ho_SkeletonsLG, out HObject ho_SkeletonsFWMScut,
            out HObject ho_RegionsGND, out HObject ho_RegionsEMPTY, out HObject ho_ImDisplacement,
            out HObject ho_SkeletonGd, out HObject ho_SkeletonPartsP, out HObject ho_EPNFWMremove,
            out HObject ho_SkeletonPartsPnoG, out HObject ho_ImageIR16, out HObject ho_ImageIRS,
            out HObject ho_RegionMS, out HObject ho_RegionsPNPi, out HObject ho_RegionsmainSNoCut,
            out HObject ho_SkelEP, out HObject ho_RegionCut, HTuple hv_GNDamin, HTuple hv_path,
            HTuple hv_fwmhw, HTuple hv_parop, HTuple hv_WidthG, HTuple hv_HeightG, HTuple hv_jrad,
            HTuple hv_gap, HTuple hv_groundpn, HTuple hv_groundgval, HTuple hv_sm_factor,
            HTuple hv_lookaround, HTuple hv_wpl, HTuple hv_smoothCADcontours, HTuple hv_maxdirSP,
            HTuple hv_wFWM, HTuple hv_wside, HTuple hv_wFWMS, HTuple hv_wsideS, HTuple hv_eradMSe,
            HTuple hv_eradNFWMremove, HTuple hv_elengthend, HTuple hv_elengthnoend, HTuple hv_jpadd,
            HTuple hv_removeGNDfromFWM, HTuple hv_cutendMSS, HTuple hv_minWGS, HTuple hv_minWG,
            HTuple hv_useODB, HTuple hv_autowidth, HTuple hv_EPexp, HTuple hv_elongLG, out HTuple hv_nFWM,
            out HTuple hv_nFWMS, out HTuple hv_nNonWFM, out HTuple hv_WG, out HTuple hv_YC,
            out HTuple hv_XC, out HTuple hv_Y1, out HTuple hv_X1, out HTuple hv_Y2, out HTuple hv_X2,
            out HTuple hv_Pnum, out HTuple hv_Pnum2, out HTuple hv_PnumR, out HTuple hv_PregC,
            out HTuple hv_PcontR, out HTuple hv_PaddrR, out HTuple hv_PaddrC, out HTuple hv_PCinRnum,
            out HTuple hv_PuseintP, out HTuple hv_WGS, out HTuple hv_YCS, out HTuple hv_XCS,
            out HTuple hv_Y1S, out HTuple hv_X1S, out HTuple hv_Y2S, out HTuple hv_X2S,
            out HTuple hv_PnumS, out HTuple hv_PnumS2, out HTuple hv_PnumRS, out HTuple hv_PregCS,
            out HTuple hv_PcontRS, out HTuple hv_PaddrRS, out HTuple hv_PaddrCS, out HTuple hv_PCinRnumS,
            out HTuple hv_PuseintS, out HTuple hv_SequenceIRB, out HTuple hv_SequenceIRBS,
            out HTuple hv_WGN, out HTuple hv_YCN, out HTuple hv_XCN, out HTuple hv_Y1N,
            out HTuple hv_X1N, out HTuple hv_Y2N, out HTuple hv_X2N, out HTuple hv_PnumN,
            out HTuple hv_PnumRN, out HTuple hv_PregCN, out HTuple hv_PcontRN, out HTuple hv_PaddrRN,
            out HTuple hv_PaddrCN, out HTuple hv_PCinRnumN, out HTuple hv_TotalPRC, out HTuple hv_CurrentOper)
        {




            // Stack for temporary objects 
            HObject[] OTemp = new HObject[20];

            // Local iconic variables 

            HObject ho_RegNoProcd, ho_DomainG, ho_RC, ho_RegProc;
            HObject ho_Rectangle, ho_RegionGi, ho_RegionGid, ho_RegionsGi;
            HObject ho_RmnoG, ho_RegNoProcC, ho_RegNoProcCd, ho_RegionsGnoholes;
            HObject ho_SelectedRegions, ho_RegionI = null, ho_RegionsGsm;
            HObject ho_RegionGsm, ho_RegionsGNDnR, ho_RegionGSi, ho_RegionsGSi;
            HObject ho_RegionsEMPTYnR, ho_RegionErosion, ho_RegionDilation;
            HObject ho_RegionIntersection, ho_SkeletonGi, ho_EndPointsG;
            HObject ho_JuncPointsG, ho_CEP, ho_RegionGSii, ho_SkeletonGSi;
            HObject ho_SkeletonGSii, ho_SkeletonsGS, ho_ConnectedRegionsS;
            HObject ho_SkelEPi, ho_JuncPoints1, ho_SkelEPu, ho_EndPointsMSSuc = null;
            HObject ho_EndPointsGS = null, ho_JuncPointsGS = null, ho_RegionDilationGS = null;
            HObject ho_RegionIntersection1 = null, ho_SkeletonLG = null;
            HObject ho_EndPointsMSS = null, ho_JuncPointsMSS = null, ho_EndPointsMSSu = null;
            HObject ho_CMSSE = null, ho_CMSSEu = null, ho_SkeletonPartsPinc;
            HObject ho_WidthImagePout, ho_DirImagePout, ho_RegForbid;
            HObject ho_RegionsPNii, ho_AngleImageP, ho_DistGPi, ho_DistGSi;
            HObject ho_DistGSii, ho_DistGP, ho_DistGS, ho_ImDisplacementi;
            HObject ho_Rd, ho_JP, ho_CJ = null, ho_Skeletonii = null, ho_SkeletonsWire = null;
            HObject ho_Rnoint = null, ho_Rmain = null, ho_ConnectedRegions = null;
            HObject ho_RegionsmainNoCut = null, ho_RegionmainNoCut = null;
            HObject ho_Regionsmain, ho_Regionmain, ho_Regionmaini, ho_SkeletonsFWMii;
            HObject ho_PC, ho_SkeletonPartsPi = null, ho_RegionsmainPNPi;
            HObject ho_SkeletonPartsPii, ho_RmainS, ho_EndPointsS, ho_JuncPointsS;
            HObject ho_JuncPointsSu, ho_CirclesSJ = null, ho_CirclesSJu = null;
            HObject ho_RmainSnoJ = null, ho_RegionsmainSs = null, ho_RegionsmainSu = null;
            HObject ho_RegionsmainSuc = null, ho_RegionsmainSn = null, ho_RegionsmainS;
            HObject ho_RegionmainSNoCut, ho_RegionmainS, ho_RegionGND;
            HObject ho_RegionGNDC, ho_RegionsNFWMBcut, ho_RegionsNFWMs;
            HObject ho_Region1, ho_Region2, ho_ImageIRBS, ho_ImageIRS16;
            HObject ho_RegForbidS, ho_EndPoints, ho_JuncPoints, ho_EP;
            HObject ho_SkeletonsFWMp, ho_CE2, ho_SkeletonsFWMSii, ho_EPS;
            HObject ho_CES, ho_SkeletonsFWMSi, ho_RegionsPNSii, ho_RegionEMPTY;
            HObject ho_RegionEMPTYC, ho_SkeletonsFWMSrc, ho_PCS;

            // Local control variables 

            HTuple hv_maxwfwm = null, hv_Row11 = null;
            HTuple hv_Column11 = null, hv_Row21 = null, hv_Column21 = null;
            HTuple hv_Area3 = null, hv_RowDKu = null, hv_ColumnDKu = null;
            HTuple hv_Row1 = new HTuple(), hv_Column1 = new HTuple();
            HTuple hv_nGND = null, hv_EMPTYamin = null, hv_nEMPTY = null;
            HTuple hv_RowsEP = null, hv_ColumnsEP = null, hv_num_iter = null;
            HTuple hv_minlen = null, hv_Area = new HTuple(), hv_RowMSS = new HTuple();
            HTuple hv_ColMSS = new HTuple(), hv_nMSS = new HTuple();
            HTuple hv_RadendMSS = new HTuple(), hv_j = null, hv_Area1 = new HTuple();
            HTuple hv_nEndPointsMSS = new HTuple(), hv_Rows = null;
            HTuple hv_Columns = null, hv_wEPs = null, hv_wEP = null;
            HTuple hv_RowsJ = null, hv_ColumnsJ = null, hv_nJ = null;
            HTuple hv_RadJ = new HTuple(), hv_RowsGi = null, hv_ColsGi = null;
            HTuple hv_WGi = null, hv_nWGi = null, hv_medWGi = null;
            HTuple hv_devWGi = null, hv_WidthP = new HTuple(), hv_RowsGSi = null;
            HTuple hv_ColsGSi = null, hv_WGSi = null, hv_WidthS = new HTuple();
            HTuple hv_nWGSi = null, hv_lTrmin = null, hv_nWr = null;
            HTuple hv_sgap = null, hv_nTr = new HTuple(), hv_WminP = new HTuple();
            HTuple hv_nWP = new HTuple(), hv_RowsM = new HTuple();
            HTuple hv_ColsM = new HTuple(), hv_RowsT = new HTuple();
            HTuple hv_ColsT = new HTuple(), hv_w = new HTuple(), hv_k = new HTuple();
            HTuple hv_nFWMP = null, hv_Width = null, hv_Height = null;
            HTuple hv_Widthd = null, hv_Heightd = null, hv_nRMPNPI = null;
            HTuple hv_nSkP = null, hv_WminS = new HTuple(), hv_nWS = null;
            HTuple hv_RowsMS = null, hv_ColsMS = null, hv_RowsJS = null;
            HTuple hv_ColumnsJS = null, hv_nJS = null, hv_rJ = null;
            HTuple hv_RJ = null, hv_nFWMPS = null, hv_nINB = null;
            HTuple hv_SequenceINBi = null, hv_PnumRN1 = null, hv_WN = null;
            HTuple hv_AreaNFWMs = null, hv_Row = null, hv_Column = null;
            HTuple hv_RowsE = null, hv_ColumnsE = null, hv_nE = null;
            HTuple hv_nFWMs = null, hv_Erad2 = null, hv_RowsES = null;
            HTuple hv_ColumnsES = null, hv_nES = null, hv_EradS = null;
            HTuple hv_nINBS = null, hv_SequenceINBSi = null, hv_useint = null;
            HTuple hv_smoothCADcontours_COPY_INP_TMP = hv_smoothCADcontours.Clone();
            HTuple hv_wFWM_COPY_INP_TMP = hv_wFWM.Clone();
            HTuple hv_wFWMS_COPY_INP_TMP = hv_wFWMS.Clone();
            HTuple hv_wsideS_COPY_INP_TMP = hv_wsideS.Clone();

            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_RegionG);
            HOperatorSet.GenEmptyObj(out ho_RegionGS);
            HOperatorSet.GenEmptyObj(out ho_SkeletonG);
            HOperatorSet.GenEmptyObj(out ho_SkeletonGS);
            HOperatorSet.GenEmptyObj(out ho_WidthImageP);
            HOperatorSet.GenEmptyObj(out ho_DirImageP);
            HOperatorSet.GenEmptyObj(out ho_WidthImageS);
            HOperatorSet.GenEmptyObj(out ho_DirImageS);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsG);
            HOperatorSet.GenEmptyObj(out ho_ImageIR);
            HOperatorSet.GenEmptyObj(out ho_RegionsPNi);
            HOperatorSet.GenEmptyObj(out ho_RegionsPNSi);
            HOperatorSet.GenEmptyObj(out ho_RegionsNFWM);
            HOperatorSet.GenEmptyObj(out ho_BoundariesNFWM);
            HOperatorSet.GenEmptyObj(out ho_SkeletonFWM);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsFWM);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMcut);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMS);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsLG);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMScut);
            HOperatorSet.GenEmptyObj(out ho_RegionsGND);
            HOperatorSet.GenEmptyObj(out ho_RegionsEMPTY);
            HOperatorSet.GenEmptyObj(out ho_ImDisplacement);
            HOperatorSet.GenEmptyObj(out ho_SkeletonGd);
            HOperatorSet.GenEmptyObj(out ho_SkeletonPartsP);
            HOperatorSet.GenEmptyObj(out ho_EPNFWMremove);
            HOperatorSet.GenEmptyObj(out ho_SkeletonPartsPnoG);
            HOperatorSet.GenEmptyObj(out ho_ImageIR16);
            HOperatorSet.GenEmptyObj(out ho_ImageIRS);
            HOperatorSet.GenEmptyObj(out ho_RegionMS);
            HOperatorSet.GenEmptyObj(out ho_RegionsPNPi);
            HOperatorSet.GenEmptyObj(out ho_RegionsmainSNoCut);
            HOperatorSet.GenEmptyObj(out ho_SkelEP);
            HOperatorSet.GenEmptyObj(out ho_RegionCut);
            HOperatorSet.GenEmptyObj(out ho_RegNoProcd);
            HOperatorSet.GenEmptyObj(out ho_DomainG);
            HOperatorSet.GenEmptyObj(out ho_RC);
            HOperatorSet.GenEmptyObj(out ho_RegProc);
            HOperatorSet.GenEmptyObj(out ho_Rectangle);
            HOperatorSet.GenEmptyObj(out ho_RegionGi);
            HOperatorSet.GenEmptyObj(out ho_RegionGid);
            HOperatorSet.GenEmptyObj(out ho_RegionsGi);
            HOperatorSet.GenEmptyObj(out ho_RmnoG);
            HOperatorSet.GenEmptyObj(out ho_RegNoProcC);
            HOperatorSet.GenEmptyObj(out ho_RegNoProcCd);
            HOperatorSet.GenEmptyObj(out ho_RegionsGnoholes);
            HOperatorSet.GenEmptyObj(out ho_SelectedRegions);
            HOperatorSet.GenEmptyObj(out ho_RegionI);
            HOperatorSet.GenEmptyObj(out ho_RegionsGsm);
            HOperatorSet.GenEmptyObj(out ho_RegionGsm);
            HOperatorSet.GenEmptyObj(out ho_RegionsGNDnR);
            HOperatorSet.GenEmptyObj(out ho_RegionGSi);
            HOperatorSet.GenEmptyObj(out ho_RegionsGSi);
            HOperatorSet.GenEmptyObj(out ho_RegionsEMPTYnR);
            HOperatorSet.GenEmptyObj(out ho_RegionErosion);
            HOperatorSet.GenEmptyObj(out ho_RegionDilation);
            HOperatorSet.GenEmptyObj(out ho_RegionIntersection);
            HOperatorSet.GenEmptyObj(out ho_SkeletonGi);
            HOperatorSet.GenEmptyObj(out ho_EndPointsG);
            HOperatorSet.GenEmptyObj(out ho_JuncPointsG);
            HOperatorSet.GenEmptyObj(out ho_CEP);
            HOperatorSet.GenEmptyObj(out ho_RegionGSii);
            HOperatorSet.GenEmptyObj(out ho_SkeletonGSi);
            HOperatorSet.GenEmptyObj(out ho_SkeletonGSii);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsGS);
            HOperatorSet.GenEmptyObj(out ho_ConnectedRegionsS);
            HOperatorSet.GenEmptyObj(out ho_SkelEPi);
            HOperatorSet.GenEmptyObj(out ho_JuncPoints1);
            HOperatorSet.GenEmptyObj(out ho_SkelEPu);
            HOperatorSet.GenEmptyObj(out ho_EndPointsMSSuc);
            HOperatorSet.GenEmptyObj(out ho_EndPointsGS);
            HOperatorSet.GenEmptyObj(out ho_JuncPointsGS);
            HOperatorSet.GenEmptyObj(out ho_RegionDilationGS);
            HOperatorSet.GenEmptyObj(out ho_RegionIntersection1);
            HOperatorSet.GenEmptyObj(out ho_SkeletonLG);
            HOperatorSet.GenEmptyObj(out ho_EndPointsMSS);
            HOperatorSet.GenEmptyObj(out ho_JuncPointsMSS);
            HOperatorSet.GenEmptyObj(out ho_EndPointsMSSu);
            HOperatorSet.GenEmptyObj(out ho_CMSSE);
            HOperatorSet.GenEmptyObj(out ho_CMSSEu);
            HOperatorSet.GenEmptyObj(out ho_SkeletonPartsPinc);
            HOperatorSet.GenEmptyObj(out ho_WidthImagePout);
            HOperatorSet.GenEmptyObj(out ho_DirImagePout);
            HOperatorSet.GenEmptyObj(out ho_RegForbid);
            HOperatorSet.GenEmptyObj(out ho_RegionsPNii);
            HOperatorSet.GenEmptyObj(out ho_AngleImageP);
            HOperatorSet.GenEmptyObj(out ho_DistGPi);
            HOperatorSet.GenEmptyObj(out ho_DistGSi);
            HOperatorSet.GenEmptyObj(out ho_DistGSii);
            HOperatorSet.GenEmptyObj(out ho_DistGP);
            HOperatorSet.GenEmptyObj(out ho_DistGS);
            HOperatorSet.GenEmptyObj(out ho_ImDisplacementi);
            HOperatorSet.GenEmptyObj(out ho_Rd);
            HOperatorSet.GenEmptyObj(out ho_JP);
            HOperatorSet.GenEmptyObj(out ho_CJ);
            HOperatorSet.GenEmptyObj(out ho_Skeletonii);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsWire);
            HOperatorSet.GenEmptyObj(out ho_Rnoint);
            HOperatorSet.GenEmptyObj(out ho_Rmain);
            HOperatorSet.GenEmptyObj(out ho_ConnectedRegions);
            HOperatorSet.GenEmptyObj(out ho_RegionsmainNoCut);
            HOperatorSet.GenEmptyObj(out ho_RegionmainNoCut);
            HOperatorSet.GenEmptyObj(out ho_Regionsmain);
            HOperatorSet.GenEmptyObj(out ho_Regionmain);
            HOperatorSet.GenEmptyObj(out ho_Regionmaini);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMii);
            HOperatorSet.GenEmptyObj(out ho_PC);
            HOperatorSet.GenEmptyObj(out ho_SkeletonPartsPi);
            HOperatorSet.GenEmptyObj(out ho_RegionsmainPNPi);
            HOperatorSet.GenEmptyObj(out ho_SkeletonPartsPii);
            HOperatorSet.GenEmptyObj(out ho_RmainS);
            HOperatorSet.GenEmptyObj(out ho_EndPointsS);
            HOperatorSet.GenEmptyObj(out ho_JuncPointsS);
            HOperatorSet.GenEmptyObj(out ho_JuncPointsSu);
            HOperatorSet.GenEmptyObj(out ho_CirclesSJ);
            HOperatorSet.GenEmptyObj(out ho_CirclesSJu);
            HOperatorSet.GenEmptyObj(out ho_RmainSnoJ);
            HOperatorSet.GenEmptyObj(out ho_RegionsmainSs);
            HOperatorSet.GenEmptyObj(out ho_RegionsmainSu);
            HOperatorSet.GenEmptyObj(out ho_RegionsmainSuc);
            HOperatorSet.GenEmptyObj(out ho_RegionsmainSn);
            HOperatorSet.GenEmptyObj(out ho_RegionsmainS);
            HOperatorSet.GenEmptyObj(out ho_RegionmainSNoCut);
            HOperatorSet.GenEmptyObj(out ho_RegionmainS);
            HOperatorSet.GenEmptyObj(out ho_RegionGND);
            HOperatorSet.GenEmptyObj(out ho_RegionGNDC);
            HOperatorSet.GenEmptyObj(out ho_RegionsNFWMBcut);
            HOperatorSet.GenEmptyObj(out ho_RegionsNFWMs);
            HOperatorSet.GenEmptyObj(out ho_Region1);
            HOperatorSet.GenEmptyObj(out ho_Region2);
            HOperatorSet.GenEmptyObj(out ho_ImageIRBS);
            HOperatorSet.GenEmptyObj(out ho_ImageIRS16);
            HOperatorSet.GenEmptyObj(out ho_RegForbidS);
            HOperatorSet.GenEmptyObj(out ho_EndPoints);
            HOperatorSet.GenEmptyObj(out ho_JuncPoints);
            HOperatorSet.GenEmptyObj(out ho_EP);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMp);
            HOperatorSet.GenEmptyObj(out ho_CE2);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMSii);
            HOperatorSet.GenEmptyObj(out ho_EPS);
            HOperatorSet.GenEmptyObj(out ho_CES);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMSi);
            HOperatorSet.GenEmptyObj(out ho_RegionsPNSii);
            HOperatorSet.GenEmptyObj(out ho_RegionEMPTY);
            HOperatorSet.GenEmptyObj(out ho_RegionEMPTYC);
            HOperatorSet.GenEmptyObj(out ho_SkeletonsFWMSrc);
            HOperatorSet.GenEmptyObj(out ho_PCS);
            hv_nFWM = new HTuple();
            hv_nNonWFM = new HTuple();
            hv_SequenceIRB = new HTuple();
            hv_SequenceIRBS = new HTuple();
            hv_PnumRN = new HTuple();
            //** TeachingMain128
            //* added elongated SkeletonsLG
            //** 240504 - problems with SgeletonsGS at ends
            //** 240224 - Cut without overlap for both P and S
            //** 231213 - new version with RegNoProc
            //**- new RegionsPNi built with forbid. reg. as SkeletonGS
            //** 231011 - i6-bit IR, IRB
            //** 231002 - new version with RegNoProc
            //** 230912 - problem with SkeletonsGS
            //** 230313 - swapping elongation (2) and PNi renumbering (1) of skeleton parts
            //****
            //** 220313 - fixed JP for SkeletomsFWM
            //** 220317 Made SkeletonGd with removed endpoints areas
            //** 220421 made circle regions to remove SP for significant dir. changes (overkills)
            //** 220430 FWM for Space
            //** 220603 wFWM, wFWMS ar parameters
            //** 220606 FWM elongation added
            //** 220607 NFWM endpoint don't care area added
            //** 220610 NFWM added ExtendedSkeletonWFMP with jpadd (rad. add to jp area removal)
            //** 220614 arbitrary number of FWM and FWMS widths (defined by tuples lengths)
            //*** and elength (elongation length). FWM elongation changed to 2 types (rect. and circ.)
            //** 220615 removed FWM in RegionGMD areas (to avoid artifacts)
            //** 220817  added PuseintP and PuseintS - intersection points which should be saved in FWM
            //** 220828  TeachingMainL109 - fixed nEMPTY=0 case, were problems in FWMS
            //** 220920 fixed edit CR for NFWM
            //** 221209 if smoothCADcontours==0, then smoothCADcontours is measured by MaesureSmoothing() proc.

            dev_update_off();


            //*** adding overlap area to R, it should become cleared
            hv_TotalPRC = 5;
            hv_CurrentOper = "Starting";
            HOperatorSet.TupleMax(hv_wFWM_COPY_INP_TMP, out hv_maxwfwm);
            ho_RegNoProcd.Dispose();
            HOperatorSet.DilationCircle(ho_RegNoProc, out ho_RegNoProcd, hv_maxwfwm);

            //difference (DomainG, ROut, RC)
            //** TeachingMain7
            //complement (ROut, RC)
            //intersection (Gi, RC, G)
            //intersection (ROut, DomainG, ROut)
            //intersection (RC, DomainG, RC)
            //stop ()

            //*************
            HOperatorSet.SetSystem("neighborhood", 8);

            ho_DomainG.Dispose();
            HOperatorSet.GetDomain(ho_Gi, out ho_DomainG);

            ho_RegProc.Dispose();
            HOperatorSet.Difference(ho_DomainG, ho_RegNoProc, out ho_RegProc);
            HOperatorSet.SmallestRectangle1(ho_RegProc, out hv_Row11, out hv_Column11, out hv_Row21,
                out hv_Column21);
            ho_Rectangle.Dispose();
            HOperatorSet.GenRectangle1(out ho_Rectangle, hv_Row11, hv_Column11, hv_Row21,
                hv_Column21);
            HOperatorSet.AreaCenter(ho_RegProc, out hv_Area3, out hv_RowDKu, out hv_ColumnDKu);
            ho_RegionCut.Dispose();
            HOperatorSet.GenRectangle1(out ho_RegionCut, hv_RowDKu, hv_Column11, hv_RowDKu + 1,
                hv_Column21 + 1);

            //CAD pattern and shape regions
            ho_RegionGi.Dispose();
            HOperatorSet.Threshold(ho_Gi, out ho_RegionGi, 127, 255);
            //closing_circle (RegionGii, RegionGi, 1)
            //difference (RegionGii, RegMargin, RegionGi1)
            {
                HObject ExpTmpOutVar_0;
                HOperatorSet.Difference(ho_RegionGi, ho_RegNoProc, out ExpTmpOutVar_0);
                ho_RegionGi.Dispose();
                ho_RegionGi = ExpTmpOutVar_0;
            }
            ho_RegionGid.Dispose();
            HOperatorSet.Difference(ho_RegionGi, ho_RegNoProcd, out ho_RegionGid);

            //read_image (Unit03, 'H:/Unit03.tiff')
            //RegionGi := RegionGii

            ho_RegionsGi.Dispose();
            HOperatorSet.Connection(ho_RegionGi, out ho_RegionsGi);
            //connection (RegionGid, RegionsGi)

            //threshold (Gi, RegionGi, 128, 255)
            //dilation_circle (RegionGi, RegionGid, 3.5)
            ho_RmnoG.Dispose();
            HOperatorSet.Difference(ho_RegNoProc, ho_RegionGid, out ho_RmnoG);

            ho_RegNoProcC.Dispose();
            HOperatorSet.Difference(ho_DomainG, ho_RegNoProc, out ho_RegNoProcC);
            ho_RegNoProcCd.Dispose();
            HOperatorSet.Difference(ho_DomainG, ho_RegNoProcd, out ho_RegNoProcCd);
            //** correcting RC right side (temp.)
            //smallest_rectangle1 (RC, Row1, Column1, Row2, Column2)
            //Column2 := Column2-5
            //gen_rectangle1 (RC, Row1, Column1, Row2, Column2)
            //******
            ho_RegionsGnoholes.Dispose();
            HOperatorSet.SelectShape(ho_RegionsGi, out ho_RegionsGnoholes, "holes_num", "and",
                0, 0);
            ho_SelectedRegions.Dispose();
            HOperatorSet.SelectShape(ho_RegionsGnoholes, out ho_SelectedRegions, "compactness",
                "and", 10, 99999);

            if ((int)(new HTuple(hv_smoothCADcontours_COPY_INP_TMP.TupleEqual(0))) != 0)
            {
                ho_RegionI.Dispose();
                HOperatorSet.Threshold(ho_Im, out ho_RegionI, 128, 255);
                MaesureSmoothing(ho_RegionsGi, ho_RegionI, out hv_smoothCADcontours_COPY_INP_TMP);
            }


            //******
            hv_TotalPRC = 7;
            hv_CurrentOper = "Smoothing regions";
            //smoothCADcontours := 0
            ho_RegionsGsm.Dispose();
            SmoothingRegion(ho_RegionsGi, out ho_RegionsGsm, hv_smoothCADcontours_COPY_INP_TMP);
            ho_RegionGsm.Dispose();
            HOperatorSet.Union1(ho_RegionsGsm, out ho_RegionGsm);
            ho_RegionG.Dispose();
            HOperatorSet.Difference(ho_RegionGsm, ho_RegNoProc, out ho_RegionG);
            ho_RegionGi.Dispose();
            ho_RegionGi = ho_RegionG.CopyObj(1, -1);
            //**** nGND - number of GND-like (big size) regions to be removed from overflowing for Pattern
            ho_RegionsGi.Dispose();
            HOperatorSet.Connection(ho_RegionGi, out ho_RegionsGi);
            ho_RegionsGND.Dispose();
            HOperatorSet.SelectShape(ho_RegionsGi, out ho_RegionsGND, "area", "and", hv_GNDamin,
                99999999);
            HOperatorSet.CountObj(ho_RegionsGND, out hv_nGND);
            //*** to avoid problem in IRB
            //nGND := nGND+1

            ho_RegionsGNDnR.Dispose();
            HOperatorSet.Difference(ho_RegionsGND, ho_RegNoProc, out ho_RegionsGNDnR);
            //read_image (Im2173, 'C:/TmpIm/Im2_173.tif')
            //threshold (Im2173, Region, 90, 255)

            //***
            //**** nEMPTY - number of empty (big size) regions to be removed from overflowing for Space
            hv_EMPTYamin = 900000;
            //16.04.2024
            //difference (DomainG, RegionGsm, RegionGSi)
            ho_RegionGSi.Dispose();
            HOperatorSet.Difference(ho_DomainG, ho_RegionG, out ho_RegionGSi);
            ho_RegionsGSi.Dispose();
            HOperatorSet.Connection(ho_RegionGSi, out ho_RegionsGSi);
            ho_RegionsEMPTY.Dispose();
            HOperatorSet.SelectShape(ho_RegionsGSi, out ho_RegionsEMPTY, "area", "and", hv_EMPTYamin,
                999999999);
            HOperatorSet.CountObj(ho_RegionsEMPTY, out hv_nEMPTY);
            if ((int)(new HTuple(hv_nEMPTY.TupleEqual(0))) != 0)
            {
                hv_nEMPTY = 1;
            }
            ho_RegionsEMPTYnR.Dispose();
            HOperatorSet.Difference(ho_RegionsEMPTY, ho_RegNoProc, out ho_RegionsEMPTYnR);
            //***********************************************
            //stop ()
            //****220130****
            ho_RegionErosion.Dispose();
            HOperatorSet.ErosionCircle(ho_RegionG, out ho_RegionErosion, 14);
            ho_RegionDilation.Dispose();
            HOperatorSet.DilationCircle(ho_RegionErosion, out ho_RegionDilation, 14 + 6);
            ho_RegionIntersection.Dispose();
            HOperatorSet.Intersection(ho_RegionDilation, ho_RegionG, out ho_RegionIntersection
                );
            //**************

            //complement (RegionGi, RegionGComplement)
            //intersection (RegionGComplement, DomainG, RegionGSi)
            //**** finding skeletons *****

            ho_RegionGS.Dispose();
            ho_RegionGS = ho_RegionGSi.CopyObj(1, -1);
            //** RegionGiic - closed CAD region to remove small artifacts 220805
            //closing_circle (RegionGi, RegionGiic, 1)
            ho_SkeletonGi.Dispose();
            HOperatorSet.Skeleton(ho_RegionGi, out ho_SkeletonGi);
            //**** cutting RecOvl
            //intersection (SkeletonGi, RC, SkeletonG)
            ho_RC.Dispose();
            HOperatorSet.Difference(ho_DomainG, ho_RegNoProc, out ho_RC);
            ho_SkeletonG.Dispose();
            HOperatorSet.Intersection(ho_SkeletonGi, ho_RC, out ho_SkeletonG);
            ho_EndPointsG.Dispose(); ho_JuncPointsG.Dispose();
            HOperatorSet.JunctionsSkeleton(ho_SkeletonG, out ho_EndPointsG, out ho_JuncPointsG
                );
            HOperatorSet.GetRegionPoints(ho_EndPointsG, out hv_RowsEP, out hv_ColumnsEP);
            //rcep := 5
            //tuple_length (RowsEP, nEP)
            //tuple_gen_const (nEP, rcep, Rcep)
            //gen_circle (CEP, RowsEP, ColumnsEP, Rcep)
            //*** SkeletonGd is for proper Open finding
            ho_CEP.Dispose();
            HOperatorSet.GenEmptyObj(out ho_CEP);
            ho_SkeletonGd.Dispose();
            HOperatorSet.Difference(ho_SkeletonG, ho_CEP, out ho_SkeletonGd);
            //******
            {
                HObject ExpTmpOutVar_0;
                HOperatorSet.Difference(ho_SkeletonG, ho_RegNoProc, out ExpTmpOutVar_0);
                ho_SkeletonG.Dispose();
                ho_SkeletonG = ExpTmpOutVar_0;
            }

            //******

            //16.04.2024
            //intersection (RegionGSi, RC, RegionGSii)
            //difference (RegionGSii, RegNoProc, RegionGSi)


            hv_num_iter = 999;
            ho_SkeletonsG.Dispose();
            HOperatorSet.Connection(ho_SkeletonG, out ho_SkeletonsG);

            ho_SkeletonGSi.Dispose();
            HOperatorSet.Skeleton(ho_RegionGSi, out ho_SkeletonGSi);
            ho_SkeletonGSii.Dispose();
            HOperatorSet.Intersection(ho_SkeletonGSi, ho_RC, out ho_SkeletonGSii);
            ho_SkeletonGS.Dispose();
            HOperatorSet.Difference(ho_SkeletonGSii, ho_RegNoProc, out ho_SkeletonGS);
            ho_SkeletonsGS.Dispose();
            HOperatorSet.Connection(ho_SkeletonGS, out ho_SkeletonsGS);

            //select_shape (ConnectedRegionsS, SkeletonsGS, 'area', 'and', minlen, 9999999)
            //junctions_skeleton (SkeletonsGS, SkelEPi, JuncPoints1)
            //union1 (SkelEPi, SkelEPu)
            //dilation_circle (SkelEPu, SkelEP, 13.5)
            //area_center (EndPointsMSSuc, Area, RowMSS, ColMSS)
            //tuple_length (RowMSS, nMSS)
            //tuple_gen_const (nMSS, 1, RadendMSS)
            //** cutting ends of MSS
            if ((int)(new HTuple(hv_elongLG.TupleGreater(0))) != 0)
            {
                ho_EndPointsGS.Dispose(); ho_JuncPointsGS.Dispose();
                HOperatorSet.JunctionsSkeleton(ho_SkeletonGS, out ho_EndPointsGS, out ho_JuncPointsGS
                    );
                ho_RegionDilationGS.Dispose();
                HOperatorSet.DilationCircle(ho_EndPointsGS, out ho_RegionDilationGS, hv_elongLG);
                ho_RegionIntersection1.Dispose();
                HOperatorSet.Intersection(ho_SkeletonGSi, ho_RegionDilationGS, out ho_RegionIntersection1
                    );
                ho_SkeletonLG.Dispose();
                HOperatorSet.Union2(ho_SkeletonGS, ho_RegionIntersection1, out ho_SkeletonLG
                    );
                ho_SkeletonsLG.Dispose();
                HOperatorSet.Connection(ho_SkeletonLG, out ho_SkeletonsLG);
            }
            //    union2
            //* elseif(cutendMSS >0)    
            HTuple end_val184 = hv_cutendMSS;
            HTuple step_val184 = 1;
            for (hv_j = 1; hv_j.Continue(end_val184, step_val184); hv_j = hv_j.TupleAdd(step_val184))
            {
                ho_EndPointsMSS.Dispose(); ho_JuncPointsMSS.Dispose();
                HOperatorSet.JunctionsSkeleton(ho_SkeletonsGS, out ho_EndPointsMSS, out ho_JuncPointsMSS
                    );
                HOperatorSet.AreaCenter(ho_EndPointsMSS, out hv_Area1, out hv_Row1, out hv_Column1);
                HOperatorSet.TupleLength(hv_Area1, out hv_nEndPointsMSS);
                if ((int)(new HTuple(hv_Area1.TupleGreater(0))) != 0)
                {
                    ho_EndPointsMSSu.Dispose();
                    HOperatorSet.Union1(ho_EndPointsMSS, out ho_EndPointsMSSu);
                    ho_EndPointsMSSuc.Dispose();
                    HOperatorSet.Connection(ho_EndPointsMSSu, out ho_EndPointsMSSuc);
                    HOperatorSet.AreaCenter(ho_EndPointsMSSuc, out hv_Area, out hv_RowMSS, out hv_ColMSS);
                    HOperatorSet.TupleLength(hv_RowMSS, out hv_nMSS);
                    HOperatorSet.TupleGenConst(hv_nMSS, 1, out hv_RadendMSS);
                    ho_CMSSE.Dispose();
                    HOperatorSet.GenCircle(out ho_CMSSE, hv_RowMSS, hv_ColMSS, hv_RadendMSS);
                    ho_CMSSEu.Dispose();
                    HOperatorSet.Union1(ho_CMSSE, out ho_CMSSEu);
                    {
                        HObject ExpTmpOutVar_0;
                        HOperatorSet.Difference(ho_SkeletonsGS, ho_CMSSEu, out ExpTmpOutVar_0);
                        ho_SkeletonsGS.Dispose();
                        ho_SkeletonsGS = ExpTmpOutVar_0;
                    }
                }
            }

            //* endif


            //junctions_skeleton (SkeletonsGS, SkelEPi, JuncPoints1)
            //union1 (SkelEPi, SkelEPu)
            //dilation_circle (SkelEPu, SkelEP, 13.5)
            //stop ()
            //ExtendedSkeletonWFMP5 (RegionG, SkeletonsGS, SkeletonGi, WidthImageP, DirImageP, SkeletonPartsPinc, WidthImagePout, DirImagePout, elengthend, elengthnoend, jpadd)



            ho_RegForbid.Dispose();
            HOperatorSet.Union2(ho_SkeletonGS, ho_RegNoProc, out ho_RegForbid);
            //RegForbid := SkeletonGS

            ho_RegionsPNii.Dispose();
            HOperatorSet.ExpandRegion(ho_SkeletonsG, ho_RegForbid, out ho_RegionsPNii, hv_num_iter,
                "image");
            ho_RegionsPNi.Dispose();
            HOperatorSet.DilationCircle(ho_RegionsPNii, out ho_RegionsPNi, 1);

            //expand_region (SkeletonsFWM, RegForbid, RegionsPNPii, num_iter, 'image')
            //dilation_circle (RegionsPNPii, RegionsPNPi, 1)

            //write_object (SkeletonGS, path+'SkeletonGS.hobj')
            //***** Building Width and Dir images *********
            HOperatorSet.SetSystem("current_runlength_number", 50000000);
            //PatternWidthDirImages (RegionGi, SkeletonG, WidthImageP, DirImageP, AngleImageP, WidthG, HeightG)
            //SpaceWidthDirImages (RegionGSi, SkeletonGS, WidthImageS, DirImageS, WidthG, HeightG)
            //** RegionGiic - closed CAD region to remove small artifacts 220805
            hv_TotalPRC = 10;
            hv_CurrentOper = "Building width and direction images";
            ho_WidthImageP.Dispose(); ho_DirImageP.Dispose(); ho_AngleImageP.Dispose();
            PatternWidthDirImages(ho_RegionGi, ho_SkeletonG, out ho_WidthImageP, out ho_DirImageP,
                out ho_AngleImageP, hv_WidthG, hv_HeightG);
            ho_RegionGSii.Dispose();
            HOperatorSet.Difference(ho_DomainG, ho_RegionGi, out ho_RegionGSii);
            //SpaceWidthDirImages (RegionGSii, SkeletonGS, WidthImageS, DirImageS, WidthG, HeightG)
            //** 24.04.02 RegionGSi -> RegionGSii
            ho_WidthImageS.Dispose(); ho_DirImageS.Dispose();
            SpaceWidthDirImages(ho_RegionGSii, ho_SkeletonLG, out ho_WidthImageS, out ho_DirImageS,
                hv_WidthG, hv_HeightG);

            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.DispText(HDevWindowStack.GetActive(), "Width/Dir processing finished",
                    "window", 10, 10, "black", new HTuple(), new HTuple());
            }
            //stop ()

            ho_SkelEPi.Dispose(); ho_JuncPoints1.Dispose();
            HOperatorSet.JunctionsSkeleton(ho_SkeletonsGS, out ho_SkelEPi, out ho_JuncPoints1
                );
            ho_SkelEPu.Dispose();
            HOperatorSet.Union1(ho_SkelEPi, out ho_SkelEPu);
            HOperatorSet.GetRegionPoints(ho_SkelEPu, out hv_Rows, out hv_Columns);
            HOperatorSet.GetGrayval(ho_WidthImageS, hv_Rows, hv_Columns, out hv_wEPs);
            HOperatorSet.TupleMax(hv_wEPs, out hv_wEP);

            ho_SkelEP.Dispose();
            HOperatorSet.DilationCircle(ho_SkelEPu, out ho_SkelEP, (hv_wEP / 2) + hv_EPexp);
            //********************************
            //** Displacement Image
            //** teaching part
            //union1 (RegionG, RegionG1)
            ho_DistGPi.Dispose();
            HOperatorSet.DistanceTransform(ho_RegionGi, out ho_DistGPi, "euclidean", "true",
                hv_WidthG, hv_HeightG);
            ho_DistGSi.Dispose();
            HOperatorSet.DistanceTransform(ho_RegionGSi, out ho_DistGSi, "euclidean", "true",
                hv_WidthG, hv_HeightG);

            ho_DistGSii.Dispose();
            HOperatorSet.ConvertImageType(ho_DistGSi, out ho_DistGSii, "byte");
            ho_DistGP.Dispose();
            HOperatorSet.ConvertImageType(ho_DistGPi, out ho_DistGP, "byte");
            ho_DistGS.Dispose();
            HOperatorSet.ScaleImage(ho_DistGSii, out ho_DistGS, 1, -1);
            ho_ImDisplacementi.Dispose();
            HOperatorSet.SubImage(ho_DistGS, ho_DistGP, out ho_ImDisplacementi, 1, 128);
            ho_Rd.Dispose();
            HOperatorSet.DilationCircle(ho_RegNoProc, out ho_Rd, 3.5);
            ho_ImDisplacement.Dispose();
            HOperatorSet.PaintRegion(ho_Rd, ho_ImDisplacementi, out ho_ImDisplacement, 127,
                "fill");
            //stop ()
            //no-shift displ. image  -double jump near the boundary
            //convert_image_type (DistGSi, DistGSii, 'int1')
            //convert_image_type (DistGPi, DistGPii, 'int1')
            //scale_image (DistGPii, DistGP, 1, -1)
            //scale_image (DistGSii, DistGS, 1, -1)
            //sub_image (DistGS, DistGP, ImDisplacement, 1, 0)
            //*** histogram-based CAD width analysis****
            //** pattern **
            hv_TotalPRC = 20;
            hv_CurrentOper = "Building Pattern Skeletons";
            hv_minlen = 50;

            ho_EndPointsG.Dispose(); ho_JuncPointsG.Dispose();
            HOperatorSet.JunctionsSkeleton(ho_SkeletonG, out ho_EndPointsG, out ho_JuncPointsG
                );
            ho_JP.Dispose();
            HOperatorSet.Union1(ho_JuncPointsG, out ho_JP);
            HOperatorSet.GetRegionPoints(ho_JP, out hv_RowsJ, out hv_ColumnsJ);
            HOperatorSet.TupleLength(hv_RowsJ, out hv_nJ);
            if ((int)(new HTuple(hv_nJ.TupleGreater(0))) != 0)
            {
                HOperatorSet.GetGrayval(ho_WidthImageP, hv_RowsJ, hv_ColumnsJ, out hv_RadJ);
                ho_CJ.Dispose();
                HOperatorSet.GenCircle(out ho_CJ, hv_RowsJ, hv_ColumnsJ, (hv_RadJ / 2) + 1);
                ho_Skeletonii.Dispose();
                HOperatorSet.Difference(ho_SkeletonG, ho_CJ, out ho_Skeletonii);
            }
            else
            {
                ho_Skeletonii.Dispose();
                ho_Skeletonii = ho_SkeletonG.CopyObj(1, -1);
            }

            HOperatorSet.GetRegionPoints(ho_Skeletonii, out hv_RowsGi, out hv_ColsGi);
            HOperatorSet.GetGrayval(ho_WidthImageP, hv_RowsGi, hv_ColsGi, out hv_WGi);
            HOperatorSet.TupleLength(hv_WGi, out hv_nWGi);
            HOperatorSet.TupleMedian(hv_WGi, out hv_medWGi);
            HOperatorSet.TupleDeviation(hv_WGi, out hv_devWGi);

            //***** correcting pattern width (image ang tuple) with minWGS
            HTuple end_val289 = hv_nWGi - 1;
            HTuple step_val289 = 1;
            for (hv_j = 0; hv_j.Continue(end_val289, step_val289); hv_j = hv_j.TupleAdd(step_val289))
            {
                if ((int)(new HTuple(((hv_WGi.TupleSelect(hv_j))).TupleLess(hv_minWG))) != 0)
                {
                    if (hv_WGi == null)
                        hv_WGi = new HTuple();
                    hv_WGi[hv_j] = hv_minWG;
                }
            }
            HOperatorSet.SetGrayval(ho_WidthImageP, hv_RowsGi, hv_ColsGi, hv_WGi);
            //*************************************************
            //tuple_min (WGi, minWGi)
            //tuple_max (WGi, maxWGi)
            //tuple_histo_range (WGi, 0, maxWGi, 1*(maxWGi+1), HistoGi, BinSize)
            //PrcWGi := HistoGi*100./nWGi
            //tuple_length (PrcWGi, nP)

            //write_tuple (PrcWGi, 'PrcWGi')
            //read_tuple ('PrcWGitst', PrcWGi)
            if ((int)(new HTuple(hv_autowidth.TupleEqual(1))) != 0)
            {
                //wside := 6
                FindWidth(hv_WGi, hv_wside, out hv_WidthP);
            }



            //stop ()
            //tuple_max (MaxInd, pmax)
            //tuple_min (MaxInd, pmin)
            //pmid := (pmax+pmin)/2
            //span := pmax-pmin
            //stop ()

            //*** space ***
            HOperatorSet.GetRegionPoints(ho_SkeletonGS, out hv_RowsGSi, out hv_ColsGSi);
            HOperatorSet.GetGrayval(ho_WidthImageS, hv_RowsGSi, hv_ColsGSi, out hv_WGSi);
            hv_wsideS_COPY_INP_TMP = 3;
            if ((int)(new HTuple(hv_autowidth.TupleEqual(1))) != 0)
            {
                FindWidth(hv_WGSi, hv_wsideS_COPY_INP_TMP, out hv_WidthS);
            }

            //tuple_length (WGSi, nWGSi)
            //tuple_max (WGSi, maxWGSi)
            //***** correcting space width (image ang tuple) with minWGS
            //for j := 0 to nWGSi-1 by 1
            //if (WGSi[j]< minWGS)
            //WGSi[j] := minWGS
            //endif
            //endfor
            //set_grayval (WidthImageS, RowsGSi, ColsGSi, WGSi)
            //*********************************************************************
            //tuple_histo_range (WGSi, 0, maxWGSi, 1*(maxWGSi+1), HistoGSi, BinSize)
            //PrcWGSi := HistoGSi*100/nWGSi


            //***** Auto setting of FWM/S widths using width histogram 16.04.2024
            //create_funct_1d_array (PrcWGSi, FPrcWGSi)
            //local_min_max_funct_1d (FPrcWGSi, 'strict_min_max', 'true', MinPrcWGS, MaxPrcWGS)
            //tuple_length (MinPrcWGS, nMinPrcWGS)
            //tuple_length (MaxPrcWGS, nMaxPrcWGS)
            //get_y_value_funct_1d (FPrcWGSi, MinPrcWGS, 'constant', YMinPrcWGS)
            //get_y_value_funct_1d (FPrcWGSi, MaxPrcWGS, 'constant', YMaxPrcWGS)


            //*** Selection of FWM by histogram *********
            //reduce_domain (WidthImageP, SkeletonG, ImageReduced)
            //get_region_points (SkeletonG, RowsG, ColumnsG)
            //get_grayval (WidthImageP, RowsG, ColumnsG, WdG)
            //tuple_length (WdG, nWdG)
            //tuple_histo_range (WdG, 1, 255, 255, HistoWdG, BinSize1)
            //PrcWdG := HistoWdG*100./nWdG
            //********************************************
            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.DispText(HDevWindowStack.GetActive(), "Pat FWM finding started",
                    "window", 10, 10, "black", new HTuple(), new HTuple());
            }
            //*********************
            //** Selection FWM from ODB++ ***
            //************
            hv_lTrmin = 50;
            hv_nWr = 0;
            hv_sgap = 0;
            if ((int)((new HTuple(hv_useODB.TupleGreater(0))).TupleAnd(new HTuple(hv_useODB.TupleLess(
                3)))) != 0)
            {
                if (HDevWindowStack.IsOpen())
                {
                    HOperatorSet.DispText(HDevWindowStack.GetActive(), "Finding wires from ODB++            ",
                        "window", 10, 10, "black", new HTuple(), new HTuple());
                }
                ho_SkeletonsWire.Dispose(); ho_Rnoint.Dispose();
                FindWires3(ho_RegionG, ho_SkeletonG, ho_RegionFlash, ho_RegionTrace, ho_DirImageP,
                    ho_WidthImageP, ho_RegionsPNPi, out ho_SkeletonsWire, out ho_Rnoint, hv_lTrmin,
                    hv_nWr, hv_wpl, hv_sgap, out hv_nTr);
                if (HDevWindowStack.IsOpen())
                {
                    HOperatorSet.DispText(HDevWindowStack.GetActive(), "Finding wires finished           ",
                        "window", 10, 10, "black", new HTuple(), new HTuple());
                }
            }

            //************
            //*** Selection of FWM amd FWMS by histogram *********
            //prcmin := 5
            //numP := 3
            //numS := 1
            //wmax := 45
            //if (useODB==1)
            //WSelbyHist (PrcWGi, wmax, prcmin, numP, WminPa)
            //endif

            //numS := 1
            //WSelbyHist (PrcWGSi, wmax, prcmin, numS, WminSa)
            //stop ()
            //****************************************************
            //stop ()
            //Wmain := [6, 14, 28]
            if ((int)(new HTuple(hv_useODB.TupleEqual(0))) != 0)
            {
                HOperatorSet.SetSystem("neighborhood", 8);
                if ((int)(new HTuple(hv_autowidth.TupleEqual(0))) != 0)
                {
                    hv_WminP = hv_wFWM_COPY_INP_TMP.Clone();
                }
                else
                {
                    hv_WminP = hv_WidthP.Clone();
                }

                HOperatorSet.TupleLength(hv_WminP, out hv_nWP);

                //**** temp. tuple based Pattern Skel. FWM
                //WminP := [16, 24, 68, 126]
                //tuple_length (WminP, nWP)
                hv_RowsM = new HTuple();
                hv_ColsM = new HTuple();
                hv_RowsT = new HTuple();
                hv_ColsT = new HTuple();
                HTuple end_val403 = hv_nWGi - 1;
                HTuple step_val403 = 1;
                for (hv_j = 0; hv_j.Continue(end_val403, step_val403); hv_j = hv_j.TupleAdd(step_val403))
                {

                    hv_w = hv_WGi.TupleSelect(hv_j);
                    HTuple end_val406 = hv_nWP - 1;
                    HTuple step_val406 = 1;
                    for (hv_k = 0; hv_k.Continue(end_val406, step_val406); hv_k = hv_k.TupleAdd(step_val406))
                    {
                        if ((int)((new HTuple(hv_w.TupleGreaterEqual((hv_WminP.TupleSelect(hv_k)) - hv_wside))).TupleAnd(
                            new HTuple(hv_w.TupleLessEqual((hv_WminP.TupleSelect(hv_k)) + hv_wside)))) != 0)
                        {
                            HOperatorSet.TupleConcat(hv_RowsT, hv_RowsGi.TupleSelect(hv_j), out hv_RowsT);
                            HOperatorSet.TupleConcat(hv_ColsT, hv_ColsGi.TupleSelect(hv_j), out hv_ColsT);
                        }
                    }
                    if ((int)(new HTuple(((hv_j % 10000)).TupleEqual(0))) != 0)
                    {
                        HOperatorSet.TupleConcat(hv_RowsM, hv_RowsT, out hv_RowsM);
                        HOperatorSet.TupleConcat(hv_ColsM, hv_ColsT, out hv_ColsM);
                        hv_RowsT = new HTuple();
                        hv_ColsT = new HTuple();
                    }
                    if ((int)(new HTuple(((hv_j % 100000)).TupleEqual(0))) != 0)
                    {
                        if (HDevWindowStack.IsOpen())
                        {
                            HOperatorSet.DispText(HDevWindowStack.GetActive(), (hv_j + " from ") + hv_nWGi,
                                "window", 10, 145, "black", new HTuple(), new HTuple());
                        }
                    }
                }
                HOperatorSet.TupleConcat(hv_RowsM, hv_RowsT, out hv_RowsM);
                HOperatorSet.TupleConcat(hv_ColsM, hv_ColsT, out hv_ColsM);

                //tuple_equal (RowsMo, RowsM, EqualR)
                //tuple_equal (ColsMo, ColsM, EqualC)

                ho_Rmain.Dispose();
                HOperatorSet.GenRegionPoints(out ho_Rmain, hv_RowsM, hv_ColsM);
                //stop ()
                ho_ConnectedRegions.Dispose();
                HOperatorSet.Connection(ho_Rmain, out ho_ConnectedRegions);
                ho_RegionsmainNoCut.Dispose();
                HOperatorSet.SelectShape(ho_ConnectedRegions, out ho_RegionsmainNoCut, "area",
                    "and", hv_minlen, 9999999);

            }
            else if ((int)(new HTuple(hv_useODB.TupleEqual(1))) != 0)
            {
                ho_RegionsmainNoCut.Dispose();
                ho_RegionsmainNoCut = ho_SkeletonsWire.CopyObj(1, -1);
            }
            else
            {
                ho_RegionmainNoCut.Dispose();
                ho_RegionmainNoCut = ho_SkeletonGS.CopyObj(1, -1);
                ho_RegionsmainNoCut.Dispose();
                HOperatorSet.Connection(ho_RegionmainNoCut, out ho_RegionsmainNoCut);
            }


            HOperatorSet.CountObj(ho_RegionsmainNoCut, out hv_nFWMP);

            //junctions_skeleton (Regionsmain, EndPoints1, JuncPoints1)
            //union1 (EndPoints1, EndPoints1u)
            //difference (Regionsmain, EndPoints1u, RegionDifference1)
            //junctions_skeleton (RegionDifference1, EndPoints2, JuncPoints2)
            //union1 (EndPoints2, EndPoints2u)
            //get_region_points (EndPoints2u, Rows2, Cols2)

            //* elongation lengths (elengthend, elengthnoend)
            //* are moved to Teaching parameters

            HOperatorSet.GetImageSize(ho_WidthImageP, out hv_Width, out hv_Height);
            HOperatorSet.GetImageSize(ho_DirImageP, out hv_Widthd, out hv_Heightd);

            //write_image (DirImageP, 'tiff', 0, path+'Test/DirImageP')
            //write_image (WidthImageP, 'tiff', 0, path+'Test/WidthImageP')
            //write_object (RegionG, path+'Test/RegionG')
            //write_object (Regionsmain, path+'Test/Regionsmain')
            //write_object (SkeletonGi, path+'Test/SkeletonGi')

            //**** rearranging Skeletons to PNi/PNSi
            //union1 (RegionsmainNoCut, Regionmain)

            //difference (SkeletonFWM, Cr, SkeletonFWM)
            //**
            ho_Regionmaini.Dispose();
            HOperatorSet.Difference(ho_RegionsmainNoCut, ho_RegionCut, out ho_Regionmaini
                );
            ho_Regionsmain.Dispose();
            HOperatorSet.Connection(ho_Regionmaini, out ho_Regionsmain);
            ho_Regionmain.Dispose();
            HOperatorSet.Union1(ho_Regionsmain, out ho_Regionmain);
            //SkeletonsFWM := SkeletonsFWMii

            //*****
            //gen_contours_skeleton_xld (RegionsmainNoCut, ContoursG, 1, 'filter')
            //count_obj (ContoursG, nC)
            //nc := 1
            //select_obj (ContoursG, ContourSelected, nc)
            //select_obj (RegionsmainNoCut, ObjectSelected, nc)
            //contour_point_num_xld (ContourSelected, np)
            //if (np < 21)
            //lookaround := 1
            //else
            //lookaround := 11
            //endif

            //get_contour_angle_xld (ContourSelected, 'abs', 'regress', lookaround, AnglesG)

            //AngleUnwrap (AnglesG, AnglesG)
            //AnglesGrad := (AnglesG-AnglesG[0])*180./3.1415926

            //create_funct_1d_array (AnglesGrad, FAnglesGrad)
            //smooth_funct_1d_mean (FAnglesGrad, 39, 1, FAnglesGradsm)

            //expand_region (RegionsmainNoCut, RegForbid, RegionsPNPnci, num_iter, 'image')
            //expand_region (RegionsmainNoCut, RegForbid, RegionsPNP2, 50, 'image')
            //tuple_length (AnglesGrad, nA)
            //get_contour_xld (ContourSelected, RowC, ColC)
            //jbeg := 0
            //gen_empty_obj (AllRegs)
            //Reg := ObjectSelected
            //ang := 0.
            //js := 5
            //elongthover := 20.5
            //for j := 0 to nA-1 by 1

            //if ((AnglesGrad[j]-ang) > 290)
            //gen_circle (Circle, RowC[j], ColC[j], 2.5)
            //if (j < nA-50)
            //get_grayval (WidthImageP, RowC[j], ColC[j], wg)
            //get_grayval (DirImageP, RowC[j], ColC[j], p)
            //pa := p*3.14159/256
            //gen_rectangle2 (Rectangle, RowC[j], ColC[j], pa, wg*4, 1)
            //intersection (Reg, Rectangle, RegionIntersection1)
            //area_center (RegionIntersection1, a, Row1, Column1)
            //if (a==0)
            //continue
            //endif
            //difference (Reg, Circle, Reg)
            //connection (Reg, Regs)
            //jl := j+js
            //get_region_index (Regs, RowC[jl], ColC[jl], Index)
            //get_region_index (Regs, RowC[j-js], ColC[j-js], IndexR)
            //select_obj (Regs, Reg, Index)
            //select_obj (Regs, Reg1, IndexR)
            //gen_region_points (PC, RowC[j], ColC[j])
            //expand_region (PC, RegionGS, RegionExpanded, elongthover, 'image')
            //intersection (SkeletonG, RegionExpanded, RegionIntersection)
            //union2 (Reg1, RegionIntersection, Reg1exp)
            //concat_obj (AllRegs, Reg1exp, AllRegs)
            //endif
            //ang := AnglesGrad[j]
            //endif

            //endfor
            //connection (Reg, Regs)
            //select_obj (Regs, Reg1, 1)
            //expand_region (PC, RegionGS, RegionExpanded, elongthover, 'image')
            //intersection (SkeletonG, RegionExpanded, RegionIntersection)
            //union2 (Reg1, RegionIntersection, Reg1exp)
            //concat_obj (AllRegs, Reg1exp, AllRegs)
            //gen_image_const (Image, 'byte', Widthd, Heightd)
            //write_image (Image, 'tiff', 0, 'Image')
            //stop ()
            //*****
            //select_obj (AllRegs, ObjectSelected1, 1)
            //select_obj (AllRegs, ObjectSelected2, 2)
            //select_obj (AllRegs, ObjectSelected3, 3)
            //gen_empty_obj (AllRegsR)
            //concat_obj (AllRegsR, ObjectSelected3, AllRegsR)
            //concat_obj (AllRegsR, ObjectSelected2, AllRegsR)
            //concat_obj (AllRegsR, ObjectSelected1, AllRegsR)
            //expand_region (AllRegs, SkeletonGS, RegionsPNPnci, 30, 'image')
            //expand_region (AllRegsR, SkeletonGS, RegionsPNPnciR, 30, 'image')
            //select_obj (AllRegs, ObjectSelected3, 3)
            //expand_region (ObjectSelected3, SkeletonGS, RegionsPNPnci3, 30, 'image')
            //concat_obj (RegionsPNPnci, RegionsPNPnci3, RegionsPNPnci)

            if (HDevWindowStack.IsOpen())
            {
                //dev_display (Gi)
            }
            if (HDevWindowStack.IsOpen())
            {
                //dev_display (AllRegs)
            }


            //stop ()

            //difference (SkeletonPartsPi, RegionCut, SkeletonFWMii)
            //connection (SkeletonFWMii, SkeletonsFWMii)
            //SkeletonsFWM := SkeletonsFWMii
            //**nA
            //connection (Regionmain, Regionsmain)
            ho_RegionsPNPi.Dispose();
            HOperatorSet.ExpandRegion(ho_Regionsmain, ho_RegForbid, out ho_RegionsPNPi, hv_num_iter,
                "image");
            //RegionsPNPii,RegionsPNPi
            //dilation_circle (RegionsPNPii, RegionsPNPi, 2)
            //**
            //intersection (RegionsPNi, Regionmain, RegionsmainPNi)
            //count_obj (RegionsmainPNPi, nRMPNI)


            ho_RegionsmainPNPi.Dispose();
            HOperatorSet.Intersection(ho_RegionsPNPi, ho_Regionmain, out ho_RegionsmainPNPi
                );
            HOperatorSet.CountObj(ho_RegionsmainPNPi, out hv_nRMPNPI);
            //elengthover := 0
            //ExtendedSkeletonWFMP5 (RegionG, Regionsmain, SkeletonGi, WidthImageP, DirImageP, SkeletonPartsPi, WidthImagePout, DirImagePout, elengthend, elengthnoend, jpadd)
            ho_SkeletonPartsPinc.Dispose(); ho_WidthImagePout.Dispose(); ho_DirImagePout.Dispose();
            ExtendedSkeletonWFMP5(ho_RegionG, ho_RegionsmainNoCut, ho_SkeletonGi, ho_WidthImageP,
                ho_DirImageP, out ho_SkeletonPartsPinc, out ho_WidthImagePout, out ho_DirImagePout,
                hv_elengthend, hv_elengthnoend, hv_jpadd);
            //ExtendedSkeletonWFMP7 (RegionG, RegionsmainPNPi, RegionsmainNoCut, WidthImageP, DirImageP, RegionCut, SkeletonPartsPi, WidthImagePout, DirImagePout, ImageOverlap, RegsOverlap, elengthend, elengthnoend, jpadd, elengthover)
            //scale_image_max (ImageOverlap, ImageScaleMax)
            //ExtendedSkeletonWFMP5 (RegionG, RegionsmainPNi, SkeletonGi, WidthImageP, DirImageP, SkeletonPartsPi, WidthImagePout, DirImagePout, elengthend, elengthnoend, jpadd)
            //*** removing R ******
            ho_SkeletonPartsPii.Dispose();
            HOperatorSet.Difference(ho_SkeletonPartsPinc, ho_RegNoProc, out ho_SkeletonPartsPii
                );
            if ((int)(new HTuple(hv_removeGNDfromFWM.TupleEqual(1))) != 0)
            {
                //******** remowing GND from FWM *****
                ho_SkeletonPartsPi.Dispose();
                HOperatorSet.Difference(ho_SkeletonPartsPii, ho_RegionsGND, out ho_SkeletonPartsPi
                    );
            }
            else
            {
                ho_SkeletonPartsPi.Dispose();
                ho_SkeletonPartsPi = ho_SkeletonPartsPii.CopyObj(1, -1);
            }
            //union1 (SkeletonPartsPi, SkeletonPartPi)
            //** prun ****
            //pruning (SkeletonPartPi, RegionPrune, 1)
            //********
            //connection (RegionPrune, SkeletonPartsPip)
            //pruning (SkeletonPartsPi, SkeletonPartsP, 1)
            ho_SkeletonPartsP.Dispose();
            ho_SkeletonPartsP = ho_SkeletonPartsPi.CopyObj(1, -1);
            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.DispText(HDevWindowStack.GetActive(), "Pat FWM finding finished              ",
                    "window", 10, 10, "black", new HTuple(), new HTuple());
            }
            //stop ()

            //intersection (RegionGu, Rectanglesu, Regs)
            //union2 (RegionExpandedi, Regs, RegionExpanded)
            //count_seconds (t2)
            //t := t2-t1
            if (HDevWindowStack.IsOpen())
            {
                //dev_display (Gi)
            }
            if (HDevWindowStack.IsOpen())
            {
                //dev_display (RegionExpanded)
            }
            if (HDevWindowStack.IsOpen())
            {
                //dev_disp_text ('t='+ (t)$'5.5', 'window', 0, 0, 'black', [], [])
            }
            //stop ()
            //stop ()

            //difference (Regionsmain, RegNoProc, Regionsmainr)
            //difference (Regionsmainr, RegionsGND, SkeletonPartsPnoG)
            //*****
            hv_TotalPRC = 40;
            hv_CurrentOper = "Building Space Skeletons";
            HOperatorSet.CountObj(ho_SkeletonPartsP, out hv_nSkP);
            if ((int)(new HTuple(hv_autowidth.TupleEqual(1))) != 0)
            {
                hv_WminS = hv_WidthS.Clone();
            }
            else
            {
                hv_WminS = hv_wFWMS_COPY_INP_TMP.Clone();
            }
            //WminS := wFWMS
            HOperatorSet.TupleLength(hv_WminS, out hv_nWS);
            //**** vector based Space Skel. FWMS
            //wsides := 2
            //RowsM := []
            //ColsM := 0
            //VectorRM := {1}
            //VectorRM.clear()
            //VectorCM := {1}
            //VectorCM.clear()
            //for j := 0 to nWGSi-1 by 1
            //w := WGSi[j]
            //for k := 0 to nWS-1 by 1
            //if (w >= (WminS[k]-wsideS) and w <= (WminS[k]+wsideS))
            //VectorRM.insert(j, RowsGSi[j])
            //VectorCM.insert(j, ColsGSi[j])
            //endif
            //endfor
            //endfor
            //convert_vector_to_tuple (VectorRM, RowsMS)
            //convert_vector_to_tuple (VectorCM, ColsMS)
            //**** temp. tuple based Space Skel. FWMS
            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.DispText(HDevWindowStack.GetActive(), "Space FWMS finding started                   ",
                    "window", 10, 10, "black", new HTuple(), new HTuple());
            }
            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.SetColor(HDevWindowStack.GetActive(), "yellow");
            }
            hv_RowsMS = new HTuple();
            hv_ColsMS = new HTuple();
            hv_RowsT = new HTuple();
            hv_ColsT = new HTuple();
            HOperatorSet.TupleLength(hv_WGSi, out hv_nWGSi);

            HTuple end_val653 = hv_nWGSi - 1;
            HTuple step_val653 = 1;
            for (hv_j = 0; hv_j.Continue(end_val653, step_val653); hv_j = hv_j.TupleAdd(step_val653))
            {
                hv_w = hv_WGSi.TupleSelect(hv_j);
                //if (RowsGSi[j]==894 and ColsGSi[j]==9086)
                //stop ()
                //endif
                HTuple end_val658 = hv_nWS - 1;
                HTuple step_val658 = 1;
                for (hv_k = 0; hv_k.Continue(end_val658, step_val658); hv_k = hv_k.TupleAdd(step_val658))
                {
                    if ((int)((new HTuple((new HTuple((new HTuple(hv_w.TupleGreaterEqual((hv_WminS.TupleSelect(
                        hv_k)) - hv_wsideS_COPY_INP_TMP))).TupleAnd(new HTuple(hv_w.TupleLessEqual(
                        (hv_WminS.TupleSelect(hv_k)) + hv_wsideS_COPY_INP_TMP))))).TupleAnd(new HTuple(((hv_ColsGSi.TupleSelect(
                        hv_j))).TupleLess(hv_Width - 10))))).TupleAnd(new HTuple(((hv_ColsGSi.TupleSelect(
                        hv_j))).TupleGreater(10)))) != 0)
                    {
                        HOperatorSet.TupleConcat(hv_RowsT, hv_RowsGSi.TupleSelect(hv_j), out hv_RowsT);
                        HOperatorSet.TupleConcat(hv_ColsT, hv_ColsGSi.TupleSelect(hv_j), out hv_ColsT);
                    }
                }
                if ((int)(new HTuple(((hv_j % 10000)).TupleEqual(0))) != 0)
                {
                    HOperatorSet.TupleConcat(hv_RowsMS, hv_RowsT, out hv_RowsMS);
                    HOperatorSet.TupleConcat(hv_ColsMS, hv_ColsT, out hv_ColsMS);
                    hv_RowsT = new HTuple();
                    hv_ColsT = new HTuple();
                }
                if ((int)(new HTuple(((hv_j % 100000)).TupleEqual(0))) != 0)
                {
                    if (HDevWindowStack.IsOpen())
                    {
                        HOperatorSet.DispText(HDevWindowStack.GetActive(), (hv_j + " from ") + hv_nWGSi,
                            "window", 10, 145, "black", new HTuple(), new HTuple());
                    }
                }
            }
            HOperatorSet.TupleConcat(hv_RowsMS, hv_RowsT, out hv_RowsMS);
            HOperatorSet.TupleConcat(hv_ColsMS, hv_ColsT, out hv_ColsMS);
            //stop ()
            ho_RmainS.Dispose();
            HOperatorSet.GenRegionPoints(out ho_RmainS, hv_RowsMS, hv_ColsMS);

            //**** removing space junctions *****
            ho_EndPointsS.Dispose(); ho_JuncPointsS.Dispose();
            HOperatorSet.JunctionsSkeleton(ho_RmainS, out ho_EndPointsS, out ho_JuncPointsS
                );
            ho_JuncPointsSu.Dispose();
            HOperatorSet.Union1(ho_JuncPointsS, out ho_JuncPointsSu);
            HOperatorSet.GetRegionPoints(ho_JuncPointsSu, out hv_RowsJS, out hv_ColumnsJS);
            HOperatorSet.TupleLength(hv_RowsJS, out hv_nJS);
            hv_rJ = 4.5;
            HOperatorSet.TupleGenConst(hv_nJS, hv_rJ, out hv_RJ);
            if ((int)(new HTuple(hv_nJS.TupleGreater(0))) != 0)
            {
                ho_CirclesSJ.Dispose();
                HOperatorSet.GenCircle(out ho_CirclesSJ, hv_RowsJS, hv_ColumnsJS, hv_RJ);
                ho_CirclesSJu.Dispose();
                HOperatorSet.Union1(ho_CirclesSJ, out ho_CirclesSJu);
                ho_RmainSnoJ.Dispose();
                HOperatorSet.Difference(ho_RmainS, ho_CirclesSJu, out ho_RmainSnoJ);
                //*********
                ho_ConnectedRegions.Dispose();
                HOperatorSet.Connection(ho_RmainSnoJ, out ho_ConnectedRegions);
                ho_RegionsmainSs.Dispose();
                HOperatorSet.SelectShape(ho_ConnectedRegions, out ho_RegionsmainSs, "area",
                    "and", hv_minlen, 9999999);
                ho_RegionsmainSu.Dispose();
                HOperatorSet.Union1(ho_RegionsmainSs, out ho_RegionsmainSu);
                ho_RegionsmainSuc.Dispose();
                HOperatorSet.Difference(ho_DomainG, ho_RegionsmainSu, out ho_RegionsmainSuc
                    );
                ho_EndPointsMSS.Dispose(); ho_JuncPointsMSS.Dispose();
                HOperatorSet.JunctionsSkeleton(ho_RegionsmainSs, out ho_EndPointsMSS, out ho_JuncPointsMSS
                    );
                ho_EndPointsMSSu.Dispose();
                HOperatorSet.Union1(ho_EndPointsMSS, out ho_EndPointsMSSu);
                ho_EndPointsMSSuc.Dispose();
                HOperatorSet.Connection(ho_EndPointsMSSu, out ho_EndPointsMSSuc);
                ho_RegionsmainSn.Dispose();
                ho_RegionsmainSn = ho_RegionsmainSs.CopyObj(1, -1);
            }
            else
            {
                ho_RegionsmainSn.Dispose();
                ho_RegionsmainSn = ho_RmainS.CopyObj(1, -1);
            }

            ho_ConnectedRegionsS.Dispose();
            HOperatorSet.Connection(ho_RmainS, out ho_ConnectedRegionsS);
            ho_RegionsmainSNoCut.Dispose();
            HOperatorSet.SelectShape(ho_ConnectedRegionsS, out ho_RegionsmainSNoCut, "area",
                "and", hv_minlen, 9999999);
            //junctions_skeleton (RegionsmainSNoCut, SkelEPi, JuncPoints1)
            //union1 (SkelEPi, SkelEPu)
            //dilation_circle (SkelEPu, SkelEP, 9.5)
            //area_center (EndPointsMSSuc, Area, RowMSS, ColMSS)
            //tuple_length (RowMSS, nMSS)
            //tuple_gen_const (nMSS, 1, RadendMSS)

            HTuple end_val712 = hv_cutendMSS;
            HTuple step_val712 = 1;
            for (hv_j = 1; hv_j.Continue(end_val712, step_val712); hv_j = hv_j.TupleAdd(step_val712))
            {
                //j := 585
                ho_EndPointsMSS.Dispose(); ho_JuncPointsMSS.Dispose();
                HOperatorSet.JunctionsSkeleton(ho_RegionsmainSNoCut, out ho_EndPointsMSS, out ho_JuncPointsMSS
                    );
                HOperatorSet.AreaCenter(ho_EndPointsMSS, out hv_Area1, out hv_Row1, out hv_Column1);
                HOperatorSet.TupleLength(hv_Area1, out hv_nEndPointsMSS);
                if ((int)(new HTuple(hv_Area1.TupleGreater(0))) != 0)
                {
                    ho_EndPointsMSSu.Dispose();
                    HOperatorSet.Union1(ho_EndPointsMSS, out ho_EndPointsMSSu);
                    ho_EndPointsMSSuc.Dispose();
                    HOperatorSet.Connection(ho_EndPointsMSSu, out ho_EndPointsMSSuc);
                    HOperatorSet.AreaCenter(ho_EndPointsMSSuc, out hv_Area, out hv_RowMSS, out hv_ColMSS);
                    HOperatorSet.TupleLength(hv_RowMSS, out hv_nMSS);
                    HOperatorSet.TupleGenConst(hv_nMSS, 1, out hv_RadendMSS);
                    ho_CMSSE.Dispose();
                    HOperatorSet.GenCircle(out ho_CMSSE, hv_RowMSS, hv_ColMSS, hv_RadendMSS);
                    ho_CMSSEu.Dispose();
                    HOperatorSet.Union1(ho_CMSSE, out ho_CMSSEu);
                    {
                        HObject ExpTmpOutVar_0;
                        HOperatorSet.Difference(ho_RegionsmainSNoCut, ho_CMSSEu, out ExpTmpOutVar_0
                            );
                        ho_RegionsmainSNoCut.Dispose();
                        ho_RegionsmainSNoCut = ExpTmpOutVar_0;
                    }
                }

            }

            //RegionsmainS := RegionsmainSn
            ho_RegionmainSNoCut.Dispose();
            HOperatorSet.Union1(ho_RegionsmainSNoCut, out ho_RegionmainSNoCut);
            ho_RegionmainS.Dispose();
            HOperatorSet.Difference(ho_RegionmainSNoCut, ho_RegionCut, out ho_RegionmainS
                );
            ho_RegionsmainS.Dispose();
            HOperatorSet.Connection(ho_RegionmainS, out ho_RegionsmainS);
            ho_RegionmainS.Dispose();
            HOperatorSet.Union1(ho_RegionsmainS, out ho_RegionmainS);
            HOperatorSet.CountObj(ho_RegionsmainS, out hv_nFWMPS);
            ho_SkeletonsFWMScut.Dispose();
            ho_SkeletonsFWMScut = ho_RegionsmainS.CopyObj(1, -1);
            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.DispText(HDevWindowStack.GetActive(), "Space FWMS finding finished                         ",
                    "window", 10, 10, "black", new HTuple(), new HTuple());
            }

            ho_SkeletonsFWMS.Dispose();
            ho_SkeletonsFWMS = ho_RegionsmainSNoCut.CopyObj(1, -1);
            HOperatorSet.CountObj(ho_SkeletonsFWMS, out hv_nFWMS);
            //stop ()
            //**********************
            //******** Preping IRB image  *******************
            hv_TotalPRC = 60;
            hv_CurrentOper = "Preparing IRB";
            //*** preparing for IRB (pattern)

            //gen_image_const (ImageIRB, 'byte', WidthG, HeightG)
            ho_ImageIR.Dispose();
            HOperatorSet.GenImageConst(out ho_ImageIR, "byte", hv_WidthG, hv_HeightG);
            ho_ImageIR16.Dispose();
            HOperatorSet.GenImageConst(out ho_ImageIR16, "uint2", hv_WidthG, hv_HeightG);
            ho_RegForbid.Dispose();
            HOperatorSet.GenEmptyRegion(out ho_RegForbid);

            //pniovl := 4
            //** if RegForbid is from RegionIS, then need to recalculate for every image
            //** alternative - create RegForbid from eroded RegionGS
            //erosion_circle (RegionGSi, RegionGSer, 4)
            //RegForbid := RegionGSer


            HOperatorSet.CountObj(ho_RegionsPNPi, out hv_nINB);
            HOperatorSet.TupleGenSequence(2, hv_nINB + 1, 1, out hv_SequenceINBi);
            //*** old GND
            //SequenceIRB := SequenceINBi%250+3
            //SequenceIRB[groundpn-1] := groundgval
            //SequenceIRB := SequenceINBi%250+3
            //SequenceIRB[groundpn-1] := groundgval
            //*** new GND
            //** start of the sequence
            //stseq := nGND+1
            //overf := 255-stseq
            //SequenceIRB := SequenceINBi%overf+stseq
            //SequenceIRB[groundpn-1] := groundgval

            //stseq := 5
            //SequenceINB := SequenceINBi+stseq
            //****
            ho_RegionGND.Dispose();
            HOperatorSet.Union1(ho_RegionsGND, out ho_RegionGND);

            //complement (RegionGND, RegionGNDC)
            ho_RegionGNDC.Dispose();
            HOperatorSet.Difference(ho_DomainG, ho_RegionGND, out ho_RegionGNDC);
            //complement (RegionG, RegionGS)
            ho_RegionGS.Dispose();
            HOperatorSet.Difference(ho_DomainG, ho_RegionG, out ho_RegionGS);

            //expand_region (SkeletonsFWM, RegionGS, RegionExpanded, 'maximal', 'image')
            //** temporary image for pnum finding for FWM area MSs, used in MSBuildFWM - should exist in Teaching!!!
            //paint_region (RegionsPNi, ImageIR, ImageIR, SequenceIRB, 'fill')
            {
                HObject ExpTmpOutVar_0;
                HOperatorSet.PaintRegion(ho_RegionsPNPi, ho_ImageIR16, out ExpTmpOutVar_0, hv_SequenceINBi,
                    "fill");
                ho_ImageIR16.Dispose();
                ho_ImageIR16 = ExpTmpOutVar_0;
            }
            //stop ()

            //**
            //**
            //**  removing false call
            //gen_circle (Cr, 11895, 3182, 10)
            //difference (SkeletonPartsP, Cr, SkeletonPartsP)
            //count_seconds (t1)
            //*** building Non-FWM MSNs
            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.DispText(HDevWindowStack.GetActive(), "Non-FWM building started                            ",
                    "window", 10, 10, "black", new HTuple(), new HTuple());
            }

            hv_TotalPRC = 70;
            hv_CurrentOper = "Building non-FWM sensors";

            ho_SkeletonFWM.Dispose(); ho_RegionsNFWM.Dispose(); ho_RegionsNFWMBcut.Dispose();
            MSNBuildNonFWM1(ho_RegionG, ho_SkeletonPartsP, ho_WidthImageP, ho_DirImageP,
                ho_RegionGS, ho_ImageIR, out ho_SkeletonFWM, out ho_RegionsNFWM, out ho_RegionsNFWMBcut,
                hv_sm_factor, hv_lookaround, hv_wpl, 0, out hv_PnumN, out hv_PnumRN1, out hv_Y1N,
                out hv_X1N, out hv_Y2N, out hv_X2N, out hv_PaddrCN, out hv_PaddrRN, out hv_WN,
                out hv_WGN, out hv_PregCN, out hv_PCinRnumN, out hv_PcontRN);
            ho_RegionsNFWMs.Dispose();
            HOperatorSet.SelectShape(ho_RegionsNFWM, out ho_RegionsNFWMs, "inner_radius",
                "and", 10, 9999999);
            ho_RegionsNFWMs.Dispose();
            HOperatorSet.SelectShape(ho_RegionsNFWM, out ho_RegionsNFWMs, "area", "and",
                400, 9999999);
            HOperatorSet.AreaCenter(ho_RegionsNFWMs, out hv_AreaNFWMs, out hv_Row, out hv_Column);
            ho_Region1.Dispose();
            HOperatorSet.GenRegionPoints(out ho_Region1, hv_Y1N, hv_X1N);
            ho_Region2.Dispose();
            HOperatorSet.GenRegionPoints(out ho_Region2, hv_Y2N, hv_X2N);
            hv_YCN = (hv_Y1N + hv_Y2N) / 2;
            hv_XCN = (hv_X1N + hv_X2N) / 2;
            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.DispText(HDevWindowStack.GetActive(), "Non-FWM building finished",
                    "window", 10, 10, "black", new HTuple(), new HTuple());
            }

            //stop ()
            //****************
            //**** rearranging Skeletons to PNi/PNSi
            //** activate not to use MS elongation
            //SkeletonPartsP := Regionsmain
            //****
            //** removed 240118
            //union1 (SkeletonPartsP, SkeletonPartsPu)
            //intersection (RegionsPNPi, SkeletonPartsPu, SkeletonPartsPr)
            //count_obj (SkeletonPartsPr, nSkPr)



            //gen_empty_obj (Empty)
            //gen_empty_obj (Parts)
            //count_obj (RegionsPNi, nPNi)
            //for j := 1 to nPNi by 1

            //if (j==10 or j==20)
            //concat_obj (Parts, SkeletonPartsPu, Parts)
            //else
            //concat_obj (Parts, Empty, Parts)
            //endif
            //endfor
            //count_obj (Parts, nParts)
            //stop ()
            //*** preparing for IRBS (space)

            ho_ImageIRBS.Dispose();
            HOperatorSet.GenImageConst(out ho_ImageIRBS, "byte", hv_WidthG, hv_HeightG);
            ho_ImageIRS.Dispose();
            HOperatorSet.GenImageConst(out ho_ImageIRS, "byte", hv_WidthG, hv_HeightG);
            ho_ImageIRS16.Dispose();
            HOperatorSet.GenImageConst(out ho_ImageIRS16, "uint2", hv_WidthG, hv_HeightG);
            ho_RegForbidS.Dispose();
            HOperatorSet.GenEmptyRegion(out ho_RegForbidS);

            //pniovl := 4
            //** if RegForbid is from RegionIS, then need to recalculate for every image
            //** alternative - create RegForbid from eroded RegionGS
            //erosion_circle (RegionGSi, RegionGSer, 4)
            //RegForbid := RegionGSer

            //add_image (ImageIR, ImageIRS, ImageIRSum, 0.5, 0)
            //regiongrowing (ImageIRSum, Regions, 3, 3, 6, 100)
            //stop ()


            //*** MS elongation
            //jradMSe := 9
            //eradMSe := 7
            //pruning (Regionsmain, SkeletonsFWMi, 50)

            //** removing areas around EP and JP for FWM Pat. Skels*****
            ho_EndPoints.Dispose(); ho_JuncPoints.Dispose();
            HOperatorSet.JunctionsSkeleton(ho_Regionsmain, out ho_EndPoints, out ho_JuncPoints
                );
            //union1 (JuncPoints, JP)
            //get_region_points (JP, RowsJ, ColumnsJ)
            //get_grayval (WidthImageP, RowsJ, ColumnsJ, RadJ)
            //gen_circle (CJ, RowsJ, ColumnsJ, RadJ)
            //difference (Regionsmain, CJ, SkeletonsFWMii)
            //**** changed 220716 (elongation skeleton is used) ***
            //SkeletonsFWMii := SkeletonPartsP
            ho_SkeletonsFWMii.Dispose();
            HOperatorSet.Difference(ho_SkeletonPartsP, ho_RegNoProcd, out ho_SkeletonsFWMii
                );
            //SkeletonsFWMii := Regionsmain
            ho_EP.Dispose();
            HOperatorSet.Union1(ho_EndPoints, out ho_EP);

            HOperatorSet.GetRegionPoints(ho_EP, out hv_RowsE, out hv_ColumnsE);
            //test_subset_region (EndPoints, SkeletonsG, IsSubset)
            //test_region_point (SkeletonG, RowsE, ColumnsE, IsInsideE)
            HOperatorSet.TupleLength(hv_RowsE, out hv_nE);
            //*** checking if EP is real EP (not working yet)
            //RowsEv := []
            //ColumnsEv := []
            //nEv := 0
            //for j := 0 to nE-1 by 1
            //test_region_point (SkeletonG, RowsE[j], ColumnsE[j], IsInsideE)
            //if (IsInsideE==0)
            //tuple_concat (RowsEv, RowsE[j], RowsEv)
            //tuple_concat (ColumnsEv, ColumnsE[j], ColumnsEv)
            //nEv := nEv+1
            //endif
            //endfor
            //stop ()
            //get_grayval (WidthImageP, RowsE, ColumnsE, RadE)
            //**** changed 220716 (elongation skeleton is used) ***
            //tuple_gen_const (nE, eradMSe, Erad)
            //gen_circle (CE, RowsE, ColumnsE, Erad)
            //difference (SkeletonsFWMii, CE, SkeletonsFWMi)

            ho_SkeletonsFWMp.Dispose();
            HOperatorSet.SelectShape(ho_SkeletonsFWMii, out ho_SkeletonsFWMp, "area", "and",
                hv_minlen, 999999);
            HOperatorSet.CountObj(ho_SkeletonsFWMp, out hv_nFWMs);
            //difference (SkeletonG, SkeletonsFWMp, RegionDifference)

            //** Non-FWM removal areas for endpoints
            //eradNFWMremove := 15
            HOperatorSet.TupleGenConst(hv_nE, hv_eradNFWMremove, out hv_Erad2);
            ho_CE2.Dispose();
            HOperatorSet.GenCircle(out ho_CE2, hv_RowsE, hv_ColumnsE, hv_Erad2);
            ho_EPNFWMremove.Dispose();
            HOperatorSet.Union1(ho_CE2, out ho_EPNFWMremove);

            //SkeletonsFWMcut := Regionsmain
            ho_SkeletonsFWMcut.Dispose();
            HOperatorSet.Intersection(ho_Regionsmain, ho_SkeletonFWM, out ho_SkeletonsFWMcut
                );
            //** FWMS (for space)
            //** removing areas around EP and JP for FWM Pat. Skels*****
            ho_EndPointsS.Dispose(); ho_JuncPointsS.Dispose();
            HOperatorSet.JunctionsSkeleton(ho_RegionsmainS, out ho_EndPointsS, out ho_JuncPointsS
                );
            ho_SkeletonsFWMSii.Dispose();
            HOperatorSet.Difference(ho_RegionsmainS, ho_RegNoProcd, out ho_SkeletonsFWMSii
                );
            //SkeletonsFWMSii := RegionsmainS
            ho_EPS.Dispose();
            HOperatorSet.Union1(ho_EndPointsS, out ho_EPS);
            HOperatorSet.GetRegionPoints(ho_EPS, out hv_RowsES, out hv_ColumnsES);
            HOperatorSet.TupleLength(hv_RowsES, out hv_nES);
            //get_grayval (WidthImageP, RowsE, ColumnsE, RadE)
            HOperatorSet.TupleGenConst(hv_nES, 7, out hv_EradS);
            ho_CES.Dispose();
            HOperatorSet.GenCircle(out ho_CES, hv_RowsES, hv_ColumnsES, hv_EradS);
            ho_SkeletonsFWMSi.Dispose();
            HOperatorSet.Difference(ho_SkeletonsFWMSii, ho_CES, out ho_SkeletonsFWMSi);

            //select_shape (SkeletonsFWMSi, SkeletonsFWMSp, 'area', 'and', minlen, 999999)
            //SkeletonsFWMSp := RegionsmainS
            //stop ()

            //union1 (SkeletonsFWMSi, SkeletonFWMS)
            //connection (SkeletonFWMS, SkeletonsFWMS)
            //count_obj (SkeletonsFWMS, nFWMS)

            ho_RegForbidS.Dispose();
            ho_RegForbidS = ho_SkeletonGi.CopyObj(1, -1);
            //expand_region (SkeletonsGS, RegForbidS, RegionsPNSii, num_iter, 'image')
            ho_RegionsPNSii.Dispose();
            HOperatorSet.ExpandRegion(ho_SkeletonsFWMScut, ho_RegForbidS, out ho_RegionsPNSii,
                hv_num_iter, "image");


            HOperatorSet.CountObj(ho_RegionsPNSii, out hv_nINBS);
            HOperatorSet.TupleGenSequence(2, hv_nINBS + 1, 1, out hv_SequenceINBSi);
            //*** old GND
            //SequenceIRB := SequenceINBi%250+3
            //SequenceIRB[groundpn-1] := groundgval
            //SequenceIRB := SequenceINBi%250+3
            //SequenceIRB[groundpn-1] := groundgval
            //*** new GND
            //** start of the sequence
            //stseqS := nEMPTY+1
            //overfS := 255-stseqS
            //SequenceIRBS := SequenceINBSi%overfS+stseqS

            //SequenceINBS := SequenceINBSi
            //SequenceINBS := SequenceINBSi+stseq
            //SequenceIRBS[groundpn-1] := groundgval
            //****
            ho_RegionEMPTY.Dispose();
            HOperatorSet.Union1(ho_RegionsEMPTY, out ho_RegionEMPTY);

            //complement (RegionGND, RegionGNDC)
            ho_RegionEMPTYC.Dispose();
            HOperatorSet.Difference(ho_DomainG, ho_RegionEMPTY, out ho_RegionEMPTYC);
            ho_RegionsPNSi.Dispose();
            HOperatorSet.DilationCircle(ho_RegionsPNSii, out ho_RegionsPNSi, 1);
            //complement (RegionG, RegionGS)
            //difference (DomainG, RegionG, RegionGS)
            //expand_region (SkeletonsFWM, RegionGS, RegionExpanded, 'maximal', 'image')
            //** temporary image for pnum finding for FWM area MSs, used in MSBuildFWM - should exist in Teaching!!!
            //paint_region (RegionsPNSi, ImageIRS, ImageIRS, SequenceIRBS, 'fill')
            {
                HObject ExpTmpOutVar_0;
                HOperatorSet.PaintRegion(ho_RegionsPNSi, ho_ImageIRS16, out ExpTmpOutVar_0, hv_SequenceINBSi,
                    "fill");
                ho_ImageIRS16.Dispose();
                ho_ImageIRS16 = ExpTmpOutVar_0;
            }

            //expand_region (SkeletonsFWMS, SkeletonG, RegionExpanded, 7, 'image')
            //intersection (RegionsPNSi, SkeletonFWMS, SkeletonsFWMS)
            //stop ()
            //**** removing GND from SkeletonsFWM
            //SkeletonsFWM := SkeletonPartsPr
            //SkeletonsFWM := SkeletonPartsPi
            //**********************************************************
            //*****   Building MS (measurement sensors)  **************
            //*********************************************************
            //** FWMS (for space)
            //MSBuildFWM4 (SkeletonsFWMS, WidthImageS, DirImageS, ImageIRS, wpl, useint, PnumS, PnumRS, YS1, XS1, YS2, XS2, PaddrCS, PaddrRS, YSC, XSC, WGS, PregCS, PCinRnumS, PcontRS, Puseint)
            //gen_region_points (PSC, YSC, XSC)
            //********************************************************
            //***** removing points between FWM skeleton line *********
            //*********************************************************
            //***** parameters for AnglesFWM:
            //endpoint circle radius
            //count_obj (SkelsEPUver, nSkelsEPUver)
            //stop ()
            //complement (SkeletonFWM, SkeletonFWMC)

            //closing_circle (SlelEPU, RegionClosing, 19.5)
            //set_system ('neighborhood', 8)
            //expand_region (SkelEPU, SkeletonFWMC, SlelEPUexp, 5, 'image')
            //connection (RegionClosing, CircsEPcl)
            //set_system ('neighborhood', 4)
            //count_obj (CircsEPcl, nEPcl)
            //***********************************************************
            //** FWMP (for pattern)
            //SkeletonsFWM := Regionsmain
            //SkeletonsFWM := SkeletonPartsPi

            //difference (SkeletonPartsPi, RegionCut, SkeletonFWMii)
            //connection (SkeletonFWMii, SkeletonsFWMii)
            //SkeletonsFWM := SkeletonsFWMii
            //SkeletonsFWM := SkeletonsFWMp
            //difference (SkeletonsFWM, RegNoProc, SkeletonsFWMrc)
            //******* test *****
            //count_obj (RegionsGND, nGND)
            //stseq := nGND+1
            //overf := 255-stseq
            //i := 1744
            //nIRB := i%overf+stseq
            //**** checking ImageIR values vs jFWM
            //count_obj (SkeletonsFWMrc, nFWM)
            //for jFWM := 1 to nFWM by 1
            //select_obj (SkeletonsFWM, Sksel, jFWM)
            //get_region_points (Sksel, Rse, Cse)
            //tuple_length (Rse, nse)
            //if (nse>0)
            //get_grayval (ImageIR, Rse[nse/2], Cse[nse/2], vIR)
            //nIRB := jFWM%overf+stseq
            //if (nIRB!=vIR)
            //stop ()
            //endif
            //endif
            //endfor
            //stop ()
            //***********************

            //*** not used int points*


            hv_useint = 10;
            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.DispText(HDevWindowStack.GetActive(), "Building FWM...                                ",
                    "window", 10, 10, "black", new HTuple(), new HTuple());
            }
            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.SetColor(HDevWindowStack.GetActive(), "cyan");
            }
            ho_SkeletonsFWM.Dispose();
            ho_SkeletonsFWM = ho_SkeletonPartsPinc.CopyObj(1, -1);

            hv_TotalPRC = 80;
            hv_CurrentOper = "Building pattern sensors";


            MSBuildFWM52(ho_SkeletonsFWM, ho_WidthImageP, ho_DirImageP, ho_ImageIR16, hv_wpl,
                hv_useint, out hv_Pnum, out hv_Pnum2, out hv_PnumR, out hv_Y1, out hv_X1,
                out hv_Y2, out hv_X2, out hv_PaddrC, out hv_PaddrR, out hv_YC, out hv_XC,
                out hv_WG, out hv_PregC, out hv_PCinRnum, out hv_PcontR, out hv_PuseintP);

            //MSBuildFWM6 (SkeletonsFWM, WidthImageP, DirImageP, ImageIR16, ImageOverlap, wpl, useint, Pnum, Pnum2, PnumR, Y1, X1, Y2, X2, PaddrC, PaddrR, YC, XC, WG, PregC, PCinRnum, PcontR, PuseintP)

            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.DispText(HDevWindowStack.GetActive(), "FWM MS building finished          ",
                    "window", 10, 10, "black", new HTuple(), new HTuple());
            }

            //*************************************

            //gen_region_line (RegionLines, Y1, X1, Y2, X2)
            ho_PC.Dispose();
            HOperatorSet.GenRegionPoints(out ho_PC, hv_YC, hv_XC);
            //** FWMS (for pattern)

            hv_TotalPRC = 90;
            hv_CurrentOper = "Building space sensors";

            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.DispText(HDevWindowStack.GetActive(), "Building FWMS...                                ",
                    "window", 10, 10, "black", new HTuple(), new HTuple());
            }
            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.SetColor(HDevWindowStack.GetActive(), "blue");
            }
            ho_SkeletonsFWMSrc.Dispose();
            HOperatorSet.Intersection(ho_SkeletonsFWMS, ho_RC, out ho_SkeletonsFWMSrc);
            MSBuildFWM52(ho_SkeletonsFWMSrc, ho_WidthImageS, ho_DirImageS, ho_ImageIRS16,
                hv_wpl, hv_useint, out hv_PnumS, out hv_PnumS2, out hv_PnumRS, out hv_Y1S,
                out hv_X1S, out hv_Y2S, out hv_X2S, out hv_PaddrCS, out hv_PaddrRS, out hv_YCS,
                out hv_XCS, out hv_WGS, out hv_PregCS, out hv_PCinRnumS, out hv_PcontRS,
                out hv_PuseintS);
            //*************************************
            ho_PCS.Dispose();
            HOperatorSet.GenRegionPoints(out ho_PCS, hv_YCS, hv_XCS);
            if (HDevWindowStack.IsOpen())
            {
                HOperatorSet.DispText(HDevWindowStack.GetActive(), "FWMS MSS building finished",
                    "window", 10, 10, "black", new HTuple(), new HTuple());
            }

            //******************************************************
            //ncut := 1000000
            //tuple_length (X1, nMS)
            //n := nMS/ncut
            //gen_empty_obj (RegionMSLines)
            //for j := 0 to n-1 by 1
            //tuple_select_range (Y1, j*ncut, (j+1)*ncut-1, Y1c)
            //tuple_select_range (X1, j*ncut, (j+1)*ncut-1, X1c)
            //tuple_select_range (Y2, j*ncut, (j+1)*ncut-1, Y2c)
            //tuple_select_range (X2, j*ncut, (j+1)*ncut-1, X2c)
            //gen_region_line (RegionMSLinesc, Y1c, X1c, Y2c, X2c)
            //union2 (RegionMSLines, RegionMSLinesc, RegionMSLines)
            //endfor
            //tuple_select_range (Y1, n*ncut, nMS-1, Y1c)
            //tuple_select_range (X1, n*ncut, nMS-1, X1c)
            //tuple_select_range (Y2, n*ncut, nMS-1, Y2c)
            //tuple_select_range (X2, n*ncut, nMS-1, X2c)
            //gen_region_line (RegionMSLinesc, Y1c, X1c, Y2c, X2c)
            //union2 (RegionMSLines, RegionMSLinesc, RegionMSLines)


            //gen_region_line (RegionMSLines, Y1, X1, Y2, X2)
            //union1 (RegionMSLines, RegionMSi)
            //closing_circle (RegionMSLines, RegionMS, 3.5)
            //******************************************************
            //stop ()
            ho_RegionsPNi.Dispose();
            ho_RegionsPNi = ho_RegionsPNPi.CopyObj(1, -1);
            hv_TotalPRC = 100;

            if ((int)(new HTuple(hv_autowidth.TupleEqual(1))) != 0)
            {
                hv_wFWM_COPY_INP_TMP = hv_Width.Clone();
                hv_wFWMS_COPY_INP_TMP = hv_WidthS.Clone();
            }

            hv_CurrentOper = "End of Teaching";
            ho_RegNoProcd.Dispose();
            ho_DomainG.Dispose();
            ho_RC.Dispose();
            ho_RegProc.Dispose();
            ho_Rectangle.Dispose();
            ho_RegionGi.Dispose();
            ho_RegionGid.Dispose();
            ho_RegionsGi.Dispose();
            ho_RmnoG.Dispose();
            ho_RegNoProcC.Dispose();
            ho_RegNoProcCd.Dispose();
            ho_RegionsGnoholes.Dispose();
            ho_SelectedRegions.Dispose();
            ho_RegionI.Dispose();
            ho_RegionsGsm.Dispose();
            ho_RegionGsm.Dispose();
            ho_RegionsGNDnR.Dispose();
            ho_RegionGSi.Dispose();
            ho_RegionsGSi.Dispose();
            ho_RegionsEMPTYnR.Dispose();
            ho_RegionErosion.Dispose();
            ho_RegionDilation.Dispose();
            ho_RegionIntersection.Dispose();
            ho_SkeletonGi.Dispose();
            ho_EndPointsG.Dispose();
            ho_JuncPointsG.Dispose();
            ho_CEP.Dispose();
            ho_RegionGSii.Dispose();
            ho_SkeletonGSi.Dispose();
            ho_SkeletonGSii.Dispose();
            ho_SkeletonsGS.Dispose();
            ho_ConnectedRegionsS.Dispose();
            ho_SkelEPi.Dispose();
            ho_JuncPoints1.Dispose();
            ho_SkelEPu.Dispose();
            ho_EndPointsMSSuc.Dispose();
            ho_EndPointsGS.Dispose();
            ho_JuncPointsGS.Dispose();
            ho_RegionDilationGS.Dispose();
            ho_RegionIntersection1.Dispose();
            ho_SkeletonLG.Dispose();
            ho_EndPointsMSS.Dispose();
            ho_JuncPointsMSS.Dispose();
            ho_EndPointsMSSu.Dispose();
            ho_CMSSE.Dispose();
            ho_CMSSEu.Dispose();
            ho_SkeletonPartsPinc.Dispose();
            ho_WidthImagePout.Dispose();
            ho_DirImagePout.Dispose();
            ho_RegForbid.Dispose();
            ho_RegionsPNii.Dispose();
            ho_AngleImageP.Dispose();
            ho_DistGPi.Dispose();
            ho_DistGSi.Dispose();
            ho_DistGSii.Dispose();
            ho_DistGP.Dispose();
            ho_DistGS.Dispose();
            ho_ImDisplacementi.Dispose();
            ho_Rd.Dispose();
            ho_JP.Dispose();
            ho_CJ.Dispose();
            ho_Skeletonii.Dispose();
            ho_SkeletonsWire.Dispose();
            ho_Rnoint.Dispose();
            ho_Rmain.Dispose();
            ho_ConnectedRegions.Dispose();
            ho_RegionsmainNoCut.Dispose();
            ho_RegionmainNoCut.Dispose();
            ho_Regionsmain.Dispose();
            ho_Regionmain.Dispose();
            ho_Regionmaini.Dispose();
            ho_SkeletonsFWMii.Dispose();
            ho_PC.Dispose();
            ho_SkeletonPartsPi.Dispose();
            ho_RegionsmainPNPi.Dispose();
            ho_SkeletonPartsPii.Dispose();
            ho_RmainS.Dispose();
            ho_EndPointsS.Dispose();
            ho_JuncPointsS.Dispose();
            ho_JuncPointsSu.Dispose();
            ho_CirclesSJ.Dispose();
            ho_CirclesSJu.Dispose();
            ho_RmainSnoJ.Dispose();
            ho_RegionsmainSs.Dispose();
            ho_RegionsmainSu.Dispose();
            ho_RegionsmainSuc.Dispose();
            ho_RegionsmainSn.Dispose();
            ho_RegionsmainS.Dispose();
            ho_RegionmainSNoCut.Dispose();
            ho_RegionmainS.Dispose();
            ho_RegionGND.Dispose();
            ho_RegionGNDC.Dispose();
            ho_RegionsNFWMBcut.Dispose();
            ho_RegionsNFWMs.Dispose();
            ho_Region1.Dispose();
            ho_Region2.Dispose();
            ho_ImageIRBS.Dispose();
            ho_ImageIRS16.Dispose();
            ho_RegForbidS.Dispose();
            ho_EndPoints.Dispose();
            ho_JuncPoints.Dispose();
            ho_EP.Dispose();
            ho_SkeletonsFWMp.Dispose();
            ho_CE2.Dispose();
            ho_SkeletonsFWMSii.Dispose();
            ho_EPS.Dispose();
            ho_CES.Dispose();
            ho_SkeletonsFWMSi.Dispose();
            ho_RegionsPNSii.Dispose();
            ho_RegionEMPTY.Dispose();
            ho_RegionEMPTYC.Dispose();
            ho_SkeletonsFWMSrc.Dispose();
            ho_PCS.Dispose();

            return;

        }





        public void Teach_Slice(HObject ho_Skel_FWMS, HObject ho_WidthImageS, HTuple hv_Win,
            HTuple hv_SensLen, HTuple hv_path_teach_tuples, HTuple hv_Type, out HTuple hv_Pointer_Skel,
            out HTuple hv_Pointer_Sens, out HTuple hv_Rows_Sens, out HTuple hv_Cols_Sens,
            out HTuple hv_Pointer_Pos, out HTuple hv_CurrentOper, out HTuple hv_TotalPRC,
            out HTuple hv_MaxPRC)
        {
            // Local iconic variables 

            HObject ho_Reg_SEL1_XLD, ho_Reg_SEL1 = null;

            // Local control variables 

            HTuple hv_Width_Sens = null, hv_WinD2 = null;
            HTuple hv_SensLenD2 = null, hv_Count = null, hv_Index = null;
            HTuple hv_Rows = new HTuple(), hv_Cols = new HTuple();
            HTuple hv_Len = new HTuple(), hv_Ind = new HTuple(), hv_GrayW = new HTuple();
            HTuple hv_Ind0 = new HTuple(), hv_Ind1 = new HTuple();
            HTuple hv_Angle = new HTuple(), hv_Angle_Deg = new HTuple();
            HTuple hv_Cos = new HTuple(), hv_Sin = new HTuple(), hv_P1X = new HTuple();
            HTuple hv_P1Y = new HTuple(), hv_P2X = new HTuple(), hv_P2Y = new HTuple();
            HTuple hv_dX = new HTuple(), hv_dY = new HTuple(), hv_Rows_S1 = new HTuple();
            HTuple hv_Cols_S1 = new HTuple(), hv_Row1 = new HTuple();
            HTuple hv_Col1 = new HTuple(), hv_Idx = new HTuple(), hv_Length = new HTuple();
            HTuple hv_Len_Sens = new HTuple();
            HTuple hv_SensLen_COPY_INP_TMP = hv_SensLen.Clone();

            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_Reg_SEL1_XLD);
            HOperatorSet.GenEmptyObj(out ho_Reg_SEL1);
            ho_Reg_SEL1_XLD.Dispose();
            HOperatorSet.GenContoursSkeletonXld(ho_Skel_FWMS, out ho_Reg_SEL1_XLD, 1, "filter");

            HObject ExpTmpOutVar_0;
            HOperatorSet.SmoothContoursXld(ho_Reg_SEL1_XLD, out ExpTmpOutVar_0, 15);
            ho_Reg_SEL1_XLD.Dispose();
            ho_Reg_SEL1_XLD = ExpTmpOutVar_0;


            hv_Pointer_Skel = new HTuple();
            hv_Pointer_Sens = new HTuple();
            hv_Rows_Sens = new HTuple();
            hv_Cols_Sens = new HTuple();
            hv_Width_Sens = new HTuple();
            hv_Pointer_Pos = 0;

            List<double> cv_Rows_Sens = new List<double>();
            List<double> cv_Cols_Sens = new List<double>();

            hv_WinD2 = (hv_Win + 3) / 2;
            hv_SensLenD2 = hv_SensLen_COPY_INP_TMP / 2;

            //tuple_concat (Pointer_Skel, 0, Pointer_Skel)
            HOperatorSet.CountObj(ho_Reg_SEL1_XLD, out hv_Count);

            hv_TotalPRC = 0;
            hv_MaxPRC = 0;
            hv_CurrentOper = "Low Gray - Formation of sensors";
            HTuple end_val18 = hv_Count;
            HTuple step_val18 = 1;
            for (hv_Index = 1; hv_Index.Continue(end_val18, step_val18); hv_Index = hv_Index.TupleAdd(step_val18))
            {
                ho_Reg_SEL1.Dispose();
                HOperatorSet.SelectObj(ho_Reg_SEL1_XLD, out ho_Reg_SEL1, hv_Index);
                HOperatorSet.GetContourXld(ho_Reg_SEL1, out hv_Rows, out hv_Cols);

                HOperatorSet.TupleLength(hv_Rows, out hv_Len);
                hv_MaxPRC = hv_MaxPRC + hv_Len;
            }

            HTuple end_val26 = hv_Count;
            HTuple step_val26 = 1;
            for (hv_Index = 1; hv_Index.Continue(end_val26, step_val26); hv_Index = hv_Index.TupleAdd(step_val26))
            {
                ho_Reg_SEL1.Dispose();
                HOperatorSet.SelectObj(ho_Reg_SEL1_XLD, out ho_Reg_SEL1, hv_Index);
                HOperatorSet.GetContourXld(ho_Reg_SEL1, out hv_Rows, out hv_Cols);

                HOperatorSet.TupleLength(hv_Rows, out hv_Len);
                if (hv_Len < 300)
                {
                    continue;
                }
                //*     for Ind := WinD2 to Len - WinD2 - 2 by 1
                HTuple end_val34 = hv_Len - 1;
                HTuple step_val34 = 1;
                for (hv_Ind = 0; hv_Ind.Continue(end_val34, step_val34); hv_Ind = hv_Ind.TupleAdd(step_val34))
                {
                    //if (SensLen == 0)
                    //get_grayval (WidthImageS, Rows[Ind], Cols[Ind], GrayW)
                    //SensLen := GrayW * 2
                    //SensLenD2 := SensLen / 2
                    //endif
                    HOperatorSet.GetGrayval(ho_WidthImageS, hv_Rows.TupleSelect(hv_Ind), hv_Cols.TupleSelect(
                        hv_Ind), out hv_GrayW);
                    //SensLen := GrayW * 2
//                    hv_SensLen_COPY_INP_TMP = (hv_GrayW + 3) * 3;
                    hv_SensLen_COPY_INP_TMP = (hv_GrayW + 3) * 3 / 2;     //gojw
                    hv_SensLenD2 = hv_SensLen_COPY_INP_TMP / 2;

                    if ((int)(new HTuple(hv_Ind.TupleLess(hv_WinD2))) != 0)
                    {
                        hv_Ind0 = 0;
                        hv_Ind1 = hv_Ind + hv_Win;
                    }
                    else if ((int)(new HTuple(hv_Ind.TupleGreater((hv_Len - 1) - hv_WinD2))) != 0)
                    {
                        hv_Ind0 = (hv_Len - 1) - hv_Win;
                        hv_Ind1 = hv_Len - 1;
                    }
                    else
                    {
                        hv_Ind0 = hv_Ind - hv_WinD2;
                        hv_Ind1 = hv_Ind + hv_WinD2;
                    }

                    if (hv_Ind0 >= hv_Rows.TupleLength()
                        || hv_Ind0 >= hv_Cols.TupleLength()
                        || hv_Ind1 >= hv_Rows.TupleLength()
                        || hv_Ind1 >= hv_Cols.TupleLength())
                        continue;

                    if (hv_Ind0 <= 0 || hv_Ind0 >= hv_Rows.Length || hv_Ind1 <= 0 || hv_Ind1 >= hv_Rows.Length
                        || hv_Ind0 >= hv_Cols.Length || hv_Ind1 >= hv_Cols.Length)
                        continue;

                    HOperatorSet.TupleAtan2((hv_Rows.TupleSelect(hv_Ind0)) - (hv_Rows.TupleSelect(
                        hv_Ind1)), (hv_Cols.TupleSelect(hv_Ind0)) - (hv_Cols.TupleSelect(hv_Ind1)),
                        out hv_Angle);
                    HOperatorSet.TupleDeg(hv_Angle, out hv_Angle_Deg);

                    HOperatorSet.TupleCos(hv_Angle + 1.5708, out hv_Cos);
                    HOperatorSet.TupleSin(hv_Angle + 1.5708, out hv_Sin);
                    hv_P1X = (hv_Cols.TupleSelect(hv_Ind)) + (hv_Cos * hv_SensLenD2);
                    hv_P1Y = (hv_Rows.TupleSelect(hv_Ind)) + (hv_Sin * hv_SensLenD2);

                    HOperatorSet.TupleCos(hv_Angle + 4.71239, out hv_Cos);
                    HOperatorSet.TupleSin(hv_Angle + 4.71239, out hv_Sin);
                    hv_P2X = (hv_Cols.TupleSelect(hv_Ind)) + (hv_Cos * hv_SensLenD2);
                    hv_P2Y = (hv_Rows.TupleSelect(hv_Ind)) + (hv_Sin * hv_SensLenD2);

                    //--------------------------------------------------------
                    if (hv_SensLen_COPY_INP_TMP == 0)
                        continue;

                    hv_dX = (hv_P1X - hv_P2X) / hv_SensLen_COPY_INP_TMP;
                    hv_dY = (hv_P1Y - hv_P2Y) / hv_SensLen_COPY_INP_TMP;
                    hv_Rows_S1 = new HTuple();
                    hv_Cols_S1 = new HTuple();
                    hv_Row1 = hv_P1Y.Clone();
                    hv_Col1 = hv_P1X.Clone();
                    HTuple end_val79 = hv_SensLen_COPY_INP_TMP - 1;
                    HTuple step_val79 = 1;
                    for (hv_Idx = 0; hv_Idx.Continue(end_val79, step_val79); hv_Idx = hv_Idx.TupleAdd(step_val79))
                    {

                        if (hv_Rows_S1 == null)
                            hv_Rows_S1 = new HTuple();
                        hv_Rows_S1[hv_Idx] = hv_Row1;
                        if (hv_Cols_S1 == null)
                            hv_Cols_S1 = new HTuple();
                        hv_Cols_S1[hv_Idx] = hv_Col1;

                        hv_Row1 = hv_Row1 - hv_dY;
                        hv_Col1 = hv_Col1 - hv_dX;
                    }


                    //gen_region_points (Region, Rows_S1, Cols_S1)
                    //disp_obj (Region, 200000)
                    //--------------------------------------------------------

                    //gen_region_line (Reg_Sens, P1Y, P1X, P2Y, P2X)
                    //get_region_contour (Reg_Sens, Rows_S1, Cols_S1)

                    //disp_obj (Reg_Sens, 200000)
                    //gen_contours_skeleton_xld (Reg_Sens, Reg_Sens_XLD, 1, 'filter')
                    //get_contour_xld (Reg_Sens_XLD, Rows_S, Cols_S)

                    //HOperatorSet.TupleConcat(hv_Rows_Sens, hv_Rows_S1, out hv_Rows_Sens);
                    //HOperatorSet.TupleConcat(hv_Cols_Sens, hv_Cols_S1, out hv_Cols_Sens);
                    HOperatorSet.TupleConcat(hv_Pointer_Sens, hv_Pointer_Pos, out hv_Pointer_Sens);


                    HOperatorSet.TupleLength(hv_Rows_S1, out hv_Length);
                    hv_Pointer_Pos = hv_Pointer_Pos + hv_Length;

                    for (int i = 0; i < hv_Length; i++)
                    {
                        cv_Rows_Sens.Add(hv_Rows_S1[i]);
                        cv_Cols_Sens.Add(hv_Cols_S1[i]);
                    }


                    hv_TotalPRC = hv_TotalPRC + 1;

                }

                HOperatorSet.TupleLength(hv_Pointer_Sens, out hv_Len_Sens);
                HOperatorSet.TupleConcat(hv_Pointer_Skel, hv_Len_Sens - 1, out hv_Pointer_Skel);


            }
            HOperatorSet.TupleConcat(hv_Pointer_Sens, hv_Pointer_Pos, out hv_Pointer_Sens);

            hv_CurrentOper = "Low Gray - Saving objects";

            hv_Rows_Sens = new HTuple(cv_Rows_Sens.ToArray());
            hv_Cols_Sens = new HTuple(cv_Cols_Sens.ToArray());

            HOperatorSet.TupleInt(hv_Rows_Sens, out hv_Rows_Sens);
            HOperatorSet.TupleInt(hv_Cols_Sens, out hv_Cols_Sens);
            if ((int)(new HTuple(hv_Type.TupleEqual("S"))) != 0)
            {
                HOperatorSet.WriteTuple(hv_Pointer_Skel, hv_path_teach_tuples + "Pointer_Skel.tup");
                HOperatorSet.WriteTuple(hv_Pointer_Sens, hv_path_teach_tuples + "Pointer_Sens.tup");
                HOperatorSet.WriteTuple(hv_Rows_Sens, hv_path_teach_tuples + "Rows_Sens.tup");
                HOperatorSet.WriteTuple(hv_Cols_Sens, hv_path_teach_tuples + "Cols_Sens.tup");
            }
            else
            {
                HOperatorSet.WriteTuple(hv_Pointer_Skel, hv_path_teach_tuples + "Pointer_Skel_P.tup");
                HOperatorSet.WriteTuple(hv_Pointer_Sens, hv_path_teach_tuples + "Pointer_Sens_P.tup");
                HOperatorSet.WriteTuple(hv_Rows_Sens, hv_path_teach_tuples + "Rows_Sens_P.tup");
                HOperatorSet.WriteTuple(hv_Cols_Sens, hv_path_teach_tuples + "Cols_Sens_P.tup");
            }

            ho_Reg_SEL1_XLD.Dispose();
            ho_Reg_SEL1.Dispose();

            return;
        }


        public void LoadPars1(HTuple hv_pathfile, HTuple hv_SetDefault, out HTuple hv_GNDamin,
            out HTuple hv_fwmhw, out HTuple hv_parop, out HTuple hv_gap, out HTuple hv_jrad,
            out HTuple hv_groundpn, out HTuple hv_groundgval, out HTuple hv_sm_factor, out HTuple hv_lookaround,
            out HTuple hv_wpl, out HTuple hv_smoothCADcontours, out HTuple hv_maxdirSP,
            out HTuple hv_wFWM, out HTuple hv_wside, out HTuple hv_wFWMS, out HTuple hv_wsideS,
            out HTuple hv_eradMSe, out HTuple hv_eradNFWMremove, out HTuple hv_elengthend,
            out HTuple hv_elengthnoend, out HTuple hv_jpadd, out HTuple hv_removeGNDfromFWM,
            out HTuple hv_cutendMSS, out HTuple hv_minWGS, out HTuple hv_minWG, out HTuple hv_useODB,
            out HTuple hv_autowidth, out HTuple hv_EPexp, out HTuple hv_elongLG, out HTuple hv_minPSklen,
            out HTuple hv_lpnum2)
        {



            // Local iconic variables 

            // Local control variables 

            HTuple hv_TeachPars = new HTuple(), hv_FileHandleIn = new HTuple();
            HTuple hv_SerializedItemHandle1 = new HTuple(), hv_TeachParsIn = new HTuple();
            HTuple hv_i = new HTuple();
            // Initialize local and output iconic variables 
            hv_GNDamin = new HTuple();
            hv_fwmhw = new HTuple();
            hv_parop = new HTuple();
            hv_gap = new HTuple();
            hv_jrad = new HTuple();
            hv_groundpn = new HTuple();
            hv_groundgval = new HTuple();
            hv_sm_factor = new HTuple();
            hv_lookaround = new HTuple();
            hv_wpl = new HTuple();
            hv_smoothCADcontours = new HTuple();
            hv_maxdirSP = new HTuple();
            hv_wFWM = new HTuple();
            hv_wside = new HTuple();
            hv_wFWMS = new HTuple();
            hv_wsideS = new HTuple();
            hv_eradMSe = new HTuple();
            hv_eradNFWMremove = new HTuple();
            hv_elengthend = new HTuple();
            hv_elengthnoend = new HTuple();
            hv_jpadd = new HTuple();
            hv_removeGNDfromFWM = new HTuple();
            hv_cutendMSS = new HTuple();
            hv_minWGS = new HTuple();
            hv_minWG = new HTuple();
            hv_useODB = new HTuple();
            hv_autowidth = new HTuple();
            hv_EPexp = new HTuple();
            hv_elongLG = new HTuple();
            hv_minPSklen = new HTuple();
            hv_lpnum2 = new HTuple();
            //*************************************
            //**** Teaching deserialization   *****
            //*************************************
            if ((int)(new HTuple(hv_SetDefault.TupleEqual(1))) != 0)
            {
                hv_GNDamin = 410000;
                hv_fwmhw = 12.5;
                hv_parop = 2.5;
                //*** for skeletons *******
                hv_gap = 1;
                hv_jrad = 10;

                hv_groundpn = 1;
                //groundpn := TeachPars[0]

                hv_groundgval = 3;
                hv_sm_factor = 7;
                hv_lookaround = 7;
                hv_wpl = 15.5;
                hv_smoothCADcontours = 1;
                hv_maxdirSP = 40;
                hv_wFWM = new HTuple();
                hv_wFWM[0] = 30;
                hv_wFWM[1] = 40;
                hv_wside = 9;
                hv_wFWMS = new HTuple();
                hv_wFWMS[0] = 20;
                hv_wFWMS[1] = 31;
                hv_wsideS = 3;
                hv_eradMSe = 9;
                hv_eradNFWMremove = 9;
                hv_elengthend = -1;
                hv_elengthnoend = -25;
                hv_jpadd = 3;
                hv_removeGNDfromFWM = 0;
                hv_cutendMSS = 25;
                hv_minWGS = 3;
                hv_minWG = 4;
                hv_useODB = 0;
                hv_autowidth = 1;
                hv_EPexp = 5;
                hv_elongLG = 10;
                hv_minPSklen = 200;
                hv_lpnum2 = 30;
            }
            else
            {
                HOperatorSet.OpenFile(hv_pathfile, "input_binary", out hv_FileHandleIn);
                HOperatorSet.FreadSerializedItem(hv_FileHandleIn, out hv_SerializedItemHandle1);
                HOperatorSet.DeserializeTuple(hv_SerializedItemHandle1, out hv_TeachParsIn);
                HOperatorSet.CloseFile(hv_FileHandleIn);
                //tuple_equal (TeachParsIn, TeachParsOut, equal)
                //stop ()

                hv_TeachPars = hv_TeachParsIn.Clone();
                hv_i = 0;
                hv_GNDamin = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_fwmhw = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_parop = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                //*** for skeletons *******
                hv_jrad = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;

                hv_gap = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;

                hv_groundpn = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_groundgval = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_sm_factor = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_lookaround = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_wpl = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_smoothCADcontours = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_maxdirSP = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_wFWM = new HTuple();
                hv_wFWM = hv_wFWM.TupleConcat(hv_TeachPars.TupleSelect(
                    hv_i));
                hv_wFWM = hv_wFWM.TupleConcat(hv_TeachPars.TupleSelect(hv_i + 1));
                hv_i = hv_i + 2;
                hv_wside = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_wFWMS = new HTuple();
                hv_wFWMS = hv_wFWMS.TupleConcat(hv_TeachPars.TupleSelect(
                    hv_i));
                hv_wFWMS = hv_wFWMS.TupleConcat(hv_TeachPars.TupleSelect(
                    hv_i + 1));
                hv_i = hv_i + 2;
                hv_wsideS = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_eradMSe = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_eradNFWMremove = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_elengthend = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_elengthnoend = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_jpadd = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_removeGNDfromFWM = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_cutendMSS = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_minWGS = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_minWG = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_useODB = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_autowidth = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_EPexp = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_elongLG = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_minPSklen = hv_TeachPars.TupleSelect(hv_i);
                hv_i = hv_i + 1;
                hv_lpnum2 = hv_TeachPars.TupleSelect(hv_i);
            }


            //**********************************

            return;
        }


        public void SavePars1(HTuple hv_pathfile, HTuple hv_GNDamin, HTuple hv_fwmhw,
            HTuple hv_parop, HTuple hv_jrad, HTuple hv_gap, HTuple hv_groundpn, HTuple hv_groundgval,
            HTuple hv_sm_factor, HTuple hv_lookaround, HTuple hv_wpl, HTuple hv_smoothCADcontours,
            HTuple hv_maxdirSP, HTuple hv_wFWM, HTuple hv_wside, HTuple hv_wFWMS, HTuple hv_wsideS,
            HTuple hv_eradMSe, HTuple hv_eradNFWMremove, HTuple hv_elengthend, HTuple hv_elengthnoend,
            HTuple hv_jpadd, HTuple hv_removeGNDfromFWM, HTuple hv_cutendMSS, HTuple hv_minWGS,
            HTuple hv_minWG, HTuple hv_useODB, HTuple hv_autowidth, HTuple hv_EPexp, HTuple hv_elongLG,
            out HTuple hv_TeachParsOut)
        {



            // Local iconic variables 

            // Local control variables 

            HTuple hv_SerializedItemHandle = null, hv_FileHandle = null;
            // Initialize local and output iconic variables 
            //*************************************
            //**** Teaching Parameters setting ****
            //*************************************
            hv_TeachParsOut = new HTuple();
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_GNDamin);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_fwmhw);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_parop);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_jrad);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_gap);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_groundpn);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_groundgval);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_sm_factor);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_lookaround);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_wpl);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_smoothCADcontours);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_maxdirSP);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_wFWM);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_wside);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_wFWMS);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_wsideS);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_eradMSe);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_eradNFWMremove);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_elengthend);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_elengthnoend);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_jpadd);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_removeGNDfromFWM);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_cutendMSS);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_minWGS);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_minWG);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_useODB);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_autowidth);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_EPexp);
            hv_TeachParsOut = hv_TeachParsOut.TupleConcat(hv_elongLG);
            HOperatorSet.SerializeTuple(hv_TeachParsOut, out hv_SerializedItemHandle);
            //fileser := pathfile+'/Teach/TeachPars.ser'
            HOperatorSet.OpenFile(hv_pathfile, "output_binary", out hv_FileHandle);
            HOperatorSet.FwriteSerializedItem(hv_FileHandle, hv_SerializedItemHandle);
            HOperatorSet.CloseFile(hv_FileHandle);

            return;
        }

    }


}
