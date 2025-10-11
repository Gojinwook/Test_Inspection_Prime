#pragma once

#ifdef USE_DLL
#include "HalconCpp.h"
#include "HDevThread.h"

using namespace HalconCpp;
using namespace std;

#ifdef CBMDLL_EXPORTS
#define CBMDLL_API __declspec(dllexport)
#else
#define CBMDLL_API __declspec(dllimport)
#endif

class CBMDLL_API CCBM;
#endif

class CCBM
{
public:
	CCBM();
	~CCBM();

public:
	void ProcessMeander(HObject ho_Im, HObject ho_ContoursGrsm, HObject ho_MeanderRects, HObject ho_Gi,
		HObject *ho_MeanderDefects, HObject *ho_Rectangle, HObject *ho_ContourGmv, HObject *ho_ContourI,// defects found as regions
		HTuple hv_thr, HTuple hv_meander, HTuple hw_absmb, HTuple hw_abssp, HTuple hv_ctype, int fsz,
		HTuple *hv_Dout); // Filtered displacement

protected:
	void DistPointToLineSegmH(HTuple hv_y, HTuple hv_x, HTuple hv_y1, HTuple hv_x1, HTuple hv_y2,
		HTuple hv_x2, HTuple *hv_distance, HTuple *hv_ym, HTuple *hv_xm, HTuple *hv_t);

	void DistPointToLineSegm(float y, float x, float y1, float x1, float y2,
		float x2, float *distance, float *ym, float *xm, float *t);

	void CircularSector(int isz, int n, int ki, int *K);

	void DistanceContours2(float *pRowI, float *pColI, int nI, float *pRowG, float *pColG, int nG,
		int isz, float *pD, int ptop);

	void DistanceContours2H(HTuple hv_RowI, HTuple hv_ColI, HTuple hv_RowG, HTuple hv_ColG,
		HTuple hv_isz, HTuple *hv_D);

	float* ConvertTupleToFloat(HTuple T);

	void FloatToTuple(float *dp, int len, HTuple &tuple);

	UINT8 Get_Gray_Val(UINT8 *pIm, int im_w, int im_h, int y, int x);

	bool Test_Hole(UINT8 *pIm, int im_w, int im_h, UINT64* rows, UINT64* cols, int count_len, int thr);

	bool Interpolation_Sub(bool reg_hole, bool direct, UINT8 gray0, UINT8 gray1, UINT8 gray2, int col_curr, int row_curr, int thr, double &xR, double &yR);

	int Region_Threshold_SubPix(HObject ho_Im, HObject ho_RegSel, int thr, float *rows_sub, float *cols_sub);

	void MoveContourH(HObject ho_Contour, HObject *ho_ContourOut, HTuple hv_mr, HTuple hv_mc);

	void Region_Threshold_SubPixH(HObject ho_RegSel, HObject ho_Im, HTuple hv_Thr, HTuple *hv_RowsSub, HTuple *hv_ColsSub);

	void ReverseContourH(HObject ho_ContourIn, HObject *ho_ContourOut);

	void MeasureDisplacementH(HObject ho_Im, HObject ho_RegionInoD, HObject ho_MeanderRects,
		HObject ho_RegionsGrMe, HObject ho_ContourGrsm, HTuple hv_meander, HTuple hv_thr,
		HTuple hv_RowGsm, HTuple hv_ColGsm, HTuple *hv_Displacement);

	void MeasureDisplacement(HObject Im, HObject ho_RegionInoD, HObject ho_MeanderRects,
		HObject ho_RegionsGrMe, HObject ho_ContourGrsm, HTuple hv_meander, HTuple hv_thr,
		HTuple hv_RowGsm, HTuple hv_ColGsm, HTuple *hv_Displacement);

	void BuildBorderContour(HObject ho_Bordersi, HObject *ho_Bordercl, HObject *ho_BorderEPs);

	void CCBM::BuildRealContour(HObject ho_Im, HObject ho_Rectangle5,
		HObject *ho_ContourOut, HObject *ho_BorderEPs, HObject *ho_RegionIR,
		HTuple hv_ctype, HTuple hv_thr);

	void SrunmedSLI(float* y, float* smo, long n, int bw_tot, int end_rule, int skip);

};

