/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
		LowGray.cpp
 *
 * @Summary
 *
 * @Description
 *		
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 3/15/2024 1:08:05 PM
 */

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include "Inspect_Common.h"
#include "LowGray.h"

/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
using namespace inspect_utils;

/* ************************************************************************** */
/* Section: Function				                                          */
/* ************************************************************************** */

/** ---------------------------------------------------------------------------
* @Summary
*
*/
void LowGrayProc(HObject ho_SkeletonsFWMS, HObject ho_Im, HObject *ho_RegionLG,
	HTuple hv_maxexp, HTuple hv_thr, HTuple hv_autothr, HTuple hv_fixedthr, HTuple hv_dt,
	HTuple *hv_thrlga)
{
	// Local iconic variables
	HObject  ho_SkeletonFWMS, ho_SkeletonFWMSd, ho_Imp;
	HObject  ho_RegionTh, ho_RegionTh1, ho_RegionLGi, ho_RegionTh3;
	HObject  ho_RegionsLowDefs, ho_RegionLG_CON, ho_RegL_SEL;
	HObject  ho_RegLopi_Diff, ho_RegL_DIL, ho_RegL_Diff, ho_Circle;
	HObject  ho_Circle1;

	// Local control variables
	HTuple  hv_Middle, hv_AbsoluteHisto, hv_FAbsoluteHisto;
	HTuple  hv_FSpace_T, hv_MinS, hv_MaxS, hv_Idx, hv_thr1;
	HTuple  hv_Count, hv_i, hv_Area, hv_Row, hv_Col, hv_Rows;
	HTuple  hv_Columns, hv_Locality_Gray, hv_Sorted, hv_Length;
	HTuple  hv_Selected, hv_mean_RegLopi, hv_Angle_Cont, hv_Rows_Cont;
	HTuple  hv_Cols_Cont, hv_Len_Cont, hv_Idx_Cont, hv_IsInside;
	HTuple  hv_Angle, hv_Count_Ang, hv_Len, hv_Ind_Ang1, hv_Ang1;
	HTuple  hv_Ind_Ang2, hv_Ang2, hv_mean_RegL, hv_dt1;

	Union1(ho_SkeletonsFWMS, &ho_SkeletonFWMS);
	DilationCircle(ho_SkeletonFWMS, &ho_SkeletonFWMSd, 9.5);
	ReduceDomain(ho_Im, ho_SkeletonFWMSd, &ho_Imp);

	hv_Middle = 180;
	if (0 != hv_autothr)
	{
		GrayHistoAbs(ho_SkeletonFWMSd, ho_Imp, 1, &hv_AbsoluteHisto);
		CreateFunct1dArray(hv_AbsoluteHisto, &hv_FAbsoluteHisto);

		TupleFirstN(hv_FAbsoluteHisto, hv_Middle, &hv_FSpace_T);
		SmoothFunct1dMean(hv_FSpace_T, 9, 3, &hv_FSpace_T);
		LocalMinMaxFunct1d(hv_FSpace_T, "plateaus_center", "true", &hv_MinS, &hv_MaxS);
		Max_Index(hv_MaxS, hv_FSpace_T, &hv_Idx);
		(*hv_thrlga) = HTuple(hv_MaxS[hv_Idx]) + hv_maxexp;

	}
	else
	{
		(*hv_thrlga) = hv_fixedthr;
	}


	//* difference(RegProc, RegionI, RegThr_NEG)
	//* gray_histo_abs(RegThr_NEG, Im, 1, AbsoluteHisto)
	//create_funct_1d_array (AbsoluteHisto, FAbsoluteHisto)
	//smooth_funct_1d_mean (FAbsoluteHisto, 9, 3, FAbsoluteHistosm)
	//local_min_max_funct_1d (FAbsoluteHistosm, 'strict_min_max', 'true', Min, Max)
	//tuple_mean (Max, thrlga)

	//threshold (Imp, RegionTh, 140, 255)
	//threshold (Imp, RegionTh1, 110, 255)
	//threshold (Imp, RegionLGi, 85, 110)

	Threshold(ho_Imp, &ho_RegionTh, hv_thr, 255);
	hv_thr1 = hv_thr - (((hv_thr - (*hv_thrlga)).TupleAbs()) / 3);
	Threshold(ho_Imp, &ho_RegionTh1, hv_thr1, 255);
	Threshold(ho_Imp, &ho_RegionLGi, (*hv_thrlga), hv_thr1);
	OpeningCircle(ho_RegionLGi, &(*ho_RegionLG), 1.5);

	Threshold(ho_Im, &ho_RegionTh3, (*hv_thrlga), 255);


	GenEmptyObj(&ho_RegionsLowDefs);
	Connection((*ho_RegionLG), &ho_RegionLG_CON);
	CountObj(ho_RegionLG_CON, &hv_Count);
	{
		HTuple end_val43 = hv_Count;
		HTuple step_val43 = 1;
		for (hv_i = 1; hv_i.Continue(end_val43, step_val43); hv_i += step_val43)
		{

			//hv_i = 10;
			SelectObj(ho_RegionLG_CON, &ho_RegL_SEL, hv_i);
			AreaCenter(ho_RegL_SEL, &hv_Area, &hv_Row, &hv_Col);
			if (0 != (hv_Area == 0))
			{
				continue;
			}

			Difference(ho_RegL_SEL, ho_RegionTh1, &ho_RegLopi_Diff);
			GetRegionPoints(ho_RegLopi_Diff, &hv_Rows, &hv_Columns);
			GetGrayval(ho_Im, hv_Rows, hv_Columns, &hv_Locality_Gray);

			TupleSort(hv_Locality_Gray, &hv_Sorted);
			TupleLength(hv_Sorted, &hv_Length);
			TupleFirstN(hv_Sorted, hv_Length / 2, &hv_Selected);
			TupleMean(hv_Selected, &hv_mean_RegLopi);

			//**************************************
			DilationCircle(ho_RegL_SEL, &ho_RegL_DIL, 5);
			Difference(ho_RegL_DIL, ho_RegL_SEL, &ho_RegL_Diff);
			Difference(ho_RegL_Diff, ho_RegionTh1, &ho_RegL_Diff);
			//difference (RegL_Diff, RegionTh3, RegL_Diff)

			//?????? ???? ? ????? ?? ???????.
			//???? ??????????? ? RegL_Diff ? ????????? ???? ???????????
			hv_Angle_Cont = HTuple();
			//gen_circle (Circle, Row, Col, 7)
			DilationCircle(ho_RegL_SEL, &ho_Circle, 4);
			GetRegionContour(ho_Circle, &hv_Rows_Cont, &hv_Cols_Cont);
			TupleLength(hv_Rows_Cont, &hv_Len_Cont);
			{
				HTuple end_val74 = hv_Len_Cont - 1;
				HTuple step_val74 = 1;
				for (hv_Idx_Cont = 0; hv_Idx_Cont.Continue(end_val74, step_val74); hv_Idx_Cont += step_val74)
				{
					TestRegionPoint(ho_RegL_Diff, HTuple(hv_Rows_Cont[hv_Idx_Cont]), HTuple(hv_Cols_Cont[hv_Idx_Cont]),
						&hv_IsInside);

					//tuple_atan2 (Row - Rows_Cont[Idx_Cont], Col - Cols_Cont[Idx_Cont], Angle)
					//*         tuple_concat(Angle_Cont, Angle, Angle_Cont)

					if (0 != (hv_IsInside == 1))
					{
						//?????? ???? ? ?????????? ? ????
						TupleAtan2(hv_Row - HTuple(hv_Rows_Cont[hv_Idx_Cont]), hv_Col - HTuple(hv_Cols_Cont[hv_Idx_Cont]),
							&hv_Angle);
						TupleConcat(hv_Angle_Cont, hv_Angle, &hv_Angle_Cont);
						GenCircle(&ho_Circle1, HTuple(hv_Rows_Cont[hv_Idx_Cont]), HTuple(hv_Cols_Cont[hv_Idx_Cont]),
							1);
					}
				}
			}

			//???? ??????????????? ????
			hv_Count_Ang = 0;
			TupleLength(hv_Angle_Cont, &hv_Len);
			{
				HTuple end_val91 = hv_Len - 1;
				HTuple step_val91 = 1;
				for (hv_Ind_Ang1 = 0; hv_Ind_Ang1.Continue(end_val91, step_val91); hv_Ind_Ang1 += step_val91)
				{
					hv_Ang1 = HTuple(hv_Angle_Cont[hv_Ind_Ang1]) + 3.14;
					if (0 != (hv_Ang1 > 6.28))
					{
						hv_Ang1 = hv_Ang1 - 6.28;
					}

					{
						HTuple end_val97 = hv_Len - 1;
						HTuple step_val97 = 1;
						for (hv_Ind_Ang2 = 0; hv_Ind_Ang2.Continue(end_val97, step_val97); hv_Ind_Ang2 += step_val97)
						{
							hv_Ang2 = HTuple(hv_Angle_Cont[hv_Ind_Ang2]);

							if (0 != (((hv_Ang1 - hv_Ang2).TupleAbs()) < 0.5))
							{
								hv_Count_Ang += 1;
							}
						}
					}
				}
			}

			GetRegionPoints(ho_RegL_Diff, &hv_Rows, &hv_Columns);
			GetGrayval(ho_Im, hv_Rows, hv_Columns, &hv_Locality_Gray);
			TupleSort(hv_Locality_Gray, &hv_Sorted);
			TupleLength(hv_Sorted, &hv_Length);
			if (0 != (hv_Length == 0))
			{
				continue;
			}
			TupleFirstN(hv_Sorted, hv_Length / 2, &hv_Selected);
			TupleMedian(hv_Selected, &hv_mean_RegL);
			hv_dt1 = hv_mean_RegLopi - hv_mean_RegL;

			if (0 != (HTuple(hv_dt1 > hv_dt).TupleAnd(hv_Count_Ang > 0)))
			{
				ConcatObj(ho_RegionsLowDefs, ho_RegL_SEL, &ho_RegionsLowDefs);
			}

			//**************************************
			//get_region_points (RegL_Diff, Rows, Columns)
			//get_grayval (Im, Rows, Columns, Locality_Gray)
			//tuple_sort (Locality_Gray, Sorted)
			//tuple_length (Sorted, Length)
			//if (Length == 0)
			  //continue
			//endif
			//tuple_first_n (Sorted, Length/2, Selected)
			//tuple_mean (Selected, mean_RegL)
			//dt1 := mean_RegLopi - mean_RegL




			//connection (RegL_Diff, RegL_Diff_CON)
			//*     select_obj (RegL_Diff_CON, Reg_SEL, 2)

			//select_region_spatial (RegL_Diff, RegL_SEL, 'above', RegionIndex11, RegionIndex21)
			//select_region_spatial (RegL_Diff, RegL_SEL, 'below', RegionIndex12, RegionIndex22)
			//select_region_spatial (RegL_Diff, RegL_SEL, 'left', RegionIndex13, RegionIndex23)
			//select_region_spatial (RegL_Diff, RegL_SEL, 'right', RegionIndex14, RegionIndex24)


			//**************************************

			//dilation_circle (RegL_SEL, RegL_DIL, 8)
			//difference (RegL_DIL, RegL_SEL, RegL_Diff)
			//difference (RegL_Diff, RegionTh1, RegL_Diff)
			//get_region_points (RegL_Diff, Rows, Columns)
			//get_grayval (Im, Rows, Columns, Locality_Gray)

			//tuple_sort (Locality_Gray, Sorted)
			//tuple_length (Sorted, Length)
			//if (Length == 0)
			  //continue
			//endif
			//tuple_first_n (Sorted, Length/2, Selected)
			//tuple_mean (Selected, mean_RegL)

			//*     dt1 := mean_RegLopi - mean_RegL
			//*     if (dt1 > dt)
			//concat_obj (RegionsLowDefs, RegL_SEL, RegionsLowDefs)
			//*     endif    

			//dilation_circle (RegL_SEL, RegL_DIL, 5)
			//difference (RegL_DIL, RegL_SEL, RegL_Diff)
			//difference (RegL_Diff, RegionH_DIL, RegL_Diff)
			//difference (RegL_Diff, RegionTh1, RegL_Diff)
			//connection (RegL_Diff, RegL_Diff_CON)
			//*     count_obj(RegL_Diff_CON, Number)
			//N_Fl := 1
			//*     for n := 1 to Number by 1


			//*         select_obj (RegL_Diff_CON, RegL_Diff_SEL, n)
			//*         get_region_points (RegL_Diff_SEL, Rows, Columns)
			//get_grayval (Im, Rows, Columns, Locality_Gray)

			//tuple_sort (Locality_Gray, Sorted)
			//tuple_length (Sorted, Length)
			//tuple_first_n (Sorted, Length/2, Selected)
			//tuple_median (Selected, mean_RegL)


			//tuple_mean (Locality_Gray, mean_RegL)
			//tuple_median (Locality_Gray, mean_RegL)

			//dt1 := mean_RegLopi - mean_RegL
			//*         if (dt1 < dt)
			//N_Fl := 0
			//*            break
			//*         endif    

			//*     endfor

			//*     if (N_Fl == 1)
			//concat_obj (RegionsLowDefs, RegL_SEL, RegionsLowDefs)
			//*     endif    




		}
	}

	HObject ho_LGC;
	DilationCircle(ho_RegionsLowDefs, &ho_LGC, 17);
	(*ho_RegionLG) = ho_LGC;
	//(*ho_RegionLG) = ho_RegionsLowDefs;



	return;
}


/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

