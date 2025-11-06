/**
 * \copyright (C) Synapse Imaging 2025
 *
 * @File Name
 *      DynThres.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors 고진욱
 * @version 1.0
 * @date 10/20/2025 6:46:47 AM
 */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#pragma once

#include "HalconCpp.h"
#include "HDevThread.h"

using namespace HalconCpp;

/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
#define MAX_DEF			(999999999)




/* ************************************************************************** */
/* Section: Function														  */
/* ************************************************************************** */

/** ---------------------------------------------------------------------------
 * @Summary
 *
 */
BOOL ValidHRegion(HObject rHObject)
{
	bool bValid = rHObject.IsInitialized();

	if (bValid)
	{
		HTuple HObjectNum;
		CountObj(rHObject, &HObjectNum);

		if (HObjectNum > 0)
		{
			HTuple Area, Row, Column;

			AreaCenter(rHObject, &Area, &Row, &Column);

			for (int i = 0; i < HObjectNum; i++)
			{
				if (Area[i].L() > 0)
					return TRUE;
			}
		}
	}

	return FALSE;
}

/** ---------------------------------------------------------------------------
 * @Summary
 *      
 */
BOOL BlobUnion(HObject *pInputRgn, long lMergeLength)
{
	try
	{
		long lDilationSize;

		lDilationSize = lMergeLength / 2;
		if ((lMergeLength % 2) == 1)
			lDilationSize += 1;

		HObject HConnInputRgn, HUnionInputRgn, HDilatedRgn, HSelectedRgn;

		Connection(*pInputRgn, &HConnInputRgn);

		Union1(*pInputRgn, &HUnionInputRgn);
		DilationCircle(HUnionInputRgn, &HDilatedRgn, lDilationSize);
		Connection(HDilatedRgn, &HDilatedRgn);

		Hlong  lNoObj, lNoBlob;
		HObject HUnionRgn;
		HTuple HlNoBlob;

		CountObj(HDilatedRgn, &HlNoBlob);
		lNoBlob = HlNoBlob.L();

		GenEmptyObj(pInputRgn);

		int i, iIndex1, iIndex2;
		HObject HConnectRgn, HObjectSelected1, HObjectSelected2, HConnectLineRgn;
		HTuple HRow, HCol;
		double dMinDistance;
		Hlong lRow1, lRow2, lCol1, lCol2;
		HTuple HlRow1, HlRow2, HlCol1, HlCol2, HdMinDistance;

		for (i = 1; i <= lNoBlob; i++)
		{
			SelectObj(HDilatedRgn, &HSelectedRgn, i);
			SelectShapeProto(HConnInputRgn, HSelectedRgn, &HUnionRgn, "overlaps_abs", 1, MAX_DEF);

			lNoObj = 0;
			HTuple HlNoObj;
			CountObj(HUnionRgn, &HlNoObj);
			lNoObj = HlNoObj.L();

			if (lNoObj > 1)
			{
				GenEmptyObj(&HConnectRgn);
				for (iIndex1 = 1; iIndex1 <= (lNoObj - 1); iIndex1++)
				{
					SelectObj(HUnionRgn, &HObjectSelected1, iIndex1);

					for (iIndex2 = iIndex1 + 1; iIndex2 <= lNoObj; iIndex2++)
					{
						SelectObj(HUnionRgn, &HObjectSelected2, iIndex2);

						DistanceRrMin(HObjectSelected1, HObjectSelected2, &HdMinDistance, &HlRow1, &HlCol1, &HlRow2, &HlCol2);
						dMinDistance = HdMinDistance.D();
						lRow1 = HlRow1.L();
						lCol1 = HlCol1.L();
						lRow2 = HlRow2.L();
						lCol2 = HlCol2.L();

						if (dMinDistance <= (double)lMergeLength)
						{
							TupleGenConst(0, 0, &HRow);
							TupleGenConst(0, 0, &HCol);
							TupleConcat(HRow, HTuple(lRow1), &HRow);
							TupleConcat(HRow, HTuple(lRow2), &HRow);
							TupleConcat(HCol, HTuple(lCol1), &HCol);
							TupleConcat(HCol, HTuple(lCol2), &HCol);
							GenRegionPolygon(&HConnectLineRgn, HRow, HCol);
							ConcatObj(HConnectRgn, HConnectLineRgn, &HConnectRgn);
						}
					}
				}

				if (ValidHRegion(HConnectRgn) == TRUE)
					ConcatObj(HUnionRgn, HConnectRgn, &HUnionRgn);

				Union1(HUnionRgn, &HUnionRgn);
			}

			if (ValidHRegion(HUnionRgn))
				ConcatObj(*pInputRgn, HUnionRgn, pInputRgn);
		}

		return TRUE;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		//Queue_Add(&m_main_queue_log, NULL, "*-- BlobUnion() Halcon Exception: <%s>, <%s>",
		//	sOperatorName, sErrMsg);

		return FALSE;
	}
}
/** ---------------------------------------------------------------------------
 * @Summary
 *      
 */
HObject Proc_DynThres_Pattern(
	HObject m_ho_CadPatternRegion_DynThresTest, 
	HObject local_Im,
	int iDt_Dark_Pattern,
	int iDt_Bright_Pattern,
	int iDt_Margin_Pattern,
	int iDt_Area_Pattern,
	int iDt_Length_Pattern,
	int iDt_Width_Pattern,
	BOOL bDtConnectDef,
	int iDt_Connect_Distance)
{
	HObject HDefectRgn;
	GenEmptyObj(&HDefectRgn);

	if (ValidHRegion(m_ho_CadPatternRegion_DynThresTest) == FALSE)
		return HDefectRgn;

	//m_inspect_status = "Proc_DynThres_Pattern()";

	//DWORD time_start = GetTickCount();

	// Param
	//int iDt_Dark_Pattern = m_Inspection_Params.iDt_Dark_Pattern;
	//int iDt_Bright_Pattern = m_Inspection_Params.iDt_Bright_Pattern;
	//int iDt_Margin_Pattern = m_Inspection_Params.iDt_Margin_Pattern;
	//int iDt_Area_Pattern = m_Inspection_Params.iDt_Area_Pattern;
	//int iDt_Length_Pattern = m_Inspection_Params.iDt_Length_Pattern;
	//int iDt_Width_Pattern = m_Inspection_Params.iDt_Width_Pattern;
	//BOOL bDtConnectDef = m_Inspection_Params.bDtConnectDef;
	//int iDt_Connect_Distance = m_Inspection_Params.iDt_Connect_Distance;

	Hlong lNoDefect = 0;
	HTuple HlNoDefect;

	BYTE *pImageData;
	char type[30];
	Hlong lImageWidth, lImageHeight;
	HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

	GetImagePointer1(local_Im, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);
	pImageData = (BYTE *)HpImageData.L();
	lImageWidth = HlImageWidth.L();
	lImageHeight = HlImageHeight.L();

	HObject HInspectAreaRgn;
	HInspectAreaRgn = m_ho_CadPatternRegion_DynThresTest;
	if (iDt_Margin_Pattern > 0)
		ErosionCircle(HInspectAreaRgn, &HInspectAreaRgn, (double)iDt_Margin_Pattern + 0.5);

	HObject HBrightRgn, HDarkRgn, HDynThreshRgn;
	HObject HDTImageReduced, HInvertImageReduced;
	HTuple HTuArea, HTuRow, HTuCol;

	GenEmptyObj(&HBrightRgn);
	GenEmptyObj(&HDarkRgn);

	ReduceDomain(local_Im, HInspectAreaRgn, &HDTImageReduced);

	HObject HThresRgn;
	HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
	long lNoHisto, lPeak, lHystUpperThreshold, lHystLowThreshold;
	BOOL bPeakFound;

	GrayHisto(HInspectAreaRgn, HDTImageReduced, &HAbsHisto, &HRelHisto);
	CreateFunct1dArray(HAbsHisto, &HFunction);
	SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
	TupleLastN(HSmoothFunction, 3, &HAbsHisto);
	TupleSortIndex(HAbsHisto, &Indices);
	TupleInverse(Indices, &Inverted);

	HTuple HlNoHisto;
	TupleLength(Inverted, &HlNoHisto);
	lNoHisto = HlNoHisto.L();

	bPeakFound = FALSE;
	for (int iii = 0; iii < lNoHisto; iii++)
	{
		lPeak = Inverted[iii].L();
		if (lPeak >= 100 && lPeak <= 250)
		{
			bPeakFound = TRUE;
			break;
		}
	}

	if (bPeakFound)
	{
		lHystUpperThreshold = lPeak + iDt_Bright_Pattern;
		if (lHystUpperThreshold > 255)
			lHystUpperThreshold = 255;

		lHystLowThreshold = lHystUpperThreshold - 10;
		if (lHystLowThreshold < 0)
			lHystLowThreshold = 0;

		HysteresisThreshold(HDTImageReduced, &HThresRgn, lHystLowThreshold, lHystUpperThreshold, 1);

		if (ValidHRegion(HThresRgn) == TRUE)
			ConcatObj(HBrightRgn, HThresRgn, &HBrightRgn);
	}

	InvertImage(HDTImageReduced, &HInvertImageReduced);

	GrayHisto(HInspectAreaRgn, HInvertImageReduced, &HAbsHisto, &HRelHisto);
	CreateFunct1dArray(HAbsHisto, &HFunction);
	SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
	TupleLastN(HSmoothFunction, 3, &HAbsHisto);
	TupleSortIndex(HAbsHisto, &Indices);
	TupleInverse(Indices, &Inverted);

	TupleLength(Inverted, &HlNoHisto);
	lNoHisto = HlNoHisto.L();

	long lDarkPeakMin, lDarkPeakMax;
	lDarkPeakMin = 255 - 250;
	if (lDarkPeakMin < 0)
		lDarkPeakMin = 0;

	lDarkPeakMax = 255 - 100;
	if (lDarkPeakMax < 0)
		lDarkPeakMax = 0;

	bPeakFound = FALSE;
	for (int iii = 0; iii < lNoHisto; iii++)
	{
		lPeak = Inverted[iii].L();
		if (lPeak >= lDarkPeakMin && lPeak <= lDarkPeakMax)
		{
			bPeakFound = TRUE;
			break;
		}
	}

	if (bPeakFound)
	{
		lHystUpperThreshold = lPeak + iDt_Dark_Pattern;
		if (lHystUpperThreshold > 255)
			lHystUpperThreshold = 255;

		lHystLowThreshold = lHystUpperThreshold - 10;
		if (lHystLowThreshold < 0)
			lHystLowThreshold = 0;

		HysteresisThreshold(HInvertImageReduced, &HThresRgn, lHystLowThreshold, lHystUpperThreshold, 1);

		if (ValidHRegion(HThresRgn) == TRUE)
			ConcatObj(HDarkRgn, HThresRgn, &HDarkRgn);
	}

	GenEmptyObj(&HDynThreshRgn);
	Union2(HBrightRgn, HDarkRgn, &HDynThreshRgn);

	OpeningCircle(HDynThreshRgn, &HDynThreshRgn, 1);
	ClosingCircle(HDynThreshRgn, &HDynThreshRgn, 1);

	Connection(HDynThreshRgn, &HDynThreshRgn);
	SelectShape(HDynThreshRgn, &HDynThreshRgn, "area", "and", iDt_Area_Pattern, MAX_DEF);
	SelectShape(HDynThreshRgn, &HDynThreshRgn, "max_diameter", "and", iDt_Length_Pattern, MAX_DEF);
	SelectShape(HDynThreshRgn, &HDynThreshRgn, "inner_radius", "and", iDt_Width_Pattern / 2, MAX_DEF);

	if (bDtConnectDef)
		BlobUnion(&HDynThreshRgn, iDt_Connect_Distance);

	if (ValidHRegion(HDynThreshRgn))
	{
		CountObj(HDynThreshRgn, &HlNoDefect);
		lNoDefect = HlNoDefect.L();

		HTuple HArea, HRow, HCol, hv_TR;
		AreaCenter(HDynThreshRgn, &HArea, &HRow, &HCol);
		TupleGenConst(lNoDefect, 20.5, &hv_TR);
		GenCircle(&HDefectRgn, HRow, HCol, hv_TR);
	}

	//DWORD time_end = GetTickCount();
	//Queue_Add(m_queue_log, NULL, "--- CInspect_Call Proc_DynThres_Pattern() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time: %0*.4f s, Defects=%d",
	//	inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000., lNoDefect);

	return HDefectRgn;
}/** ---------------------------------------------------------------------------
 * @Summary
 *      
 */

HObject Proc_DynThres_Space(
	HObject ho_CadSpaceRegion_DynThresTest,
	HObject local_Im,
	int iDt_Dark_Space,
	int iDt_Bright_Space,
	int iDt_Mean_Space,
	int iDt_Margin_Space,
	int iDt_Area_Space)
{
	HObject HDefectRgn;
	GenEmptyObj(&HDefectRgn);

	if (ValidHRegion(ho_CadSpaceRegion_DynThresTest) == FALSE)
		return HDefectRgn;

	//m_inspect_status = "Proc_DynThres_Space()";

	//DWORD time_start = GetTickCount();

	// Param
	//int iDt_Dark_Space = m_Inspection_Params.iDt_Dark_Space;
	//int iDt_Bright_Space = m_Inspection_Params.iDt_Bright_Space;
	//int iDt_Mean_Space = m_Inspection_Params.iDt_Mean_Space;
	//int iDt_Margin_Space = m_Inspection_Params.iDt_Margin_Space;
	//int iDt_Area_Space = m_Inspection_Params.iDt_Area_Space;

	Hlong lNoDefect = 0;
	HTuple HlNoDefect;

	BYTE *pImageData;
	char type[30];
	Hlong lImageWidth, lImageHeight;
	HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

	GetImagePointer1(local_Im, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);
	pImageData = (BYTE *)HpImageData.L();
	lImageWidth = HlImageWidth.L();
	lImageHeight = HlImageHeight.L();

	HObject HInspectAreaRgn;
	HObject HOpeningRgn, HDiffRgn;
	OpeningCircle(ho_CadSpaceRegion_DynThresTest, &HOpeningRgn, 4.5);
	Difference(ho_CadSpaceRegion_DynThresTest, HOpeningRgn, &HDiffRgn);
	Difference(ho_CadSpaceRegion_DynThresTest, HDiffRgn, &HInspectAreaRgn);
	Connection(HInspectAreaRgn, &HInspectAreaRgn);
	SelectShape(HInspectAreaRgn, &HInspectAreaRgn, "area", "and", 500, MAX_DEF);
	Union1(HInspectAreaRgn, &HInspectAreaRgn);

	HObject HInspectAreaSkelRgn;
	Skeleton(HInspectAreaRgn, &HInspectAreaSkelRgn);

	HObject HErodeInspectAreaRgn, HInspectMarginRgn;
	if (iDt_Margin_Space > 0)
		ErosionCircle(HInspectAreaRgn, &HInspectMarginRgn, (double)iDt_Margin_Space + 0.5);
	else
		HInspectMarginRgn = HInspectAreaRgn;

	HObject HRemovedSpaceSkelRgn;
	Difference(HInspectAreaSkelRgn, HInspectMarginRgn, &HRemovedSpaceSkelRgn);
	Connection(HRemovedSpaceSkelRgn, &HRemovedSpaceSkelRgn);
	SelectShape(HRemovedSpaceSkelRgn, &HRemovedSpaceSkelRgn, "area", "and", 30, MAX_DEF);
	Union2(HInspectMarginRgn, HRemovedSpaceSkelRgn, &HErodeInspectAreaRgn);

	HObject HBrightRgn, HDarkRgn, HDynThreshRgn;
	HObject HDTImageReduced, HDTInspectImageReduced, HExpandReduceImage, HMeanImage;
	HTuple HTuArea, HTuRow, HTuCol;

	GenEmptyObj(&HBrightRgn);
	GenEmptyObj(&HDarkRgn);

	ReduceDomain(local_Im, HErodeInspectAreaRgn, &HDTImageReduced);
	ReduceDomain(local_Im, HInspectMarginRgn, &HDTInspectImageReduced);

	int iFilterSize;
	iFilterSize = iDt_Mean_Space;
	if (iFilterSize < 3)
		iFilterSize = 3;
	else
		iFilterSize = iDt_Mean_Space / 2 * 2 + 1;

	int iExpandSize;
	iExpandSize = iFilterSize / 2 + iDt_Margin_Space;

	ExpandDomainGray(HDTImageReduced, &HExpandReduceImage, iExpandSize);
	ReduceDomain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);
	MeanImage(HExpandReduceImage, &HMeanImage, iFilterSize, iFilterSize);
	DynThreshold(HDTInspectImageReduced, HMeanImage, &HBrightRgn, iDt_Bright_Space, "light");
	DynThreshold(HDTInspectImageReduced, HMeanImage, &HDarkRgn, iDt_Dark_Space, "dark");

	GenEmptyObj(&HDynThreshRgn);
	Union2(HBrightRgn, HDarkRgn, &HDynThreshRgn);

	OpeningCircle(HDynThreshRgn, &HDynThreshRgn, 1);
	ClosingCircle(HDynThreshRgn, &HDynThreshRgn, 1);

	Connection(HDynThreshRgn, &HDynThreshRgn);
	SelectShape(HDynThreshRgn, &HDynThreshRgn, "area", "and", iDt_Area_Space, MAX_DEF);

	if (ValidHRegion(HDynThreshRgn))
	{
		CountObj(HDynThreshRgn, &HlNoDefect);
		lNoDefect = HlNoDefect.L();

		HTuple HArea, HRow, HCol, hv_TR;
		AreaCenter(HDynThreshRgn, &HArea, &HRow, &HCol);
		TupleGenConst(lNoDefect, 20.5, &hv_TR);
		GenCircle(&HDefectRgn, HRow, HCol, hv_TR);
	}

	//DWORD time_end = GetTickCount();
	//Queue_Add(m_queue_log, NULL, "--- CInspect_Call Proc_DynThres_Space() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time: %0*.4f s, Defects=%d",
	//	inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000., lNoDefect);

	return HDefectRgn;
}

int	Proc_HistoPeakG(HObject HCadPatternRgn, HObject local_lm)
{
	int iHistoPeakGV = 170;

	if (ValidHRegion(HCadPatternRgn) == FALSE)
		return iHistoPeakGV;

	HObject HImageReduced;
	HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;

	ReduceDomain(local_lm, HCadPatternRgn, &HImageReduced);

	GrayHisto(HCadPatternRgn, HImageReduced, &HAbsHisto, &HRelHisto);
	CreateFunct1dArray(HAbsHisto, &HFunction);
	SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
	TupleLastN(HSmoothFunction, 3, &HAbsHisto);
	TupleSortIndex(HAbsHisto, &Indices);
	TupleInverse(Indices, &Inverted);

	iHistoPeakGV = Inverted[0].L();

	if (iHistoPeakGV > 250)
	{
		iHistoPeakGV = 170;
	}

	return iHistoPeakGV;
}


/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

