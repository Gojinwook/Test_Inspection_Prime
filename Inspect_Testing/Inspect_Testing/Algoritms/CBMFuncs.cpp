#ifndef USE_DLL
#include "Inspect_Common.h"
#endif

#include "CBMFuncs.h"

#if !defined(USE_DLL) || defined(CBMDLL_EXPORTS)


CCBM::CCBM()
{


}

CCBM::~CCBM()
{

}

HTuple ht1, ht3, ht2, ht4, ht5, ht6;
void CCBM::ProcessMeander(HObject ho_Im, HObject ho_ContoursGrsm, HObject ho_MeanderRects, HObject ho_Gi,
	HObject *ho_MeanderDefects, HObject *ho_Rectangle, HObject *ho_ContourGmv, HObject *ho_ContourI,// defects found as regions
	HTuple hv_thr, HTuple hv_meander, HTuple hw_absmb, HTuple hw_abssp, HTuple hv_ctype, int fsz,
	HTuple *hv_Dout) // Filtered displacement
{

	HObject ho_CG, ho_Imr, ho_Imc, ho_Rectangle5, ho_ContourOut, ho_RegionIR, ho_ContourGm, ho_BorderEPs;
	HTuple hv_RowI, hv_ColI, hv_RowG, hv_ColG, hv_ai, hv_rowIc, hv_colIc, hv_poI, hv_ag, hv_rowGc, hv_colGc, hv_poG, hv_mr, hv_mc, Sign, Displacement,
		Isinside, hv_DFilt;// , hv_Dout;
	CountSeconds(&ht1);
	SelectObj(ho_ContoursGrsm, &ho_CG, hv_meander);
	SelectObj(ho_MeanderRects, &ho_Rectangle5, hv_meander);
	//GetContourXld(m_ho_CG, &m_hv_RowG, &m_hv_ColG);
	*ho_Rectangle = ho_Rectangle5;

	ReduceDomain(ho_Im, ho_Rectangle5, &ho_Imr);
	CropDomain(ho_Imr, &ho_Imc);

	BuildRealContour(ho_Imr, ho_Rectangle5, //  building I contour
		&ho_ContourOut, &ho_BorderEPs, &ho_RegionIR,
		hv_ctype, hv_thr);
	HTuple ha, hr, hc;
	AreaCenter(ho_BorderEPs, &ha, &hr, &hc);
	CountSeconds(&ht2);
	GetContourXld(ho_ContourOut, &hv_RowI, &hv_ColI);
	GetContourXld(ho_CG, &hv_RowG, &hv_ColG);

	AreaCenterXld(ho_ContourOut, &hv_ai, &hv_rowIc, &hv_colIc, &hv_poI);
	AreaCenterXld(ho_CG, &hv_ag, &hv_rowGc, &hv_colGc, &hv_poG);
	hv_mr = hv_rowIc - hv_rowGc;
	hv_mc = hv_colIc - hv_colGc;

	// Region-based center
	//HObject RegionGr, RegionGr5, RegionGr5b, ho_RegionIRb;
	//HTuple Area, RowG5, ColG5;
	//GenRegionContourXld(m_ho_CG, &RegionGr, "filled");
	//Intersection(RegionGr, ho_Rectangle5, &RegionGr5);
	//Boundary(RegionGr5, &RegionGr5b, "inner");
	//AreaCenter(RegionGr5b, &Area, &RowG5, &ColG5);

	//HTuple AreaIR, RowIR, ColIR;
	//Boundary(ho_RegionIR, &ho_RegionIRb, "inner");
	//AreaCenter(ho_RegionIRb, &AreaIR, &RowIR, &ColIR);
	//hv_mr = RowIR - RowG5;
	//hv_mc = ColIR - ColG5;

	hv_RowG = hv_RowG + hv_mr;
	hv_ColG = hv_ColG + hv_mc;
	GenContourPolygonXld(&ho_ContourGm, hv_RowG, hv_ColG);
	//GenRegionPolygonXld

	HTuple hv_nI, hv_nG, hv_D;

	float* pRowI = ConvertTupleToFloat(hv_RowI);
	float* pColI = ConvertTupleToFloat(hv_ColI);
	float* pRowG = ConvertTupleToFloat(hv_RowG);
	float* pColG = ConvertTupleToFloat(hv_ColG);
	TupleLength(hv_RowI, &hv_nI);
	TupleLength(hv_RowG, &hv_nG);
	int nI = hv_nI.I();
	int nG = hv_nG.I();
	int isz = 21;
	float *pD = (float *)malloc((nI) * sizeof(float));

	DistanceContours2(pRowI, pColI, nI, pRowG, pColG, nG,
		isz, pD, 0);

	//HTuple hv_isz = (HTuple)isz;
	//DistanceContours2H(m_hv_RowI, m_hv_ColI, m_hv_RowG, m_hv_ColG,
	//	hv_isz, &m_hv_DistIG2);
	//hv_D = m_hv_DistIG2;

	FloatToTuple(pD, nI, hv_D);
	//TupleGenConst((HTuple)nI, false, &Isinside);
	CountSeconds(&ht3);


	//HObject ho_RegionGm;
	//GenRegionContourXld(ho_ContourGm, &ho_RegionGm, "filled"); // simplified "is inside" check
	//HTuple Row, Beg, End;
	//GetRegionRuns(ho_RegionGm, &Row,&Beg, &End);*/
	//HTuple RowGA1, ColGA1, RowGA2, ColGA2;
	//SmallestRectangle1(ho_RegionGm, &RowGA1, &ColGA1, &RowGA2, &ColGA2);
	//SmallestRectangle1(ho_RegionGm, &RowGA1, &ColGA1, &RowGA2, &ColGA2);
	//HObject ImGA;
	//RegionToBin(ho_RegionGm, &ImGA, 1, 0, ColGA2 - ColGA1, RowGA2 - RowGA1);

	// Gi-based, not working yet (19.09.2025)
	//GetGrayval(ho_Gi, hv_RowI, hv_ColI, &Isinside);

	//Polygon-based.... Working, ~160 msec for 165 meanders (19.09.2025)
	HObject Pol;
	GenPolygonsXld(ho_ContourGm, &Pol, "ramer", 0.25);
	TestXldPoint(Pol, hv_RowI, hv_ColI, &Isinside);
	//Contour-based.... Working, ~470 msec for 165 meanders (19.09.2025)
	//TestXldPoint(ho_ContourGm, hv_RowI, hv_ColI, &Isinside);

	//gen_region_contour_xld(ContourGrsma, RegionGA, 'filled')
	//test_region_point(RegionGA, RowI, ColI, IsInside)

	//GenRegionContourXld(ho_ContourGm, &ho_RegionGm, "filled"); // simplified "is inside" check
	//TestRegionPoint(ho_RegionGm, hv_RowI, hv_ColI, &Isinside);
	CountSeconds(&ht4);
	Sign = Isinside * 2.0 - 1.0;
	Displacement = -Sign * hv_D;

	HTuple hv_DispElong, hv_Wch, hv_Wch1, hv_Def, hv_Def1;
	HTuple hv_sz = 31;
	// making elongation (one side)
	hv_DispElong.Clear();
	hv_DispElong.Append(Displacement.TupleSelectRange((hv_nI - hv_sz) - 0, hv_nI - 1));
	hv_DispElong.Append(Displacement);
	/*FilterFloatMedDef((*hv_DispElong), hv_sz, hv_absmb, hv_abssp, &(*hv_Wfilt), &(*hv_Wch1),
		&(*hv_Def1));*/

	float* pDisp = ConvertTupleToFloat(hv_DispElong);
	int sz = hv_sz.L();
	int nIe = nI + sz;
	float *pFilt = (float *)malloc(nIe * sizeof(float));

	SrunmedSLI(pDisp, pFilt, (long)(nIe), fsz, 0, 0);
	FloatToTuple(pFilt, (nIe), hv_DFilt);

	HTuple hv_nIe = nIe;
	hv_Wch1 = hv_DispElong - hv_DFilt;
	(*hv_Dout) = hv_Wch1;
	////////////////////////////////



	HTuple ThrMB, ThrSP, DefSP, DefMB, Def, Def1, DefA, RowD, ColD, nD, Rd;
	HObject CircleD, CircleDu;

	TupleGenConst(hv_nIe, -hw_absmb, &ThrMB);
	TupleGenConst(hv_nIe, hw_abssp, &ThrSP);
	TupleGreaterEqualElem(hv_Wch1, ThrSP, &DefSP);
	TupleLessEqualElem(hv_Wch1, ThrMB, &DefMB);
	Def1 = DefSP - DefMB;
	//TupleAbs(Def1, &DefA);

	hv_Wch.Clear();
	hv_Wch.Append(hv_Wch1.TupleSelectRange(hv_sz, hv_nI - 1));
	hv_Wch.Append(hv_Wch1.TupleSelectRange(0, hv_sz - 1));
	//hv_Def.Clear();
	//hv_Def.Append(hv_Def1.TupleSelectRange(hv_sz, hv_nI - 1));
	//hv_Def.Append(hv_Def1.TupleSelectRange(0, hv_sz - 1));
	hv_Def = Def1.TupleSelectRange(hv_sz, (hv_nI + hv_sz) - 1);
	TupleAbs(hv_Def, &DefA);
	//float* pDisp = ConvertTupleToFloat(Displacement);

	//float *pFilt = (float *)malloc((nI) * sizeof(float));
	//
	//SrunmedSLI(pDisp, pFilt, (long)nI, fsz, 0, 0);

	/*FloatToTuple(pFilt, nI, hv_DFilt);
	(*hv_Dout) = Displacement - hv_DFilt;*/
	CountSeconds(&ht5);

//	if (save)
	if (0)
	{
		WriteObject(ho_CG, "C:\\Temp1\\m_ho_CG.hobj");
		WriteObject(ho_ContourOut, "C:\\Temp1\\ho_ContourOut.hobj");

		WriteTuple(hv_D, "C:\\Temp1\\hv_D.tup");
		WriteTuple(Displacement, "C:\\Temp1\\hv_Displacement.tup");
		WriteTuple((*hv_Dout), "C:\\Temp1\\hv_Dout.tup");
		WriteTuple((hv_Wch), "C:\\Temp1\\hv_Wch.tup");

		WriteObject(ho_Imr, "C:\\Temp1\\ho_Imr");
		WriteObject(ho_Rectangle5, "C:\\Temp1\\ho_Rectangle5");

		WriteObject(ho_BorderEPs, "C:\\Temp1\\ho_BorderEPs");
	}



	/*HTuple ThrMB, ThrSP, DefSP, DefMB, Def, DefA, RowD, ColD, nD, Rd;
	HObject CircleD, CircleDu;

	TupleGenConst((HTuple)nI, -hw_absmb, &ThrMB);
	TupleGenConst((HTuple)nI, hw_abssp, &ThrSP);
	TupleGreaterEqualElem((*hv_Dout), ThrSP, &DefSP);
	TupleLessEqualElem((*hv_Dout), ThrMB, &DefMB);
	Def = DefSP - DefMB;
	TupleAbs(Def, &DefA);*/
	TupleSelectMask(hv_RowI, DefA, &RowD);
	TupleSelectMask(hv_ColI, DefA, &ColD);
	TupleLength(RowD, &nD);
	if (nD > 1)
	{
		TupleGenConst(nD, 2.5, &Rd);
		GenCircle(&CircleD, RowD, ColD, Rd);
		Union1(CircleD, &CircleDu);
		HObject RInt;
		Intersection(CircleDu, ho_BorderEPs, &RInt);
		AreaCenter(RInt, &ha, &hr, &hc);
		if (ha > 0)
			GenEmptyObj(&CircleDu);
	}
	else
		GenEmptyObj(&CircleDu);
	CountSeconds(&ht6);
	//(*ho_ContourOut) = ho_ContourH;
	//CopyObj(CircleDu, ho_MeanderDefects, 0, 1);
	(*ho_MeanderDefects) = CircleDu;
	*ho_ContourGmv = ho_ContourGm;
	*ho_ContourI = ho_ContourOut;
	free(pD);
	free(pRowI);
	free(pColI);
	free(pRowG);
	free(pColG);
	free(pFilt);
}

// Procedures 
void CCBM::DistPointToLineSegmH(HTuple hv_y, HTuple hv_x, HTuple hv_y1, HTuple hv_x1, HTuple hv_y2,
	HTuple hv_x2, HTuple *hv_distance, HTuple *hv_ym, HTuple *hv_xm, HTuple *hv_t)
{

	// Local iconic variables

	// Local control variables
	HTuple  hv_td;

	hv_td = ((hv_x2 - hv_x1)*(hv_x2 - hv_x1)) + ((hv_y2 - hv_y1)*(hv_y2 - hv_y1));
	if (0 != (hv_td != 0.0))
	{
		(*hv_t) = (((hv_x - hv_x1)*(hv_x2 - hv_x1)) + ((hv_y - hv_y1)*(hv_y2 - hv_y1))) / hv_td;
	}
	else
	{
		(*hv_t) = 111;
	}

	if (0 != ((*hv_t) >= 1.))
	{
		//distance := sqrt((x - x2)*(x - x2) + (y - y2)*(y - y2))
		(*hv_xm) = hv_x2;
		(*hv_ym) = hv_y2;
	}
	else if (0 != ((*hv_t) <= 0.))
	{
		(*hv_xm) = hv_x1;
		(*hv_ym) = hv_y1;
		//distance := sqrt((x - x1)*(x - x1) + (y - y1)*(y - y1))
	}
	else
	{
		(*hv_xm) = hv_x1 + ((*hv_t)*(hv_x2 - hv_x1));
		(*hv_ym) = hv_y1 + ((*hv_t)*(hv_y2 - hv_y1));
	}
	(*hv_distance) = (((hv_x - (*hv_xm))*(hv_x - (*hv_xm))) + ((hv_y - (*hv_ym))*(hv_y - (*hv_ym)))).TupleSqrt();
	return;
}

void CCBM::DistPointToLineSegm(float y, float x, float y1, float x1, float y2,
	float x2, float *distance, float *ym, float *xm, float *t)
{

	// Local iconic variables

	// Local control variables
	float td;

	td = ((x2 - x1)*(x2 - x1)) + ((y2 - y1)*(y2 - y1));
	if (td != 0.)
	{
		(*t) = (((x - x1)*(x2 - x1)) + ((y - y1)*(y2 - y1))) / td;
	}
	else
	{
		(*t) = 111.;
	}

	if (*t >= 1.)
	{
		//distance := sqrt((x - x2)*(x - x2) + (y - y2)*(y - y2))
		(*xm) = x2;
		(*ym) = y2;
	}
	else if (*t <= 0.)
	{
		(*xm) = x1;
		(*ym) = y1;
		//distance := sqrt((x - x1)*(x - x1) + (y - y1)*(y - y1))
	}
	else
	{
		(*xm) = x1 + ((*t)*(x2 - x1));
		(*ym) = y1 + ((*t)*(y2 - y1));
	}
	(*distance) = sqrt(((x - *xm)*(x - *xm)) + ((y - *ym)*(y - *ym)));
	return;
}

void CCBM::CircularSector(int isz, int n, int ki, int *K)
{

	// Local iconic variables

	// Local control variables
	int kb, ke;

	//***CircularSector 03.09.2025
	kb = ki - isz;
	if (kb < 0)
		kb = n + kb;
	ke = ki + isz - 1;
	if (ke >= n)
		ke = ke - n;

	if (kb <= ke)
	{
		int m = ke - kb;
		for (int j = 0; j < m; j++)
			*(K + j) = kb + j;
		//(*hv_K) = HTuple::TupleGenSequence(hv_kb, hv_ke, 1);

	}
	else
	{
		int m1 = ke + 1;
		int m2 = n - kb;
		for (int j = 0; j < m1; j++)
			*(K + j) = j;
		for (int j = 0; j < m2; j++)
			*(K + m1 + j) = kb + j;
		/*(*hv_K).Clear();
		(*hv_K).Append(HTuple::TupleGenSequence(0, hv_ke, 1));
		(*hv_K).Append(HTuple::TupleGenSequence(hv_kb, hv_n - 1, 1));*/
	}

	return;
}
void CCBM::DistanceContours2(float *pRowI, float *pColI, int nI, float *pRowG, float *pColG, int nG,
	int isz, float *pD, int ptop)
{
	//float *pColGS, *pRowGS;
	int *pK;

	float digs, digc, digr, digsmin;
	int kmin;
	int iz = isz;


	//pColGS = (float *)malloc((isz*2) * sizeof(float));
	//pRowGS = (float *)malloc((isz * 2) * sizeof(float));
	pK = (int *)malloc((nG) * sizeof(int));

	int ki = 0, ip = 0;
	float colI, rowI, dp;

	float rowGm1, colGm1, t1, d1, d, dd;
	float rowGm2, colGm2, t2, d2;

	for (int i = 0; i < nI; i++)
	{
		/*if (i == 876)
			i = i;*/
		if (i == 0)
			isz = nG / 2;
		else
			isz = iz;
		CircularSector(isz, nG, ki, pK);
		colI = *(pColI + i);
		rowI = *(pRowI + i);

		digsmin = 9999.;
		int nS = isz * 2;
		for (int k = 0; k < nS; k++)
		{
			digr = *(pRowG + *(pK + k)) - rowI;
			digc = *(pColG + *(pK + k)) - colI;
			//digc = *(pColGS+k) - colI;
			digs = sqrt(digr*digr + digc * digc);
			if (digs < digsmin)
			{
				digsmin = digs;
				kmin = k;
			}
		}

		ki = *(pK + kmin);

		float rowG = *(pRowG + ki);
		float colG = *(pColG + ki);

		if (ptop == 1)
		{
			d = sqrt((colI - colG)*(rowI - rowG));

		}
		else
		{
			int ki1 = ki - 1;
			if (ki1 < 0)
				ki1 = ki1 + nG;

			float colG1 = *(pColG + ki1);
			float rowG1 = *(pRowG + ki1);

			int ki2 = ki + 1;
			if (ki2 >= nG)
				ki2 = ki2 - nG;

			float colG2 = *(pColG + ki2);
			float rowG2 = *(pRowG + ki2);


			DistPointToLineSegm(rowI, colI, rowG, colG, rowG1, colG1, &d1,
				&rowGm1, &colGm1, &t1);
			DistPointToLineSegm(rowI, colI, rowG, colG, rowG2, colG2, &d2,
				&rowGm2, &colGm2, &t2);

			if (d1 < d2)
				d = d1;
			else d = d2;
			dd = d;

			if ((t1 < 0 && t2 < 0) || (t1 > 1 && t2 > 1))
			{
				int kim2 = ki - 2;
				if (kim2 < 0)
					kim2 = kim2 + nG;
				float colGm1 = *(pColG + kim2);
				float rowGm1 = *(pRowG + kim2);

				float dm1, tm1, dp2, tp2;
				DistPointToLineSegm(rowI, colI, rowGm1, colGm1, rowG1, colG1, &dm1,
					&rowGm1, &colGm1, &tm1);

				if ((tm1 < 0) || (tm1 > 1))
				{
					int kip2 = ki + 2;
					if (kip2 >= nG)
						kip2 = kip2 - nG;

					float colGp2 = *(pColG + kip2);
					float rowGp2 = *(pRowG + kip2);

					DistPointToLineSegm(rowI, colI, rowG2, colG2, rowGp2, colGp2, &dp2,
						&rowGp2, &colGp2, &tp2);

					dd = dp2;
				}
				else
				{
					dd = dm1;
				}
			}
			if (dd < d)
				d = dd;
		}
		ip = i - 1;
		if (ip < 0)
			ip = nI - 1;
		dp = abs(*(pColI + i) - *(pColI + ip)) + abs(*(pRowI + i) - *(pRowI + ip));
		if (dp <= 5.) // checking if it's not disconnected points
			*(pD + i) = d;
		else
		{
			*(pD + i) = 0.;
			*(pD + ip) = 0.;
		}

	}
	free(pK);

}

void CCBM::DistanceContours2H(HTuple hv_RowI, HTuple hv_ColI, HTuple hv_RowG, HTuple hv_ColG,
	HTuple hv_isz, HTuple *hv_D)
{

	// Local iconic variables
	HObject  ho_CI, ho_CG, ho_CG1, ho_CG2;

	// Local control variables
	HTuple  hv_nI, hv_nG, hv_ki, hv_i, hv_colI, hv_rowI;
	HTuple  hv_kb, hv_ke, hv_ColGs, hv_RowGs, hv_K, hv_num;
	HTuple  hv_DIGcOut, hv_DIGr, hv_DIGs, hv_minD, hv_k, hv_IndicesD;
	HTuple  hv_rowG, hv_colG, hv_ki1, hv_colG1, hv_rowG1, hv_ki2;
	HTuple  hv_colG2, hv_rowG2, hv_d1, hv_rowm1, hv_colGm1;
	HTuple  hv_t1, hv_d2, hv_rowm2, hv_colGm2, hv_t2, hv_d;
	HTuple  hv_dd, hv_kim2, hv_rowGm1, hv_dm1, hv_tm1, hv_kip2;
	HTuple  hv_colGp2, hv_rowGp2, hv_dp2, hv_tp2;

	//***** DistanceContours2 (RowI, ColI, RowG, ColG, isz, DistIG)
	//*** finding distance betweel contours given as points
	//*** 02.09.2025

	hv_nI = hv_RowI.TupleLength();
	hv_nG = hv_RowG.TupleLength();
	hv_ki = 0;
	{
		HTuple end_val7 = hv_nI - 1;
		HTuple step_val7 = 1;
		for (hv_i = 0; hv_i.Continue(end_val7, step_val7); hv_i += step_val7)
		{

			//if (i==613)
			  //stop ()
			//endif
			hv_colI = HTuple(hv_ColI[hv_i]);
			hv_rowI = HTuple(hv_RowI[hv_i]);

			hv_kb = hv_ki - hv_isz;
			if (0 != (hv_kb < 0))
			{
				hv_kb = hv_nG + hv_kb;
			}
			hv_ke = (hv_ki + hv_isz) - 1;
			if (0 != (hv_ke >= hv_nG))
			{
				hv_ke = hv_ke - hv_nG;
			}

			if (0 != (hv_kb <= hv_ke))
			{
				hv_ColGs = hv_ColG.TupleSelectRange(hv_kb, hv_ke);
				hv_RowGs = hv_RowG.TupleSelectRange(hv_kb, hv_ke);
				hv_K = HTuple::TupleGenSequence(hv_kb, hv_ke, 1);
			}
			else
			{
				hv_ColGs.Clear();
				hv_ColGs.Append(hv_ColG.TupleSelectRange(0, hv_ke));
				hv_ColGs.Append(hv_ColG.TupleSelectRange(hv_kb, hv_nG - 1));
				hv_RowGs.Clear();
				hv_RowGs.Append(hv_RowG.TupleSelectRange(0, hv_ke));
				hv_RowGs.Append(hv_RowG.TupleSelectRange(hv_kb, hv_nG - 1));
				hv_K.Clear();
				hv_K.Append(HTuple::TupleGenSequence(0, hv_ke, 1));
				hv_K.Append(HTuple::TupleGenSequence(hv_kb, hv_nG - 1, 1));
				hv_num = hv_RowGs.TupleLength();
				//DIGcs := DIGcOut[kb:nG-1]
			}
			//***** ????? ****
			//ColGs := ColG
			//RowGs := RowG
			//***** ????? ****
			hv_DIGcOut = hv_ColGs - hv_colI;
			hv_DIGr = hv_RowGs - hv_rowI;
			hv_DIGs = ((hv_DIGcOut*hv_DIGcOut) + (hv_DIGr*hv_DIGr)).TupleSqrt();
			//DIG := DIGcOut+DIGr
			//tuple_min_ind (DIGs, minD, k)
			TupleSortIndex(hv_DIGs, &hv_IndicesD);

			TupleMin(hv_DIGs, &hv_minD);
			//stop ()

			hv_k = ((const HTuple&)hv_IndicesD)[0];
			hv_ki = HTuple(hv_K[hv_k]);
			hv_rowG = HTuple(hv_RowG[hv_ki]);
			hv_colG = HTuple(hv_ColG[hv_ki]);
			//gen_region_points (PGs, RowG, ColG)

			//distance_pp (rowG, colG, rowI, colI, Distance)
			hv_ki1 = hv_ki - 1;
			if (0 != (hv_ki1 < 0))
			{
				hv_ki1 = hv_nG + hv_ki1;
			}
			hv_colG1 = HTuple(hv_ColG[hv_ki1]);
			hv_rowG1 = HTuple(hv_RowG[hv_ki1]);
			hv_ki2 = hv_ki + 1;
			if (0 != (hv_ki2 >= hv_nG))
			{
				hv_ki2 = (-hv_nG) + hv_ki2;
			}
			hv_colG2 = HTuple(hv_ColG[hv_ki2]);
			hv_rowG2 = HTuple(hv_RowG[hv_ki2]);



			DistPointToLineSegmH(hv_rowI, hv_colI, hv_rowG, hv_colG, hv_rowG1, hv_colG1, &hv_d1,
				&hv_rowm1, &hv_colGm1, &hv_t1);
			DistPointToLineSegmH(hv_rowI, hv_colI, hv_rowG, hv_colG, hv_rowG2, hv_colG2, &hv_d2,
				&hv_rowm2, &hv_colGm2, &hv_t2);
			hv_d = hv_d1.TupleMin2(hv_d2);
			hv_dd = hv_d;
			if (0 != (HTuple(HTuple(hv_t1 < 0).TupleAnd(hv_t2 < 0)).TupleOr(HTuple(hv_t1 > 1).TupleAnd(hv_t2 > 1))))
			{
				hv_kim2 = hv_ki - 2;
				if (0 != (hv_kim2 < 0))
				{
					hv_kim2 = hv_nG + hv_kim2;
				}
				hv_colGm1 = HTuple(hv_ColG[hv_kim2]);
				hv_rowGm1 = HTuple(hv_RowG[hv_kim2]);
				DistPointToLineSegmH(hv_rowI, hv_colI, hv_rowGm1, hv_colGm1, hv_rowG1, hv_colG1,
					&hv_dm1, &hv_rowGm1, &hv_colGm1, &hv_tm1);
				//dev_set_color ('dark turquoise')
				//gen_cross_contour_xld (CG2, rowGm1, colGm1, 0.07, 6)
				if (0 != (HTuple(hv_tm1 < 0).TupleOr(hv_tm1 > 1)))
				{
					hv_kip2 = hv_ki + 2;
					if (0 != (hv_kip2 >= hv_nG))
					{
						hv_kip2 = (-hv_nG) + hv_kip2;
					}
					hv_colGp2 = HTuple(hv_ColG[hv_kip2]);
					hv_rowGp2 = HTuple(hv_RowG[hv_kip2]);
					//dev_set_color ('steel blue')
					//gen_cross_contour_xld (CG2, rowGp2, colGp2, 0.23, 6)
					DistPointToLineSegmH(hv_rowI, hv_colI, hv_rowG2, hv_colG2, hv_rowGp2, hv_colGp2,
						&hv_dp2, &hv_rowGp2, &hv_colGp2, &hv_tp2);
					hv_dd = hv_dp2;
				}
				else
				{
					hv_dd = hv_dm1;
				}
			}
			hv_d = hv_d.TupleMin2(hv_dd);
			//d := DIG[k]
			//D[i] := minD
			(*hv_D)[hv_i] = hv_d;
			//distance_pp (rowI, colI, rowG, colG, d1)
		}
	}
	return;
}

float* CCBM::ConvertTupleToFloat(HTuple T)
{
	double *dptr, d;
	HTuple tl;
	float *dp;

	TupleLength(T, &tl);
	int itl = (int)tl.L();

	dptr = T.ToDArr();
	dp = (float *)malloc(itl * sizeof(float));
	for (int i = 0; i < itl; i++)
	{
		d = *(dptr + i);
		*(dp + i) = (float)d;
	}
	return dp;
}


void CCBM::FloatToTuple(float *dp, int len, HTuple &tuple)
{
	tuple = HTuple(dp, len);
	TupleReal(tuple, &tuple);

	//INT64* ptr = ConvertFloatToINT64(dp, len);
	//tuple = HTuple(ptr, len);
}


/** ---------------------------------------------------------------------------
* @Summary
*	¬±¬à¬Ý¬å¬é¬Ö¬ß¬Ú¬Ö ¬Ù¬ß¬Ñ¬é¬Ö¬ß¬Ú¬ñ ¬ñ¬â¬Ü¬à¬ã¬ä¬Ú ¬á¬Ú¬Ü¬ã¬Ö¬Ý¬ñ
*/
UINT8 CCBM::Get_Gray_Val(UINT8 *pIm, int im_w, int im_h, int y, int x)
{
	return *(pIm + y * im_w + x);
}
/** ---------------------------------------------------------------------------
* @Summary
*	¬±¬â¬à¬Ó¬Ö¬â¬Ü¬Ñ, ¬ñ¬Ó¬Ý¬ñ¬Ö¬ä¬ã¬ñ ¬Ý¬Ú ¬â¬Ö¬Ô¬Ú¬à¬ß ¬Õ¬í¬â¬Ü¬à¬Û
*/
bool CCBM::Test_Hole(UINT8 *pIm, int im_w, int im_h, UINT64* rows, UINT64* cols, int count_len, int thr)
{
	for (int pos = 0; pos < count_len; pos++)
	{
		UINT8 gray0 = Get_Gray_Val(pIm, im_w, im_h, (int)rows[pos], (int)cols[pos]);
		if (gray0 < thr)
		{
			return true;
		}
		else if (gray0 > thr)
		{
			return false;
		}
	}
	return false;
}

/** ---------------------------------------------------------------------------
* @Summary
*	¬ª¬ß¬ä¬Ö¬â¬á¬à¬Ý¬ñ¬è¬Ú¬ñ. ¬°¬á¬â¬Ö¬Õ¬Ö¬Ý¬Ö¬ß¬Ú¬Ö ¬ã¬å¬Ò¬á¬Ú¬Ü¬ã¬Ö¬Ý¬î¬ß¬à¬Û ¬Ü¬à¬à¬â¬Õ¬Ú¬ß¬Ñ¬ä¬í.
*/
bool CCBM::Interpolation_Sub(bool reg_hole, bool direct, UINT8 gray0, UINT8 gray1, UINT8 gray2, int col_curr, int row_curr, int thr, double &xR, double &yR)
{
	int x0 = -1, x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	// * ¬ª¬Õ¬Ö¬Þ ¬Ó¬ß¬Ú¬Ù.¬³¬â¬Ñ¬Ó¬ß¬Ú¬Ó¬Ñ¬Ö¬Þ ¬ã ¬á¬à¬Ý¬Ö¬Þ ¬ã¬á¬â¬Ñ¬Ó¬Ñ
	// * ¬ª¬Õ¬Ö¬Þ ¬Ó¬Ý¬Ö¬Ó¬à. ¬³¬â¬Ñ¬Ó¬ß¬Ú¬Ó¬Ñ¬Ö¬Þ ¬ã ¬á¬à¬Ý¬Ö¬Þ ¬ã¬ß¬Ú¬Ù¬å
	if (direct && ((reg_hole && gray1 > thr) || (!reg_hole && gray1 < thr)))
	{
		x0 = thr;
		x1 = gray0;
		x2 = gray1;
		y1 = col_curr;
	}
	// * ¬ª¬Õ¬Ö¬Þ ¬Ó¬Ó¬Ö¬â¬ç. ¬³¬â¬Ñ¬Ó¬ß¬Ú¬Ó¬Ñ¬Ö¬Þ ¬ã ¬á¬à¬Ý¬Ö¬Þ ¬ã¬Ý¬Ö¬Ó¬Ñ
	// * ¬ª¬Õ¬Ö¬Þ ¬Ó¬á¬â¬Ñ¬Ó¬à. ¬³¬â¬Ñ¬Ó¬ß¬Ú¬Ó¬Ñ¬Ö¬Þ ¬ã ¬á¬à¬Ý¬Ö¬Þ ¬ã¬Ó¬Ö¬â¬ç¬å
	else if (!direct && ((reg_hole && gray2 > thr) || (!reg_hole && gray2 < thr)))
	{
		x0 = thr;
		x1 = gray2;
		x2 = gray0;
		y1 = col_curr - 1;
	}

	if (x0 >= 0 && x2 != x1)
	{
		xR = (double)y1 + (double)(x0 - x1) / (double)(x2 - x1);
		yR = row_curr;
		return true;
	}
	else
	{
		return false;
	}
}
/* @Summary
*	¬¶¬à¬â¬Þ¬Ú¬â¬à¬Ó¬Ñ¬ß¬Ú¬Ö ¬ã¬å¬Ò¬á¬Ú¬Ü¬ã¬Ö¬Ý¬î¬ß¬í¬ç ¬Ü¬à¬à¬â¬Õ¬Ú¬ß¬Ñ¬ä ¬Õ¬Ý¬ñ ¬â¬Ö¬Ô¬Ú¬à¬ß¬Ñ
*/
int CCBM::Region_Threshold_SubPix(HObject ho_Im, HObject ho_RegSel, int thr, float *rows_sub, float *cols_sub)
{
	HTuple tl, h, w, t, hiptr;
	GetImagePointer1(ho_Im, &hiptr, &t, &w, &h);
	UINT8 *pIm = (UINT8 *)(Hlong *)hiptr.L();
	int im_w = (int)w.L();
	int im_h = (int)h.L();


	HTuple hv_Rows, hv_Cols, hv_Cont_Len;
	UINT8 gray0, gray1, gray2;
	int x0 = 0, x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	double xR1, yR1, xR2, yR2;
	int pos_sub = 0;

	GetRegionContour(ho_RegSel, &hv_Rows, &hv_Cols);
	UINT64* rows = (UINT64*)hv_Rows.LArr();
	UINT64* cols = (UINT64*)hv_Cols.LArr();

	TupleLength(hv_Rows, &hv_Cont_Len);
	int count_len = (int)hv_Cont_Len.LArr()[0];

	// ¬´¬Ö¬ã¬ä ¬ß¬Ñ ¬Õ¬í¬â¬Ü¬å
	bool reg_hole = Test_Hole(pIm, im_w, im_h, rows, cols, count_len, thr);

	int pos;
	for (pos = 0; pos < count_len; pos++)
	{
		// * ¬°¬Ü¬â¬å¬Ø¬Ö¬ß¬Ú¬Ö ¬Ú¬ã¬ã¬Ý¬Ö¬Õ¬å¬Ö¬Þ¬à¬Ô¬à ¬á¬Ú¬Ü¬ã¬Ö¬Ý¬ñ
		int pos_next = pos + 1;
		if (pos_next > (count_len - 1))
		{
			pos_next = 0;
		}
		int pos_prev = pos - 1;
		if (pos_prev < 0)
		{
			pos_prev = count_len - 1;
		}
		int row_prev = (int)rows[pos_prev];
		int col_prev = (int)cols[pos_prev];
		int row_next = (int)rows[pos_next];
		int col_next = (int)cols[pos_next];
		int row_curr = (int)rows[pos];
		int col_curr = (int)cols[pos];

		// ¬©¬Ñ ¬á¬â¬Ö¬Õ¬Ö¬Ý¬Ñ¬Þ¬Ú ¬Ú¬Ù¬à¬Ò¬â¬Ñ¬Ø¬Ö¬ß¬Ú¬ñ
		if (row_curr >= im_h - 1 || col_curr >= im_w - 1 || row_curr == 0 || col_curr == 0)
		{
			continue;
		}

		// * ¬¯¬Ñ¬á¬â¬Ñ¬Ó¬Ý¬Ö¬ß¬Ú¬Ö
		bool f_Down, f_Left;
		f_Down = row_curr > row_prev || row_curr < row_next;
		f_Left = col_curr < col_prev || col_curr > col_next;

		gray0 = Get_Gray_Val(pIm, im_w, im_h, (int)rows[pos], (int)cols[pos]);

		// *-- Cols ----------------------------------------------------------------------
		gray1 = Get_Gray_Val(pIm, im_w, im_h, row_curr, col_curr + 1);
		gray2 = Get_Gray_Val(pIm, im_w, im_h, row_curr, col_curr - 1);
		bool f_Cols_Available = Interpolation_Sub(reg_hole, f_Down, gray0, gray1, gray2, col_curr, row_curr, thr, xR1, yR1);

		// *--- Rows ----------------------------------------------------------------------
		gray1 = Get_Gray_Val(pIm, im_w, im_h, row_curr + 1, col_curr);
		gray2 = Get_Gray_Val(pIm, im_w, im_h, row_curr - 1, col_curr);
		bool f_Rows_Available = Interpolation_Sub(reg_hole, f_Left, gray0, gray1, gray2, row_curr, col_curr, thr, yR2, xR2);

		// *--- ¬³¬à¬Ò¬Ú¬â¬Ñ¬Ö¬Þ ¬â¬Ö¬Ù¬å¬Ý¬î¬ä¬Ñ¬ä ------------------------------------------------------
		if (f_Cols_Available && f_Rows_Available)
		{
			if ((xR1 > xR2 && yR1 > yR2) || (xR1 <= xR2 && yR1 < yR2))
			{
				rows_sub[pos_sub] = yR2;
				cols_sub[pos_sub] = xR2;
				pos_sub += 1;

				rows_sub[pos_sub] = yR1;
				cols_sub[pos_sub] = xR1;
				pos_sub += 1;
			}
			else
			{
				rows_sub[pos_sub] = yR1;
				cols_sub[pos_sub] = xR1;
				pos_sub += 1;

				rows_sub[pos_sub] = yR2;
				cols_sub[pos_sub] = xR2;
				pos_sub += 1;
			}
		}
		else if (f_Cols_Available)
		{
			rows_sub[pos_sub] = yR1;
			cols_sub[pos_sub] = xR1;
			pos_sub += 1;
		}
		else if (f_Rows_Available)
		{
			rows_sub[pos_sub] = yR2;
			cols_sub[pos_sub] = xR2;
			pos_sub += 1;
		}
	}

	return pos_sub;
}


void CCBM::MoveContourH(HObject ho_Contour, HObject *ho_ContourOut, HTuple hv_mr, HTuple hv_mc)
{

	// Local iconic variables

	// Local control variables
	HTuple  hv_Row, hv_Col, hv_Rowout, hv_Colout;

	GetContourXld(ho_Contour, &hv_Row, &hv_Col);
	hv_Rowout = hv_Row + hv_mr;
	hv_Colout = hv_Col + hv_mc;
	GenContourPolygonXld(&(*ho_ContourOut), hv_Rowout, hv_Colout);
	return;
}

void CCBM::Region_Threshold_SubPixH(HObject ho_RegSel, HObject ho_Im, HTuple hv_Thr, HTuple *hv_RowsSub,
	HTuple *hv_ColsSub)
{

	// Local iconic variables
	HObject  ho_Cross;

	// Local control variables
	HTuple  hv_Pos_Sub, hv_Im_H, hv_Im_W, hv_Rows;
	HTuple  hv_Cols, hv_Cont_Len, hv_Pos, hv_Gray0, hv_Hole;
	HTuple  hv_Pos_Next, hv_Pos_Prev, hv_Row_Prev, hv_Col_Prev;
	HTuple  hv_Row_Next, hv_Col_Next, hv_Row_Curr, hv_Col_Curr;
	HTuple  hv_Down, hv_Left, hv_Gray1, hv_Gray2, hv_X0, hv_X1;
	HTuple  hv_X2, hv_Xr1, hv_Y1, hv_Y2, hv_Yr1, hv_Yr2, hv_Xr2;

	(*hv_RowsSub) = HTuple();
	(*hv_ColsSub) = HTuple();
	hv_Pos_Sub = 0;

	GetImageSize(ho_Im, &hv_Im_H, &hv_Im_W);
	GetRegionContour(ho_RegSel, &hv_Rows, &hv_Cols);
	TupleLength(hv_Rows, &hv_Cont_Len);

	//???? ?? ?????
	{
		HTuple end_val9 = hv_Cont_Len - 1;
		HTuple step_val9 = 1;
		for (hv_Pos = 0; hv_Pos.Continue(end_val9, step_val9); hv_Pos += step_val9)
		{
			GetGrayval(ho_Im, HTuple(hv_Rows[hv_Pos]), HTuple(hv_Cols[hv_Pos]), &hv_Gray0);

			if (0 != (hv_Gray0 < hv_Thr))
			{
				hv_Hole = 1;
				break;
			}
			else if (0 != (hv_Gray0 > hv_Thr))
			{
				hv_Hole = 0;
				break;
			}

		}
	}


	{
		HTuple end_val23 = hv_Cont_Len - 1;
		HTuple step_val23 = 1;
		for (hv_Pos = 0; hv_Pos.Continue(end_val23, step_val23); hv_Pos += step_val23)
		{
			GetGrayval(ho_Im, HTuple(hv_Rows[hv_Pos]), HTuple(hv_Cols[hv_Pos]), &hv_Gray0);

			hv_Pos_Next = hv_Pos + 1;
			if (0 != (hv_Pos_Next > (hv_Cont_Len - 1)))
			{
				hv_Pos_Next = 0;
			}
			hv_Pos_Prev = hv_Pos - 1;
			if (0 != (hv_Pos_Prev < 0))
			{
				hv_Pos_Prev = hv_Cont_Len - 1;
			}
			hv_Row_Prev = HTuple(hv_Rows[hv_Pos_Prev]);
			hv_Col_Prev = HTuple(hv_Cols[hv_Pos_Prev]);
			hv_Row_Next = HTuple(hv_Rows[hv_Pos_Next]);
			hv_Col_Next = HTuple(hv_Cols[hv_Pos_Next]);
			hv_Row_Curr = HTuple(hv_Rows[hv_Pos]);
			hv_Col_Curr = HTuple(hv_Cols[hv_Pos]);

			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "red");
			GenCrossContourXld(&ho_Cross, hv_Row_Curr, hv_Col_Curr, 1, 0);


			if (0 != (HTuple(HTuple(HTuple(hv_Row_Curr >= (hv_Im_H - 1)).TupleOr(hv_Col_Curr >= (hv_Im_W - 1))).TupleOr(hv_Row_Curr == 0)).TupleOr(hv_Col_Curr == 0)))
			{
				continue;
			}

			//???????????
			if (0 != (HTuple(hv_Row_Curr > hv_Row_Prev).TupleOr(hv_Row_Curr < hv_Row_Next)))
			{
				hv_Down = 1;
			}
			else
			{
				hv_Down = 0;
			}

			if (0 != (HTuple(hv_Col_Curr < hv_Col_Prev).TupleOr(hv_Col_Curr > hv_Col_Next)))
			{
				hv_Left = 1;
			}
			else
			{
				hv_Left = 0;
			}

			//-- Cols -------------------------------------------
			GetGrayval(ho_Im, hv_Row_Curr, hv_Col_Curr + 1, &hv_Gray1);
			GetGrayval(ho_Im, hv_Row_Curr, hv_Col_Curr - 1, &hv_Gray2);
			hv_X0 = -1;
			hv_X1 = 0;
			hv_X2 = 0;
			hv_Xr1 = -1;

			//???? ????. ?????????? ? ????? ??????
			if (0 != (hv_Down.TupleAnd(HTuple(HTuple(hv_Hole == 1).TupleAnd(hv_Gray1 > hv_Thr)).TupleOr(HTuple(hv_Hole == 0).TupleAnd(hv_Gray1 < hv_Thr)))))
			{
				hv_X0 = hv_Thr;
				hv_X1 = hv_Gray0;
				hv_X2 = hv_Gray1;
				hv_Y1 = HTuple(hv_Cols[hv_Pos]);
				hv_Y2 = HTuple(hv_Cols[hv_Pos]) + 1;
				//???? ?????. ?????????? ? ????? ?????
			}
			else if (0 != (HTuple(hv_Down.TupleNot()).TupleAnd(HTuple(HTuple(hv_Hole == 1).TupleAnd(hv_Gray2 > hv_Thr)).TupleOr(HTuple(hv_Hole == 0).TupleAnd(hv_Gray2 < hv_Thr)))))
			{
				hv_X0 = hv_Thr;
				hv_X1 = hv_Gray2;
				hv_X2 = hv_Gray0;
				hv_Y1 = HTuple(hv_Cols[hv_Pos]) - 1;
				hv_Y2 = HTuple(hv_Cols[hv_Pos]);
			}

			if (0 != (HTuple(hv_X0 >= 0).TupleAnd(hv_X2 != hv_X1)))
			{
				hv_Xr1 = (hv_Y1.TupleReal()) + ((((hv_X0 - hv_X1).TupleReal())*((hv_Y2 - hv_Y1).TupleReal())) / ((hv_X2 - hv_X1).TupleReal()));
				hv_Yr1 = HTuple(hv_Rows[hv_Pos]);

				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "green");
				GenCrossContourXld(&ho_Cross, hv_Yr1, hv_Xr1, 0.5, 0);
			}

			//--- Rows -------------------------------------------
			GetGrayval(ho_Im, hv_Row_Curr + 1, hv_Col_Curr, &hv_Gray1);
			GetGrayval(ho_Im, hv_Row_Curr - 1, hv_Col_Curr, &hv_Gray2);
			hv_X0 = -1;
			hv_Yr2 = -1;

			//???? ?????. ?????????? ? ????? ?????
			if (0 != (hv_Left.TupleAnd(HTuple(HTuple(hv_Hole == 1).TupleAnd(hv_Gray1 > hv_Thr)).TupleOr(HTuple(hv_Hole == 0).TupleAnd(hv_Gray1 < hv_Thr)))))
			{
				hv_X0 = hv_Thr;
				hv_X1 = hv_Gray0;
				hv_X2 = hv_Gray1;
				hv_Y1 = HTuple(hv_Rows[hv_Pos]);
				hv_Y2 = HTuple(hv_Rows[hv_Pos]) + 1;
				//???? ??????. ?????????? ? ????? ??????
			}
			else if (0 != (HTuple(hv_Left.TupleNot()).TupleAnd(HTuple(HTuple(hv_Hole == 1).TupleAnd(hv_Gray2 > hv_Thr)).TupleOr(HTuple(hv_Hole == 0).TupleAnd(hv_Gray2 < hv_Thr)))))
			{
				hv_X0 = hv_Thr;
				hv_X1 = hv_Gray2;
				hv_X2 = hv_Gray0;
				hv_Y1 = HTuple(hv_Rows[hv_Pos]) - 1;
				hv_Y2 = HTuple(hv_Rows[hv_Pos]);
			}

			if (0 != (HTuple(hv_X0 >= 0).TupleAnd(hv_X2 != hv_X1)))
			{
				hv_Yr2 = (hv_Y1.TupleReal()) + ((((hv_X0 - hv_X1).TupleReal())*((hv_Y2 - hv_Y1).TupleReal())) / ((hv_X2 - hv_X1).TupleReal()));
				hv_Xr2 = HTuple(hv_Cols[hv_Pos]);

				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "red");
				GenCrossContourXld(&ho_Cross, hv_Yr2, hv_Xr2, 0.5, 0);
			}



			if (0 != (HTuple(hv_Xr1 >= 0).TupleAnd(hv_Yr2 >= 0)))
			{
				if (0 != (hv_Xr1 > hv_Xr2))
				{
					if (0 != (hv_Yr1 > hv_Yr2))
					{
						//Yr_Avg := (Yr2 + Yr1) / 2
						//Xr_Avg := (Xr2 + Xr1) / 2

						(*hv_RowsSub)[hv_Pos_Sub] = hv_Yr2;
						(*hv_ColsSub)[hv_Pos_Sub] = hv_Xr2;
						hv_Pos_Sub += 1;

						(*hv_RowsSub)[hv_Pos_Sub] = hv_Yr1;
						(*hv_ColsSub)[hv_Pos_Sub] = hv_Xr1;
						hv_Pos_Sub += 1;
					}
					else
					{
						(*hv_RowsSub)[hv_Pos_Sub] = hv_Yr1;
						(*hv_ColsSub)[hv_Pos_Sub] = hv_Xr1;
						hv_Pos_Sub += 1;

						(*hv_RowsSub)[hv_Pos_Sub] = hv_Yr2;
						(*hv_ColsSub)[hv_Pos_Sub] = hv_Xr2;
						hv_Pos_Sub += 1;
					}
				}
				else
				{
					if (0 != (hv_Yr1 < hv_Yr2))
					{
						(*hv_RowsSub)[hv_Pos_Sub] = hv_Yr2;
						(*hv_ColsSub)[hv_Pos_Sub] = hv_Xr2;
						hv_Pos_Sub += 1;

						(*hv_RowsSub)[hv_Pos_Sub] = hv_Yr1;
						(*hv_ColsSub)[hv_Pos_Sub] = hv_Xr1;
						hv_Pos_Sub += 1;
					}
					else
					{
						(*hv_RowsSub)[hv_Pos_Sub] = hv_Yr1;
						(*hv_ColsSub)[hv_Pos_Sub] = hv_Xr1;
						hv_Pos_Sub += 1;

						(*hv_RowsSub)[hv_Pos_Sub] = hv_Yr2;
						(*hv_ColsSub)[hv_Pos_Sub] = hv_Xr2;
						hv_Pos_Sub += 1;
					}
				}
			}
			else if (0 != (hv_Xr1 >= 0))
			{
				(*hv_RowsSub)[hv_Pos_Sub] = hv_Yr1;
				(*hv_ColsSub)[hv_Pos_Sub] = hv_Xr1;
				hv_Pos_Sub += 1;
			}
			else if (0 != (hv_Yr2 >= 0))
			{
				(*hv_RowsSub)[hv_Pos_Sub] = hv_Yr2;
				(*hv_ColsSub)[hv_Pos_Sub] = hv_Xr2;
				hv_Pos_Sub += 1;
			}

		}
	}
	return;
}

void CCBM::ReverseContourH(HObject ho_ContourIn, HObject *ho_ContourOut)
{

	// Local iconic variables
	HObject  ho_ContourGrsmi;

	// Local control variables
	HTuple  hv_RowGrsm, hv_ColGrsm, hv_RowGrsmi, hv_ColGrsmi;

	(*ho_ContourOut) = ho_ContourIn;
	GetContourXld((*ho_ContourOut), &hv_RowGrsm, &hv_ColGrsm);
	TupleInverse(hv_RowGrsm, &hv_RowGrsmi);
	TupleInverse(hv_ColGrsm, &hv_ColGrsmi);
	GenContourPolygonXld(&ho_ContourGrsmi, hv_RowGrsmi, hv_ColGrsmi);
	//area_center_xld (ContourGrsmi, AreaCGR, Row9, Column9, PointOrderCGr)
	(*ho_ContourOut) = ho_ContourGrsmi;
	return;
}

void CCBM::MeasureDisplacementH(HObject ho_Im, HObject ho_RegionInoD, HObject ho_MeanderRects,
	HObject ho_RegionsGrMe, HObject ho_ContourGrsm, HTuple hv_meander, HTuple hv_thr,
	HTuple hv_RowGsm, HTuple hv_ColGsm, HTuple *hv_Displacement)
{

	// Local iconic variables
	HObject  ho_MeanderRect, ho_RegionIR, ho_RegionsIR;
	HObject  ho_ContourIK, ho_ContourGrsma;

	// Local control variables
	HTuple  hv_AreaIR, hv_Row, hv_Column, hv_Indices;
	HTuple  hv_Inverted, hv_RowsSubI, hv_ColsSubI, hv_AreaIK;
	HTuple  hv_RowIK, hv_ColIK, hv_PointOrderIK, hv_mr, hv_mc;
	HTuple  hv_RowI, hv_ColI, hv_RowG, hv_ColG, hv_nI, hv_isz;
	HTuple  hv_DistIG, hv_IsInside, hv_Sign;

	//MeasureDisplacement() - 07.09.2925

	SelectObj(ho_MeanderRects, &ho_MeanderRect, hv_meander);
	//union1 (RegionsGrMe, RegionGrMe)
	Intersection(ho_RegionInoD, ho_MeanderRect, &ho_RegionIR);
	Connection(ho_RegionIR, &ho_RegionsIR);
	AreaCenter(ho_RegionsIR, &hv_AreaIR, &hv_Row, &hv_Column);
	TupleSortIndex(hv_AreaIR, &hv_Indices);
	TupleInverse(hv_Indices, &hv_Inverted);
	SelectObj(ho_RegionsIR, &ho_RegionIR, HTuple(hv_Inverted[0]) + 1);

	Region_Threshold_SubPixH(ho_RegionIR, ho_Im, hv_thr, &hv_RowsSubI, &hv_ColsSubI);
	GenContourPolygonXld(&ho_ContourIK, hv_RowsSubI, hv_ColsSubI);
	AreaCenterXld(ho_ContourIK, &hv_AreaIK, &hv_RowIK, &hv_ColIK, &hv_PointOrderIK);
	if (0 != (hv_PointOrderIK == HTuple("negative")))
	{
		ReverseContourH(ho_ContourIK, &ho_ContourIK);
		//area_center_xld (ContourIK, Area, RowBcl, ColBcl, PointOrderB1)
		//nrev := nrev+1
	}

	//RowGsm := RowGsm[meander-1]
	//ColGsm := ColGsm[meander-1]

	hv_mr = hv_RowIK - hv_RowGsm;
	hv_mc = hv_ColIK - hv_ColGsm;

	//* moving the contour
	//mr := mr+2
	MoveContourH(ho_ContourGrsm, &ho_ContourGrsma, hv_mr, hv_mc);

	GetContourXld(ho_ContourIK, &hv_RowI, &hv_ColI);
	GetContourXld(ho_ContourGrsma, &hv_RowG, &hv_ColG);
	TupleLength(hv_RowI, &hv_nI);
	hv_isz = 21;
	DistanceContours2H(hv_RowI, hv_ColI, hv_RowG, hv_ColG, hv_isz, &hv_DistIG);

	//stop ()
	//** signed displacement
	TestXldPoint(ho_ContourGrsma, hv_RowI, hv_ColI, &hv_IsInside);

	//gen_contour_polygon_xld (Line, [0,RowI], [0,ColI])
	//intersection_contours_xld (Line, ContourIK, 'all', Row1, Column1, IsOverlapping)
	//intersection_line_contour_xld (ContourIK, 0, RowI, 0, ColI, Row1, Column1, IsOverlapping)
	hv_Sign = (hv_IsInside*2.0) - 1.0;
	(*hv_Displacement) = (-hv_Sign)*hv_DistIG;
	return;
}

void CCBM::MeasureDisplacement(HObject Im, HObject ho_RegionInoD, HObject ho_MeanderRects,
	HObject ho_RegionsGrMe, HObject ho_ContourGrsm, HTuple hv_meander, HTuple hv_thr,
	HTuple hv_RowGsm, HTuple hv_ColGsm, HTuple *hv_Displacement)
{

	// Local iconic variables
	HObject  ho_MeanderRect, ho_RegionIR, ho_RegionsIR;
	HObject  ho_ContourIK, ho_ContourGrsma;

	// Local control variables
	HTuple  hv_AreaIR, hv_Row, hv_Column, hv_Indices;
	HTuple  hv_Inverted, hv_RowsSubI, hv_ColsSubI, hv_AreaIK;
	HTuple  hv_RowIK, hv_ColIK, hv_PointOrderIK, hv_mr, hv_mc;
	HTuple  hv_RowI, hv_ColI, hv_RowG, hv_ColG, hv_nI, hv_isz;
	HTuple  hv_DistIG, hv_IsInside, hv_Sign;

	//MeasureDisplacement() - 07.09.2925

	SelectObj(ho_MeanderRects, &ho_MeanderRect, hv_meander);
	//union1 (RegionsGrMe, RegionGrMe)
	Intersection(ho_RegionInoD, ho_MeanderRect, &ho_RegionIR);
	Connection(ho_RegionIR, &ho_RegionsIR);
	AreaCenter(ho_RegionsIR, &hv_AreaIR, &hv_Row, &hv_Column);
	TupleSortIndex(hv_AreaIR, &hv_Indices);
	TupleInverse(hv_Indices, &hv_Inverted);
	SelectObj(ho_RegionsIR, &ho_RegionIR, HTuple(hv_Inverted[0]) + 1);
	float * pRowsSubI, *pColsSubI;
	pRowsSubI = (float *)malloc(5000 * sizeof(float));
	pColsSubI = (float *)malloc(5000 * sizeof(float));
	Region_Threshold_SubPix(Im, ho_RegionIR, hv_thr, pRowsSubI, pColsSubI);

	GenContourPolygonXld(&ho_ContourIK, hv_RowsSubI, hv_ColsSubI);
	AreaCenterXld(ho_ContourIK, &hv_AreaIK, &hv_RowIK, &hv_ColIK, &hv_PointOrderIK);
	if (0 != (hv_PointOrderIK == HTuple("negative")))
	{
		ReverseContourH(ho_ContourIK, &ho_ContourIK);
		//area_center_xld (ContourIK, Area, RowBcl, ColBcl, PointOrderB1)
		//nrev := nrev+1
	}

	//RowGsm := RowGsm[meander-1]
	//ColGsm := ColGsm[meander-1]

	hv_mr = hv_RowIK - hv_RowGsm;
	hv_mc = hv_ColIK - hv_ColGsm;

	//* moving the contour
	//mr := mr+2
	MoveContourH(ho_ContourGrsm, &ho_ContourGrsma, hv_mr, hv_mc);

	GetContourXld(ho_ContourIK, &hv_RowI, &hv_ColI);
	GetContourXld(ho_ContourGrsma, &hv_RowG, &hv_ColG);
	TupleLength(hv_RowI, &hv_nI);
	hv_isz = 21;
	DistanceContours2H(hv_RowI, hv_ColI, hv_RowG, hv_ColG, hv_isz, &hv_DistIG);

	//stop ()
	//** signed displacement
	TestXldPoint(ho_ContourGrsma, hv_RowI, hv_ColI, &hv_IsInside);

	//gen_contour_polygon_xld (Line, [0,RowI], [0,ColI])
	//intersection_contours_xld (Line, ContourIK, 'all', Row1, Column1, IsOverlapping)
	//intersection_line_contour_xld (ContourIK, 0, RowI, 0, ColI, Row1, Column1, IsOverlapping)
	hv_Sign = (hv_IsInside*2.0) - 1.0;
	(*hv_Displacement) = (-hv_Sign)*hv_DistIG;

	free(pRowsSubI);
	free(pColsSubI);
	return;
}
void CCBM::BuildBorderContour(HObject ho_Bordersi, HObject *ho_Bordercl, HObject *ho_BorderEPs)
{

	// Local iconic variables
	HObject  ho_Borders, ho_Border, HB, HBEp;

	// Local control variables
	HTuple  hv_nB, hv_LengthBI, hv_Indices, hv_Inverted;
	HTuple  hv_imax1, hv_imax2, hv_RowB, hv_ColB, hRB, hCB, hnB, hi, hl;

	CountObj(ho_Bordersi, &hv_nB);
	LengthXld(ho_Bordersi, &hv_LengthBI);
	TupleSortIndex(hv_LengthBI, &hv_Indices);
	TupleInverse(hv_Indices, &hv_Inverted);
	hv_imax1 = HTuple(hv_Inverted[0]) + 1;
	if (0 != (hv_nB > 1))
	{
		hv_imax2 = HTuple(hv_Inverted[1]) + 1;
		//* check if there's only one boundary contour
		if (0 != (HTuple(hv_LengthBI[hv_imax2 - 1]) < (HTuple(hv_LengthBI[hv_imax1 - 1]) / 2)))
		{
			hv_imax2 = HTuple();
		}
		SelectObj(ho_Bordersi, &ho_Borders, hv_imax1.TupleConcat(hv_imax2));
	}
	else
	{
		ho_Borders = ho_Bordersi;
	}
	//WriteObject(ho_Borders, "C:\\Temp1\\ho_Borders");
	UnionAdjacentContoursXld(ho_Borders, &ho_Border, 35, 35, "attr_forget");
	CountObj(ho_Borders, &hnB);
	hv_RowB.Clear();
	hv_ColB.Clear();
	for (hi = 1; hi <= hnB; hi = hi + 1)
	{
		SelectObj(ho_Borders, &HB, hi);
		GetContourXld(HB, &hRB, &hCB);
		TupleLength(hRB, &hl);
		TupleConcat(hv_RowB, hRB[0], &hv_RowB);
		TupleConcat(hv_ColB, hCB[0], &hv_ColB);
		TupleConcat(hv_RowB, hRB[hl - 1], &hv_RowB);
		TupleConcat(hv_ColB, hCB[hl - 1], &hv_ColB);
	}
	GenRegionPoints(&HBEp, hv_RowB, hv_ColB);
	*ho_BorderEPs = HBEp;
	//test_closed_xld (Border, IsClosed)
	CloseContoursXld(ho_Border, &(*ho_Bordercl));
	return;
}

void CCBM::BuildRealContour(HObject ho_Im, HObject ho_Rectangle5,
	HObject *ho_ContourOut, HObject *ho_BorderEPs, HObject *ho_RegionIR,
	HTuple hv_ctype, HTuple hv_thr)
{

	// Local iconic variables
	HObject  ho_Bordersi, ho_ContourH, ho_RegionIr;
	HObject  ho_RegionsIR, ho_ContourIK;// , ho_BorderEPs;

	// Local control variables
	HTuple  hv_RowsSubI, hv_ColsSubI, hv_AreaI, hv_RowIK;
	HTuple  hv_ColIK, hv_PointOrderG, hv_AreaIR, hv_Row, hv_Column;
	HTuple  hv_Indices, hv_Inverted;

	float * pRowsSubI, *pColsSubI;


	//** BuildRealContour 11.09.2025
	if (0 != (hv_ctype == 1))
	{
		//* H
		//reduce_domain (Im, Rectangle5, ImageReduced)
		ThresholdSubPix(ho_Im, &ho_Bordersi, hv_thr);
		BuildBorderContour(ho_Bordersi, &ho_ContourH, ho_BorderEPs);
		GetContourXld(ho_ContourH, &hv_RowsSubI, &hv_ColsSubI);
		AreaCenterXld(ho_ContourH, &hv_AreaI, &hv_RowIK, &hv_ColIK, &hv_PointOrderG);
		GenRegionContourXld(ho_ContourH, &(*ho_RegionIR), "filled");
		(*ho_ContourOut) = ho_ContourH;
		//H end
	}
	else
	{
		pRowsSubI = (float *)malloc(5000 * sizeof(float));
		pColsSubI = (float *)malloc(5000 * sizeof(float));
		//* IK
		Threshold(ho_Im, &ho_RegionIr, hv_thr, 255);
		Intersection(ho_RegionIr, ho_Rectangle5, &(*ho_RegionIR));
		Connection((*ho_RegionIR), &ho_RegionsIR);
		AreaCenter(ho_RegionsIR, &hv_AreaIR, &hv_Row, &hv_Column);
		TupleSortIndex(hv_AreaIR, &hv_Indices);
		TupleInverse(hv_Indices, &hv_Inverted);
		SelectObj(ho_RegionsIR, &(*ho_RegionIR), HTuple(hv_Inverted[0]) + 1);

		int n = Region_Threshold_SubPix(ho_Im, *ho_RegionIR, hv_thr, pRowsSubI, pColsSubI);

		hv_RowsSubI = HTuple(pRowsSubI, n);
		hv_ColsSubI = HTuple(pColsSubI, n);
		GenContourPolygonXld(&ho_ContourIK, hv_RowsSubI, hv_ColsSubI);

		//Region_Threshold_SubPix((*ho_RegionIR), ho_Im, hv_thr, &hv_RowsSubI, &hv_ColsSubI);
		//GenContourPolygonXld(&ho_ContourIK, hv_RowsSubI, hv_ColsSubI);
		(*ho_ContourOut) = ho_ContourIK;
		//* IK end
		free(pRowsSubI);
		free(pColsSubI);
	}
	return;
}


void CCBM::SrunmedSLI(float* y, float* smo, long n, int bw_tot, int end_rule, int skip) // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
{
	/*
		 *  Computes "Running Median" smoother ("Stuetzle" algorithm) with medians of 'band'

		 *  Input:
		 *	y(n)	- responses in order of increasing predictor values
		 *	n	- number of observations
		 *	bw_tot	- span of running medians (MUST be ODD !!)
		 *	end_rule -- 0: Keep original data at ends {j; j < b2 | j > n-b2}
		 *		     -- 1: Constant ends = median(y[1],..,y[bw]) "robust"
		 *  Output:
		 *	smo(n)	- smoothed data output (should be allocated)

		 * NOTE:  The 'end' values are just copied !! this is fast but not too nice !
		 */

		 /* Local variables */
	double rmed, rmin, temp, rnew, yout, yi;
	double rbe, rtb, rse, yin, rts, fa, fb;
	int imin, ismo, i, j, first, last, band2, kminus, kplus, shift, bw, step, num_meds, last_med, a, b, x;

	step = skip + 1;
	bw = (((bw_tot - 1) / 2) / step) * 2 + 1;
	if (n < bw_tot)
		return;
	//std::cout << "new: bw=" << bw << ", radius=" << bw / 2 << ", step=" << step << "\n";

	if (n < bw)
		return;
	//double *scrat = (double *)malloc(bw, sizeof(double));
	//was
	double *scrat = (double *)malloc((unsigned)bw * sizeof(double));

	//if ((bw-1)*step+1 > n)
	//	error(_("bandwidth/span of running medians is larger than n"));

	/* 1. Compute  'rmed' := Median of the first 'band' values
	   ======================================================== */

	shift = 0;

	for (i = 0; i < bw; ++i)
		scrat[i] = y[i*step + shift];

	/* find minimal value  rmin = scrat[imin] <= scrat[j] */
	rmin = scrat[0]; imin = 0;
	for (i = 1; i < bw; ++i)
		if (scrat[i] < rmin) {
			rmin = scrat[i]; imin = i;
		}
	/* swap scrat[0] <-> scrat[imin] */
	temp = scrat[0]; scrat[0] = rmin; scrat[imin] = temp;
	/* sort the rest of of scrat[] by bubble (?) sort -- */
	for (i = 2; i < bw; ++i) {
		if (scrat[i] < scrat[i - 1]) {/* find the proper place for scrat[i] */
			temp = scrat[i];
			j = i;
			do {
				scrat[j] = scrat[j - 1];
				--j;
			} while (scrat[j - 1] > temp); /* now:  scrat[j-1] <= temp */
			scrat[j] = temp;
		}
	}
	band2 = bw / 2;
	rmed = scrat[band2];/* == Median( y[(1:band2)-1] ) */
	/* "malloc" had  free( (char*) scrat);*/ /*-- release scratch memory --*/
	if (shift == 0) {
		if (end_rule == 0) { /*-- keep DATA at end values */
			for (i = 0; i < band2*step; ++i)
				smo[i] = y[i];
		}
		else { /* if(end_rule == 1)  copy median to CONSTANT end values */
			for (i = 0; i < band2*step; ++i)
				smo[i] = (float)rmed;
		}
	}
	smo[band2*step + shift] = (float)rmed;
	num_meds = 1;
	band2++; /* = bw / 2 + 1*/;

	/* Big	FOR Loop: RUNNING median, update the median 'rmed'
	   ======================================================= */
	for (first = step + shift, last = bw * step + shift, ismo = band2 * step + shift;
		last < n;
		first += step, last += step, ismo += step) {

		yin = y[last];
		yout = y[first - step];
		rnew = rmed; /* New median = old one   in all the simple cases --*/

		if (yin < rmed) {
			if (yout >= rmed) {
				kminus = 0;
				if (yout > rmed) {/*	--- yin < rmed < yout --- */
					//if (print_level >= 2) REprintf(": yin < rmed < yout ");
					rnew = yin;/* was -rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi < rmed) {
							++kminus;
							if (yi > rnew)	rnew = yi;
						}
					}
					if (kminus < band2)		rnew = rmed;
				}
				else {/*		--- yin < rmed = yout --- */
					rse = rts = yin;/* was -rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi <= rmed) {
							if (yi < rmed) {
								++kminus;
								if (yi > rts)	rts = yi;
								if (yi > rse)	rse = yi;
							}
							else		rse = yi;

						}
					}
					rnew = (kminus == band2) ? rts : rse;
				}
			} /* else: both  yin, yout < rmed -- nothing to do .... */
		}
		else if (yin != rmed) { /* yin > rmed */
			if (yout <= rmed) {
				kplus = 0;
				if (yout < rmed) {/* -- yout < rmed < yin --- */
					rnew = yin; /* was rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi > rmed) {
							++kplus;
							if (yi < rnew)	rnew = yi;
						}
					}
					if (kplus < band2)	rnew = rmed;
				}
				else { /* -- yout = rmed < yin --- */
					rbe = rtb = yin; /* was rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi >= rmed) {
							if (yi > rmed) {
								++kplus;
								if (yi < rtb)	rtb = yi;
								if (yi < rbe)	rbe = yi;
							}
							else		rbe = yi;
						}
					}
					rnew = (kplus == band2) ? rtb : rbe;
				}
			} /* else: both  yin, yout > rmed --> nothing to do */
		} /* else: yin == rmed -- nothing to do .... */
		rmed = rnew;
		smo[ismo] = (float)rmed;
		num_meds++;
	} /*     END FOR ------------ big Loop -------------------- */

	free(scrat);

	band2 = bw / 2;
	last_med = band2 * step + (num_meds - 1)*step;

	if (end_rule == 0) { /*-- keep DATA at end values */
		for (i = last_med + 1; i < n; ++i)
			smo[i] = y[i];
	}
	else { /* if(end_rule == 1)  copy median to CONSTANT end values */
		for (i = last_med + 1; i < n; ++i)
			smo[i] = smo[last_med];
	}

	if (skip > 0) {
		for (i = 0; i < num_meds - 1; i++) {
			for (j = 1; j < step; j++) {
				x = (band2 + i)*step + j;
				a = (band2 + i)*step;
				b = (band2 + i + 1)*step;
				fa = smo[a];
				fb = smo[b];
				smo[x] = (float)(fa + j * (fb - fa) / step); //b-a=step, x-a=j
			}
		}
	}
} /* SrunmedSLI */

#endif
