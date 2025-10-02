/**
 * \copyright (C) Synapse Imaging 2023
 *
 * @File Name
		OpenShort.cpp
 *
 * @Summary
 *
 * @Description
 *		Scratch detection on conductors
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 2023.09.24
 */
 /* ************************************************************************** */
 /* Section: Included Files                                                    */
 /* ************************************************************************** */
#include "Inspect_Common.h"
#include "OpenShort.h"


/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */


/* ************************************************************************** */
/* Section: MakeSkeletonGnoR
/* ************************************************************************** */
/** ---------------------------------------------------------------------------
* @Summary
*      MakeSkeletonGnoR
*/
void MakeSkeletonGnoR(HObject ho_Im, HObject ho_RegionI, HObject ho_R, HObject ho_SkeletonG,
	HObject *ho_SkeletonGnoR)
{

	// Local iconic variables
	HObject  ho_DomainI, ho_RegionIS, ho_RS, ho_SkeletonGR;

	GetDomain(ho_Im, &ho_DomainI);
	Difference(ho_DomainI, ho_RegionI, &ho_RegionIS);
	Difference(ho_R, ho_RegionI, &ho_RS);
	Intersection(ho_SkeletonG, ho_RS, &ho_SkeletonGR);
	//***
	SetSystem("neighborhood", 8);
	Difference(ho_SkeletonG, ho_R, &(*ho_SkeletonGnoR));
	return;
}
/** ---------------------------------------------------------------------------
* @Summary
*      SkeletonsCheckPrep
*/
void SkeletonsCheckPrep(HObject ho_RegionFWM, HObject ho_RegionInoIslnoPh, HObject *ho_RegionFWMdil,
	HObject *ho_RegSkel, HObject *ho_RegSkeld, HObject *ho_SkeletonI, HObject *ho_EndPointsI,
	HObject *ho_JuncPointsI, HObject *ho_SkelPointsI, HObject *ho_RegionCheck, HTuple hv_dilFWM,
	HTuple hv_s1, HTuple hv_dilS)
{

	// Local iconic variables
	HObject  ho_RegSkelo, ho_SkelPointsIi, ho_RegionChecki;

	DilationCircle(ho_RegionFWM, &(*ho_RegionFWMdil), hv_dilFWM);
	Intersection(ho_RegionInoIslnoPh, (*ho_RegionFWMdil), &(*ho_RegSkel));
	SetSystem("neighborhood", 8);
	DilationCircle((*ho_RegSkel), &(*ho_RegSkeld), 2);
	OpeningCircle((*ho_RegSkeld), &ho_RegSkelo, 2);
	Skeleton(ho_RegSkelo, &(*ho_SkeletonI));
	//split_skeleton_region (SkeletonI, RegionLines, 30)
	//pruning (SkeletonI, SkeletonIPrune, 10)
	//count_seconds (s2)
	//sprep := s2-s1

	JunctionsSkeleton((*ho_SkeletonI), &(*ho_EndPointsI), &(*ho_JuncPointsI));
	SetSystem("neighborhood", 8);
	Union2((*ho_EndPointsI), (*ho_JuncPointsI), &ho_SkelPointsIi);
	DilationCircle(ho_SkelPointsIi, &(*ho_SkelPointsI), hv_dilS);
	//***********************
	ho_RegionChecki = (*ho_SkelPointsI);
	ClosingCircle(ho_RegionChecki, &(*ho_RegionCheck), 13.5);
	return;
}
/** ---------------------------------------------------------------------------
* @Summary
*      DetectShortN4
*/
void DetectShortN4(HObject ho_SkeletonGS, HObject ho_RegionInoPHnoIsl, HObject ho_RegionISnoPHnoIsl,
	HObject ho_WidthImageS, HObject ho_DirImageS, HObject ho_RegionG, HObject ho_RegionFWM,
	HObject ho_RegionCheck, HObject *ho_RegionsSh)
{

	// Local iconic variables
	HObject  ho_RB, ho_SkeletonGSnoB, ho_ShCandAll;
	HObject  ho_ShCands, ho_ShCand, ho_R2, ho_RegionISR2, ho_RegionsISR2;
	HObject  ho_RegionGSR2, ho_RegionsGSR2, ho_Rsel, ho_Rskel;
	HObject  ho_Rskeld, ho_RegionIntersection, ho_Rsh;

	// Local control variables
	HTuple  hv_r1, hv_c1, hv_r2, hv_c2, hv_nShCands;
	HTuple  hv_j, hv_AreaC, hv_RowC, hv_ColumnC, hv_Rows, hv_Columns;
	HTuple  hv_SpW, hv_spw, hv_SpD, hv_spd, hv_sa, hv_RowCm;
	HTuple  hv_ColumnCm, hv_Phi, hv_Length1, hv_Length2, hv_nGSR2;
	HTuple  hv_jj, hv_IsSubset, hv_Rowsrs, hv_Columnsrs, hv_l;
	HTuple  hv_skcrow, hv_skccol, hv_lr, hv_Area, hv_Rowi, hv_Columni;
	HTuple  hv_Arnonz, hv_asum, hv_Rowrnonz, hv_Columnrnonz;
	HTuple  hv_nArnonz, hv_Skcrow, hv_Skccol, hv_Dr, hv_Mr;

	//*** DetectShortN4 : new detect short proc. 19.01.2023
	//** remowed last SkeletonGS contour
	//** 12.09.2023

	//get_domain (WidthImageS, Domain)
	//boundary (Domain, DomainB, 'inner')
	//get_image_size (WidthImageS, Width, Height)
	//gen_rectangle1 (RB, bdist, bdist, Height- bdist, Width-bdist)
	//* for rectengular outer shape
	RegionFeatures(ho_RegionG, "row1", &hv_r1);
	RegionFeatures(ho_RegionG, "column1", &hv_c1);
	RegionFeatures(ho_RegionG, "row2", &hv_r2);
	RegionFeatures(ho_RegionG, "column2", &hv_c2);
	GenRectangle1(&ho_RB, hv_r1, hv_c1, hv_r2, hv_c2);

	//fill_up (RegionG, RegionFillUpG)
	//* for arbitrary outer shape
	//closing_circle (RegionG, RB, 101)

	Intersection(ho_SkeletonGS, ho_RB, &ho_SkeletonGSnoB);
	Intersection(ho_SkeletonGSnoB, ho_RegionInoPHnoIsl, &ho_ShCandAll);

	//intersection (ShCandAlli, RegionCheck, ShCandAll)
	//set_system ('neighborhood', 8)
	Connection(ho_ShCandAll, &ho_ShCands);
	//set_system ('neighborhood', 8)
	CountObj(ho_ShCands, &hv_nShCands);
	GenEmptyObj(&(*ho_RegionsSh));
	{
		HTuple end_val28 = hv_nShCands;
		HTuple step_val28 = 1;
		for (hv_j = 1; hv_j.Continue(end_val28, step_val28); hv_j += step_val28)
		{
			//intersection (RegionInoPHnoIsl, SkeletonGS, RInt)
			//connection (RInt, RInts)
			//if (j==56)
			  //stop ()
			//endif
			//if (j==1237)
			  //stop ()
			//endif
			//if (j==1008)
			  //stop ()
			//endif
			//j := 154
			SelectObj(ho_ShCands, &ho_ShCand, hv_j);
			AreaCenter(ho_ShCand, &hv_AreaC, &hv_RowC, &hv_ColumnC);
			GetRegionPoints(ho_ShCand, &hv_Rows, &hv_Columns);
			GetGrayval(ho_WidthImageS, hv_Rows, hv_Columns, &hv_SpW);
			TupleMedian(hv_SpW, &hv_spw);
			GetGrayval(ho_DirImageS, hv_Rows, hv_Columns, &hv_SpD);
			TupleMedian(hv_SpD, &hv_spd);
			hv_sa = (hv_spd*3.14159) / 256;
			if (0 != (hv_spw > 100))
			{
				continue;
			}
			if (0 != (hv_AreaC > 1))
			{
				SmallestRectangle2(ho_ShCand, &hv_RowCm, &hv_ColumnCm, &hv_Phi, &hv_Length1,
					&hv_Length2);
				GenRectangle2(&ho_R2, hv_RowCm, hv_ColumnCm, hv_Phi, hv_Length1 + 10, (hv_Length2 + hv_spw) + 10);
			}
			else
			{
				GenRectangle2(&ho_R2, hv_RowC, hv_ColumnC, hv_sa, hv_spw*2.5, hv_spw*2.5);
			}

			//gen_rectangle2 (R2, RowC, ColumnC, sa, spw*2.5, spw*2.5)
			Intersection(ho_RegionISnoPHnoIsl, ho_R2, &ho_RegionISR2);
			Connection(ho_RegionISR2, &ho_RegionsISR2);
			Intersection(ho_SkeletonGS, ho_R2, &ho_RegionGSR2);
			Connection(ho_RegionGSR2, &ho_RegionsGSR2);
			CountObj(ho_RegionsGSR2, &hv_nGSR2);
			{
				HTuple end_val65 = hv_nGSR2;
				HTuple step_val65 = 1;
				for (hv_jj = 1; hv_jj.Continue(end_val65, step_val65); hv_jj += step_val65)
				{
					SelectObj(ho_RegionsGSR2, &ho_Rsel, hv_jj);
					TestSubsetRegion(ho_ShCand, ho_Rsel, &hv_IsSubset);
					if (0 != hv_IsSubset)
					{
						break;
					}
				}
			}
			if (0 != (hv_jj == (hv_nGSR2 + 1)))
			{
				continue;
			}
			SelectObj(ho_RegionsGSR2, &ho_Rskel, hv_jj);
			GetRegionPoints(ho_Rskel, &hv_Rowsrs, &hv_Columnsrs);
			TupleLength(hv_Rowsrs, &hv_l);
			hv_skcrow = HTuple(hv_Rowsrs[hv_l / 2]);
			hv_skccol = HTuple(hv_Columnsrs[hv_l / 2]);

			DistancePp(HTuple(hv_Rowsrs[0]), HTuple(hv_Columnsrs[0]), HTuple(hv_Rowsrs[hv_l - 1]),
				HTuple(hv_Columnsrs[hv_l - 1]), &hv_lr);

			DilationCircle(ho_Rskel, &ho_Rskeld, 1);
			Intersection(ho_RegionsISR2, ho_Rskeld, &ho_RegionIntersection);
			AreaCenter(ho_RegionIntersection, &hv_Area, &hv_Rowi, &hv_Columni);
			TupleSelectMask(hv_Area, hv_Area, &hv_Arnonz);
			TupleSum(hv_Area, &hv_asum);
			TupleSelectMask(hv_Rowi, hv_Rowi.TupleInt(), &hv_Rowrnonz);
			TupleSelectMask(hv_Columni, hv_Columni.TupleInt(), &hv_Columnrnonz);

			TupleLength(hv_Arnonz, &hv_nArnonz);
			TupleGenConst(hv_nArnonz, hv_skcrow, &hv_Skcrow);
			TupleGenConst(hv_nArnonz, hv_skccol, &hv_Skccol);
			DistancePp(hv_Rowrnonz, hv_Columnrnonz, hv_Skcrow, hv_Skccol, &hv_Dr);

			if (0 != (hv_nArnonz > 0))
			{
				hv_Mr = hv_Dr.TupleMin();
			}
			else
			{
				hv_Mr = 10;
			}
			//**
			//intersection (RegionG, R2, RegionGR2)

			//**
			if (0 != (HTuple(HTuple(hv_nArnonz > 1).TupleAnd(hv_Mr < (hv_lr / 2))).TupleAnd(hv_asum > 15)))
			{
				//dev_display (R2)
				Intersection(ho_R2, ho_RegionInoPHnoIsl, &ho_Rsh);
				ConcatObj((*ho_RegionsSh), ho_Rsh, &(*ho_RegionsSh));
				//stop ()
			}
		}
	}
	return;
}

void DetectShortN6(HObject ho_SkeletonGS, HObject ho_RegionInoPHnoIsl, HObject ho_RegionISnoPHnoIsl,
	HObject ho_WidthImageS, HObject ho_DirImageS, HObject ho_RegionG, HObject ho_RegionFWM,
	HObject ho_RegionCheck, HObject ho_RegNoProc, HObject *ho_RegionsSh)
{

	// Local iconic variables
	HObject  ho_RegNoProcB, ho_RB, ho_SkeletonGSnoB;
	HObject  ho_ShCandAll, ho_ShCands, ho_ShCand, ho_RegionIntersection1;
	HObject  ho_R2, ho_RegionISR2, ho_RegionsISR2, ho_RegionGSR2;
	HObject  ho_RegionsGSR2, ho_Rsel, ho_Rskel, ho_Rskeld, ho_RegionIntersection;
	HObject  ho_Rsh, ho_Rshs, ho_Rshf;

	// Local control variables
	HTuple  hv_r1, hv_c1, hv_r2, hv_c2, hv_nShCands;
	HTuple  hv_Area1, hv_Row1, hv_Column1, hv_j, hv_a, hv_Row;
	HTuple  hv_Column, hv_AreaC, hv_RowC, hv_ColumnC, hv_Rows;
	HTuple  hv_Columns, hv_SpW, hv_spw, hv_SpD, hv_spd, hv_sa;
	HTuple  hv_RowCm, hv_ColumnCm, hv_Phi, hv_Length1, hv_Length2;
	HTuple  hv_nGSR2, hv_jj, hv_IsSubset, hv_Rowsrs, hv_Columnsrs;
	HTuple  hv_l, hv_skcrow, hv_skccol, hv_lr, hv_Area, hv_Rowi;
	HTuple  hv_Columni, hv_Arnonz, hv_asum, hv_Rowrnonz, hv_Columnrnonz;
	HTuple  hv_nArnonz, hv_Skcrow, hv_Skccol, hv_Dr, hv_Mr;
	HTuple  hv_AreaRshs, hv_RowRshs, hv_ColumnRshs, hv_no, hv_Ind;
	HTuple  hv_Inverted;

	//*** DetectShortN6 : new detect short proc. 19.01.2023
	//** 01.11.2023 added sheck in on RegNoProc boundary
	//** 25.09.2023 small pieces removed
	//** remowed last SkeletonGS contour
	//** 12.09.2023
	Boundary(ho_RegNoProc, &ho_RegNoProcB, "outer");
	//get_domain (WidthImageS, Domain)
	//boundary (Domain, DomainB, 'inner')
	//get_image_size (WidthImageS, Width, Height)
	//gen_rectangle1 (RB, bdist, bdist, Height- bdist, Width-bdist)
	//* for rectengular outer shape
	RegionFeatures(ho_RegionG, "row1", &hv_r1);
	RegionFeatures(ho_RegionG, "column1", &hv_c1);
	RegionFeatures(ho_RegionG, "row2", &hv_r2);
	RegionFeatures(ho_RegionG, "column2", &hv_c2);
	GenRectangle1(&ho_RB, hv_r1, hv_c1, hv_r2, hv_c2);

	//fill_up (RegionG, RegionFillUpG)
	//* for arbitrary outer shape
	//closing_circle (RegionG, RB, 101)

	Intersection(ho_SkeletonGS, ho_RB, &ho_SkeletonGSnoB);
	Intersection(ho_SkeletonGSnoB, ho_RegionInoPHnoIsl, &ho_ShCandAll);

	//intersection (ShCandAlli, RegionCheck, ShCandAll)
	//set_system ('neighborhood', 8)
	Connection(ho_ShCandAll, &ho_ShCands);
	SetSystem("neighborhood", 8);
	CountObj(ho_ShCands, &hv_nShCands);
	AreaCenter(ho_ShCands, &hv_Area1, &hv_Row1, &hv_Column1);
	GenEmptyObj(&(*ho_RegionsSh));
	if (0 != (hv_Area1 > 0))
	{



		{
			HTuple end_val35 = hv_nShCands;
			HTuple step_val35 = 1;
			for (hv_j = 1; hv_j.Continue(end_val35, step_val35); hv_j += step_val35)
			{
				//intersection (RegionInoPHnoIsl, SkeletonGS, RInt)
				//connection (RInt, RInts)
				//if (j==247)
				  //stop ()
				//endif
				//if (j==1237)
				  //stop ()
				//endif
				//if (j==1008)
				  //stop ()
				//endif
				//j := 299
				SelectObj(ho_ShCands, &ho_ShCand, hv_j);
				Intersection(ho_ShCand, ho_RegNoProcB, &ho_RegionIntersection1);
				AreaCenter(ho_RegionIntersection1, &hv_a, &hv_Row, &hv_Column);
				if (0 != (hv_a > 0))
				{
					ConcatObj((*ho_RegionsSh), ho_ShCand, &(*ho_RegionsSh));
					continue;
				}
				AreaCenter(ho_ShCand, &hv_AreaC, &hv_RowC, &hv_ColumnC);
				GetRegionPoints(ho_ShCand, &hv_Rows, &hv_Columns);
				GetGrayval(ho_WidthImageS, hv_Rows, hv_Columns, &hv_SpW);
				TupleMedian(hv_SpW, &hv_spw);
				GetGrayval(ho_DirImageS, hv_Rows, hv_Columns, &hv_SpD);
				TupleMedian(hv_SpD, &hv_spd);
				hv_sa = (hv_spd*3.14159) / 256;

				if (0 != (hv_spw > 200))
				{
					continue;
				}
				if (0 != (hv_AreaC > 1))
				{
					SmallestRectangle2(ho_ShCand, &hv_RowCm, &hv_ColumnCm, &hv_Phi, &hv_Length1,
						&hv_Length2);
					GenRectangle2(&ho_R2, hv_RowCm, hv_ColumnCm, hv_Phi, hv_Length1 + 10, (hv_Length2 + hv_spw) + 10);
				}
				else
				{
					GenRectangle2(&ho_R2, hv_RowC, hv_ColumnC, hv_sa, hv_spw*2.5, hv_spw*2.5);
				}

				//gen_rectangle2 (R2, RowC, ColumnC, sa, spw*2.5, spw*2.5)
				Intersection(ho_RegionISnoPHnoIsl, ho_R2, &ho_RegionISR2);
				Connection(ho_RegionISR2, &ho_RegionsISR2);
				Intersection(ho_SkeletonGS, ho_R2, &ho_RegionGSR2);
				Connection(ho_RegionGSR2, &ho_RegionsGSR2);
				CountObj(ho_RegionsGSR2, &hv_nGSR2);
				{
					HTuple end_val79 = hv_nGSR2;
					HTuple step_val79 = 1;
					for (hv_jj = 1; hv_jj.Continue(end_val79, step_val79); hv_jj += step_val79)
					{
						SelectObj(ho_RegionsGSR2, &ho_Rsel, hv_jj);
						TestSubsetRegion(ho_ShCand, ho_Rsel, &hv_IsSubset);
						if (0 != hv_IsSubset)
						{
							break;
						}
					}
				}
				if (0 != (hv_jj == (hv_nGSR2 + 1)))
				{
					continue;
				}
				//get_system ('neighborhood', nb)
				SelectObj(ho_RegionsGSR2, &ho_Rskel, hv_jj);
				GetRegionPoints(ho_Rskel, &hv_Rowsrs, &hv_Columnsrs);
				TupleLength(hv_Rowsrs, &hv_l);
				hv_skcrow = HTuple(hv_Rowsrs[hv_l / 2]);
				hv_skccol = HTuple(hv_Columnsrs[hv_l / 2]);

				DistancePp(HTuple(hv_Rowsrs[0]), HTuple(hv_Columnsrs[0]), HTuple(hv_Rowsrs[hv_l - 1]),
					HTuple(hv_Columnsrs[hv_l - 1]), &hv_lr);

				DilationCircle(ho_Rskel, &ho_Rskeld, 1);
				Intersection(ho_RegionsISR2, ho_Rskeld, &ho_RegionIntersection);
				AreaCenter(ho_RegionIntersection, &hv_Area, &hv_Rowi, &hv_Columni);
				TupleSelectMask(hv_Area, hv_Area, &hv_Arnonz);
				TupleSum(hv_Area, &hv_asum);
				TupleSelectMask(hv_Rowi, hv_Rowi.TupleInt(), &hv_Rowrnonz);
				TupleSelectMask(hv_Columni, hv_Columni.TupleInt(), &hv_Columnrnonz);

				TupleLength(hv_Arnonz, &hv_nArnonz);
				TupleGenConst(hv_nArnonz, hv_skcrow, &hv_Skcrow);
				TupleGenConst(hv_nArnonz, hv_skccol, &hv_Skccol);
				DistancePp(hv_Rowrnonz, hv_Columnrnonz, hv_Skcrow, hv_Skccol, &hv_Dr);

				if (0 != (hv_nArnonz > 0))
				{
					hv_Mr = hv_Dr.TupleMin();
				}
				else
				{
					hv_Mr = 10;
				}
				//**
				//intersection (RegionG, R2, RegionGR2)

				//**
				//if (0 != (HTuple(HTuple(hv_nArnonz > 1).TupleAnd(hv_Mr < (hv_lr / 1.5))).TupleAnd(hv_asum > 15)))
				if (0 != (HTuple(HTuple(hv_nArnonz > 1)).TupleAnd(hv_asum > 15)))
				{
					//dev_display (R2)
					Intersection(ho_R2, ho_RegionInoPHnoIsl, &ho_Rsh);
					//** only biggeast region should remain
					Connection(ho_Rsh, &ho_Rshs);
					AreaCenter(ho_Rshs, &hv_AreaRshs, &hv_RowRshs, &hv_ColumnRshs);
					TupleLength(hv_AreaRshs, &hv_no);
					if (0 != (hv_no > 1))
					{
						TupleSortIndex(hv_AreaRshs, &hv_Ind);
						TupleInverse(hv_Ind, &hv_Inverted);
						SelectObj(ho_Rshs, &ho_Rshf, HTuple(hv_Inverted[0]) + 1);
						ConcatObj((*ho_RegionsSh), ho_Rshf, &(*ho_RegionsSh));
					}
					else
					{
						ConcatObj((*ho_RegionsSh), ho_Rsh, &(*ho_RegionsSh));
					}

					//stop ()
				}
			}
		}
	}
	return;
}


int DetectShortN5(HObject ho_SkeletonGS, HObject ho_RegionInoPHnoIsl, HObject ho_RegionISnoPHnoIsl,
	HObject ho_WidthImageS, HObject ho_DirImageS, HObject ho_RegionG, HObject ho_RegionFWM,
	HObject ho_RegionCheck,  HObject *ho_RegionsSh, HTuple hv_short_max_active)
{

	// Local iconic variables
	HObject  ho_RB, ho_SkeletonGSnoB, ho_ShCandAll;
	HObject  ho_ShCands, ho_ShCand, ho_R2, ho_RegionISR2, ho_RegionsISR2;
	HObject  ho_RegionGSR2, ho_RegionsGSR2, ho_Rsel, ho_Rskel;
	HObject  ho_Rskeld, ho_RegionIntersection, ho_Rsh, ho_Rshs;
	HObject  ho_Rshf;

	// Local control variables
	HTuple  hv_r1, hv_c1, hv_r2, hv_c2, hv_nShCands;
	HTuple  hv_j, hv_AreaC, hv_RowC, hv_ColumnC, hv_Rows, hv_Columns;
	HTuple  hv_SpW, hv_spw, hv_SpD, hv_spd, hv_sa, hv_RowCm;
	HTuple  hv_ColumnCm, hv_Phi, hv_Length1, hv_Length2, hv_nGSR2;
	HTuple  hv_jj, hv_IsSubset, hv_Rowsrs, hv_Columnsrs, hv_l;
	HTuple  hv_skcrow, hv_skccol, hv_lr, hv_Area, hv_Rowi, hv_Columni;
	HTuple  hv_Arnonz, hv_asum, hv_Rowrnonz, hv_Columnrnonz;
	HTuple  hv_nArnonz, hv_Skcrow, hv_Skccol, hv_Dr, hv_Mr;
	HTuple  hv_AreaRshs, hv_RowRshs, hv_ColumnRshs, hv_no, hv_Ind;
	HTuple  hv_Inverted;

	//*** DetectShortN5 : new detect short proc. 19.01.2023
	//** 25.09.2023 small pieces removed
	//** remowed last SkeletonGS contour
	//** 12.09.2023

	//get_domain (WidthImageS, Domain)
	//boundary (Domain, DomainB, 'inner')
	//get_image_size (WidthImageS, Width, Height)
	//gen_rectangle1 (RB, bdist, bdist, Height- bdist, Width-bdist)
	//* for rectengular outer shape
	RegionFeatures(ho_RegionG, "row1", &hv_r1);
	RegionFeatures(ho_RegionG, "column1", &hv_c1);
	RegionFeatures(ho_RegionG, "row2", &hv_r2);
	RegionFeatures(ho_RegionG, "column2", &hv_c2);
	GenRectangle1(&ho_RB, hv_r1, hv_c1, hv_r2, hv_c2);

	//fill_up (RegionG, RegionFillUpG)
	//* for arbitrary outer shape
	//closing_circle (RegionG, RB, 101)

	Intersection(ho_SkeletonGS, ho_RB, &ho_SkeletonGSnoB);
	Intersection(ho_SkeletonGSnoB, ho_RegionInoPHnoIsl, &ho_ShCandAll);

	//intersection (ShCandAlli, RegionCheck, ShCandAll)
	SetSystem("neighborhood", 8);
	Connection(ho_ShCandAll, &ho_ShCands);
	//set_system ('neighborhood', 8)
	CountObj(ho_ShCands, &hv_nShCands);
	GenEmptyObj(&(*ho_RegionsSh));
	int short_count = 0;
	{
		HTuple end_val29 = hv_nShCands;
		HTuple step_val29 = 1;
		for (hv_j = 1; hv_j.Continue(end_val29, step_val29); hv_j += step_val29)
		{
			//intersection (RegionInoPHnoIsl, SkeletonGS, RInt)
			//connection (RInt, RInts)
			//if (j==56)
			  //stop ()
			//endif
			//if (j==1237)
			  //stop ()
			//endif
			//if (j==1008)
			  //stop ()
			//endif
			//j := 442
			SelectObj(ho_ShCands, &ho_ShCand, hv_j);
			AreaCenter(ho_ShCand, &hv_AreaC, &hv_RowC, &hv_ColumnC);
			GetRegionPoints(ho_ShCand, &hv_Rows, &hv_Columns);
			GetGrayval(ho_WidthImageS, hv_Rows, hv_Columns, &hv_SpW);
			TupleMedian(hv_SpW, &hv_spw);
			GetGrayval(ho_DirImageS, hv_Rows, hv_Columns, &hv_SpD);
			TupleMedian(hv_SpD, &hv_spd);
			hv_sa = (hv_spd*3.14159) / 256;
			if (0 != (hv_spw > 100))
			{
				continue;
			}
			if (0 != (hv_AreaC > 1))
			{
				SmallestRectangle2(ho_ShCand, &hv_RowCm, &hv_ColumnCm, &hv_Phi, &hv_Length1,
					&hv_Length2);
				GenRectangle2(&ho_R2, hv_RowCm, hv_ColumnCm, hv_Phi, hv_Length1 + 10, (hv_Length2 + hv_spw) + 10);
			}
			else
			{
				GenRectangle2(&ho_R2, hv_RowC, hv_ColumnC, hv_sa, hv_spw*2.5, hv_spw*2.5);
			}

			//gen_rectangle2 (R2, RowC, ColumnC, sa, spw*2.5, spw*2.5)
			HalconCpp::Intersection(ho_RegionISnoPHnoIsl, ho_R2, &ho_RegionISR2);
			Connection(ho_RegionISR2, &ho_RegionsISR2);
			HalconCpp::Intersection(ho_SkeletonGS, ho_R2, &ho_RegionGSR2);
			Connection(ho_RegionGSR2, &ho_RegionsGSR2);
			CountObj(ho_RegionsGSR2, &hv_nGSR2);
			{
				HTuple end_val66 = hv_nGSR2;
				HTuple step_val66 = 1;
				for (hv_jj = 1; hv_jj.Continue(end_val66, step_val66); hv_jj += step_val66)
				{
					SelectObj(ho_RegionsGSR2, &ho_Rsel, hv_jj);
					TestSubsetRegion(ho_ShCand, ho_Rsel, &hv_IsSubset);
					if (0 != hv_IsSubset)
					{
						break;
					}
				}
			}
			if (0 != (hv_jj == (hv_nGSR2 + 1)))
			{
				continue;
			}
			SelectObj(ho_RegionsGSR2, &ho_Rskel, hv_jj);
			GetRegionPoints(ho_Rskel, &hv_Rowsrs, &hv_Columnsrs);
			TupleLength(hv_Rowsrs, &hv_l);
			hv_skcrow = HTuple(hv_Rowsrs[hv_l / 2]);
			hv_skccol = HTuple(hv_Columnsrs[hv_l / 2]);

			DistancePp(HTuple(hv_Rowsrs[0]), HTuple(hv_Columnsrs[0]), HTuple(hv_Rowsrs[hv_l - 1]),
				HTuple(hv_Columnsrs[hv_l - 1]), &hv_lr);

			DilationCircle(ho_Rskel, &ho_Rskeld, 1);
			HalconCpp::Intersection(ho_RegionsISR2, ho_Rskeld, &ho_RegionIntersection);
			AreaCenter(ho_RegionIntersection, &hv_Area, &hv_Rowi, &hv_Columni);
			TupleSelectMask(hv_Area, hv_Area, &hv_Arnonz);
			TupleSum(hv_Area, &hv_asum);
			TupleSelectMask(hv_Rowi, hv_Rowi.TupleInt(), &hv_Rowrnonz);
			TupleSelectMask(hv_Columni, hv_Columni.TupleInt(), &hv_Columnrnonz);

			TupleLength(hv_Arnonz, &hv_nArnonz);
			TupleGenConst(hv_nArnonz, hv_skcrow, &hv_Skcrow);
			TupleGenConst(hv_nArnonz, hv_skccol, &hv_Skccol);
			DistancePp(hv_Rowrnonz, hv_Columnrnonz, hv_Skcrow, hv_Skccol, &hv_Dr);

			if (0 != (hv_nArnonz > 0))
			{
				hv_Mr = hv_Dr.TupleMin();
			}
			else
			{
				hv_Mr = 10;
			}
			//**
			//intersection (RegionG, R2, RegionGR2)

			//**
			if (0 != (HTuple(HTuple(hv_nArnonz > 1).TupleAnd(hv_Mr < (hv_lr / 1.5))).TupleAnd(hv_asum > 15)))
			{
				//dev_display (R2)
				HalconCpp::Intersection(ho_R2, ho_RegionInoPHnoIsl, &ho_Rsh);
				//** only biggeast region should remain
				Connection(ho_Rsh, &ho_Rshs);
				AreaCenter(ho_Rshs, &hv_AreaRshs, &hv_RowRshs, &hv_ColumnRshs);
				TupleLength(hv_AreaRshs, &hv_no);
				if (0 != (hv_no > 1))
				{
					TupleSortIndex(hv_AreaRshs, &hv_Ind);
					TupleInverse(hv_Ind, &hv_Inverted);
					SelectObj(ho_Rshs, &ho_Rshf, HTuple(hv_Inverted[0]) + 1);
					ConcatObj((*ho_RegionsSh), ho_Rshf, &(*ho_RegionsSh));
				}
				else
				{
					ConcatObj((*ho_RegionsSh), ho_Rsh, &(*ho_RegionsSh));
				}
				if ( (short_count++) >= 99 && (hv_short_max_active == 1))
				{
					break;
				}
				//stop ()
			}
		}
	}
	return short_count;
}
/** ---------------------------------------------------------------------------
* @Summary
*      DetectOpenN5
*/
void DetectOpenN5_1(HObject ho_RegionG, HObject ho_SkeletonG, HObject ho_RegionInoPHnoIsl,
	HObject ho_RegionISnoPHnoIsl, HObject ho_WidthImageP, HObject ho_DirImageP, HObject ho_RegionFWM,
	HObject ho_RegionCheck, HObject *ho_RegionsOp, HObject *ho_RegionsOpCon)
{

	// Local iconic variables
	HObject  ho_OpCandAlli, ho_OpCandAll, ho_OpCands;
	HObject  ho_OpCand, ho_R2, ho_R3, ho_RegionIR2, ho_RegionsIR2;
	HObject  ho_RegionGR2, ho_RegionsGR2, ho_RegionGR, ho_RegionsGR;
	HObject  ho_Rsel, ho_Rskeli, ho_Rskel, ho_RegionIntersection;
	HObject  ho_SmallIs, ho_SmallI, ho_BigIs, ho_BigI, ho_BigInoSs;
	HObject  ho_RegionGR3, ho_RegionsGR3, ho_Obj, ho_CurBigInoS;
	HObject  ho_CurBigInoSG, ho_RegionG2, ho_RGR3, ho_Rop, ho_Ropf;

	// Local control variables
	HTuple  hv_nOpCands, hv_j, hv_AreaC, hv_RowC;
	HTuple  hv_ColumnC, hv_Rows, hv_Columns, hv_PpW, hv_ppw;
	HTuple  hv_PpD, hv_ppd, hv_pa, hv_RowCm, hv_ColumnCm, hv_Phi;
	HTuple  hv_Length1, hv_Length2, hv_nGR2, hv_jj, hv_IsSubset;
	HTuple  hv_Area, hv_Row, hv_Column, hv_Arnonz, hv_nArnonz;
	HTuple  hv_Index, hv_IndexGAll, hv_RowsO, hv_ColumnsO, hv_nOp;
	HTuple  hv_RowsB, hv_ColumnsB, hv_nB, hv_IndexG;

	//*** DetectOpenN5 : new detect open proc. 11.08.2022
	//** modified 12.09.2023, overkills removed

	SetSystem("neighborhood", 8);
	Intersection(ho_SkeletonG, ho_RegionISnoPHnoIsl, &ho_OpCandAlli);
	Intersection(ho_OpCandAlli, ho_RegionCheck, &ho_OpCandAll);
	Connection(ho_OpCandAll, &ho_OpCands);
	CountObj(ho_OpCands, &hv_nOpCands);
	GenEmptyObj(&(*ho_RegionsOp));
	GenEmptyObj(&(*ho_RegionsOpCon));
	{
		HTuple end_val10 = hv_nOpCands;
		HTuple step_val10 = 1;
		for (hv_j = 1; hv_j.Continue(end_val10, step_val10); hv_j += step_val10)
		{
			//if (j==1233)
			  //stop ()
			//endif
			//j := 999
			//j := 893
			SelectObj(ho_OpCands, &ho_OpCand, hv_j);
			AreaCenter(ho_OpCand, &hv_AreaC, &hv_RowC, &hv_ColumnC);
			if (0 != (hv_AreaC > 250))
			{
				//stop ()
			}
			GetRegionPoints(ho_OpCand, &hv_Rows, &hv_Columns);
			GetGrayval(ho_WidthImageP, hv_Rows, hv_Columns, &hv_PpW);
			TupleMedian(hv_PpW, &hv_ppw);
			GetGrayval(ho_DirImageP, hv_Rows, hv_Columns, &hv_PpD);
			TupleMedian(hv_PpD, &hv_ppd);
			hv_pa = (hv_ppd*3.14159) / 256;
			if (0 != (hv_ppw > 100))
			{
				continue;
			}
			if (0 != (hv_AreaC > 1))
			{
				SmallestRectangle2(ho_OpCand, &hv_RowCm, &hv_ColumnCm, &hv_Phi, &hv_Length1,
					&hv_Length2);
				GenRectangle2(&ho_R2, hv_RowCm, hv_ColumnCm, hv_Phi, hv_Length1 + hv_ppw, (hv_Length2 + hv_ppw) + hv_ppw);
				GenRectangle2(&ho_R3, hv_RowCm, hv_ColumnCm, hv_Phi, (hv_Length1 + hv_ppw) + 5,
					((hv_Length2 + hv_ppw) + hv_ppw) + 5);
			}
			else
			{
				GenRectangle2(&ho_R2, hv_RowC, hv_ColumnC, hv_pa, hv_ppw*2.5, hv_ppw*2.5);
				GenRectangle2(&ho_R3, hv_RowC, hv_ColumnC, hv_pa, (hv_ppw*2.5) + 5, (hv_ppw*2.5) + 5);
			}
			Intersection(ho_RegionInoPHnoIsl, ho_R2, &ho_RegionIR2);
			Connection(ho_RegionIR2, &ho_RegionsIR2);
			Intersection(ho_SkeletonG, ho_R2, &ho_RegionGR2);

			Connection(ho_RegionGR2, &ho_RegionsGR2);
			Intersection(ho_RegionG, ho_R2, &ho_RegionGR);
			Connection(ho_RegionGR, &ho_RegionsGR);

			CountObj(ho_RegionsGR2, &hv_nGR2);
			{
				HTuple end_val47 = hv_nGR2;
				HTuple step_val47 = 1;
				for (hv_jj = 1; hv_jj.Continue(end_val47, step_val47); hv_jj += step_val47)
				{
					SelectObj(ho_RegionsGR2, &ho_Rsel, hv_jj);
					TestSubsetRegion(ho_OpCand, ho_Rsel, &hv_IsSubset);
					if (0 != hv_IsSubset)
					{
						break;
					}
				}
			}
			if (0 != (hv_jj == (hv_nGR2 + 1)))
			{
				continue;
			}
			SelectObj(ho_RegionsGR2, &ho_Rskeli, hv_jj);
			DilationCircle(ho_Rskeli, &ho_Rskel, 3);
			Intersection(ho_RegionsIR2, ho_Rskel, &ho_RegionIntersection);
			AreaCenter(ho_RegionIntersection, &hv_Area, &hv_Row, &hv_Column);
			TupleSelectMask(hv_Area, hv_Area, &hv_Arnonz);
			TupleLength(hv_Arnonz, &hv_nArnonz);

			if (0 != (hv_nArnonz > 1))
			{
				//Index := []
				hv_IndexGAll = HTuple();
				ho_SmallIs = ho_RegionIntersection;
				Union1(ho_SmallIs, &ho_SmallI);
				Connection(ho_SmallI, &ho_SmallIs);

				//BigIs := RegionsIR2
				//gen_rectangle2 (R3, RowCm, ColumnCm, Phi, Length1+ppw+5, Length2+ppw+ppw+5)
				Intersection(ho_RegionInoPHnoIsl, ho_R3, &ho_BigI);
				Connection(ho_BigI, &ho_BigIs);
				Difference(ho_BigIs, ho_SmallI, &ho_BigInoSs);

				Intersection(ho_RegionG, ho_R3, &ho_RegionGR3);
				Connection(ho_RegionGR3, &ho_RegionsGR3);
				{
					HTuple end_val79 = hv_nArnonz;
					HTuple step_val79 = 1;
					for (hv_jj = 1; hv_jj.Continue(end_val79, step_val79); hv_jj += step_val79)
					{
						SelectObj(ho_SmallIs, &ho_Obj, hv_jj);
						GetRegionPoints(ho_Obj, &hv_RowsO, &hv_ColumnsO);
						TupleLength(hv_RowsO, &hv_nOp);
						GetRegionIndex(ho_BigIs, HTuple(hv_RowsO[hv_nOp / 2]).TupleInt(), HTuple(hv_ColumnsO[hv_nOp / 2]).TupleInt(),
							&hv_Index);
						SelectObj(ho_BigInoSs, &ho_CurBigInoS, hv_Index);
						Intersection(ho_CurBigInoS, ho_RegionG, &ho_CurBigInoSG);
						GetRegionPoints(ho_CurBigInoSG, &hv_RowsB, &hv_ColumnsB);
						TupleLength(hv_RowsB, &hv_nB);
						if (hv_nB == 0)
						{
							continue;
						}
						GetRegionIndex(ho_RegionsGR3, HTuple(hv_RowsB[hv_nB / 2]).TupleInt(), HTuple(hv_ColumnsB[hv_nB / 2]).TupleInt(),
							&hv_IndexG);
						hv_IndexGAll[hv_jj - 1] = hv_IndexG;
					}
				}

				if (0 != (HTuple(hv_IndexGAll[0]) == HTuple(hv_IndexGAll[1])))
				{
					Intersection(ho_RegionG, ho_R2, &ho_RegionG2);
					//dev_display (R2)
					SelectObj(ho_RegionsGR3, &ho_RGR3, hv_IndexG);
					Intersection(ho_R2, ho_RegionInoPHnoIsl, &ho_Rop);
					Intersection(ho_Rop, ho_RGR3, &ho_Ropf);
					ConcatObj((*ho_RegionsOp), ho_Ropf, &(*ho_RegionsOp));
					//dilation_circle (Rskel, RskelSd, 1)
					//concat_obj (RegionsOpCon, RskelSd, RegionsOpCon)
					ConcatObj((*ho_RegionsOpCon), ho_Rskel, &(*ho_RegionsOpCon));
				}

				//stop ()
				//*     elseif(AreaC > 10)
				//dilation_circle (OpCand, OpCandd, 3)
				//intersection (OpCandd, RegionIR2, RegionIntCandI)
				//connection (RegionIntCandI, RegionsIntCandI)
				//count_obj (RegionsIntCandI, n)
				//if (n==2)
				  //intersection (R2, RegionInoPHnoIsl, Rop)
				  //concat_obj (RegionsOp, Rop, RegionsOp)
				  //dilation_circle (Rskel, RskelSd, 1)
				  //concat_obj (RegionsOpCon, RskelSd, RegionsOpCon)
				//endif
			}
		}
	}
	return;
}
/** ---------------------------------------------------------------------------
* @Summary
*      DetectOpenN5
*/
void DetectOpenN5(HObject ho_RegionG, HObject ho_SkeletonG, HObject ho_RegionInoPHnoIsl,
	HObject ho_RegionISnoPHnoIsl, HObject ho_WidthImageP, HObject ho_DirImageP, HObject ho_RegionFWM,
	HObject ho_RegionCheck, HObject *ho_RegionsOp, HObject *ho_RegionsOpCon)
{

	// Local iconic variables
	HObject  ho_OpCandAlli, ho_OpCandAll, ho_OpCands;
	HObject  ho_OpCand, ho_R2, ho_R3, ho_RegionIR2, ho_RegionsIR2;
	HObject  ho_RegionGR2, ho_RegionsGR2, ho_RegionGR, ho_RegionsGR;
	HObject  ho_Rsel, ho_Rskeli, ho_Rskel, ho_RegionIntersection;
	HObject  ho_SmallIs, ho_SmallI, ho_BigIs, ho_BigI, ho_BigInoSs;
	HObject  ho_RegionGR3, ho_RegionsGR3, ho_Obj, ho_CurBigInoS;
	HObject  ho_CurBigInoSG, ho_RGR3, ho_Rop, ho_Ropf;

	// Local control variables
	HTuple  hv_AreaOpCandAll, hv_Row1, hv_Column1;
	HTuple  hv_nOpCands, hv_j, hv_AreaC, hv_RowC, hv_ColumnC;
	HTuple  hv_Rows, hv_Columns, hv_PpW, hv_ppw, hv_PpD, hv_ppd;
	HTuple  hv_pa, hv_RowCm, hv_ColumnCm, hv_Phi, hv_Length1;
	HTuple  hv_Length2, hv_nGR2, hv_jj, hv_IsSubset, hv_Area;
	HTuple  hv_Row, hv_Column, hv_Arnonz, hv_nArnonz, hv_Index;
	HTuple  hv_IndexGAll, hv_RowsO, hv_ColumnsO, hv_nOp, hv_RowsB;
	HTuple  hv_ColumnsB, hv_nB, hv_IndexG, hv_nGAll;

	//*** DetectOpenN5 : new detect open proc. 11.08.2022
	//** modified 12.09.2023, overkills removed

	SetSystem("neighborhood", 8);
	Intersection(ho_SkeletonG, ho_RegionISnoPHnoIsl, &ho_OpCandAlli);
	Intersection(ho_OpCandAlli, ho_RegionCheck, &ho_OpCandAll);
	AreaCenter(ho_OpCandAll, &hv_AreaOpCandAll, &hv_Row1, &hv_Column1);
	Connection(ho_OpCandAll, &ho_OpCands);
	CountObj(ho_OpCands, &hv_nOpCands);
	GenEmptyObj(&(*ho_RegionsOp));
	GenEmptyObj(&(*ho_RegionsOpCon));
	if (0 != (hv_AreaOpCandAll > 0))
	{
		{
			HTuple end_val12 = hv_nOpCands;
			HTuple step_val12 = 1;
			for (hv_j = 1; hv_j.Continue(end_val12, step_val12); hv_j += step_val12)
			{
				//if (j==1233)
				  //stop ()
				//endif
				//j := 999
				//j := 18
				SelectObj(ho_OpCands, &ho_OpCand, hv_j);
				AreaCenter(ho_OpCand, &hv_AreaC, &hv_RowC, &hv_ColumnC);
				if (0 != (hv_AreaC > 250))
				{
					//stop ()
				}
				GetRegionPoints(ho_OpCand, &hv_Rows, &hv_Columns);
				GetGrayval(ho_WidthImageP, hv_Rows, hv_Columns, &hv_PpW);
				TupleMedian(hv_PpW, &hv_ppw);

				GetGrayval(ho_DirImageP, hv_Rows, hv_Columns, &hv_PpD);
				TupleMedian(hv_PpD, &hv_ppd);
				hv_pa = (hv_ppd*3.14159) / 256;
				if (0 != (hv_ppw > 100))
				{
					continue;
				}
				if (0 != (hv_AreaC > 1))
				{
					SmallestRectangle2(ho_OpCand, &hv_RowCm, &hv_ColumnCm, &hv_Phi, &hv_Length1,
						&hv_Length2);
					GenRectangle2(&ho_R2, hv_RowCm, hv_ColumnCm, hv_Phi, hv_Length1 + hv_ppw, (hv_Length2 + hv_ppw) + (hv_ppw / 2));
					GenRectangle2(&ho_R3, hv_RowCm, hv_ColumnCm, hv_Phi, (hv_Length1 + hv_ppw) + 5,
						((hv_Length2 + hv_ppw) + hv_ppw) + 5);
				}
				else
				{
					GenRectangle2(&ho_R2, hv_RowC, hv_ColumnC, hv_pa, hv_ppw*2.5, hv_ppw*2.5);
					GenRectangle2(&ho_R3, hv_RowC, hv_ColumnC, hv_pa, (hv_ppw*2.5) + 5, (hv_ppw*2.5) + 5);
				}
				Intersection(ho_RegionInoPHnoIsl, ho_R2, &ho_RegionIR2);
				Connection(ho_RegionIR2, &ho_RegionsIR2);
				Intersection(ho_SkeletonG, ho_R2, &ho_RegionGR2);

				Connection(ho_RegionGR2, &ho_RegionsGR2);
				Intersection(ho_RegionG, ho_R2, &ho_RegionGR);
				Connection(ho_RegionGR, &ho_RegionsGR);

				CountObj(ho_RegionsGR2, &hv_nGR2);
				{
					HTuple end_val50 = hv_nGR2;
					HTuple step_val50 = 1;
					for (hv_jj = 1; hv_jj.Continue(end_val50, step_val50); hv_jj += step_val50)
					{
						SelectObj(ho_RegionsGR2, &ho_Rsel, hv_jj);
						TestSubsetRegion(ho_OpCand, ho_Rsel, &hv_IsSubset);
						if (0 != hv_IsSubset)
						{
							break;
						}
					}
				}
				if (0 != (hv_jj == (hv_nGR2 + 1)))
				{
					continue;
				}
				SelectObj(ho_RegionsGR2, &ho_Rskeli, hv_jj);
				DilationCircle(ho_Rskeli, &ho_Rskel, 3);
				Intersection(ho_RegionsIR2, ho_Rskel, &ho_RegionIntersection);
				AreaCenter(ho_RegionIntersection, &hv_Area, &hv_Row, &hv_Column);
				TupleSelectMask(hv_Area, hv_Area, &hv_Arnonz);
				TupleLength(hv_Arnonz, &hv_nArnonz);

				if (0 != (hv_nArnonz > 1))
				{
					//Index := []
					hv_IndexGAll = HTuple();
					ho_SmallIs = ho_RegionIntersection;
					Union1(ho_SmallIs, &ho_SmallI);
					Connection(ho_SmallI, &ho_SmallIs);

					//BigIs := RegionsIR2
					//gen_rectangle2 (R3, RowCm, ColumnCm, Phi, Length1+ppw+5, Length2+ppw+ppw+5)
					Intersection(ho_RegionInoPHnoIsl, ho_R3, &ho_BigI);
					Connection(ho_BigI, &ho_BigIs);
					Difference(ho_BigIs, ho_SmallI, &ho_BigInoSs);

					Intersection(ho_RegionG, ho_R3, &ho_RegionGR3);
					Connection(ho_RegionGR3, &ho_RegionsGR3);
					{
						HTuple end_val82 = hv_nArnonz;
						HTuple step_val82 = 1;
						for (hv_jj = 1; hv_jj.Continue(end_val82, step_val82); hv_jj += step_val82)
						{
							SelectObj(ho_SmallIs, &ho_Obj, hv_jj);
							GetRegionPoints(ho_Obj, &hv_RowsO, &hv_ColumnsO);
							TupleLength(hv_RowsO, &hv_nOp);
							GetRegionIndex(ho_BigIs, HTuple(hv_RowsO[hv_nOp / 2]).TupleInt(), HTuple(hv_ColumnsO[hv_nOp / 2]).TupleInt(),
								&hv_Index);
							SelectObj(ho_BigInoSs, &ho_CurBigInoS, hv_Index);
							Intersection(ho_CurBigInoS, ho_RegionG, &ho_CurBigInoSG);
							GetRegionPoints(ho_CurBigInoSG, &hv_RowsB, &hv_ColumnsB);
							TupleLength(hv_RowsB, &hv_nB);
							if (0 != (hv_nB == 0))
							{
								continue;
							}
							GetRegionIndex(ho_RegionsGR3, HTuple(hv_RowsB[hv_nB / 2]).TupleInt(), HTuple(hv_ColumnsB[hv_nB / 2]).TupleInt(),
								&hv_IndexG);
							hv_IndexGAll[hv_jj - 1] = hv_IndexG;
						}
					}
					TupleLength(hv_IndexGAll, &hv_nGAll);
					//if (IndexGAll[0]==IndexGAll[1])
					  //intersection (RegionG, R2, RegionG2)
					  //dev_display (R2)
					SelectObj(ho_RegionsGR3, &ho_RGR3, hv_IndexG);
					Intersection(ho_R2, ho_RegionInoPHnoIsl, &ho_Rop);
					Intersection(ho_Rop, ho_RGR3, &ho_Ropf);
					ConcatObj((*ho_RegionsOp), ho_Ropf, &(*ho_RegionsOp));
					//dilation_circle (Rskel, RskelSd, 1)
					//concat_obj (RegionsOpCon, RskelSd, RegionsOpCon)
					ConcatObj((*ho_RegionsOpCon), ho_Rskel, &(*ho_RegionsOpCon));
					//endif

					//stop ()
					//*     elseif(AreaC > 10)
					//dilation_circle (OpCand, OpCandd, 3)
					//intersection (OpCandd, RegionIR2, RegionIntCandI)
					//connection (RegionIntCandI, RegionsIntCandI)
					//count_obj (RegionsIntCandI, n)
					//if (n==2)
					  //intersection (R2, RegionInoPHnoIsl, Rop)
					  //concat_obj (RegionsOp, Rop, RegionsOp)
					  //dilation_circle (Rskel, RskelSd, 1)
					  //concat_obj (RegionsOpCon, RskelSd, RegionsOpCon)
					//endif
				}
			}
		}
	}

	return;
}


/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

