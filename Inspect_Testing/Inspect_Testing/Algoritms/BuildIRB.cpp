#include "Inspect_Common.h"
#include "BuildIRB.h"

void BuildIRBP7B2(HObject ho_SkeletonsFWM, HObject ho_RegionInoD, HObject ho_ImIRBP,
	HObject ho_RegionsPNi, HObject *ho_ImIRBPOut, HTuple hv_expsize, HTuple hv_nEmpty,
	HTuple hv_dil, HTuple *hv_nFWMP)
{

	// Local iconic variables
	HObject  ho_SkeletonsFWMd, ho_RegionInoDS, ho_RecInoD;
	HObject  ho_RegionsIntersection, ho_RegsInoD, ho_Obj, ho_Objs;
	HObject  ho_RegionsFWMI, ho_RegionFWMI, ho_RegionsFWMBii;
	HObject  ho_RegionInoDB, ho_RegionsFWMB;

	// Local control variables
	HTuple  hv_no, hv_Row1, hv_Column1, hv_Row2, hv_Column2;
	HTuple  hv_n, hv_i, hv_nO, hv_AreaO, hv_Row3, hv_Column3;
	HTuple  hv_IndicesO, hv_InvertedO, hv_nRFWM, hv_stseq, hv_overf;
	HTuple  hv_SequenceINBi, hv_SequenceIRB;

	//**** BuildIRBP7B2
	//**- 28.02.2024 - for 2 pnum
	//** big shift comp. attempt
	//**- 19.01.2024 - for SSPI
	//**- 07.12.2023 - same as S but for P
	(*ho_ImIRBPOut) = ho_ImIRBP;
	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
	//dilation_circle (SkeletonsFWM, SkeletonsFWMSd, 2.5)
	//1+2
	hv_dil = 11;
	DilationCircle(ho_SkeletonsFWM, &ho_SkeletonsFWMd, hv_dil);
	CountObj(ho_SkeletonsFWMd, &hv_no);

	//union1 (SkeletonsFWMd, SkeletonFWMd)
	//dilation_circle (SkeletonFWMd, RegionFWM, 40)
	//intersection (RegionFWM, RegionInoD, RegionInoDfwm)



	//2
	//union1 (SkeletonsFWM, SkeletonFWMS)
	//difference (SkeletonFWMS, RegionInoDS, SkeletonFWMSdif)
	//connection (SkeletonFWMS, SkeletonsFWMSc)
	//dilation_circle (SkeletonsFWM, SkeletonsFWMSd, dil)

	//count_obj (SkeletonsFWMSc, n2)


	//dilation_circle (SkeletonsFWM, SkeletonsFWMSd2, dil)
	//count_obj (SkeletonsFWMSd2, n2)


	//1
	//difference (SkeletonsFWMSd, RegionInoDS, SkeletonsFWMSm)
	//3
	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_RecInoD, hv_Row1 - 5, hv_Column1 - 5, hv_Row2 + 5, hv_Column2 + 5);
	//difference (RecInoD, RegionInoDfwm, RegionInoDSfwm)
	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDS);
	//expand_region (SkeletonsFWMd, RegionInoDS, RegionExpanded, 'maximal', 'image')
	//intersection (RegionExpanded, RegionInoD, RegionsIntersection)
	Intersection(ho_SkeletonsFWMd, ho_RegionInoD, &ho_RegionsIntersection);
	CountObj(ho_RegionsIntersection, &hv_n);
	GenEmptyObj(&ho_RegsInoD);
	{
		HTuple end_val44 = hv_n;
		HTuple step_val44 = 1;
		for (hv_i = 1; hv_i.Continue(end_val44, step_val44); hv_i += step_val44)
		{
			SelectObj(ho_RegionsIntersection, &ho_Obj, hv_i);
			Connection(ho_Obj, &ho_Objs);
			CountObj(ho_Objs, &hv_nO);
			if (0 != (hv_nO > 1))
			{
				AreaCenter(ho_Objs, &hv_AreaO, &hv_Row3, &hv_Column3);
				TupleSortIndex(hv_AreaO, &hv_IndicesO);
				TupleInverse(hv_IndicesO, &hv_InvertedO);
				SelectObj(ho_Objs, &ho_Obj, HTuple(hv_InvertedO[0]) + 1);
			}
			ConcatObj(ho_RegsInoD, ho_Obj, &ho_RegsInoD);
		}
	}
	
	ExpandRegion(ho_RegsInoD, ho_RegionInoDS, &ho_RegionsFWMI, "maximal", "image");
	//expand_region (RegsInoD, RegionInoDS, RegionsFWMI, 20, 'image')
	//stop ()

	Union1(ho_RegionsFWMI, &ho_RegionFWMI);
	//
	CountObj(ho_SkeletonsFWM, &hv_nRFWM);
	hv_stseq = 1 + 1;
	hv_overf = 255 - hv_stseq;
	TupleGenSequence(hv_stseq, (hv_nRFWM + hv_stseq) - 1, 1, &hv_SequenceINBi);
	hv_SequenceIRB = (hv_SequenceINBi%hv_overf) + hv_stseq;
	Boundary(ho_RegionsFWMI, &ho_RegionsFWMBii, "inner");
	Boundary(ho_RegionFWMI, &ho_RegionInoDB, "inner");
	Intersection(ho_RegionsFWMBii, ho_RegionInoDB, &ho_RegionsFWMB);

	//dilation_circle (RegCut, RegCutdi, 2)
	//intersection (RegionInoDB, RegCutdi, RegionInoDBcut)
	//difference (RegCutdi, RegionInoDBcut, RegCutd)
	//difference (RegionsFWMBi, RegCutd, RegionsFWMB)
	CountObj(ho_RegionsFWMB, &(*hv_nFWMP));

	//if (1)
	//{
	//	WriteObject(ho_RegionsFWMI, "C:\\Temp\\ho_RegsInoD");
	//	//WriteObject(ho_RegionInoDS, "C:\\Temp\\ho_RegionInoDS");

	//	//WriteObject(m_pFWM->ho_ImIRBS, "C:\\Temp\\ho_ImIRBS");
	//}
	PaintRegion(ho_RegionsFWMI, (*ho_ImIRBPOut), &(*ho_ImIRBPOut), 1, "fill");
	PaintRegion(ho_RegionsFWMB, (*ho_ImIRBPOut), &(*ho_ImIRBPOut), hv_SequenceINBi,
		"fill");
	return;
}


void BuildIRBS5B2(HObject ho_SkeletonsFWMS, HObject ho_SkeletonsFWMScut, HObject ho_ImIRBS,
	HObject ho_RegionInoD, HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty,
	HTuple hv_dil, HTuple *hv_nFWMSB)
{

	// Local iconic variables
	HObject  ho_EndPoints, ho_JuncPoints, ho_EndPointsd;
	HObject  ho_EndPointsdu, ho_SkeletonsFWMScutd, ho_RecInoD;
	HObject  ho_RegionInoDC, ho_SkeletonsFWMScutm, ho_SkeletonsFWMScutdu;
	HObject  ho_SkeletonsFWMScutmu, ho_SkeletonsFWMScutm2, ho_RegionForbid;
	HObject  ho_RegionsFWMSI, ho_RegionsFWMSB, ho_RegionsFWMSBii;
	HObject  ho_RegionFWMSI, ho_RegionInoDSB;

	// Local control variables
	HTuple  hv_no, hv_Row1, hv_Column1, hv_Row2, hv_Column2;
	HTuple  hv_nRFWMS, hv_stseq, hv_overf, hv_SequenceINBSi;
	HTuple  hv_SequenceIRBS;

	//****BuildIRBS5B2 - 18.04.2023 - removed difference for regions tuple
	//13.03.2024 - modified
	//15.03.2024 - remobe ends by no cut skel.
	(*ho_ImIRBSOut) = ho_ImIRBS;
	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, 2.5)
	//1+2
	//dil := 3
	JunctionsSkeleton(ho_SkeletonsFWMS, &ho_EndPoints, &ho_JuncPoints);
	DilationCircle(ho_EndPoints, &ho_EndPointsd, 7.5);
	Union1(ho_EndPointsd, &ho_EndPointsdu);

	DilationCircle(ho_SkeletonsFWMScut, &ho_SkeletonsFWMScutd, hv_dil);
	CountObj(ho_SkeletonsFWMScutd, &hv_no);

	//2
	//union1 (SkeletonsFWMS, SkeletonFWMS)
	//difference (SkeletonFWMS, RegionInoD, SkeletonFWMSdif)
	//connection (SkeletonFWMS, SkeletonsFWMSc)
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, dil)

	//count_obj (SkeletonsFWMSc, n2)


	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd2, dil)
	//count_obj (SkeletonsFWMSd2, n2)


	//1
	//difference (SkeletonsFWMSd, RegionInoD, SkeletonsFWMSm)
	//3
	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_RecInoD, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDC);
	//area_center (SkeletonsFWMSd, Area, Row, Column)
	//tuple_min (Area, Min)
	Intersection(ho_SkeletonsFWMScutd, ho_RegionInoDC, &ho_SkeletonsFWMScutm);
	Union1(ho_SkeletonsFWMScutd, &ho_SkeletonsFWMScutdu);
	Intersection(ho_SkeletonsFWMScutdu, ho_RegionInoDC, &ho_SkeletonsFWMScutmu);
	Connection(ho_SkeletonsFWMScutmu, &ho_SkeletonsFWMScutm2);
	//3
	//difference (SkeletonsFWMSm, SkeletonsFWMSm2, RegionDifference1)


	//area_center (SkeletonsFWMSm, Area, Row, Column)
	//tuple_min (Area, amin)
	//count_obj (SkeletonsFWMSm, nm)

	//symm_difference (SkeletonsFWMSm, SkeletonsFWMSc, RegionDifference)

	//intersection (SkeletonsFWMS, RegionInoD, RegionIntersection)
	//union2 (SkeletonsFWMS, RegionIntersection, RegionUnion)
	//expand_region (RegionUnion, RegionInoD, RegionsFWMS, expsize, 'image')
	Union2(ho_RegionInoD, ho_EndPointsdu, &ho_RegionForbid);
	Difference(ho_SkeletonsFWMScutm, ho_EndPointsdu, &ho_SkeletonsFWMScutm2);
	ExpandRegion(ho_SkeletonsFWMScutm2, ho_RegionForbid, &ho_RegionsFWMSI, hv_expsize,
		"image");
	//expand_region (SkeletonsFWMS, RegionInoD, RegionsFWMS2, expsize, 'image')
	CountObj(ho_SkeletonsFWMScut, &hv_nRFWMS);
	hv_stseq = hv_nEmpty + 1;
	hv_overf = 255 - hv_stseq;
	TupleGenSequence(2, hv_nRFWMS + 1, 1, &hv_SequenceINBSi);
	hv_SequenceIRBS = (hv_SequenceINBSi%hv_overf) + hv_stseq;
	Boundary(ho_RegionsFWMSI, &ho_RegionsFWMSB, "inner");

	Boundary(ho_RegionsFWMSI, &ho_RegionsFWMSBii, "inner");
	Union1(ho_RegionsFWMSI, &ho_RegionFWMSI);
	Boundary(ho_RegionFWMSI, &ho_RegionInoDSB, "inner");
	Intersection(ho_RegionsFWMSBii, ho_RegionInoDSB, &ho_RegionsFWMSB);

	CountObj(ho_RegionsFWMSB, &(*hv_nFWMSB));
	PaintRegion(ho_RegionsFWMSI, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), 1, "fill");
	PaintRegion(ho_RegionsFWMSB, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), hv_SequenceINBSi,
		"fill");
	return;
}

void BuildIRBS41B2(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_SkelEP, HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty,
	HTuple hv_dil, HTuple *hv_nFWMSB)
{

	// Local iconic variables
	HObject  ho_SkeletonsFWMSd, ho_SkeletonsFWMSm;
	HObject  ho_RecInoD, ho_RegionInoDC, ho_SkeletonsFWMSdu;
	HObject  ho_SkeletonsFWMSmu, ho_SkeletonsFWMSm2, ho_RegForbid;
	HObject  ho_RegionsFWMS2i, ho_RegionsFWMS2, ho_RegionsFWMSB;

	// Local control variables
	HTuple  hv_no, hv_Row1, hv_Column1, hv_Row2, hv_Column2;
	HTuple  hv_nRFWMS, hv_stseq, hv_overf, hv_SequenceINBSi;
	HTuple  hv_SequenceIRBS;

	//****BuildIRBS41B2 - 18.04.2023 - removed difference for regions tuple
	(*ho_ImIRBSOut) = ho_ImIRBS;
	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, 2.5)
	//1+2
	hv_dil = 15;
	DilationCircle(ho_SkeletonsFWMS, &ho_SkeletonsFWMSd, hv_dil);
	CountObj(ho_SkeletonsFWMSd, &hv_no);

	//2
	//union1 (SkeletonsFWMS, SkeletonFWMS)
	//difference (SkeletonFWMS, RegionInoD, SkeletonFWMSdif)
	//connection (SkeletonFWMS, SkeletonsFWMSc)
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, dil)

	//count_obj (SkeletonsFWMSc, n2)


	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd2, dil)
	//count_obj (SkeletonsFWMSd2, n2)


	//1
	//difference (SkeletonsFWMSd, RegionInoD, SkeletonsFWMSm)
	//3
	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_RecInoD, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDC);
	//area_center (SkeletonsFWMSd, Area, Row, Column)
	//tuple_min (Area, Min)
	Intersection(ho_SkeletonsFWMSd, ho_RegionInoDC, &ho_SkeletonsFWMSm);
	Union1(ho_SkeletonsFWMSd, &ho_SkeletonsFWMSdu);
	Intersection(ho_SkeletonsFWMSdu, ho_RegionInoDC, &ho_SkeletonsFWMSmu);
	Connection(ho_SkeletonsFWMSmu, &ho_SkeletonsFWMSm2);
	//3
	//difference (SkeletonsFWMSm, SkeletonsFWMSm2, RegionDifference1)


	//area_center (SkeletonsFWMSm, Area, Row, Column)
	//tuple_min (Area, amin)
	//count_obj (SkeletonsFWMSm, nm)

	//symm_difference (SkeletonsFWMSm, SkeletonsFWMSc, RegionDifference)

	//intersection (SkeletonsFWMS, RegionInoD, RegionIntersection)
	//union2 (SkeletonsFWMS, RegionIntersection, RegionUnion)
	//expand_region (RegionUnion, RegionInoD, RegionsFWMS, expsize, 'image')
	Union2(ho_RegionInoD, ho_SkelEP, &ho_RegForbid);
	ExpandRegion(ho_SkeletonsFWMSm, ho_RegForbid, &ho_RegionsFWMS2i, hv_expsize, "image");
	//expand_region (SkeletonsFWMSm, RegionInoD, RegionsFWMS2, expsize, 'image')
	//expand_region (SkeletonsFWMS, RegionInoD, RegionsFWMS2, expsize, 'image')
	Difference(ho_RegionsFWMS2i, ho_SkelEP, &ho_RegionsFWMS2);
	CountObj(ho_SkeletonsFWMS, &hv_nRFWMS);
	hv_stseq = hv_nEmpty + 1;
	hv_overf = 255 - hv_stseq;
	TupleGenSequence(2, hv_nRFWMS + 1, 1, &hv_SequenceINBSi);
	hv_SequenceIRBS = (hv_SequenceINBSi%hv_overf) + hv_stseq;
	Boundary(ho_RegionsFWMS2, &ho_RegionsFWMSB, "inner");
	CountObj(ho_RegionsFWMSB, &(*hv_nFWMSB));
	PaintRegion(ho_RegionsFWMS2, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), 1, "fill");
	PaintRegion(ho_RegionsFWMSB, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), hv_SequenceINBSi,
		"fill");
	return;
}

void BuildIRBS42B3(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_DirImageS, HObject ho_WidthImageS, HObject ho_SkeletonGS, HObject ho_SkeletonsFWMScut,
	HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty, HTuple hv_dil, HTuple hv_stopperT,
	HTuple *hv_nFWMSB)
{

	// Local iconic variables
	HObject  ho_SkelSEP, ho_JuncPoints, ho_SkelSEPu;
	HObject  ho_hoSkeletonFWMS, ho_R2, ho_RegionIntersection;
	HObject  ho_EndPoints1, ho_JuncPoints1, ho_StCents, ho_StCentsu;
	HObject  ho_RSt, ho_SkeletonsFWMSd, ho_RecInoD, ho_RegionInoDC;
	HObject  ho_SkeletonsFWMSmi, ho_SkeletonsFWMSm, ho_RegForbid;
	HObject  ho_RegionsFWMS2, ho_RegionsFWMSB;

	// Local control variables
	HTuple  hv_RowsSEP, hv_ColumnsSEP, hv_AngS, hv_WidthS;
	HTuple  hv_Phi, hv_nEPS, hv_LSt, hv_RowsStC, hv_ColumnsStC;
	HTuple  hv_no, hv_Row1, hv_Column1, hv_Row2, hv_Column2;
	HTuple  hv_nRFWMS, hv_stseq, hv_overf, hv_SequenceINBSi;
	HTuple  hv_SequenceIRBS;

	HTuple hv_j, hv_n;
	HObject ho_R2s, ho_StCent, ho_RStj;

	//****BuildIRBS42B3 - 18.04.2023 - removed difference for regions tuple
	//* 12/09/2024 - changed to cut of the ends using rec2
	(*ho_ImIRBSOut) = ho_ImIRBS;

	JunctionsSkeleton(ho_SkeletonsFWMS, &ho_SkelSEP, &ho_JuncPoints);
	Union1(ho_SkelSEP, &ho_SkelSEPu);
	Union1(ho_SkeletonsFWMS, &ho_hoSkeletonFWMS);

	//count_obj (SkelSEP, nSEP)
	GetRegionPoints(ho_SkelSEPu, &hv_RowsSEP, &hv_ColumnsSEP);
	GetGrayval(ho_DirImageS, hv_RowsSEP, hv_ColumnsSEP, &hv_AngS);
	GetGrayval(ho_WidthImageS, hv_RowsSEP, hv_ColumnsSEP, &hv_WidthS);
	//AngS := [AngS, 17,232]
	//RowsSEP := [RowsSEP, 913,663]
	//ColumnsSEP := [ColumnsSEP, 257,2254]
	//WidthS := [WidthS, 31,31]

	hv_Phi = (((hv_AngS - 128.) / 255.)*3.14) - (3.14 / 2.);
	//AngSt := (AngS)/255.*3.14
	//AngEnd := (AngS)/255.*3.14+3.14
	TupleLength(hv_AngS, &hv_nEPS);
	//stopperT := 7
	TupleGenConst(hv_nEPS, hv_stopperT, &hv_LSt);
	GenRectangle2(&ho_R2, hv_RowsSEP, hv_ColumnsSEP, hv_Phi, (hv_WidthS / 2.) + 7, hv_LSt + 4);

	GenEmptyObj(&ho_RSt);
	TupleLength(hv_RowsSEP, &hv_n);
	{
		HTuple end_val27 = hv_n;
		HTuple step_val27 = 1;
		for (hv_j = 1; hv_j.Continue(end_val27, step_val27); hv_j += step_val27)
		{
			SelectObj(ho_R2, &ho_R2s, hv_j);
			Intersection(ho_R2s, ho_SkeletonGS, &ho_RegionIntersection);
			JunctionsSkeleton(ho_RegionIntersection, &ho_EndPoints1, &ho_JuncPoints1);
			Difference(ho_EndPoints1, ho_hoSkeletonFWMS, &ho_StCent);
			GetRegionPoints(ho_StCent, &hv_RowsStC, &hv_ColumnsStC);
			GenRectangle2(&ho_RStj, hv_RowsStC, hv_ColumnsStC, HTuple(hv_Phi[hv_j - 1]), (HTuple(hv_WidthS[hv_j - 1]) / 2.) + 13,
				HTuple(hv_LSt[hv_j - 1]) + 2);
			ConcatObj(ho_RSt, ho_RStj, &ho_RSt);
		}
	}

	//gen_circle_sector (CircleSector, RowsSEP, ColumnsSEP, WidthS/2.+3, AngSt, AngEnd)
	/*Intersection(ho_R2, ho_SkeletonGS, &ho_RegionIntersection);
	JunctionsSkeleton(ho_RegionIntersection, &ho_EndPoints1, &ho_JuncPoints1);
	Difference(ho_EndPoints1, ho_hoSkeletonFWMS, &ho_StCents);
	Union1(ho_StCents, &ho_StCentsu);
	GetRegionPoints(ho_StCentsu, &hv_RowsStC, &hv_ColumnsStC);
	GenRectangle2(&ho_RSt, hv_RowsStC, hv_ColumnsStC, hv_Phi, (hv_WidthS / 2.) + 9, hv_LSt + 4);*/

	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, 2.5)
	//1+2
	//hv_dil = 7;
	DilationCircle(ho_SkeletonsFWMScut, &ho_SkeletonsFWMSd, hv_dil);
	CountObj(ho_SkeletonsFWMSd, &hv_no);

	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_RecInoD, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDC);
	//area_center (SkeletonsFWMSd, Area, Row, Column)
	//tuple_min (Area, Min)
	Intersection(ho_SkeletonsFWMSd, ho_RegionInoDC, &ho_SkeletonsFWMSmi);

	Difference(ho_SkeletonsFWMSmi, ho_RSt, &ho_SkeletonsFWMSm);

	//* difference(SkelEP, SkeletonsFWMSd, SkelEP1)
	Union2(ho_RegionInoD, ho_RSt, &ho_RegForbid);
	//expsize := 101
	ExpandRegion(ho_SkeletonsFWMSm, ho_RegForbid, &ho_RegionsFWMS2, hv_expsize, "image");
	//expand_region (SkeletonsFWMSm, RegionInoD, RegionsFWMS2, expsize, 'image')
	//expand_region (SkeletonsFWMS, RegionInoD, RegionsFWMS2, expsize, 'image')
	//difference (RegionsFWMS2i, SkelEP1, RegionsFWMS2)
	CountObj(ho_SkeletonsFWMScut, &hv_nRFWMS);
	hv_stseq = hv_nEmpty + 1;
	hv_overf = 255 - hv_stseq;
	TupleGenSequence(2, hv_nRFWMS + 1, 1, &hv_SequenceINBSi);
	hv_SequenceIRBS = (hv_SequenceINBSi%hv_overf) + hv_stseq;
	Boundary(ho_RegionsFWMS2, &ho_RegionsFWMSB, "inner");
	CountObj(ho_RegionsFWMSB, &(*hv_nFWMSB));
	PaintRegion(ho_RegionsFWMS2, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), 1, "fill");
	PaintRegion(ho_RegionsFWMSB, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), hv_SequenceINBSi,
		"fill");
	return;
}

void BuildIRBS42B3_GM(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_DirImageS, HObject ho_WidthImageS, HObject ho_SkeletonGS, HObject ho_SkeletonsFWMScut,
	HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty, HTuple hv_dil, HTuple hv_stopperT,
	HTuple *hv_nFWMSB)
{

	// Local iconic variables
	HObject  ho_SkelSEP, ho_JuncPoints, ho_SkelSEPu;
	HObject  ho_hoSkeletonFWMS, ho_R2, ho_RegionIntersection;
	HObject  ho_EndPoints1, ho_JuncPoints1, ho_StCents, ho_StCentsu;
	HObject  ho_RSt, ho_SkeletonsFWMSd, ho_RecInoD, ho_RegionInoDC;
	HObject  ho_SkeletonsFWMSmi, ho_SkeletonsFWMSm, ho_RegForbid;
	HObject  ho_RegionsFWMS2, ho_RegionsFWMSB;

	// Local control variables
	HTuple  hv_RowsSEP, hv_ColumnsSEP, hv_AngS, hv_WidthS;
	HTuple  hv_Phi, hv_nEPS, hv_LSt, hv_RowsStC, hv_ColumnsStC;
	HTuple  hv_no, hv_Row1, hv_Column1, hv_Row2, hv_Column2;
	HTuple  hv_nRFWMS, hv_stseq, hv_overf, hv_SequenceINBSi;
	HTuple  hv_SequenceIRBS;

	//****BuildIRBS42B3 - 18.04.2023 - removed difference for regions tuple
	//* 12/09/2024 - changed to cut of the ends using rec2
	(*ho_ImIRBSOut) = ho_ImIRBS;

	JunctionsSkeleton(ho_SkeletonsFWMS, &ho_SkelSEP, &ho_JuncPoints);
	Union1(ho_SkelSEP, &ho_SkelSEPu);
	Union1(ho_SkeletonsFWMS, &ho_hoSkeletonFWMS);

	//count_obj (SkelSEP, nSEP)
	GetRegionPoints(ho_SkelSEPu, &hv_RowsSEP, &hv_ColumnsSEP);
	GetGrayval(ho_DirImageS, hv_RowsSEP, hv_ColumnsSEP, &hv_AngS);
	GetGrayval(ho_WidthImageS, hv_RowsSEP, hv_ColumnsSEP, &hv_WidthS);
	//AngS := [AngS, 17,232]
	//RowsSEP := [RowsSEP, 913,663]
	//ColumnsSEP := [ColumnsSEP, 257,2254]
	//WidthS := [WidthS, 31,31]

	hv_Phi = (((hv_AngS - 128.) / 255.)*3.14) - (3.14 / 2.);
	//AngSt := (AngS)/255.*3.14
	//AngEnd := (AngS)/255.*3.14+3.14
	TupleLength(hv_AngS, &hv_nEPS);
	//stopperT := 7
	TupleGenConst(hv_nEPS, hv_stopperT, &hv_LSt);
	GenRectangle2(&ho_R2, hv_RowsSEP, hv_ColumnsSEP, hv_Phi, (hv_WidthS / 2.) + 2, hv_LSt + 4);
	//gen_circle_sector (CircleSector, RowsSEP, ColumnsSEP, WidthS/2.+3, AngSt, AngEnd)
	Intersection(ho_R2, ho_SkeletonGS, &ho_RegionIntersection);
	JunctionsSkeleton(ho_RegionIntersection, &ho_EndPoints1, &ho_JuncPoints1);
	Difference(ho_EndPoints1, ho_hoSkeletonFWMS, &ho_StCents);
	Union1(ho_StCents, &ho_StCentsu);
	GetRegionPoints(ho_StCentsu, &hv_RowsStC, &hv_ColumnsStC);
	GenRectangle2(&ho_RSt, hv_RowsStC, hv_ColumnsStC, hv_Phi, (hv_WidthS / 2.) + 9, hv_LSt + 4);

	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, 2.5)
	//1+2
	//hv_dil = 7;
	DilationCircle(ho_SkeletonsFWMScut, &ho_SkeletonsFWMSd, hv_dil);
	CountObj(ho_SkeletonsFWMSd, &hv_no);

	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_RecInoD, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDC);
	//area_center (SkeletonsFWMSd, Area, Row, Column)
	//tuple_min (Area, Min)
	Intersection(ho_SkeletonsFWMSd, ho_RegionInoDC, &ho_SkeletonsFWMSmi);

	Difference(ho_SkeletonsFWMSmi, ho_RSt, &ho_SkeletonsFWMSm);

	//* difference(SkelEP, SkeletonsFWMSd, SkelEP1)
	Union2(ho_RegionInoD, ho_RSt, &ho_RegForbid);
	//expsize := 101
	ExpandRegion(ho_SkeletonsFWMSm, ho_RegForbid, &ho_RegionsFWMS2, hv_expsize, "image");
	//expand_region (SkeletonsFWMSm, RegionInoD, RegionsFWMS2, expsize, 'image')
	//expand_region (SkeletonsFWMS, RegionInoD, RegionsFWMS2, expsize, 'image')
	//difference (RegionsFWMS2i, SkelEP1, RegionsFWMS2)
	CountObj(ho_SkeletonsFWMScut, &hv_nRFWMS);
	hv_stseq = hv_nEmpty + 1;
	hv_overf = 255 - hv_stseq;
	TupleGenSequence(2, hv_nRFWMS + 1, 1, &hv_SequenceINBSi);
	hv_SequenceIRBS = (hv_SequenceINBSi%hv_overf) + hv_stseq;
	Boundary(ho_RegionsFWMS2, &ho_RegionsFWMSB, "inner");
	CountObj(ho_RegionsFWMSB, &(*hv_nFWMSB));
	PaintRegion(ho_RegionsFWMS2, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), 1, "fill");
	PaintRegion(ho_RegionsFWMSB, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), hv_SequenceINBSi,
		"fill");
	return;
}

void BuildIRBS42B4(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_DirImageS, HObject ho_WidthImageS, HObject ho_SkeletonGS, HObject ho_SkeletonsFWMScut,
	HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty, HTuple hv_dil, HTuple hv_stopperT,
	HTuple *hv_nFWMSB)
{

	// Local iconic variables
	HObject  ho_SkelSEP, ho_JuncPoints, ho_SkelSEPu;
	HObject  ho_hoSkeletonFWMS, ho_R2, ho_RSt, ho_RegStC, ho_R2s;
	HObject  ho_RegionIntersection, ho_EndPoints1, ho_JuncPoints1;
	HObject  ho_StCent, ho_RStj, ho_RStC, ho_SkeletonsFWMSd;
	HObject  ho_RecInoD, ho_RegionInoDC, ho_SkeletonsFWMSmi;
	HObject  ho_SkeletonsFWMSm, ho_RStm, ho_RStmu, ho_RegStCu;
	HObject  ho_RStmm, ho_RStmseli, ho_RStmsel, ho_RegionIntersection1;
	HObject  ho_RegForbid, ho_SkeletonsFWMSmu, ho_ConnectedRegions;
	HObject  ho_SelectedRegions, ho_RegionUnion, ho_SkeletonsFWMSmm;
	HObject  ho_RegionsFWMS2, ho_RegionsFWMSB;

	// Local control variables
	HTuple  hv_RowsSEP, hv_ColumnsSEP, hv_AngS, hv_WidthS;
	HTuple  hv_Phi, hv_nEPS, hv_LSt, hv_n, hv_j, hv_RowsStC;
	HTuple  hv_ColumnsStC, hv_no, hv_Row1, hv_Column1, hv_Row2;
	HTuple  hv_Column2, hv_Area, hv_Row, hv_Column, hv_nRStm;
	HTuple  hv_AreaFWMSmu, hv_Row3, hv_Column3, hv_nRFWMS, hv_stseq;
	HTuple  hv_overf, hv_SequenceINBSi, hv_SequenceIRBS;
	HTuple hv_maxWS;

	//****BuildIRBS42B4
	//**18.04.2023 - removed difference for regions tuple
	//* 12/09/2024 - changed to cut of the ends using rec2
	//* 07/05/2025 - changed rec2 not to cut next space
	//* 11/06/2025 - made unsensitive to dil
	(*ho_ImIRBSOut) = ho_ImIRBS;

	JunctionsSkeleton(ho_SkeletonsFWMS, &ho_SkelSEP, &ho_JuncPoints);
	Union1(ho_SkelSEP, &ho_SkelSEPu);
	Union1(ho_SkeletonsFWMS, &ho_hoSkeletonFWMS);

	//count_obj (SkelSEP, nSEP)
	GetRegionPoints(ho_SkelSEPu, &hv_RowsSEP, &hv_ColumnsSEP);
	GetGrayval(ho_DirImageS, hv_RowsSEP, hv_ColumnsSEP, &hv_AngS);
	GetGrayval(ho_WidthImageS, hv_RowsSEP, hv_ColumnsSEP, &hv_WidthS);
	TupleMax(hv_WidthS, &hv_maxWS);
	//AngS := [AngS, 17,232]
	//RowsSEP := [RowsSEP, 913,663]
	//ColumnsSEP := [ColumnsSEP, 257,2254]
	//WidthS := [WidthS, 31,31]

	hv_Phi = (((hv_AngS - 128.) / 255.)*3.14) - (3.14 / 2.);
	//AngSt := (AngS)/255.*3.14
	//AngEnd := (AngS)/255.*3.14+3.14
	TupleLength(hv_AngS, &hv_nEPS);
	hv_stopperT = 7;
	TupleGenConst(hv_nEPS, hv_stopperT, &hv_LSt);
	GenRectangle2(&ho_R2, hv_RowsSEP, hv_ColumnsSEP, hv_Phi, (hv_WidthS / 2.) + hv_maxWS+2, hv_LSt + 4);
	//****** changed on 2025.03.18
	GenEmptyObj(&ho_RSt);
	GenEmptyObj(&ho_RegStC);
	TupleLength(hv_RowsSEP, &hv_n);
	{
		HTuple end_val31 = hv_n;
		HTuple step_val31 = 1;
		for (hv_j = 1; hv_j.Continue(end_val31, step_val31); hv_j += step_val31)
		{
			SelectObj(ho_R2, &ho_R2s, hv_j);
			Intersection(ho_R2s, ho_SkeletonGS, &ho_RegionIntersection);
			JunctionsSkeleton(ho_RegionIntersection, &ho_EndPoints1, &ho_JuncPoints1);
			Difference(ho_EndPoints1, ho_hoSkeletonFWMS, &ho_StCent);
			GetRegionPoints(ho_StCent, &hv_RowsStC, &hv_ColumnsStC);
			GenRectangle2(&ho_RStj, hv_RowsStC, hv_ColumnsStC, HTuple(hv_Phi[hv_j - 1]), (HTuple(hv_WidthS[hv_j - 1]) / 2.) + 7,
				HTuple(hv_LSt[hv_j - 1]) + 2);
			ConcatObj(ho_RSt, ho_RStj, &ho_RSt);
			GenRegionPoints(&ho_RStC, hv_RowsStC, hv_ColumnsStC);
			ConcatObj(ho_RegStC, ho_RStC, &ho_RegStC);
		}
	}

	//changed to upper on 2025.03.18 because in rare case sequence of points changes
	//gen_circle_sector (CircleSector, RowsSEP, ColumnsSEP, WidthS/2.+3, AngSt, AngEnd)
	//intersection (R2, SkeletonGS, RegionIntersection)
	//junctions_skeleton (RegionIntersection, EndPoints1, JuncPoints1)
	//difference (EndPoints1, hoSkeletonFWMS, StCents)
	//union1 (StCents, StCentsu)
	//get_region_points (StCentsu, RowsStC, ColumnsStC)
	//gen_rectangle2 (RSt, RowsStC, ColumnsStC, Phi, WidthS/2.+9, LSt+2)

	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, 2.5)
	//1+2
	//dil := 5
	DilationCircle(ho_SkeletonsFWMScut, &ho_SkeletonsFWMSd, hv_dil);
	CountObj(ho_SkeletonsFWMSd, &hv_no);

	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_RecInoD, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDC);
	//area_center (SkeletonsFWMSd, Area, Row, Column)
	//tuple_min (Area, Min)
	Intersection(ho_SkeletonsFWMSd, ho_RegionInoDC, &ho_SkeletonsFWMSmi);

	Difference(ho_SkeletonsFWMSmi, ho_RSt, &ho_SkeletonsFWMSm);

	//difference (SkelEP, SkeletonsFWMSd, SkelEP1)
	Difference(ho_RSt, ho_RegionInoD, &ho_RStm);
	Union1(ho_RStm, &ho_RStmu);
	Connection(ho_RStmu, &ho_RStm);
	CountObj(ho_RStm, &hv_nRStm);
	Union1(ho_RegStC, &ho_RegStCu);
	GenEmptyObj(&ho_RStmm);
	{
		HTuple end_val75 = hv_nRStm;
		HTuple step_val75 = 1;
		for (hv_j = 1; hv_j.Continue(end_val75, step_val75); hv_j += step_val75)
		{
			SelectObj(ho_RStm, &ho_RStmseli, hv_j);
			DilationCircle(ho_RStmseli, &ho_RStmsel, 5);
			Intersection(ho_RStmsel, ho_RegStCu, &ho_RegionIntersection1);
			AreaCenter(ho_RegionIntersection1, &hv_Area, &hv_Row, &hv_Column);
			if (0 != hv_Area)
			{
				ConcatObj(ho_RStmm, ho_RStmsel, &ho_RStmm);
			}
		}
	}

	Union2(ho_RegionInoD, ho_RStmm, &ho_RegForbid);
	Union1(ho_SkeletonsFWMSm, &ho_SkeletonsFWMSmu);
	AreaCenter(ho_SkeletonsFWMSmu, &hv_AreaFWMSmu, &hv_Row3, &hv_Column3);
	Connection(ho_SkeletonsFWMSmu, &ho_ConnectedRegions);
	CountObj(ho_ConnectedRegions, &hv_n);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 1, hv_AreaFWMSmu / 4);
	Union1(ho_SelectedRegions, &ho_RegionUnion);
	Difference(ho_SkeletonsFWMSm, ho_RegionUnion, &ho_SkeletonsFWMSmm);

	//expsize := 101
	ExpandRegion(ho_SkeletonsFWMSmm, ho_RegForbid, &ho_RegionsFWMS2, hv_expsize, "image");
	//expand_region (SkeletonsFWMSm, RegionInoD, RegionsFWMS2, expsize, 'image')
	//expand_region (SkeletonsFWMS, RegionInoD, RegionsFWMS2, expsize, 'image')
	//difference (RegionsFWMS2i, SkelEP1, RegionsFWMS2)
	CountObj(ho_SkeletonsFWMScut, &hv_nRFWMS);
	hv_stseq = hv_nEmpty + 1;
	hv_overf = 255 - hv_stseq;
	TupleGenSequence(2, hv_nRFWMS + 1, 1, &hv_SequenceINBSi);
	hv_SequenceIRBS = (hv_SequenceINBSi%hv_overf) + hv_stseq;
	Boundary(ho_RegionsFWMS2, &ho_RegionsFWMSB, "inner");
	CountObj(ho_RegionsFWMSB, &(*hv_nFWMSB));
	PaintRegion(ho_RegionsFWMS2, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), 1, "fill");
	PaintRegion(ho_RegionsFWMSB, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), hv_SequenceINBSi,
		"fill");
	return;
}



void BuildIRBS42B4o(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_DirImageS, HObject ho_WidthImageS, HObject ho_SkeletonGS, HObject ho_SkeletonsFWMScut,
	HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty, HTuple hv_dil, HTuple hv_stopperT,
	HTuple *hv_nFWMSB)
{

	// Local iconic variables
	HObject  ho_SkelSEP, ho_JuncPoints, ho_SkelSEPu;
	HObject  ho_hoSkeletonFWMS, ho_R2, ho_RSt, ho_RegStC, ho_R2s;
	HObject  ho_RegionIntersection, ho_EndPoints1, ho_JuncPoints1;
	HObject  ho_StCent, ho_RStj, ho_RStC, ho_SkeletonsFWMSd;
	HObject  ho_RecInoD, ho_RegionInoDC, ho_SkeletonsFWMSmi;
	HObject  ho_SkeletonsFWMSm, ho_RStm, ho_RStmu, ho_RegStCu;
	HObject  ho_RStmm, ho_RStmseli, ho_RStmsel, ho_RegionIntersection1, ho_RegForbid;
	HObject  ho_RegionsFWMS2, ho_RegionsFWMSB;

	// Local control variables
	HTuple  hv_RowsSEP, hv_ColumnsSEP, hv_AngS, hv_WidthS;
	HTuple  hv_Phi, hv_nEPS, hv_LSt, hv_n, hv_j, hv_RowsStC;
	HTuple  hv_ColumnsStC, hv_no, hv_Row1, hv_Column1, hv_Row2;
	HTuple  hv_Column2, hv_Area, hv_Row, hv_Column, hv_nRStm;
	HTuple  hv_nRFWMS, hv_stseq, hv_overf, hv_SequenceINBSi;
	HTuple  hv_SequenceIRBS;

	//****BuildIRBS42B4
	//**18.04.2023 - removed difference for regions tuple
	//* 12/09/2024 - changed to cut of the ends using rec2
	//* 07/05/2025 - changed rec2 not to cut next space
	(*ho_ImIRBSOut) = ho_ImIRBS;

	JunctionsSkeleton(ho_SkeletonsFWMS, &ho_SkelSEP, &ho_JuncPoints);
	Union1(ho_SkelSEP, &ho_SkelSEPu);
	Union1(ho_SkeletonsFWMS, &ho_hoSkeletonFWMS);

	//count_obj (SkelSEP, nSEP)
	GetRegionPoints(ho_SkelSEPu, &hv_RowsSEP, &hv_ColumnsSEP);
	GetGrayval(ho_DirImageS, hv_RowsSEP, hv_ColumnsSEP, &hv_AngS);
	GetGrayval(ho_WidthImageS, hv_RowsSEP, hv_ColumnsSEP, &hv_WidthS);
	//AngS := [AngS, 17,232]
	//RowsSEP := [RowsSEP, 913,663]
	//ColumnsSEP := [ColumnsSEP, 257,2254]
	//WidthS := [WidthS, 31,31]

	hv_Phi = (((hv_AngS - 128.) / 255.)*3.14) - (3.14 / 2.);
	//AngSt := (AngS)/255.*3.14
	//AngEnd := (AngS)/255.*3.14+3.14
	TupleLength(hv_AngS, &hv_nEPS);
	hv_stopperT = 7;
	TupleGenConst(hv_nEPS, hv_stopperT, &hv_LSt);
	GenRectangle2(&ho_R2, hv_RowsSEP, hv_ColumnsSEP, hv_Phi, (hv_WidthS / 2.) + 13, hv_LSt + 4);
	//****** changed on 2025.03.18
	GenEmptyObj(&ho_RSt);
	GenEmptyObj(&ho_RegStC);
	TupleLength(hv_RowsSEP, &hv_n);
	{
		HTuple end_val30 = hv_n;
		HTuple step_val30 = 1;
		for (hv_j = 1; hv_j.Continue(end_val30, step_val30); hv_j += step_val30)
		{
			SelectObj(ho_R2, &ho_R2s, hv_j);
			Intersection(ho_R2s, ho_SkeletonGS, &ho_RegionIntersection);
			JunctionsSkeleton(ho_RegionIntersection, &ho_EndPoints1, &ho_JuncPoints1);
			Difference(ho_EndPoints1, ho_hoSkeletonFWMS, &ho_StCent);
			GetRegionPoints(ho_StCent, &hv_RowsStC, &hv_ColumnsStC);
			GenRectangle2(&ho_RStj, hv_RowsStC, hv_ColumnsStC, HTuple(hv_Phi[hv_j - 1]), (HTuple(hv_WidthS[hv_j - 1]) / 2.) + 14,
				HTuple(hv_LSt[hv_j - 1]) + 2);
			ConcatObj(ho_RSt, ho_RStj, &ho_RSt);
			GenRegionPoints(&ho_RStC, hv_RowsStC, hv_ColumnsStC);
			ConcatObj(ho_RegStC, ho_RStC, &ho_RegStC);
		}
	}

	//changed to upper on 2025.03.18 because in rare case sequence of points changes
	//gen_circle_sector (CircleSector, RowsSEP, ColumnsSEP, WidthS/2.+3, AngSt, AngEnd)
	//intersection (R2, SkeletonGS, RegionIntersection)
	//junctions_skeleton (RegionIntersection, EndPoints1, JuncPoints1)
	//difference (EndPoints1, hoSkeletonFWMS, StCents)
	//union1 (StCents, StCentsu)
	//get_region_points (StCentsu, RowsStC, ColumnsStC)
	//gen_rectangle2 (RSt, RowsStC, ColumnsStC, Phi, WidthS/2.+9, LSt+2)

	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, 2.5)
	//1+2
	//dil := 5
	DilationCircle(ho_SkeletonsFWMScut, &ho_SkeletonsFWMSd, hv_dil);
	CountObj(ho_SkeletonsFWMSd, &hv_no);

	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_RecInoD, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDC);
	//area_center (SkeletonsFWMSd, Area, Row, Column)
	//tuple_min (Area, Min)
	Intersection(ho_SkeletonsFWMSd, ho_RegionInoDC, &ho_SkeletonsFWMSmi);

	Difference(ho_SkeletonsFWMSmi, ho_RSt, &ho_SkeletonsFWMSm);

	//difference (SkelEP, SkeletonsFWMSd, SkelEP1)
	Difference(ho_RSt, ho_RegionInoD, &ho_RStm);
	Union1(ho_RStm, &ho_RStmu);
	Connection(ho_RStmu, &ho_RStm);
	CountObj(ho_RStm, &hv_nRStm);
	Union1(ho_RegStC, &ho_RegStCu);
	GenEmptyObj(&ho_RStmm);
	{
		HTuple end_val74 = hv_nRStm;
		HTuple step_val74 = 1;
		for (hv_j = 1; hv_j.Continue(end_val74, step_val74); hv_j += step_val74)
		{
			SelectObj(ho_RStm, &ho_RStmseli, hv_j);
			DilationCircle(ho_RStmseli, &ho_RStmsel, 3);
			Intersection(ho_RStmsel, ho_RegStCu, &ho_RegionIntersection1);
			AreaCenter(ho_RegionIntersection1, &hv_Area, &hv_Row, &hv_Column);
			if (0 != hv_Area)
			{
				ConcatObj(ho_RStmm, ho_RStmsel, &ho_RStmm);
			}
		}
	}

	Union2(ho_RegionInoD, ho_RStmm, &ho_RegForbid);
	//expsize := 101
	ExpandRegion(ho_SkeletonsFWMSm, ho_RegForbid, &ho_RegionsFWMS2, hv_expsize, "image");
	//expand_region (SkeletonsFWMSm, RegionInoD, RegionsFWMS2, expsize, 'image')
	//expand_region (SkeletonsFWMS, RegionInoD, RegionsFWMS2, expsize, 'image')
	//difference (RegionsFWMS2i, SkelEP1, RegionsFWMS2)
	CountObj(ho_SkeletonsFWMScut, &hv_nRFWMS);
	hv_stseq = hv_nEmpty + 1;
	hv_overf = 255 - hv_stseq;
	TupleGenSequence(2, hv_nRFWMS + 1, 1, &hv_SequenceINBSi);
	hv_SequenceIRBS = (hv_SequenceINBSi%hv_overf) + hv_stseq;
	Boundary(ho_RegionsFWMS2, &ho_RegionsFWMSB, "inner");
	CountObj(ho_RegionsFWMSB, &(*hv_nFWMSB));
	PaintRegion(ho_RegionsFWMS2, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), 1, "fill");
	PaintRegion(ho_RegionsFWMSB, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), hv_SequenceINBSi,
		"fill");
	return;
}

//void BuildIRBS4B2(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
//	HObject ho_SkelSEP, HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty,
//	HTuple hv_dil, HTuple *hv_nFWMSB)
//{
//
//	// Local iconic variables
//	HObject  ho_SkeletonsFWMSd, ho_SkeletonsFWMSm;
//	HObject  ho_RecInoD, ho_RegionInoDC, ho_SkeletonsFWMSdu;
//	HObject  ho_SkeletonsFWMSmu, ho_SkeletonsFWMSm2, ho_RForbid;
//	HObject  ho_RegionsFWMSI, ho_RegionsFWMSB, ho_RegionsFWMSBii;
//	HObject  ho_RegionFWMSI, ho_RegionInoDSB;
//
//	// Local control variables
//	HTuple  hv_no, hv_Row1, hv_Column1, hv_Row2, hv_Column2;
//	HTuple  hv_nRFWMS, hv_stseq, hv_overf, hv_SequenceINBSi;
//	HTuple  hv_SequenceIRBS;
//
//	//****BuildIRBS4 - 18.04.2023 - removed difference for regions tuple
//	//13.03.2024 - modified
//	(*ho_ImIRBSOut) = ho_ImIRBS;
//	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
//	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, 2.5)
//	//1+2
//	hv_dil = 3;
//	DilationCircle(ho_SkeletonsFWMS, &ho_SkeletonsFWMSd, hv_dil);
//	CountObj(ho_SkeletonsFWMSd, &hv_no);
//
//	//2
//	//union1 (SkeletonsFWMS, SkeletonFWMS)
//	//difference (SkeletonFWMS, RegionInoD, SkeletonFWMSdif)
//	//connection (SkeletonFWMS, SkeletonsFWMSc)
//	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, dil)
//
//	//count_obj (SkeletonsFWMSc, n2)
//
//
//	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd2, dil)
//	//count_obj (SkeletonsFWMSd2, n2)
//
//
//	//1
//	//difference (SkeletonsFWMSd, RegionInoD, SkeletonsFWMSm)
//	//3
//	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
//	GenRectangle1(&ho_RecInoD, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
//	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDC);
//	//area_center (SkeletonsFWMSd, Area, Row, Column)
//	//tuple_min (Area, Min)
//	Intersection(ho_SkeletonsFWMSd, ho_RegionInoDC, &ho_SkeletonsFWMSm);
//	Union1(ho_SkeletonsFWMSd, &ho_SkeletonsFWMSdu);
//	Intersection(ho_SkeletonsFWMSdu, ho_RegionInoDC, &ho_SkeletonsFWMSmu);
//	Connection(ho_SkeletonsFWMSmu, &ho_SkeletonsFWMSm2);
//	//3
//	//difference (SkeletonsFWMSm, SkeletonsFWMSm2, RegionDifference1)
//
//
//	//area_center (SkeletonsFWMSm, Area, Row, Column)
//	//tuple_min (Area, amin)
//	//count_obj (SkeletonsFWMSm, nm)
//
//	//symm_difference (SkeletonsFWMSm, SkeletonsFWMSc, RegionDifference)
//
//	//intersection (SkeletonsFWMS, RegionInoD, RegionIntersection)
//	//union2 (SkeletonsFWMS, RegionIntersection, RegionUnion)
//	//expand_region (RegionUnion, RegionInoD, RegionsFWMS, expsize, 'image')
//	Union2(ho_RegionInoD, ho_SkelSEP, &ho_RForbid);
//	//ExpandRegion(ho_SkeletonsFWMSm, ho_RForbid, &ho_RegionsFWMSI, hv_expsize, "image");
//	ExpandRegion(ho_SkeletonsFWMSm, ho_RForbid, &ho_RegionsFWMSI, "maximal", "image");
//
//	//difference (RegionsFWMSIi, SkelSEP, RegionsFWMSI)
//
//	//expand_region (SkeletonsFWMS, RegionInoD, RegionsFWMS2, expsize, 'image')
//	CountObj(ho_SkeletonsFWMS, &hv_nRFWMS);
//	hv_stseq = hv_nEmpty + 1;
//	hv_overf = 255 - hv_stseq;
//	TupleGenSequence(2, hv_nRFWMS + 1, 1, &hv_SequenceINBSi);
//	hv_SequenceIRBS = (hv_SequenceINBSi%hv_overf) + hv_stseq;
//	Boundary(ho_RegionsFWMSI, &ho_RegionsFWMSB, "inner");
//
//	Boundary(ho_RegionsFWMSI, &ho_RegionsFWMSBii, "inner");
//	Union1(ho_RegionsFWMSI, &ho_RegionFWMSI);
//	Boundary(ho_RegionFWMSI, &ho_RegionInoDSB, "inner");
//	Intersection(ho_RegionsFWMSBii, ho_RegionInoDSB, &ho_RegionsFWMSB);
//
//	CountObj(ho_RegionsFWMSB, &(*hv_nFWMSB));
//	PaintRegion(ho_RegionsFWMSI, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), 1, "fill");
//	PaintRegion(ho_RegionsFWMSB, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), hv_SequenceINBSi,
//		"fill");
//	return;
//}

void BuildIRBS4B2(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty, HTuple hv_dil, HTuple *hv_nFWMSB)
{

	// Local iconic variables
	HObject  ho_SkeletonsFWMSd, ho_SkeletonsFWMSm;
	HObject  ho_RecInoD, ho_RegionInoDC, ho_SkeletonsFWMSdu;
	HObject  ho_SkeletonsFWMSmu, ho_SkeletonsFWMSm2, ho_RegionsFWMS2;
	HObject  ho_RegionsFWMSB;

	// Local control variables
	HTuple  hv_no, hv_Row1, hv_Column1, hv_Row2, hv_Column2;
	HTuple  hv_nRFWMS, hv_stseq, hv_overf, hv_SequenceINBSi;
	HTuple  hv_SequenceIRBS;

	//****BuildIRBS4 - 18.04.2023 - removed difference for regions tuple
	(*ho_ImIRBSOut) = ho_ImIRBS;
	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, 2.5)
	//1+2
	hv_dil = 3;
	DilationCircle(ho_SkeletonsFWMS, &ho_SkeletonsFWMSd, hv_dil);
	CountObj(ho_SkeletonsFWMSd, &hv_no);

	//2
	//union1 (SkeletonsFWMS, SkeletonFWMS)
	//difference (SkeletonFWMS, RegionInoD, SkeletonFWMSdif)
	//connection (SkeletonFWMS, SkeletonsFWMSc)
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, dil)

	//count_obj (SkeletonsFWMSc, n2)


	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd2, dil)
	//count_obj (SkeletonsFWMSd2, n2)


	//1
	//difference (SkeletonsFWMSd, RegionInoD, SkeletonsFWMSm)
	//3
	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_RecInoD, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDC);
	//area_center (SkeletonsFWMSd, Area, Row, Column)
	//tuple_min (Area, Min)
	Intersection(ho_SkeletonsFWMSd, ho_RegionInoDC, &ho_SkeletonsFWMSm);
	Union1(ho_SkeletonsFWMSd, &ho_SkeletonsFWMSdu);
	Intersection(ho_SkeletonsFWMSdu, ho_RegionInoDC, &ho_SkeletonsFWMSmu);
	Connection(ho_SkeletonsFWMSmu, &ho_SkeletonsFWMSm2);
	//3
	//difference (SkeletonsFWMSm, SkeletonsFWMSm2, RegionDifference1)


	//area_center (SkeletonsFWMSm, Area, Row, Column)
	//tuple_min (Area, amin)
	//count_obj (SkeletonsFWMSm, nm)

	//symm_difference (SkeletonsFWMSm, SkeletonsFWMSc, RegionDifference)

	//intersection (SkeletonsFWMS, RegionInoD, RegionIntersection)
	//union2 (SkeletonsFWMS, RegionIntersection, RegionUnion)
	//expand_region (RegionUnion, RegionInoD, RegionsFWMS, expsize, 'image')
	ExpandRegion(ho_SkeletonsFWMSm, ho_RegionInoD, &ho_RegionsFWMS2, hv_expsize, "image");
	//expand_region (SkeletonsFWMS, RegionInoD, RegionsFWMS2, expsize, 'image')
	CountObj(ho_SkeletonsFWMS, &hv_nRFWMS);
	hv_stseq = hv_nEmpty + 1;
	hv_overf = 255 - hv_stseq;
	TupleGenSequence(2, hv_nRFWMS + 1, 1, &hv_SequenceINBSi);
	hv_SequenceIRBS = (hv_SequenceINBSi%hv_overf) + hv_stseq;
	Boundary(ho_RegionsFWMS2, &ho_RegionsFWMSB, "inner");
	CountObj(ho_RegionsFWMSB, &(*hv_nFWMSB));
	PaintRegion(ho_RegionsFWMS2, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), 1, "fill");
	PaintRegion(ho_RegionsFWMSB, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), hv_SequenceINBSi,
		"fill");
	return;
}






void BuildRegionInoD(HObject ho_RegionsShi, HObject ho_SkeletonGS, HObject ho_RegionInoIslnoPh,
	HObject ho_RegionOpCon, HObject ho_R, HObject *ho_RegionInoD)
{

	// Local iconic variables
	HObject  ho_RegionShUnion, ho_RegionSkIntersection;
	HObject  ho_RegionRemoveSh, ho_RegionI1noPh5noIsl1noSh1;
	HObject  ho_RegionsSh, ho_RegionInoDi;

	// Local control variables
	HTuple  hv_nShi, hv_nSh;
	//Union1(ho_RegionsShi, &ho_RegionRemoveSh);
	Union1(ho_RegionsShi, &ho_RegionShUnion);
	ho_RegionRemoveSh = ho_RegionShUnion;
	//Intersection(ho_RegionShUnion, ho_SkeletonGS, &ho_RegionSkIntersection);
	//DilationCircle(ho_RegionSkIntersection, &ho_RegionRemoveSh, 1.5);
	//GenEmptyObj(&ho_RegionRemoveSh);
	//****** fixing Shorts ******
	Difference(ho_RegionInoIslnoPh, ho_RegionRemoveSh, &ho_RegionI1noPh5noIsl1noSh1	);
	//difference (RegionI1noPh5noIsl1, RegionRemoveSh, RegionI1noPh5noIsl1noSh1)
	//difference (RegionI2noPh5noIsl1, RegionRemoveSh, RegionI2noPh5noIsl1noSh1)
	//difference (RegionI3noPh5noIsl1, RegionRemoveSh, RegionI3noPh5noIsl1noSh1)
	//difference (RegionI4noPh5noIsl1, RegionRemoveSh, RegionI4noPh5noIsl1noSh1)
	//difference (RegionI5noPh5noIsl1, RegionRemoveSh, RegionI5noPh5noIsl1noSh1)

	CountObj(ho_RegionsShi, &hv_nShi);
	Connection(ho_RegionShUnion, &ho_RegionsSh);
	CountObj(ho_RegionsSh, &hv_nSh);
	//**** final RegionInoD 1,2,3,4,5 ******
	Union2(ho_RegionI1noPh5noIsl1noSh1, ho_RegionOpCon, &ho_RegionInoDi);
	Difference(ho_RegionInoDi, ho_R, &(*ho_RegionInoD));
	return;
}
//{
//
//	// Local iconic variables
//	HObject  ho_RegionShUnion, ho_RegionSkIntersection;
//	HObject  ho_RegionRemoveSh, ho_RegionI1noPh5noIsl1noSh1;
//	HObject  ho_RegionsSh, ho_RegionInoDi;
//
//	// Local control variables
//	HTuple  hv_nShi, hv_nSh;
//	Intersection(ho_RegionInoIslnoPh, ho_RegionInoIslnoPh, &(*ho_RegionInoD));
//	//Union1(ho_RegionsShi, &ho_RegionShUnion);
//	//Intersection(ho_RegionShUnion, ho_SkeletonGS, &ho_RegionSkIntersection);
//	////dilation_circle (RegionSkIntersection, RegionRemoveSh, 1.5)
//	//GenEmptyObj(&ho_RegionRemoveSh);
//	////****** fixing Shorts ******
//	//Difference(ho_RegionInoIslnoPh, ho_RegionRemoveSh, &ho_RegionI1noPh5noIsl1noSh1);
//	////difference (RegionI1noPh5noIsl1, RegionRemoveSh, RegionI1noPh5noIsl1noSh1)
//	////difference (RegionI2noPh5noIsl1, RegionRemoveSh, RegionI2noPh5noIsl1noSh1)
//	////difference (RegionI3noPh5noIsl1, RegionRemoveSh, RegionI3noPh5noIsl1noSh1)
//	////difference (RegionI4noPh5noIsl1, RegionRemoveSh, RegionI4noPh5noIsl1noSh1)
//	////difference (RegionI5noPh5noIsl1, RegionRemoveSh, RegionI5noPh5noIsl1noSh1)
//
//	//CountObj(ho_RegionsShi, &hv_nShi);
//	//Connection(ho_RegionShUnion, &ho_RegionsSh);
//	//CountObj(ho_RegionsSh, &hv_nSh);
//	////**** final RegionInoD 1,2,3,4,5 ******
//	//Union2(ho_RegionI1noPh5noIsl1noSh1, ho_RegionOpCon, &ho_RegionInoDi);
//	//Difference(ho_RegionInoDi, ho_R, &(*ho_RegionInoD));
//	return;
//}


void BuildIRBS6B2(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_SkelSEP, HObject ho_RegionCut, HObject *ho_ImIRBSOut, HTuple hv_expsize,
	HTuple hv_nEmpty, HTuple hv_dil, HTuple *hv_nFWMSB)
{

	// Local iconic variables
	HObject  ho_SkeletonsFWMSd, ho_SkeletonsFWMSm;
	HObject  ho_RecInoD, ho_RegionInoDC, ho_SkeletonsFWMSdu;
	HObject  ho_SkeletonsFWMSmu, ho_SkeletonsFWMSm2, ho_RForbidi;
	HObject  ho_RForbid, ho_RegionsFWMSI, ho_RegionsFWMSIi, ho_RegionsFWMSB;
	HObject  ho_RegionsFWMSBii, ho_RegionFWMSI, ho_RegionInoDSB;

	// Local control variables
	HTuple  hv_no, hv_Row1, hv_Column1, hv_Row2, hv_Column2;
	HTuple  hv_nRFWMS, hv_stseq, hv_overf, hv_SequenceINBSi;
	HTuple  hv_SequenceIRBS;

	//****BuildIRBS6B2
	//- 18.04.2023 - removed difference for regions tuple
	//13.03.2024 - modified
	//28.03.2024 - cur region added
	(*ho_ImIRBSOut) = ho_ImIRBS;
	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, 2.5)
	//1+2
	hv_dil = 3;
	DilationCircle(ho_SkeletonsFWMS, &ho_SkeletonsFWMSd, hv_dil);
	CountObj(ho_SkeletonsFWMSd, &hv_no);

	//2
	//union1 (SkeletonsFWMS, SkeletonFWMS)
	//difference (SkeletonFWMS, RegionInoD, SkeletonFWMSdif)
	//connection (SkeletonFWMS, SkeletonsFWMSc)
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, dil)

	//count_obj (SkeletonsFWMSc, n2)


	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd2, dil)
	//count_obj (SkeletonsFWMSd2, n2)


	//1
	//difference (SkeletonsFWMSd, RegionInoD, SkeletonsFWMSm)
	//3
	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_RecInoD, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDC);
	//area_center (SkeletonsFWMSd, Area, Row, Column)
	//tuple_min (Area, Min)
	Intersection(ho_SkeletonsFWMSd, ho_RegionInoDC, &ho_SkeletonsFWMSm);
	Union1(ho_SkeletonsFWMSd, &ho_SkeletonsFWMSdu);
	Intersection(ho_SkeletonsFWMSdu, ho_RegionInoDC, &ho_SkeletonsFWMSmu);
	Connection(ho_SkeletonsFWMSmu, &ho_SkeletonsFWMSm2);
	//3
	//difference (SkeletonsFWMSm, SkeletonsFWMSm2, RegionDifference1)


	//area_center (SkeletonsFWMSm, Area, Row, Column)
	//tuple_min (Area, amin)
	//count_obj (SkeletonsFWMSm, nm)

	//symm_difference (SkeletonsFWMSm, SkeletonsFWMSc, RegionDifference)

	//intersection (SkeletonsFWMS, RegionInoD, RegionIntersection)
	//union2 (SkeletonsFWMS, RegionIntersection, RegionUnion)
	//expand_region (RegionUnion, RegionInoD, RegionsFWMS, expsize, 'image')
	Union2(ho_RegionInoD, ho_SkelSEP, &ho_RForbidi);
	Union2(ho_RForbidi, ho_RegionCut, &ho_RForbid);
	//expand_region (SkeletonsFWMSm, RForbid, RegionsFWMSI, expsize, 'image')
	//expand_region (SkeletonsFWMSm, RForbid, RegionsFWMSI, expsize, 'image')
	ExpandRegion(ho_SkeletonsFWMSm, ho_RForbid, &ho_RegionsFWMSIi, "maximal", "image");
	ExpandRegion(ho_RegionsFWMSIi, ho_RForbidi, &ho_RegionsFWMSI, 1, "image");
	//boundary (RegionInoDC, RegionInoDB, 'inner')
	//difference (RegionsFWMSIi, SkelSEP, RegionsFWMSI)

	//expand_region (SkeletonsFWMS, RegionInoD, RegionsFWMS2, expsize, 'image')
	CountObj(ho_SkeletonsFWMS, &hv_nRFWMS);
	hv_stseq = hv_nEmpty + 1;
	hv_overf = 255 - hv_stseq;
	TupleGenSequence(2, hv_nRFWMS + 1, 1, &hv_SequenceINBSi);
	hv_SequenceIRBS = (hv_SequenceINBSi%hv_overf) + hv_stseq;
	Boundary(ho_RegionsFWMSI, &ho_RegionsFWMSB, "inner");

	Boundary(ho_RegionsFWMSI, &ho_RegionsFWMSBii, "inner");
	Union1(ho_RegionsFWMSI, &ho_RegionFWMSI);
	Boundary(ho_RegionFWMSI, &ho_RegionInoDSB, "inner");
	Intersection(ho_RegionsFWMSBii, ho_RegionInoDSB, &ho_RegionsFWMSB);

	CountObj(ho_RegionsFWMSB, &(*hv_nFWMSB));
	PaintRegion(ho_RegionsFWMSI, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), 1, "fill");
	PaintRegion(ho_RegionsFWMSB, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), hv_SequenceINBSi,
		"fill");
	return;
}

/*
void BuildIRBS6B2(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_SkelSEP, HObject ho_RegionCut, 
	HObject *ho_ImIRBSOut, 
	HTuple hv_expsize, HTuple hv_nEmpty, HTuple hv_dil, 
	HTuple *hv_nFWMSB)
{

	// Local iconic variables
	HObject  ho_SkeletonsFWMSd, ho_SkeletonsFWMSm;
	HObject  ho_RecInoD, ho_RegionInoDC, ho_SkeletonsFWMSdu;
	HObject  ho_SkeletonsFWMSmu, ho_SkeletonsFWMSm2, ho_RForbidi;
	HObject  ho_RForbid, ho_RegionsFWMSI, ho_RegionsFWMSIi, ho_RegionsFWMSB;
	HObject  ho_RegionsFWMSBii, ho_RegionFWMSI, ho_RegionInoDSB;

	// Local control variables
	HTuple  hv_no, hv_Row1, hv_Column1, hv_Row2, hv_Column2;
	HTuple  hv_nRFWMS, hv_stseq, hv_overf, hv_SequenceINBSi;
	HTuple  hv_SequenceIRBS;

	//****BuildIRBS6B2
	//- 18.04.2023 - removed difference for regions tuple
	//13.03.2024 - modified
	//28.03.2024 - cur region added
	(*ho_ImIRBSOut) = ho_ImIRBS;
	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, 2.5)
	//1+2
	hv_dil = 3;
	DilationCircle(ho_SkeletonsFWMS, &ho_SkeletonsFWMSd, hv_dil);
	CountObj(ho_SkeletonsFWMSd, &hv_no);

	//2
	//union1 (SkeletonsFWMS, SkeletonFWMS)
	//difference (SkeletonFWMS, RegionInoD, SkeletonFWMSdif)
	//connection (SkeletonFWMS, SkeletonsFWMSc)
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, dil)

	//count_obj (SkeletonsFWMSc, n2)


	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd2, dil)
	//count_obj (SkeletonsFWMSd2, n2)


	//1
	//difference (SkeletonsFWMSd, RegionInoD, SkeletonsFWMSm)
	//3
	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_RecInoD, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDC);
	//area_center (SkeletonsFWMSd, Area, Row, Column)
	//tuple_min (Area, Min)
	Intersection(ho_SkeletonsFWMSd, ho_RegionInoDC, &ho_SkeletonsFWMSm);
	Union1(ho_SkeletonsFWMSd, &ho_SkeletonsFWMSdu);
	Intersection(ho_SkeletonsFWMSdu, ho_RegionInoDC, &ho_SkeletonsFWMSmu);
	Connection(ho_SkeletonsFWMSmu, &ho_SkeletonsFWMSm2);
	//3
	//difference (SkeletonsFWMSm, SkeletonsFWMSm2, RegionDifference1)


	//area_center (SkeletonsFWMSm, Area, Row, Column)
	//tuple_min (Area, amin)
	//count_obj (SkeletonsFWMSm, nm)

	//symm_difference (SkeletonsFWMSm, SkeletonsFWMSc, RegionDifference)

	//intersection (SkeletonsFWMS, RegionInoD, RegionIntersection)
	//union2 (SkeletonsFWMS, RegionIntersection, RegionUnion)
	//expand_region (RegionUnion, RegionInoD, RegionsFWMS, expsize, 'image')
	Union2(ho_RegionInoD, ho_SkelSEP, &ho_RForbidi);
	Union2(ho_RForbidi, ho_RegionCut, &ho_RForbid);
	//expand_region (SkeletonsFWMSm, RForbid, RegionsFWMSI, expsize, 'image')
	//expand_region (SkeletonsFWMSm, RForbid, RegionsFWMSI, expsize, 'image')
	ExpandRegion(ho_SkeletonsFWMSm, ho_RForbid, &ho_RegionsFWMSIi, "maximal", "image");
	ExpandRegion(ho_RegionsFWMSIi, ho_RForbidi, &ho_RegionsFWMSI, 1, "image");
	//boundary (RegionInoDC, RegionInoDB, 'inner')
	//difference (RegionsFWMSIi, SkelSEP, RegionsFWMSI)

	//expand_region (SkeletonsFWMS, RegionInoD, RegionsFWMS2, expsize, 'image')
	CountObj(ho_SkeletonsFWMS, &hv_nRFWMS);
	hv_stseq = hv_nEmpty + 1;
	hv_overf = 255 - hv_stseq;
	TupleGenSequence(2, hv_nRFWMS + 1, 1, &hv_SequenceINBSi);
	hv_SequenceIRBS = (hv_SequenceINBSi%hv_overf) + hv_stseq;
	Boundary(ho_RegionsFWMSI, &ho_RegionsFWMSB, "inner");

	Boundary(ho_RegionsFWMSI, &ho_RegionsFWMSBii, "inner");
	Union1(ho_RegionsFWMSI, &ho_RegionFWMSI);
	Boundary(ho_RegionFWMSI, &ho_RegionInoDSB, "inner");
	Intersection(ho_RegionsFWMSBii, ho_RegionInoDSB, &ho_RegionsFWMSB);

	CountObj(ho_RegionsFWMSB, &(*hv_nFWMSB));

	if (1)
	{
		WriteObject(ho_RegionsFWMSI, "C:\\Temp\\ho_RegionsFWMSI");
		WriteObject(ho_RegionCut, "C:\\Temp\\ho_RegionCut");
		//WriteImage(m_local_Im, "jpg", 0, "C:\\PATTERN\\AOI_102723\\m_local_image");
	}
	PaintRegion(ho_RegionsFWMSI, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), 1, "fill");
	PaintRegion(ho_RegionsFWMSB, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), hv_SequenceINBSi,
		"fill");
	return;
}
*/

void BuildIRBS42B2(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_SkelEP, HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty,
	HTuple hv_dil, HTuple *hv_nFWMSB)
{

	// Local iconic variables
	HObject  ho_SkeletonsFWMSd, ho_SkeletonsFWMSm;
	HObject  ho_RecInoD, ho_RegionInoDC, ho_SkeletonsFWMSdu;
	HObject  ho_SkeletonsFWMSmu, ho_SkeletonsFWMSm2, ho_SkelEP1;
	HObject  ho_RegForbid, ho_RegionsFWMS2i, ho_RegionsFWMS2;
	HObject  ho_RegionsFWMSB;

	// Local control variables
	HTuple  hv_no, hv_Row1, hv_Column1, hv_Row2, hv_Column2;
	HTuple  hv_nRFWMS, hv_stseq, hv_overf, hv_SequenceINBSi;
	HTuple  hv_SequenceIRBS;

	//****BuildIRBS42B2 - 18.04.2023 - removed difference for regions tuple
	(*ho_ImIRBSOut) = ho_ImIRBS;
	//* dif. is used to exclude points of SkeletonsFWMS from RegionsFWMS if necessary
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, 2.5)
	//1+2
	hv_expsize = 51;
	hv_dil = 9;
	//hv_dil = 13; - artifacts on p
	//hv_dil = 11;
	DilationCircle(ho_SkeletonsFWMS, &ho_SkeletonsFWMSd, hv_dil);
	CountObj(ho_SkeletonsFWMSd, &hv_no);

	//2
	//union1 (SkeletonsFWMS, SkeletonFWMS)
	//difference (SkeletonFWMS, RegionInoD, SkeletonFWMSdif)
	//connection (SkeletonFWMS, SkeletonsFWMSc)
	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd, dil)

	//count_obj (SkeletonsFWMSc, n2)


	//dilation_circle (SkeletonsFWMS, SkeletonsFWMSd2, dil)
	//count_obj (SkeletonsFWMSd2, n2)


	//1
	//difference (SkeletonsFWMSd, RegionInoD, SkeletonsFWMSm)
	//3
	SmallestRectangle1(ho_RegionInoD, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_RecInoD, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	Difference(ho_RecInoD, ho_RegionInoD, &ho_RegionInoDC);
	//area_center (SkeletonsFWMSd, Area, Row, Column)
	//tuple_min (Area, Min)
	Intersection(ho_SkeletonsFWMSd, ho_RegionInoDC, &ho_SkeletonsFWMSm);
	Union1(ho_SkeletonsFWMSd, &ho_SkeletonsFWMSdu);
	Intersection(ho_SkeletonsFWMSdu, ho_RegionInoDC, &ho_SkeletonsFWMSmu);
	Connection(ho_SkeletonsFWMSmu, &ho_SkeletonsFWMSm2);
	//3
	//difference (SkeletonsFWMSm, SkeletonsFWMSm2, RegionDifference1)


	//area_center (SkeletonsFWMSm, Area, Row, Column)
	//tuple_min (Area, amin)
	//count_obj (SkeletonsFWMSm, nm)

	//symm_difference (SkeletonsFWMSm, SkeletonsFWMSc, RegionDifference)

	//intersection (SkeletonsFWMS, RegionInoD, RegionIntersection)
	//union2 (SkeletonsFWMS, RegionIntersection, RegionUnion)
	//expand_region (RegionUnion, RegionInoD, RegionsFWMS, expsize, 'image')
	Difference(ho_SkelEP, ho_SkeletonsFWMSd, &ho_SkelEP1);
	Union2(ho_RegionInoD, ho_SkelEP1, &ho_RegForbid);
	ExpandRegion(ho_SkeletonsFWMSm, ho_RegForbid, &ho_RegionsFWMS2i, hv_expsize, "image");
	//expand_region (SkeletonsFWMSm, RegionInoD, RegionsFWMS2, expsize, 'image')
	//expand_region (SkeletonsFWMS, RegionInoD, RegionsFWMS2, expsize, 'image')
	Difference(ho_RegionsFWMS2i, ho_SkelEP1, &ho_RegionsFWMS2);
	CountObj(ho_SkeletonsFWMS, &hv_nRFWMS);
	hv_stseq = hv_nEmpty + 1;
	hv_overf = 255 - hv_stseq;
	TupleGenSequence(2, hv_nRFWMS + 1, 1, &hv_SequenceINBSi);
	hv_SequenceIRBS = (hv_SequenceINBSi%hv_overf) + hv_stseq;
	Boundary(ho_RegionsFWMS2, &ho_RegionsFWMSB, "inner");
	CountObj(ho_RegionsFWMSB, &(*hv_nFWMSB));
	PaintRegion(ho_RegionsFWMS2, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), 1, "fill");
	PaintRegion(ho_RegionsFWMSB, (*ho_ImIRBSOut), &(*ho_ImIRBSOut), hv_SequenceINBSi,
		"fill");
	return;
}


