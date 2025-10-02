/**
 * \copyright (C) Synapse Imaging 2023
 *
 * @File Name
		PinholeIsland.cpp
 *
 * @Summary
 *
 * @Description
 *		Scratch detection on conductors
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 2023.09.22
 */

 /* ************************************************************************** */
 /* Section: Included Files                                                    */
 /* ************************************************************************** */
#include "Inspect_Common.h"
#include "PinholeIsland.h"

/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */

/* ************************************************************************** */
/* Section: Function				                                          */
/* ************************************************************************** */

/** ---------------------------------------------------------------------------
 * @Summary
 *      PinholeIsland
 */
/// Calculating defect features for Pinhole/Island
void ThresholdPHIsp(HObject ho_RegionsPH, HObject ho_RegionsIsl,
				    HTuple  &hv_APh, HTuple  &hv_InRPh, HTuple  &hv_AIsl, HTuple  &hv_InRIsl)
{
	HTuple  hv_R, hv_C;
	AreaCenter(ho_RegionsPH, &hv_APh, &hv_R, &hv_C);
	InnerCircle(ho_RegionsPH, &hv_R, &hv_C, &hv_InRPh);

	AreaCenter(ho_RegionsIsl, &hv_AIsl, &hv_R, &hv_C);
	InnerCircle(ho_RegionsIsl, &hv_R, &hv_C, &hv_InRIsl);
}

/// Thresholding and Pinhole/Island detection
void ThresholdPHIsp(HObject ho_Im, HObject ho_RegProc, HObject *ho_RegionsPH, HObject *ho_RegionsIsl,
	HObject *ho_RegionISnoPhnoIsl, HObject *ho_RegionInoPhnoIsl, HObject *ho_RegionI,
	HTuple hv_thr, HTuple hv_PHrmin, HTuple hv_PHamin, HTuple hv_Islrmin, HTuple hv_Islamin, HTuple hv_PHash, HObject *m_local_ImModified)
{

	// Local iconic variables
	HObject  ho_RegionIi, ho_DomainIm, ho_RegionISi;
	HObject  ho_RegionIS, ho_RegionFillUp1, ho_RegionDifference3;
	HObject  ho_RegionPH, ho_RegionIsl, ho_RegionsPHi, ho_RegionsIsli;
	HObject  ho_RegionsPHii, ho_RegionsIslii, ho_RegionsPHiii, ho_RegionPHiii;
	//HTuple hv_PHash;

	//hv_PHrmin = 3.0;
	//hv_PHamin = 5;
	//Islrmin := 2
	//Islamin := 5

	/*hv_Islrmin = 2;
	hv_Islamin = 10;*/
	if (hv_thr > 250)
		(hv_thr = 250);

	CopyImage(ho_Im, &(*m_local_ImModified));

	//Threshold(ho_Im, &ho_RegionIi, hv_thr, 255);
	//GetDomain(ho_Im, &ho_DomainIm);
	//Difference(ho_DomainIm, ho_RegionIi, &ho_RegionISi);
	//Intersection(ho_RegionIi, ho_RegProc, &(*ho_RegionI));
	//Intersection(ho_RegionISi, ho_RegProc, &ho_RegionIS);

	////difference (RegionISi, RegProc, RegionIS)

	//FillUpShape((*ho_RegionI), &ho_RegionFillUp1, "area", 1, 8000);
	//Difference(ho_DomainIm, ho_RegionFillUp1, &ho_RegionDifference3);
	//Difference(ho_RegionFillUp1, (*ho_RegionI), &ho_RegionPH);

	//FillUpShape(ho_RegionDifference3, &(*ho_RegionISnoPhnoIsl), "area", 1, 8000);
	//Difference(ho_DomainIm, (*ho_RegionISnoPhnoIsl), &(*ho_RegionInoPhnoIsl));
	//Difference((*ho_RegionISnoPhnoIsl), ho_RegionDifference3, &ho_RegionIsl);

	//Connection(ho_RegionPH, &ho_RegionsPHi);
	//Connection(ho_RegionIsl, &ho_RegionsIsli);

	//SelectShape(ho_RegionsPHi, &ho_RegionsPHii, "area", "and", hv_PHamin, 999999);
	//SelectShape(ho_RegionsPHii, &(*ho_RegionsPH), "inner_radius", "and", hv_PHrmin,	999999);
	
	Threshold(ho_Im, &ho_RegionIi, hv_thr, 255);
	GetDomain(ho_Im, &ho_DomainIm);
	Difference(ho_DomainIm, ho_RegionIi, &ho_RegionISi);
	Intersection(ho_RegionIi, ho_RegProc, &(*ho_RegionI));
	Intersection(ho_RegionISi, ho_RegProc, &ho_RegionIS);

	//difference (RegionISi, RegProc, RegionIS)

	FillUpShape((*ho_RegionI), &ho_RegionFillUp1, "area", 1, 3000);
	Difference(ho_DomainIm, ho_RegionFillUp1, &ho_RegionDifference3);
	Difference(ho_RegionFillUp1, (*ho_RegionI), &ho_RegionPH);
	PaintRegion(ho_RegionPH, *m_local_ImModified, &(*m_local_ImModified), hv_thr+1,"fill");

	//FillUpShape(ho_RegionDifference3, &(*ho_RegionISnoPhnoIsl), "area", 1, 8000);
	FillUpShape(ho_RegionDifference3, &(*ho_RegionISnoPhnoIsl), "area", 1, 800);
	Difference(ho_DomainIm, (*ho_RegionISnoPhnoIsl), &(*ho_RegionInoPhnoIsl));
	Difference((*ho_RegionISnoPhnoIsl), ho_RegionDifference3, &ho_RegionIsl);

	Connection(ho_RegionPH, &ho_RegionsPHi);
	Connection(ho_RegionIsl, &ho_RegionsIsli);

	SelectShape(ho_RegionsPHi, &ho_RegionsPHii, "area", "and", hv_PHamin, 999999);
	SelectShape(ho_RegionsPHii, &(*ho_RegionsPH), "inner_radius", "and", hv_PHrmin,
		999999);

	SelectShape(ho_RegionsIsli, &ho_RegionsIslii, "area", "and", hv_Islamin, 999999);
	SelectShape(ho_RegionsIslii, &(*ho_RegionsIsl), "inner_radius", "and", hv_Islrmin,
		999999);

	hv_PHash = 200;
	SelectShape(ho_RegionsPHii, &ho_RegionsPHiii, "area", "and", hv_PHash, 999999);
	Union1(ho_RegionsPHiii, &ho_RegionPHiii);

	/*Difference(*ho_RegionInoPhnoIsl, ho_RegionPHiii, &( *ho_RegionInoPhnoIsl));

	SelectShape(ho_RegionsIsli, &ho_RegionsIslii, "area", "and", hv_Islamin, 999999);
	SelectShape(ho_RegionsIslii, &(*ho_RegionsIsl), "inner_radius", "and", hv_Islrmin,
		999999);*/
	

	return;
}

/** ---------------------------------------------------------------------------
 * @Summary
 *      ImagePrepThr
 */
void ImagePrepThr(HObject ho_Im, HObject ho_R,  HObject *ho_Domain,
	HObject *ho_RegionI, HObject *ho_RegionIi, HTuple hv_thr, HTuple *hv_Width, HTuple *hv_Height)
{

	// Local iconic variables
	HObject  ho_DomainBorder, ho_DomainBorderComplement;
	HObject  ho_RBorder, ho_RBorderComplement, ho_RC;

	// Local control variables
	HTuple  hv_nRBC;

	GetImageSize(ho_Im, &(*hv_Width), &(*hv_Height));
	GetDomain(ho_Im, &(*ho_Domain));
	Difference(*ho_Domain, ho_R, &ho_RC);
	Boundary((*ho_Domain), &ho_DomainBorder, "inner");
	Complement(ho_DomainBorder, &ho_DomainBorderComplement);
	Boundary(ho_R, &ho_RBorder, "outer");
	Complement(ho_RBorder, &ho_RBorderComplement);

	//histo_to_thresh(RelativeHisto,
	Threshold(ho_Im, &(*ho_RegionIi), hv_thr, 255);

	Intersection((*ho_RegionIi), ho_RC, &(*ho_RegionI));
	Intersection((*ho_RegionI), ho_DomainBorderComplement, &(*ho_RegionI));
	CountObj(ho_RBorderComplement, &hv_nRBC);
	//* checking if not empty
	if (0 != (hv_nRBC != 0))
	{
		Intersection((*ho_RegionI), ho_RBorderComplement, &(*ho_RegionI));
	}

	return;
}
/** ---------------------------------------------------------------------------
 * @Summary
 *      PrepRC
 */
void PrepRC(HObject ho_Im, HObject ho_R, HObject *ho_Domain, HObject *ho_RC)
{

	// Local iconic variables

	// Local control variables
	HTuple  hv_aR, hv_r, hv_c, hv_l;

	GetDomain(ho_Im, &(*ho_Domain));
	AreaCenter(ho_R, &hv_aR, &hv_r, &hv_c);
	TupleLength(hv_aR, &hv_l);
	if (0 != (HTuple(hv_aR == 0).TupleOr(hv_l == 0)))
	{
		(*ho_RC) = (*ho_Domain);
	}
	else
	{
		Difference((*ho_Domain), ho_R, &(*ho_RC));
	}
	return;
}

/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */
