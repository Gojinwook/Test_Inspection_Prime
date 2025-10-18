using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using HalconDotNet;

namespace Teaching_DLL
{
    public class Teaching_DLL
    {
        HDevelopExport teaching = new HDevelopExport();


        public void Start_Teaching(
            HObject ho_RegNoProc, HObject ho_Gi, HObject ho_Im,
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
            HTuple hv_useODB, HTuple hv_autowidth, HTuple hv_EPexp,
            HTuple hv_minPSklen, HTuple hv_lpnum2, // New paramtrs 2025/06/02
            HTuple hv_elongLG, out HTuple hv_nFWM,
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

            teaching.TeachingMain129(ho_RegNoProc, ho_Gi, ho_Im,
                  ho_RegionFlash, ho_RegionTrace, out ho_RegionG, out ho_RegionGS,
                  out ho_SkeletonG, out ho_SkeletonGS, out ho_WidthImageP,
                  out ho_DirImageP, out ho_WidthImageS, out ho_DirImageS,
                  out ho_SkeletonsG, out ho_ImageIR, out ho_RegionsPNi,
                  out ho_RegionsPNSi, out ho_RegionsNFWM, out ho_BoundariesNFWM,
                  out ho_SkeletonFWM, out ho_SkeletonsFWM, out ho_SkeletonsFWMcut,
                  out ho_SkeletonsFWMS, out ho_SkeletonsLG, out ho_SkeletonsFWMScut,
                  out ho_RegionsGND, out ho_RegionsEMPTY, out ho_ImDisplacement,
                  out ho_SkeletonGd, out ho_SkeletonPartsP, out ho_EPNFWMremove,
                  out ho_SkeletonPartsPnoG, out ho_ImageIR16, out ho_ImageIRS,
                  out ho_RegionMS, out ho_RegionsPNPi, out ho_RegionsmainSNoCut,
                  out ho_SkelEP, out ho_RegionCut, hv_GNDamin, hv_path,
                  hv_fwmhw, hv_parop, hv_WidthG, hv_HeightG, hv_jrad,
                  hv_gap, hv_groundpn, hv_groundgval, hv_sm_factor,
                  hv_lookaround, hv_wpl, hv_smoothCADcontours, hv_maxdirSP,
                  hv_wFWM, hv_wside, hv_wFWMS, hv_wsideS, hv_eradMSe,
                  hv_eradNFWMremove, hv_elengthend, hv_elengthnoend, hv_jpadd,
                  hv_removeGNDfromFWM, hv_cutendMSS, hv_minWGS, hv_minWG,
                  hv_useODB, hv_autowidth, hv_EPexp, hv_elongLG,
                  hv_minPSklen, hv_lpnum2, // New paramtrs 2025/06/02
                  out hv_nFWM, out hv_nFWMS, out hv_nNonWFM, out hv_WG, out hv_YC,
                  out hv_XC, out hv_Y1, out hv_X1, out hv_Y2, out hv_X2,
                  out hv_Pnum, out hv_Pnum2, out hv_PnumR, out hv_PregC,
                  out hv_PcontR, out hv_PaddrR, out hv_PaddrC, out hv_PCinRnum,
                  out hv_PuseintP, out hv_WGS, out hv_YCS, out hv_XCS,
                  out hv_Y1S, out hv_X1S, out hv_Y2S, out hv_X2S,
                  out hv_PnumS, out hv_PnumS2, out hv_PnumRS, out hv_PregCS,
                  out hv_PcontRS, out hv_PaddrRS, out hv_PaddrCS, out hv_PCinRnumS,
                  out hv_PuseintS, out hv_SequenceIRB, out hv_SequenceIRBS,
                  out hv_WGN, out hv_YCN, out hv_XCN, out hv_Y1N,
                  out hv_X1N, out hv_Y2N, out hv_X2N, out hv_PnumN,
                  out hv_PnumRN, out hv_PregCN, out hv_PcontRN, out hv_PaddrRN,
                  out hv_PaddrCN, out hv_PCinRnumN, out hv_TotalPRC, out hv_CurrentOper);
        }

    }




}
