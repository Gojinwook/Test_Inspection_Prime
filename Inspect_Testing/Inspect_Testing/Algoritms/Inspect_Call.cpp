/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
		Inspect_Call.h.cpp
 *
 * @Summary
 *
 * @Description
 *		
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 2/28/2024 2:53:00 PM
 */

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include "Inspect_Common.h"
#include ".\Lib\FWMFuncs.h"
#include "PinholeIsland.h"
#include "OpenShort.h"
#include "Alig2.h"
#include "BuildIRB.h"
#include "LowGray.h"
//#include ".\Lib\Slice.h"
#include "Slice.h"
#include ".\Lib\Open_Short_2.h"

#include "Inspect_Call.h"

#include "..\Define.h"


#define HCall(ret, x)  ret = x;

using namespace inspect_utils;

namespace inspect_call
{

	CInspect_Call::CInspect_Call(int thread_num, TSQueueSM<CString>* queue_log)
	{
		m_thread_num = thread_num;
		m_queue_log = queue_log;

		////utils::params.max_shorts_num = 100;
		//utils::params.shorts_num = 0;
		////utils::params.short_max_active = false;
		////utils::params.short_max_active = param.m_iDefectNum;
		//utils::params.skip_proc = false;

		

		m_pFWM = new CFWM();
		m_slice = new CSlice();

		//--- Open_Short_2 -------------
		op2 = new Open_Short_2();

		GenEmptyObj(&m_ho_RegionsDT_S);
		GenEmptyObj(&m_ho_RegionsDT_P);
	}
	CInspect_Call::~CInspect_Call()
	{
		// TODO: Freeing up memory
	}

	void CInspect_Call::Init()
	{
		m_pFWM->Alloc2(Np, NpS);


		m_slice->m_SensLen = m_Inspection_Params.LG_SensLen;
		m_slice->m_SensLenD2 = m_slice->m_SensLen / 2;
		m_slice->m_szw = m_Inspection_Params.LG_m_szw;
		m_slice->m_prsLG = m_Inspection_Params.LG_m_prsLG;
		m_slice->Init();

		op2->Init_Inspection(HTuple(m_Path_Model));
		if (op2->params.model_loaded == 1)
		{
			Queue_Add(m_queue_log, NULL, "--- Inspection Open_Short_2() - Model loading successful");
		}
		else
		{
			Queue_Add(m_queue_log, NULL, "### Inspection Open_Short_2() - Error loading model");
		}

	}

	bool CInspect_Call::Alignment()
	{
		m_inspect_status = "Alignment()";

		HTuple hv_alig2_ok;
		DWORD time_start = GetTickCount();

		HTuple hv_Сombination = 1;
		if (m_Inspection_Params.alignment_type == 1)
		{
			alig2::Alig2(inspect_param.image, &m_local_Im, hv_Сombination, &m_hv_Shift_Row, &m_hv_Shift_Col, &hv_alig2_ok, &hv_ReturnHomMat2D);
		}
		else
		{
			alig2::Alig2_Lite(inspect_param.image, &m_local_Im, hv_Сombination, &m_hv_Shift_Row, &m_hv_Shift_Col, &hv_alig2_ok, &hv_ReturnHomMat2D);
		}

		GetImageSize(m_local_Im, &m_hv_Width, &m_hv_Height);

		DWORD time_end = GetTickCount();

		if (!hv_alig2_ok == 1)
		{
			Queue_Add(m_queue_log, NULL, "--- Alignment() - FALSE (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time: %0*.4f s",
				inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

			return false;
		}
		else
		{
			Queue_Add(m_queue_log, NULL, "--- Alignment() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time: %0*.4f s",
				inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

			//GetImageSize(m_local_Im, &m_hv_Width, &m_hv_Height);
			return true;
		}
	}


	void CInspect_Call::GM_Auto_Threshold(int &threshold_open, int &threshold_short)
	{

		// Local iconic variables
		HObject  ho_Gi, ho_ImAlig, ho_Regnoproc, ho_Gi_Reg;
		HObject  ho_Reg_Patern, ho_Reg_P1, ho_Reg_Space, ho_Reg_Patern_Sel;
		HObject  ho_Skeleton, ho_Reg_Space_Sel;

		// Local control variables
		HTuple  hv_path_model, hv_NumObj, hv_th_patern_min;
		HTuple  hv_th_patern_max, hv_Index, hv_Min1, hv_Max1, hv_Range1;
		HTuple  hv_th_patern_min_Sort, hv_Selected, hv_th_min, hv_th_max;
		HTuple  hv_th_open, hv_th_space_min, hv_th_space_max, hv_th_space_max_Sort;
		HTuple  hv_th_short, hv_Min, hv_Max, hv_Range, hv_th_open1;
		HTuple  hv_th_short1;

		//******************************************************
		//** Загрузка данных
		//******************************************************
		//hv_path_model = "G:/SAMSUNG/Image/Old2/Old/GM/250312_OpenShort2/250312/Glassmask_LayerL1-SHORT3.3-0200_250312/Model/";
		//ReadImage(&ho_Gi, "G:/SAMSUNG/Image/Old2/Old/GM/250312_OpenShort2/250312/Glassmask_LayerL1-SHORT3.3-0200_250312/Model/Gi.tif");
		//ReadImage(&ho_ImAlig, "G:/SAMSUNG/Image/Old2/Old/GM/250312_OpenShort2/250312/Glassmask_LayerL1-SHORT3.3-0200_250312/Model/Im-Alig.tif");
		//ReadObject(&ho_Regnoproc, "G:/SAMSUNG/Image/Old2/Old/GM/250312_OpenShort2/250312/Glassmask_LayerL1-SHORT3.3-0200_250312/Model/RegNoProc.hobj");

		//**************************************************************

		//Threshold(ho_Gi, &ho_Gi_Reg, 128, 255);
		Difference(m_ho_RegionG, m_ho_RegNoProc, &ho_Reg_Patern);
		Connection(ho_Reg_Patern, &ho_Reg_Patern);
		SelectShape(ho_Reg_Patern, &ho_Reg_Patern, "area", "or", 10.0, 10000);

		//**************************************************************

		Union1(ho_Reg_Patern, &ho_Reg_Patern);
		ClosingRectangle1(ho_Reg_Patern, &ho_Reg_P1, 10, 10);
		Intersection(m_ho_RegionG, ho_Reg_P1, &ho_Reg_Patern);
		Difference(ho_Reg_P1, ho_Reg_Patern, &ho_Reg_Space);
		Connection(ho_Reg_Patern, &ho_Reg_Patern);
		Connection(ho_Reg_Space, &ho_Reg_Space);

		//**************************************************************

		CountObj(ho_Reg_Patern, &hv_NumObj);
		hv_th_patern_min = HTuple();
		hv_th_patern_max = HTuple();
		{
			HTuple end_val29 = hv_NumObj;
			HTuple step_val29 = 1;
			for (hv_Index = 1; hv_Index.Continue(end_val29, step_val29); hv_Index += step_val29)
			{
				SelectObj(ho_Reg_Patern, &ho_Reg_Patern_Sel, hv_Index);
				Skeleton(ho_Reg_Patern_Sel, &ho_Skeleton);
				MinMaxGray(ho_Skeleton, m_local_ImModified, 0, &hv_Min1, &hv_Max1, &hv_Range1);
				hv_th_patern_min[hv_Index - 1] = hv_Min1;
				hv_th_patern_max[hv_Index - 1] = hv_Max1;
			}
		}

		TupleSort(hv_th_patern_min, &hv_th_patern_min_Sort);
		TupleFirstN(hv_th_patern_min_Sort, 10, &hv_Selected);

		TupleMean(hv_Selected, &hv_th_min);
		TupleMean(hv_th_patern_max, &hv_th_max);
		hv_th_open = hv_th_min + ((hv_th_max - hv_th_min) / 2);

		//**************************************************************

		CountObj(ho_Reg_Space, &hv_NumObj);
		hv_th_space_min = HTuple();
		hv_th_space_max = HTuple();
		{
			HTuple end_val49 = hv_NumObj;
			HTuple step_val49 = 1;
			for (hv_Index = 1; hv_Index.Continue(end_val49, step_val49); hv_Index += step_val49)
			{
				SelectObj(ho_Reg_Space, &ho_Reg_Space_Sel, hv_Index);
				Skeleton(ho_Reg_Space_Sel, &ho_Skeleton);
				MinMaxGray(ho_Skeleton, m_local_ImModified, 0, &hv_Min1, &hv_Max1, &hv_Range1);
				hv_th_space_min[hv_Index - 1] = hv_Min1;
				hv_th_space_max[hv_Index - 1] = hv_Max1;
			}
		}

		TupleSort(hv_th_space_max, &hv_th_space_max_Sort);
		TupleInverse(hv_th_space_max_Sort, &hv_th_space_max_Sort);
		TupleFirstN(hv_th_space_max_Sort, 10, &hv_Selected);

		TupleMean(hv_th_space_min, &hv_th_min);
		TupleMean(hv_Selected, &hv_th_max);
		hv_th_short = hv_th_min + ((hv_th_max - hv_th_min) / 2);

		//**************************************************************
		TupleInt(hv_th_open, &hv_th_open);
		TupleInt(hv_th_short, &hv_th_short);
		threshold_open = hv_th_open;
		threshold_short = hv_th_short;

		//**************************************************************
		if (0)
		{
			SelectObj(ho_Reg_Patern, &ho_Reg_Patern_Sel, 88);
			MinMaxGray(ho_Reg_Patern_Sel, m_local_ImModified, 0, &hv_Min, &hv_Max, &hv_Range);
			hv_th_open1 = hv_Max - (hv_Range / 2);

			SelectObj(ho_Reg_Space, &ho_Reg_Space_Sel, 63);
			MinMaxGray(ho_Reg_Space_Sel, m_local_ImModified, 0, &hv_Min1, &hv_Max1, &hv_Range1);
			hv_th_short1 = hv_Max1 - (hv_Range1 / 2);
		}


	}

	void CInspect_Call::OpenShort2_Split_Defect(HObject ho_Defects, HObject &ho_Defects_Split, HTuple hv_Distanse_Defect = 10)
	{
		HTuple hv_Count, hv_Count1, hv_Index, hv_Index1, hv_Rows, hv_Cols, hv_Point_Count, hv_Pos, hv_Radius;
		HObject ho_Conn_Reg, ho_Reg_SEL, ho_Skeleton, ho_Reg_SEL_XLD, ho_Reg_SEL_XLD1;
		HTuple hv_Split_Rows = HTuple();
		HTuple hv_Split_Cols = HTuple();

		try
		{
			Connection(ho_Defects, &ho_Conn_Reg);
			GenEmptyObj(&ho_Defects_Split);
			CountObj(ho_Conn_Reg, &hv_Count);

			HTuple end_val713 = hv_Count;
			HTuple step_val713 = 1;
			for (hv_Index = 1; hv_Index <= hv_Count; hv_Index += 1)
			{
				//Получим объект дефекта
				SelectObj(ho_Conn_Reg, &ho_Reg_SEL, hv_Index);
				//Скелетон на объект
				Skeleton(ho_Reg_SEL, &ho_Skeleton);
				//Получим XLD контур
				GenContoursSkeletonXld(ho_Skeleton, &ho_Reg_SEL_XLD, 1, "filter");

				CountObj(ho_Reg_SEL_XLD, &hv_Count1);
				for (hv_Index1 = 1; hv_Index1 <= hv_Count1; hv_Index1 += 1)
				{
					SelectObj(ho_Reg_SEL_XLD, &ho_Reg_SEL_XLD1, hv_Index1);

					//Получим координаты всех точек
					GetContourXld(ho_Reg_SEL_XLD1, &hv_Rows, &hv_Cols);
					TupleLength(hv_Rows, &hv_Point_Count);

					for (hv_Pos = 1; hv_Pos < hv_Point_Count; hv_Pos += hv_Distanse_Defect)
					{
						hv_Split_Rows[hv_Split_Rows.TupleLength()] = HTuple(hv_Rows[hv_Pos]);
						hv_Split_Cols[hv_Split_Cols.TupleLength()] = HTuple(hv_Cols[hv_Pos]);
					}
				}
			}

			if (hv_Split_Cols.TupleLength() > 0)
			{
				TupleGenConst(hv_Split_Cols.TupleLength(), 10, &hv_Radius);
				GenCircle(&ho_Defects_Split, hv_Split_Rows, hv_Split_Cols, hv_Radius);
			}
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

			Queue_Add(m_queue_log, NULL, "*** Halcon Exception [%s] : <%s>%s (FrameNo=%d, UnitNo=%d, Thread=%d)",
				"OpenShort2_Split_Defect", sOperatorName, sErrMsg, inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num);
			return;
		}

	}

	int CInspect_Call::OpenShort2()
	{
		m_inspect_status = "Open_Short_2()";
		DWORD time_start = GetTickCount();

		if (op2->params.model_loaded == 0)
		{
			Queue_Add(m_queue_log, NULL, "--- Inspection Open_Short_2() - ERROR. Model not loaded");
			return 0;
		}
		{
			// --- Обучение
			// Допустимый разрыв серого при обучении
			op2->params.cv_Gray_Gap_Teach = 20;								// 20
			// Размер буфера для Points_Buff. Размер изображения?
			op2->params.buff_size = 100000000;								// 100000000
			// Размер инкремента буффера для Cross_Gray
			op2->params.alloc_step = 1000000;
			op2->params.div_Min_CAD_Area_Open = 3;						// Минимальный размер CAD / div_Min_CAD_Area
			op2->params.div_Min_CAD_Area_Short = 3;						// Минимальный размер CAD / div_Min_CAD_Area

			//op2->params.сv_Min_CAD_Area_Open = 384;						// Минмальная допустимая площадь региона CAD для идентификации номеров закрашивания
			//op2->params.сv_Gray_Gap_Open = 1569;						// Допустимый разрыв серого
			//op2->params.сv_Min_CAD_Area_Short = 3841;					// Минмальная допустимая площадь региона CAD для идентификации номеров закрашивания
			//op2->params.сv_Gray_Gap_Short = 1569;						// Допустимый разрыв серого

			//op2->params.сv_Min_CAD_Area_Open = 290;						// Минмальная допустимая площадь региона CAD для идентификации номеров закрашивания
			//op2->params.сv_Min_CAD_Area_Short = 2903;					// Минмальная допустимая площадь региона CAD для идентификации номеров закрашивания
			//op2->params.сv_Gray_Gap_Open = 1224;						// Допустимый разрыв серого
			//op2->params.сv_Gray_Gap_Short = 1224;						// Допустимый разрыв серого

			// --- Inspection parameters 
			op2->params.cv_Min_CAD_Area_Open = inspect_call::m_Inspection_Params.cv_Min_CAD_Area_Open;			// Minimum allowed CAD region area for identifying fill numbers Open
			op2->params.cv_Min_CAD_Area_Short = inspect_call::m_Inspection_Params.cv_Min_CAD_Area_Short;		// Minimum allowed CAD region area for identifying fill numbers Short
			op2->params.cv_Gray_Gap_Open = inspect_call::m_Inspection_Params.cv_Gray_Gap_Open;					// Acceptable gray gap Open
			op2->params.cv_Gray_Gap_Short = inspect_call::m_Inspection_Params.cv_Gray_Gap_Short;				// Acceptable gray gap Shore

			op2->params.check_diagonals = inspect_call::m_Inspection_Params.check_diagonals;					// Check diagonal pixels when filling

			// Check Open/Short overkills
			op2->params.check_overkills_Open = 1;
			op2->params.check_overkills_Short = 1;

			op2->params.save_test_file = 0;
			op2->params.path_test = "C:\\TmpIm\\";
		}

		// Threshold

		//if (op2->params.cv_Min_CAD_Area_Open == op2->params.cv_Min_CAD_Area_Short &&
		//	op2->params.cv_Gray_Gap_Open == op2->params.cv_Gray_Gap_Short &&
		//	op2->params.cv_Min_CAD_Area_Open == 10)
		if (inspect_call::m_Inspection_Params.open_short_GM == 1)
		{
			CopyImage(m_local_Im, &m_local_ImModified);

			int threshold_open, threshold_short;
			GM_Auto_Threshold(threshold_open, threshold_short);

			if (inspect_call::m_Inspection_Params.open_threshold == 0)
			{
				op2->params.cv_Thr_Open = threshold_open;
			}
			else
			{
				op2->params.cv_Thr_Open = inspect_call::m_Inspection_Params.open_threshold;
			}

			if (inspect_call::m_Inspection_Params.short_threshold == 0)
			{
				op2->params.cv_Thr_Short = threshold_short;
			}
			else
			{
				op2->params.cv_Thr_Short = inspect_call::m_Inspection_Params.short_threshold;
			}

			op2->params.cv_Thr_Inspect = inspect_call::m_Inspection_Params.short_threshold;

		}
		else
		{
			op2->params.cv_Thr_Open = threshold;
			op2->params.cv_Thr_Short = threshold;
			op2->params.cv_Thr_Inspect = threshold;
		}

		// Добавим PinHoleIsland для удаления из регионов инспекции
		op2->m_ho_RegionsIsl = m_ho_RegionsIsl;
		op2->m_ho_RegionPH = m_ho_RegionPH;

		op2->Inspect_Open_Short(m_local_ImModified, m_ho_RegionG, m_ho_RegNoProc);

		/// TODO: Add to the parameters, if necessary
		bool params_split_Open_Short2 = true;
		HTuple hv_Distanse_Defect = 10;

		if (params_split_Open_Short2)
		{
			OpenShort2_Split_Defect(op2->ho_Open, m_ho_RegionsOpen2, hv_Distanse_Defect);
			OpenShort2_Split_Defect(op2->ho_Short, m_ho_RegionsShort2, hv_Distanse_Defect);
			op2->ho_Open = m_ho_RegionsOpen2;
			op2->ho_Short = m_ho_RegionsShort2;
		}
		else
		{
			m_ho_RegionsOpen2 = op2->ho_Open;
			m_ho_RegionsShort2 = op2->ho_Short;
			m_ho_Inspect_ID = op2->ho_Inspect_ID;
		}







		int test = 0;
		if (test == 1)
		{
			WriteObject(m_ho_RegionsOpen2, "C:\\Temp\\m_ho_RegionsOpen2");
			WriteObject(m_ho_RegionsShort2, "C:\\Temp\\m_ho_RegionsShort2");
			WriteObject(m_ho_RegionsIsl, "C:\\Temp\\m_ho_Regionm_ho_RegionsIslsOpen2");
			WriteObject(m_ho_RegionPH, "C:\\Temp\\m_ho_RegionPH");

		}








		/// --------------------------------------------------------------------
		/// TODO: defect attributes --------------------------------------------
		/// --------------------------------------------------------------------


		/// --------------------------------------------------------------------
		/// TODO: How to use defect attributes ---------------------------------
		/// --------------------------------------------------------------------
		HObject ho_Defects_C, ho_Sel_Defect;
		HTuple hv_Number_Defects, hv_Area, hv_Row, hv_Col;
		Connection(m_ho_RegionsOpen2, &ho_Defects_C); // You must use defect m_ho_RegionsOpen2 or m_ho_RegionsShort2
		CountObj(ho_Defects_C, &hv_Number_Defects);
		Hlong cv_Number_Defects = hv_Number_Defects.ToLArr()[0];
		for (int cv_Idx = 1; cv_Idx <= cv_Number_Defects; cv_Idx++)
		{
			SelectObj(ho_Defects_C, &ho_Sel_Defect, cv_Idx);
			AreaCenter(ho_Sel_Defect, &hv_Area, &hv_Row, &hv_Col);

			// Defect coordinates hv_Row, hv_Col
			TupleInt(hv_Row, &hv_Row);
			TupleInt(hv_Col, &hv_Col);

			// Defect attributes
			HTuple hv_Gray_Min, hv_Gray_Max, hv_Gray_Median;
			op2->Get_Defect_Attribute(
				m_local_Im,
				ho_Sel_Defect,
				hv_Gray_Min, hv_Gray_Max, hv_Gray_Median);
		}

		//GenEmptyObj(&m_ho_RegionsOp);
		//GenEmptyObj(&m_ho_RegionOpCon);
		m_ho_RegionOpCon = m_ho_RegionsOpen2;
		m_ho_RegionsShi = m_ho_RegionsShort2;

		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- CInspect_Call Open_Short_2() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time: %0*.4fs. Open=%d(%d), Short=%d(%d)",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.,
			op2->params.count_defects_open, op2->params.count_overkill_open, op2->params.count_defects_short, op2->params.count_overkill_short);
		if ((double)(time_end - time_start) / 1000. > 5.)
			time_end = time_end;
		return op2->params.count_defects_open + op2->params.count_defects_short;

	}

	int CInspect_Call::OpenShort()
	{
		m_inspect_status = "OpenShort()";

		int defect_count = 0;
		DWORD time_start = GetTickCount();

		HObject ho_SkeletonGnoR;
		MakeSkeletonGnoR(m_local_Im, m_ho_RegionI, m_ho_RegNoProc, m_ho_SkeletonG, &ho_SkeletonGnoR);
		SetSystem("neighborhood", 8);

		HTuple hv_s1; // ???

		HObject ho_RegionFWMdil, ho_RegSkel, ho_RegSkeld, ho_SkeletonI, ho_EndPointsI, ho_JuncPointsI, ho_SkelPointsI, ho_RegionCheck;
		SkeletonsCheckPrep(
			m_ho_RegionFWM, m_ho_RegionInoIslnoPh,
			&ho_RegionFWMdil, &ho_RegSkel, &ho_RegSkeld, &ho_SkeletonI, &ho_EndPointsI, &ho_JuncPointsI, &ho_SkelPointsI, &ho_RegionCheck,
			m_Inspection_Params.hv_dilFWM, hv_s1, m_Inspection_Params.hv_dilS);

		m_inspect_status = "DetectShortN6()";
		DetectShortN6(m_ho_SkeletonGS, m_ho_RegionInoIslnoPh, m_ho_RegionISnoIslnoPh,
			m_ho_WidthImageS, m_ho_DirImageS, m_ho_RegionG, ho_RegionFWMdil,
			m_ho_GiDomain, //ho_RegionCheck,
			m_ho_RegNoProc,
			&m_ho_RegionsShi
		);

		GenEmptyObj(&m_ho_RegionsShi);

		OpeningCircle(m_ho_RegionsShi, &m_ho_RegionsShi, 1.5);
		{
			HTuple hv_num;
			CountObj(m_ho_RegionsShi, &hv_num);
			int short_num = hv_num.L();
		}

		m_inspect_status = "DetectOpenN5()";
		HObject ho_RegionsOpCon;
		GenEmptyObj(&m_ho_RegionsOp);
		GenEmptyObj(&m_ho_RegionOpCon);
		DetectOpenN5(m_ho_RegionG, ho_SkeletonGnoR, m_ho_RegionInoIslnoPh, m_ho_RegionISnoIslnoPh,
			m_ho_WidthImageP, m_ho_DirImageP, ho_RegionFWMdil,
			m_ho_GiDomain, //ho_RegionCheck, 
			&m_ho_RegionsOp, &ho_RegionsOpCon);
		Union1(ho_RegionsOpCon, &m_ho_RegionOpCon);
		{
			HTuple hv_num;
			CountObj(m_ho_RegionsOp, &hv_num);
			int open_num = hv_num.L();
		}

		SetSystem("neighborhood", 4);

		//int test = 0;
		if (WRTEST)
		{
			WriteObject(m_ho_RegionG, "C:\\Temp\\m_ho_RegionG");//match
			WriteObject(ho_SkeletonGnoR, "C:\\Temp\\ho_SkeletonGnoR");//match
			WriteObject(m_ho_RegionInoIslnoPh, "C:\\Temp\\m_ho_RegionInoIslnoPh"); //not match
			WriteObject(m_ho_RegionISnoIslnoPh, "C:\\Temp\\m_ho_RegionISnoIslnoPh");//not match
			WriteObject(ho_RegionFWMdil, "C:\\Temp\\ho_RegionFWMdil");//not match
			WriteObject(m_ho_RegionsShi, "C:\\Temp\\m_ho_RegionsShi");
			WriteObject(m_ho_RegionOpCon, "C:\\Temp\\m_ho_RegionOpCon");

			WriteObject(m_ho_RegionsOp, "C:\\Temp\\ho_RegionsOp");
			//WriteImage(m_local_Im, "jpg", 0, "C:\\PATTERN\\AOI_102723\\m_local_image");
		}

		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- CInspect_Call OpenShort() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time: %0*.4f s",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

		return defect_count;
	}

	int CInspect_Call::Low_Gray(int threshold)
	{
		//HTuple hv_maxexp = 10.;
		//HTuple hv_fixedthr = 57;
		//HTuple hv_autothr = 1;
		//HTuple hv_dt = 12;
		//HTuple hv_thrlga;
		//LowGrayProc(m_ho_SkeletonsFWMS, m_local_Im, &m_ho_RegionsLG,
		//	hv_maxexp, threshold, hv_autothr, hv_fixedthr, hv_dt,
		//	&hv_thrlga);

		if (m_Slice_Teach.m_Readed == false)
		{
			return 0;
		}
		m_slice->teach = &m_Slice_Teach;

		m_inspect_status = "Low_Gray()";

		int defect_count = 0;
		DWORD time_start = GetTickCount();

		m_slice->m_Threshold = threshold;
		m_slice->m_SensLen = m_Inspection_Params.LG_SensLen;
		m_slice->m_SensLenD2 = m_slice->m_SensLen / 2;
		m_slice->m_szw = m_Inspection_Params.LG_m_szw;
		m_slice->m_prsLG = m_Inspection_Params.LG_m_prsLG;

		m_slice->m_szw_P = m_Inspection_Params.LG_m_szw_P;
		m_slice->m_prsLG_P = m_Inspection_Params.LG_m_prsLG_P;
		m_slice->m_Avg_Par = m_Inspection_Params.LG_m_Avg_Par;
		//m_slice->m_szw_P = 50;
		//m_slice->m_prsLG_P = 12;
		//m_slice->m_Avg_Par = 30;//30;

		HTuple tl, h, w, t, hiptr;
		GetImagePointer1(m_local_Im, &hiptr, &t, &w, &h);
		UINT8 *pIm = (UINT8 *)(Hlong *)hiptr.L();
		int im_w = (int)w.L();
		int im_h = (int)h.L();

		// ------------------------------------------------------

		// Added by gojw
		m_slice->m_Count_Skel = m_slice->teach->m_iNoSkel;

		HTuple hv_Def_Rows;
		HTuple hv_Def_Cols;
		HTuple hv_PrcM_Min, hv_PrcM_Max, hv_PrcM_Median;
		m_slice->Proc(pIm, im_w, im_h, &hv_Def_Rows, &hv_Def_Cols, 
			m_hv_PrcM_Min, m_hv_PrcM_Max, m_hv_PrcM_Median);

		HTuple hv_len, hv_TR;
		TupleLength(hv_Def_Rows, &hv_len);
		if (hv_len > 0)
		{
			TupleGenConst(hv_len, 20, &hv_TR);
			GenCircle(&m_ho_RegionsLG, hv_Def_Rows, hv_Def_Cols, hv_TR);
		}
		int defects = hv_len.ToLArr()[0];
		// ------------------------------------------------------
		HTuple hv_Def_Rows_P;
		HTuple hv_Def_Cols_P;

		// Added by gojw
		//m_slice->m_Count_Skel = m_slice->teach->m_iNoSkel_P;

		//m_slice->Proc_P(pIm, im_w, im_h, &hv_Def_Rows_P, &hv_Def_Cols_P, 
		//	m_hv_PrcM_Min_P, m_hv_PrcM_Max_P, m_hv_PrcM_Median_P);

		//TupleLength(hv_Def_Rows_P, &hv_len);
		//GenEmptyObj(&m_ho_RegionsLG_P);
		//if (hv_len > 0)
		//{
		//	TupleGenConst(hv_len, 20, &hv_TR);
		//	GenCircle(&m_ho_RegionsLG_P, hv_Def_Rows_P, hv_Def_Cols_P, hv_TR);
		//}
		//int defects_P = hv_len.ToLArr()[0];
		int defects_P = 0;
		// ------------------------------------------------------

		/// --------------------------------------------------------------------
		/// TODO: defect attributes --------------------------------------------
		/// --------------------------------------------------------------------
		/// Attributes: m_hv_PrcM_Min, m_hv_PrcM_Max, m_hv_PrcM_Median
#ifdef _TEST_SW
		//Union2(m_ho_RegionsLG, m_ho_RegionsLG_P, &m_ho_RegionsLG);
		//TupleUnion(hv_PrcM_Min, m_hv_PrcM_Min_P, &m_hv_PrcM_Min_P);
		//TupleUnion(hv_PrcM_Max, m_hv_PrcM_Max_P, &m_hv_PrcM_Max_P);
		//TupleUnion(hv_PrcM_Median, m_hv_PrcM_Median_P, &m_hv_PrcM_Median_P);

		m_hv_PrcM_Min_P = hv_PrcM_Min;
		m_hv_PrcM_Max_P = hv_PrcM_Max;
		m_hv_PrcM_Median_P = hv_PrcM_Median;

#endif // _TEST_SW




		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- CInspect_Call Low_Gray() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time: %0*.4f s, Defects=%d, Defects_P=%d",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000., defects, defects_P);

		return defect_count;
	}

	int CInspect_Call::Pinhole_Island()
	{
		m_inspect_status = "Pinhole_Island()";

		int defect_count = 0;

		DWORD time_start = GetTickCount();

		//HObject ho_RegionPH;
		//HObject ho_RegionsIsl;
		ThresholdPHIsp(
			m_local_Im,	m_ho_RegProc,
			&m_ho_RegionPH, &m_ho_RegionsIsl,
			&m_ho_RegionISnoIslnoPh, &m_ho_RegionInoIslnoPh, &m_ho_RegionI,
			threshold, m_Inspection_Params.PHrmin, m_Inspection_Params.PHamin,
			m_Inspection_Params.Islrmin, m_Inspection_Params.Islamin, 200, &m_local_ImModified);
		HTuple IslR, IslA, PhR, PhA;
		RegionFeatures(m_ho_RegionsIsl, "inner_radius", &IslR);
		RegionFeatures(m_ho_RegionsIsl, "area", &IslA);
		RegionFeatures(m_ho_RegionPH, "inner_radius", &PhR);
		RegionFeatures(m_ho_RegionPH, "area", &PhA);
		if (WRTEST)
		{
			WriteObject(m_ho_RegionPH, "C:\\Temp1\\m_ho_RegionPH");
			WriteObject(m_ho_RegionsIsl, "C:\\Temp1\\m_ho_RegionsIsl");
			WriteObject(m_ho_RegionInoIslnoPh, "C:\\Temp1\\m_ho_RegionInoIslnoPh");
			WriteObject(m_local_ImModified, "C:\\Temp1\\m_local_ImModified");
		}

		
		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- Pinhole_Island() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time: %0*.4f s",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

		return defect_count;
	}

	void CInspect_Call::FixThinSpace(HObject ho_RegionInoD, HObject ho_DomainG, HObject *ho_RegionInoDr)
	{

		// Local iconic variables
		HObject  ho_DomR, ho_RegionInoDCr, ho_RegionsInoDCr;
		HObject  ho_RegionsInoDCrs, ho_RegionsInoDr;

		// Local control variables
		HTuple  hv_nb;

		GetSystem("neighborhood", &hv_nb);
		SetSystem("neighborhood", 4);
		//smallest_rectangle1 (RegionInoD, Row11, Column11, Row21, Column21)
		//* gen_rectangle1(DomR, Row11, Column11, Row21, Column21) 
		ho_DomR = ho_DomainG;
		Difference(ho_DomR, ho_RegionInoD, &ho_RegionInoDCr);
		Connection(ho_RegionInoDCr, &ho_RegionsInoDCr);
		SelectShape(ho_RegionsInoDCr, &ho_RegionsInoDCrs, "area", "and", 150, 99999999);
		//select_obj (RegionsInoDCr, RegionInoDCr, 1)
		Union1(ho_RegionsInoDCrs, &ho_RegionInoDCr);
		Difference(ho_DomR, ho_RegionInoDCr, &(*ho_RegionInoDr));
		Connection((*ho_RegionInoDr), &ho_RegionsInoDr);
		SetSystem("neighborhood", hv_nb);
		return;
	}

	int CInspect_Call::Build_IRB()
	{
		m_inspect_status = "Build_IRB()";

		DWORD time_start = GetTickCount();

		GenImageConst(&m_pFWM->ho_ImIRBS, "uint2", m_hv_Width, m_hv_Height);

		HObject /*ho_RegionInoD,*/ ho_RegionsEMPTY, ho_Domain;
		
		SetSystem("neighborhood", 4);
		/*GenEmptyObj(&m_ho_RegionsShi);
		GenEmptyObj(&m_ho_RegionOpCon);*/
		/*m_ho_RegionsShi = m_ho_RegionsOpen2;
		m_ho_RegionOpCon = m_ho_RegionsShort2;*/
		BuildRegionInoD(m_ho_RegionsShi, m_ho_SkeletonGS, m_ho_RegionInoIslnoPh, m_ho_RegionOpCon, m_ho_RegNoProc, 
			&m_pFWM->ho_RegionInoD);
		GetDomain(m_pFWM->ho_ImIRBS, &ho_Domain);
		//FixThinSpace(m_pFWM->ho_RegionInoD, ho_Domain, &m_pFWM->ho_RegionInoD);

		if (WRTEST) //WRTEST
		{
			WriteObject(m_ho_RegionsShi, "C:\\Temp\\m_ho_RegionsShi");
			WriteObject(m_ho_SkeletonGS, "C:\\Temp\\m_ho_SkeletonGS");
			WriteObject(m_ho_RegionInoIslnoPh, "C:\\Temp\\m_ho_RegionInoIslnoPh");
			WriteObject(m_ho_RegionOpCon, "C:\\Temp\\m_ho_RegionOpCon");
			WriteObject(m_ho_RegNoProc, "C:\\Temp1\\m_ho_RegNoProc");

			WriteObject(m_pFWM->ho_RegionInoD, "C:\\Temp1\\ho_RegionInoD");

		}

		HTuple hv_s1, hv_nEmpty, hv_nFWMSB, hv_nFWMP, hv_s, hv_s2, hv_stopperT;

		//BuildIRBS4B2(m_ho_SkeletonsFWMScut, m_pFWM->ho_RegionInoD, m_pFWM->ho_ImIRBS, 
		//	&m_pFWM->ho_ImIRBS,
		//	m_Inspection_Params.hv_expsize,
		//	hv_nEmpty, m_Inspection_Params.hv_dil,
		//	&hv_nFWMSB);
		/*m_Inspection_Params.hv_expsize = 451;
		inspect_call::m_Inspection_Params.hv_dil_S = 5;
		inspect_call::m_Inspection_Params.hv_dil_P = 9;*/
		/*DilationCircle(m_ho_SkelSEP, &m_ho_SkelSEP, 3);
		HObject ho_SkelSEP_Dil;
		DilationCircle(m_ho_SkelSEP, &ho_SkelSEP_Dil, 3);
		BuildIRBS42B2(m_ho_SkeletonsFWMScut, m_pFWM->ho_RegionInoD, m_pFWM->ho_ImIRBS, ho_SkelSEP_Dil,
			&m_pFWM->ho_ImIRBS,
			m_Inspection_Params.hv_expsize,
			hv_nEmpty, m_Inspection_Params.hv_dil,
			&hv_nFWMSB);*/
		/// нужно убрать отсюда и сохранить в параметрах
		//m_Inspection_Params.hv_stopperT = 11;
		///////
		BuildIRBS42B4(m_ho_SkeletonsFWMS, m_pFWM->ho_RegionInoD, m_pFWM->ho_ImIRBS,  m_ho_DirImageS, m_ho_WidthImageS, m_ho_SkeletonGS, m_ho_SkeletonsFWMScut,
			&m_pFWM->ho_ImIRBS,
			m_Inspection_Params.hv_expsize,
			//"maximal",
			hv_nEmpty, m_Inspection_Params.hv_dil_S, m_Inspection_Params.hv_stopperT,
			&hv_nFWMSB);

		/*BuildIRBS4B2(m_ho_SkeletonsFWMScut, m_pFWM->ho_RegionInoD, m_pFWM->ho_ImIRBS, m_ho_SkelSEP, 
			&m_pFWM->ho_ImIRBS,
			m_Inspection_Params.hv_expsize,
			hv_nEmpty, m_Inspection_Params.hv_dil,
			&hv_nFWMSB);*/

		/*BuildIRBS6B2(m_ho_SkeletonsFWMScut,  m_pFWM->ho_RegionInoD, m_pFWM->ho_ImIRBS, m_ho_SkelSEP, m_ho_RegionCut,
			&m_pFWM->ho_ImIRBS,
			m_Inspection_Params.hv_expsize,
			hv_nEmpty, m_Inspection_Params.hv_dil,
			&hv_nFWMSB);*/

		/*BuildIRBS5B2(m_ho_SkeletonsFWMScut, m_pFWM->ho_RegionInoD, m_pFWM->ho_ImIRBS, m_ho_SkelSEP,
			&m_pFWM->ho_ImIRBS, 
			m_Inspection_Params.hv_expsize,
			hv_nEmpty, m_Inspection_Params.hv_dil, 
			&hv_nFWMSB);*/

		//BuildIRBS5B2(m_ho_SkeletonsFWMS, m_ho_SkeletonsFWMScut,  m_pFWM->ho_ImIRBS, ho_RegionInoD, &m_pFWM->ho_ImIRBS, m_Inspection_Params.hv_expsize,
		//	hv_nEmpty, m_Inspection_Params.hv_dil, &hv_nFWMSB);

		GenImageConst(&m_pFWM->ho_ImIRBP, "uint2", m_hv_Width, m_hv_Height);
		BuildIRBP7B2(m_ho_SkeletonsFWMcut, m_pFWM->ho_RegionInoD, m_pFWM->ho_ImIRBP, m_ho_RegionsPNPi,
			&m_pFWM->ho_ImIRBP,
			m_Inspection_Params.hv_expsize, 
			hv_nEmpty, m_Inspection_Params.hv_dil_S,
			&hv_nFWMP);
		if (WRTEST)
		{
			WriteObject(m_pFWM->ho_RegionInoD, "C:\\Temp\\ho_RegionInoD");
			WriteObject(m_ho_SkeletonsFWMScut, "C:\\Temp\\m_ho_SkeletonsFWMScut");
			WriteObject(m_ho_SkeletonsFWMS, "C:\\Temp\\m_ho_SkeletonsFWMS");
			WriteObject(m_ho_RegionInoIslnoPh, "C:\\Temp\\m_ho_RegionInoIslnoPh");
			WriteObject(m_ho_RegionsShi, "C:\\Temp\\m_ho_RegionsShi");
			WriteObject(m_ho_RegNoProc, "C:\\Temp\\m_ho_RegNoProc");
			WriteObject(m_ho_RegionCut, "C:\\Temp\\m_ho_RegionCut");
			WriteObject(m_pFWM->ho_ImIRBS, "C:\\Temp1\\ho_ImIRBS");
			WriteObject(m_pFWM->ho_ImIRBP, "C:\\Temp1\\ho_ImIRBP");
			WriteObject(m_ho_DirImageS, "C:\\Temp\\ho_DirImageS");
			WriteObject(m_ho_WidthImageS, "C:\\Temp\\ho_WidthImageS");
			WriteObject(m_ho_SkelSEP, "C:\\Temp\\m_ho_SkelSEP");
			WriteObject(m_ho_RegionOpCon, "C:\\Temp\\m_ho_RegionOpCon");
			WriteObject(m_ho_RegionsPNPi, "C:\\Temp\\m_ho_RegionsPNPi");
			
			
		}


		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- Build_IRB() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time: %0*.4f s",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

		return 0;
	}

	int CInspect_Call::Proc_FWMS()
	{
		m_inspect_status = "Proc_FWMS()";

		HTuple tl, h, w, t, hiptr, hiptrIRBS, hiptrCLCS;

		DWORD time_start = GetTickCount();

		GetImagePointer1(m_ho_ImCLC, &hiptrCLCS, &t, &w, &h);
		GetImagePointer1(m_pFWM->ho_ImIRBS, &hiptrIRBS, &t, &w, &h);
		GetImagePointer1(m_local_Im, &hiptr, &t, &w, &h);
		pImCLCS = (UINT16 *)(Hlong *)hiptrCLCS.L();
		pImIRBS = (UINT16 *)(Hlong *)hiptrIRBS.L();
		pIm = (UINT8 *)(Hlong *)hiptr.L();
		m_iw = (int)w.L();
		m_ih = (int)h.L();

		m_pFWM->CPU_MSSpProc36B2(pIm, pImIRBS, pImCLCS, m_iw, m_ih, 0, NpS,
			(float)threshold,
			m_Inspection_Params.dclcmax,
			pP1Sx, pP1Sy, pP2Sx, pP2Sy, pPnumS, pPnum2S,
			m_pFWM->pWidthS, m_pFWM->pWminS, m_pFWM->pPInt1Sx, m_pFWM->pPInt1Sy, m_pFWM->pPInt2Sx, m_pFWM->pPInt2Sy, m_pFWM->pShiftS, m_pFWM->pPuseintS);


		int test =1;
		if (test)
		{
			//WriteObject(m_ho_ImCLC, "C:\\Temp\\m_ho_ImCLC");
			WriteObject(m_pFWM->ho_ImIRBS, "C:\\Temp\\ho_ImIRBS");
			HTuple PWidthS_tuple((float*)m_pFWM->pWidthS, NpS * 1);
			WriteTuple(PWidthS_tuple, "C:\\Temp\\PWidthS_tuple1.tup");
			//AfxMessageBox(_T("Done...PWidthS_tuple.tup saved!"));

		}



		m_pFWM->SPnumS = 0;
		m_pFWM->MBnumS = 0;

		// Post processing (finding defects)
		m_pFWM->FWMS_PostProcCPUnew(
			m_pFWM->pWidthS, pWGS, m_pFWM->pmedoS, pP3Sx, pP3Sy, NaCS, NpS,
			m_Inspection_Params.nvoid,
			pPaddrCS,
			m_Inspection_Params.sz1,
			m_Inspection_Params.sz2,
			m_Inspection_Params.rmskip,
			m_Inspection_Params.ang,
			m_Inspection_Params.prcMBS,
			m_Inspection_Params.prcSPS,
			m_Inspection_Params.absMBS,
			m_Inspection_Params.absSPS,
			m_Inspection_Params.minMBS,
			m_Inspection_Params.narWSpace,
			m_Inspection_Params.dlen,
			m_pFWM->pDefMBS,
			m_pFWM->pDefMBSprc,
			m_pFWM->pXdefMBS, m_pFWM->pYdefMBS,
			m_pFWM->pDefSPS,
			m_pFWM->pDefSPSprc,
			m_pFWM->pXdefSPS, m_pFWM->pYdefSPS,
			&m_pFWM->MBnumS, &m_pFWM->SPnumS
		);

		// -------------------------------------------------------------------

		/// --------------------------------------------------------------------
		/// TODO: defect attributes --------------------------------------------
		/// --------------------------------------------------------------------
		/// Attributes: m_hv_pDefMBSprc, m_hv_pDefSPSprc, m_hv_pDefMBS, m_hv_pDefSPS

		m_hv_pDefMBSprc = HTuple(m_pFWM->pDefMBSprc, m_pFWM->MBnumS);
		m_hv_pDefSPSprc = HTuple(m_pFWM->pDefSPSprc, m_pFWM->SPnumS);
		m_hv_pDefMBS = HTuple(m_pFWM->pDefMBS, m_pFWM->MBnumS);
		m_hv_pDefSPS = HTuple(m_pFWM->pDefSPS, m_pFWM->SPnumS);

		//FloatToTuple(m_pFWM->pDefMBSprc, m_pFWM->MBnumS, m_hv_pDefMBSprc);
		//FloatToTuple(m_pFWM->pDefSPSprc, m_pFWM->SPnumS, m_hv_pDefSPSprc);
		//FloatToTuple(m_pFWM->pDefMBS, m_pFWM->MBnumS, m_hv_pDefMBS);
		//FloatToTuple(m_pFWM->pDefSPS, m_pFWM->SPnumS, m_hv_pDefSPS);

		// -------------------------------------------------------------------

		//{
		//	HTuple PMBSx_tuple;
		//	HTuple PMBSy_tuple;
		//	FloatToTuple(m_pFWM->pXdefMBS, m_pFWM->MBnumS, PMBSx_tuple);
		//	FloatToTuple(m_pFWM->pYdefMBS, m_pFWM->MBnumS, PMBSy_tuple);

		//	GenEmptyObj(&m_ho_RegionsMBS);
		//	HTuple  hv_nMBS = HTuple(m_pFWM->MBnumS);
		//	if (hv_nMBS > 0)
		//	{
		//		HTuple hv_TR;
		//		TupleGenConst(hv_nMBS, 30, &hv_TR);
		//		GenCircle(&m_ho_RegionsMBS, PMBSx_tuple, PMBSy_tuple, hv_TR);
		//	}
		//}

		//// -------------------------------------------------------------------
		//{
		//	HTuple PSPSx_tuple;
		//	HTuple PSPSy_tuple;
		//	FloatToTuple(m_pFWM->pXdefSPS, m_pFWM->SPnumS, PSPSx_tuple);
		//	FloatToTuple(m_pFWM->pYdefSPS, m_pFWM->SPnumS, PSPSy_tuple);

		//	GenEmptyObj(&m_ho_RegionsSPS);
		//	HTuple  hv_nSPS = HTuple(m_pFWM->SPnumS);
		//	if (hv_nSPS > 0)
		//	{
		//		HTuple hv_TR;
		//		TupleGenConst(hv_nSPS, 30, &hv_TR);
		//		GenCircle(&m_ho_RegionsSPS, PSPSy_tuple, PSPSx_tuple, hv_TR);
		//	}
		//}

		// -------------------------------------------------------------------

		// ---------
		//INT64 *PSx = ConvertFloatToINT64(m_pFWM->pXdefMBS, m_pFWM->MBnumS);
		//INT64 *PSy = ConvertFloatToINT64(m_pFWM->pYdefMBS, m_pFWM->MBnumS);
		//INT64 *PSdef = ConvertFloatToINT64(m_pFWM->pDefMBS, m_pFWM->MBnumS);

		//HTuple PMBSx_tuple;
		//HTuple PMBSy_tuple;
		//HTuple PMBSdef_tuple;

		////for (int i = 0; i < m_pFWM->MBnumS; i++)
		////{
		////	PMBSx_tuple[i] = (HTuple)*(PSx + i);
		////	PMBSy_tuple[i] = (HTuple)*(PSy + i);
		////}
		//PMBSx_tuple = HTuple(PSx, m_pFWM->MBnumS);
		//PMBSy_tuple = HTuple(PSy, m_pFWM->MBnumS);
		//PMBSdef_tuple = HTuple(PSdef, m_pFWM->MBnumS);

		HTuple PMBSx_tuple;
		HTuple PMBSy_tuple;
		HTuple PMBSdef_tuple;
		FloatToTuple(m_pFWM->pXdefMBS, m_pFWM->MBnumS, PMBSx_tuple);
		FloatToTuple(m_pFWM->pYdefMBS, m_pFWM->MBnumS, PMBSy_tuple);
		FloatToTuple(m_pFWM->pDefMBS, m_pFWM->MBnumS, PMBSdef_tuple);


		GenEmptyObj(&m_ho_RegionsMBS);
		HTuple  hv_nMBS = HTuple(m_pFWM->MBnumS);
		HTuple hv_DistNmin, hv_DistNmax;
		if (hv_nMBS > 0)
		{
			HTuple hv_TR, hv_TRSh, hv_Tm5, hv_dSh, PMBSshx_tuple, PMBSshy_tuple, hv_numSh;
			HObject ho_RegionsSpSh;
			TupleGenConst(hv_nMBS, 30, &hv_TR);
			GenCircle(&m_ho_RegionsMBS, PMBSy_tuple, PMBSx_tuple, hv_TR);
			// Short detected in FWMS processing
			TupleGenConst(hv_nMBS, -5.0505, &hv_Tm5);
			TupleEqualElem(PMBSdef_tuple, hv_Tm5, &hv_dSh);
			TupleSelectMask(PMBSx_tuple, hv_dSh, &PMBSshx_tuple);
			TupleSelectMask(PMBSy_tuple, hv_dSh, &PMBSshy_tuple);
			TupleLength(PMBSshy_tuple, &hv_numSh);
			if (hv_numSh > 0)
			{
				TupleGenConst(hv_numSh, 35, &hv_TRSh);
				GenCircle(&ho_RegionsSpSh, PMBSshy_tuple, PMBSshx_tuple, hv_TRSh);
				DistancePr(m_ho_RegNoProc, PMBSshy_tuple, PMBSshx_tuple, &hv_DistNmin, &hv_DistNmax);
				ConcatObj(m_ho_RegionsShi, ho_RegionsSpSh, &m_ho_RegionsShi);
			}
			//GenCircle(&ho_RegionsSpSh, PMBSshy_tuple, PMBSshx_tuple, hv_TR);
			
		}

		// ---------
		//INT64 *PSSPx = ConvertFloatToINT64(m_pFWM->pXdefSPS, m_pFWM->SPnumS);
		//INT64 *PSSPy = ConvertFloatToINT64(m_pFWM->pYdefSPS, m_pFWM->SPnumS);

		//HTuple PSPSx_tuple;
		//HTuple PSPSy_tuple;
		////for (int i = 0; i < m_pFWM->SPnumS; i++)
		////{
		////	PSPSx_tuple[i] = (HTuple)*(PSSPx + i);
		////	PSPSy_tuple[i] = (HTuple)*(PSSPy + i);
		////}
		//PSPSx_tuple = HTuple(PSSPx, m_pFWM->SPnumS);
		//PSPSy_tuple = HTuple(PSSPy, m_pFWM->SPnumS);

		HTuple PSPSx_tuple;
		HTuple PSPSy_tuple;
		FloatToTuple(m_pFWM->pXdefSPS, m_pFWM->SPnumS, PSPSx_tuple);
		FloatToTuple(m_pFWM->pYdefSPS, m_pFWM->SPnumS, PSPSy_tuple);


		GenEmptyObj(&m_ho_RegionsSPS);
		HTuple  hv_nSPS = HTuple(m_pFWM->SPnumS);
		if (hv_nSPS > 0)
		{
			HTuple hv_TR;
			TupleGenConst(hv_nSPS, 30, &hv_TR);
			DistancePr(m_ho_RegNoProc, PSPSy_tuple, PSPSx_tuple, &hv_DistNmin, &hv_DistNmax);
			GenCircle(&m_ho_RegionsSPS, PSPSy_tuple, PSPSx_tuple, hv_TR);
		}

		if (0)
		{
			//WriteObject(m_ho_ImCLC, "C:\\Temp\\m_ho_ImCLC");
			WriteObject(m_ho_RegionsMBS, "C:\\Temp\\m_ho_RegionsMBS");
			HTuple PWidthS_tuple((float*)m_pFWM->pWidthS, NpS * 1);
			WriteObject(m_ho_RegionsSPS, "C:\\Temp\\m_ho_RegionsSPS");
			//AfxMessageBox(_T("Done...PWidthS_tuple.tup saved!"));

		}


		// ------
		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- Build_FWMS() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time %0*.4f s",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

		return 0;

	}

	int CInspect_Call::SetWpNarrowWide(HObject ho_SkeletonsFWM, HObject ho_WidthImageP, HTuple hv_kw,
		HTuple hv_dw, float *narrowwp, float *widewp)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_Rows1, hv_Columns1, hv_Grayval1, hv_wpmin;
		HTuple  hv_wpmax;

		//* SetWpNarrowWide - Auto setting narrowwp and widewp parameters
		//* 250513
		HObject ho_SkeletonFWMi;
		Union1(ho_SkeletonsFWM, &ho_SkeletonFWMi);
		GetRegionPoints(ho_SkeletonFWMi, &hv_Rows1, &hv_Columns1);
		GetGrayval(ho_WidthImageP, hv_Rows1, hv_Columns1, &hv_Grayval1);
		TupleMin(hv_Grayval1, &hv_wpmin);
		TupleMax(hv_Grayval1, &hv_wpmax);
		(*narrowwp) = (hv_wpmin*hv_kw) / 2.;
		(*widewp) = (hv_wpmax*hv_kw) + hv_dw;
		return 0;
	}

	void HistoMax0(HTuple hv_GrealminS_tuple,  HTuple *hv_hmax)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_Histo, hv_BinSize, hv_histoSum, hv_FHisto, hv_hthr;
		HTuple  hv_FHistoSm, hv_Min, hv_Max, hv_XValuesM, hv_YValuesM;
		HTuple  hv_nmax, hv_Hthr, hv_GreaterMaxThr, hv_Selected;

		//* HistoMax0 - 15.06.2025 finds 1st (0) max on histogram
		//* modified 26.06.2025 to avoid loosing small peaks on histogram

		TupleHistoRange(hv_GrealminS_tuple, 0, 255, 256, &hv_Histo, &hv_BinSize);
		TupleSum(hv_Histo, &hv_histoSum);
		CreateFunct1dArray(hv_Histo, &hv_FHisto);
		SmoothFunct1dMean(hv_FHisto, 9, 3, &hv_FHistoSm);
		//integrate_funct_1d (FHistoSm, Positive, Negative)
		LocalMinMaxFunct1d(hv_FHistoSm, "strict_min_max", "true", &hv_Min, &hv_Max);
		Funct1dToPairs(hv_FHistoSm, &hv_XValuesM, &hv_YValuesM);
		TupleLength(hv_Max, &hv_nmax);

		//for j := 0 to nmax-1 by 1
		  //*     vmax:=YValuesM[int(Max[j])]
		//endfor

		//tuple_max (Histo, hmax)
		TupleMax(hv_YValuesM, &(*hv_hmax));
		hv_hthr = (*hv_hmax) / 5;
		TupleGenConst(hv_nmax, hv_hthr, &hv_Hthr);

		TupleGreaterElem(HTuple(hv_YValuesM[hv_Max.TupleInt()]), hv_Hthr, &hv_GreaterMaxThr);
		TupleSelectMask(HTuple(hv_XValuesM[hv_Max.TupleInt()]), hv_GreaterMaxThr, &hv_Selected);
		(*hv_hmax) = HTuple(hv_Selected[0]).TupleInt();
		return;
	}

	int CInspect_Call::Proc_MinG(int iRunningMedianSpan)
	{
		m_inspect_status = "Proc_MinG()";

		HTuple tl, h, w, t, hiptr, hiptrIRBS, hiptrCLCS, h_gmaxmin, hv_NpM, hv_Grater, hv_hmax, hv_gmax, hv_hthr, hv_greater, hv_gsum, hv_gprc;
		int hmax, rmspan, gmaxmin;

		DWORD time_start = GetTickCount();

		
		GetImagePointer1(m_local_Im, &hiptr, &t, &w, &h);
		
		pIm = (UINT8 *)(Hlong *)hiptr.L();
		m_iw = (int)w.L();
		m_ih = (int)h.L();

		m_pFWM->CPU_SpMeasGray(pIm, m_iw, m_ih,  NpS,
			pP1Sx, pP1Sy, pP2Sx, pP2Sy, 
			m_pFWM->pShiftS);

		HTuple PGrealminS_tuple((float*)m_pFWM->pShiftS, NpS);

		TupleGenConst(NpS, 150, &hv_NpM);
		TupleGreaterElem(PGrealminS_tuple, hv_NpM, &hv_greater);
		TupleSum(hv_greater, &hv_gsum);

		
		TupleMax(PGrealminS_tuple, &hv_gmax);

		hv_gprc = float(hv_gsum) / NpS;

		if (hv_gprc > 0.3 || hv_gmax>180)
		{
			//hv_hthr = NpS * 0.01;
			HistoMax0(PGrealminS_tuple,  &hv_hmax);
			gmaxmin = (int)hv_hmax.D();
		}
		else
		{
			TupleGenConst(NpS, 150, &hv_NpM);

			TupleGreaterEqualElem(PGrealminS_tuple, hv_NpM, &hv_Grater);
			TupleSum(hv_Grater, &hv_gsum);
			hmax = hv_gsum.I();
			if (hmax > iRunningMedianSpan)
				rmspan = 3 * hmax / 4;// NpS / 2;// hmax * 4;
			else
				rmspan = iRunningMedianSpan;


			m_pFWM->SrunmedSLI(m_pFWM->pShiftS, m_pFWM->pmedoS, NpS, rmspan, 1, 10);
			HTuple PGminS_tuple((float*)m_pFWM->pmedoS, NpS);
			
			TupleMax(PGminS_tuple, &h_gmaxmin);
			gmaxmin = (int)h_gmaxmin.D();

			if (WRTEST)
				WriteTuple(PGminS_tuple, "C:\\Temp\\GminS_tuple.tup");
		}
		
		//iRunningMedianSpan = NpS / 2;
		
		
		if (WRTEST)
		{
			
			WriteTuple(PGrealminS_tuple, "C:\\Temp\\GrealminS_tuple.tup");
			
		}
		// ------
		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- Proc_MinG() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time %0*.4f s",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);
		if (gmaxmin > 250)
		{
			gmaxmin = 150;
		}
		return gmaxmin;
	}

	int	CInspect_Call::Proc_HistoPeakG(HObject HCadPatternRgn, HObject local_lm)
	{
		m_inspect_status = "Proc_HistoPeakG()";

		DWORD time_start = GetTickCount();

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

		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- Proc_HistoPeakG() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time %0*.4f s",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

		return iHistoPeakGV;
	}


	void SetNarrowWide(HTuple hv_W_tuple, HTuple hv_WG_tuple, HTuple hv_widewpprc, HTuple hv_narrowwpprc,
		HTuple *hv_narrowwp, HTuple *hv_widewp)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_WGprc, hv_Wmedian, hv_WGmedian, hv_cmed;
		HTuple  hv_WGmax, hv_WGmin;

		//* SetNarrowWide - widewp, narrowwp calculation
		//* 27.06.2025

		hv_WGprc = (hv_W_tuple * 100) / hv_WG_tuple;
		TupleMedian(hv_W_tuple, &hv_Wmedian);
		TupleMedian(hv_WG_tuple, &hv_WGmedian);
		hv_cmed = hv_Wmedian / hv_WGmedian;
		TupleMax(hv_WG_tuple, &hv_WGmax);
		TupleMin(hv_WG_tuple, &hv_WGmin);
		(*hv_widewp) = (hv_WGmax*hv_cmed)*((hv_widewpprc / 100.) + 1.);
		(*hv_narrowwp) = (hv_WGmin*hv_cmed)*(1. - (hv_narrowwpprc / 100.));
		return;
	}

	int CInspect_Call::Proc_FWM()
	{
		m_inspect_status = "Proc_FWM()";

		HTuple tl, h, w, t, hiptr, hiptrIRB, hiptrCLC;
		HTuple hv_DistNmin, hv_DistNmax;
		DWORD time_start = GetTickCount();

		GetImagePointer1(m_ho_ImCLC, &hiptrCLC, &t, &w, &h);
		GetImagePointer1(m_pFWM->ho_ImIRBP, &hiptrIRB, &t, &w, &h);
		GetImagePointer1(m_local_Im, &hiptr, &t, &w, &h);
		pImCLC = (UINT16 *)(Hlong *)hiptrCLC.L();
		m_pFWM->pImIRBP = (UINT16 *)(Hlong *)hiptrIRB.L();
		pIm = (UINT8 *)(Hlong *)hiptr.L();
		m_iw = (int)w.L();
		m_ih = (int)h.L();

		//	FWM_BR_CPU_LaunchN(mmpIm, mmpImIRB, mmpImIRBS, mmpImCLC, iw, ih, 0, mmNp, mmthr, 0, mmpP1x, mmpP1y, mmpP2x, mmpP2y, mmpPnum, mmpWidth, mmpWmin, mmpPInt1x, mmpPInt1y, mmpPInt2x, mmpPInt2y, mmpShift, mmpPuseint);
		m_pFWM->CPU_MSPtProc40B2(pIm, m_pFWM->pImIRBP, pImCLC, m_iw, m_ih, 0, Np,
			(float)threshold, m_Inspection_Params.dclcmax,
			pP1x, pP1y, pP2x, pP2y, pPnum, pPnum2,
			m_pFWM->pWidth, m_pFWM->pWmin, m_pFWM->pPInt1x, m_pFWM->pPInt1y, m_pFWM->pPInt2x, m_pFWM->pPInt2y, m_pFWM->pShift, m_pFWM->pPuseint);


		static int stat_cnt = 9;
		stat_cnt += 1;
		if (stat_cnt % 10 == 0 || WRTEST)
		{
			HTuple PWidthPAll_tuple((float*)m_pFWM->pWidth, Np);
			TupleMin(PWidthPAll_tuple, &hv_Wmin);
			TupleMax(PWidthPAll_tuple, &hv_Wmax);
			TupleMean(PWidthPAll_tuple, &hv_Wmean);
			TupleMedian(PWidthPAll_tuple, &hv_Wmed);

			m_Inspection_Params.fwm_Wmin = hv_Wmin;
			m_Inspection_Params.fwm_Wmax = hv_Wmax;
			m_Inspection_Params.fwm_Wmean = hv_Wmean;
			m_Inspection_Params.fwm_Wmed = hv_Wmed;
		}
		
		int caddrb, caddre, cnum, j;
		float *pW;
		CString Csnum;
		if (WRTEST)
			for (j = 0; j < NaC; j++)
			{
				caddrb = *(pPaddrC + j + 0);
				if (j + 1 < NaC)
					caddre = *(pPaddrC + j + 1) - 1;
				else
					caddre = Np - 1;
				cnum = (caddre - caddrb);
				//pW= (float*)m_pFWM->pWidth+
				HTuple PWidthP_tuple((float*)m_pFWM->pWidth + caddrb,cnum);
				Csnum.Format("C:\\Temp1\\PWidthP_tuple%d.tup", j);
				WriteTuple(PWidthP_tuple,  HTuple(Csnum));
			}
		
		
		//m_pFWM->FWM_PostProcCPU(m_pFWM->pWidth, pWG, m_pFWM->pmedo, pP3x, pP3y, NaC, Np,
		//	pPaddrC,
		//	m_Inspection_Params.sz1, 
		//	m_Inspection_Params.rmskip, 
		//	m_Inspection_Params.prcMB,
		//	m_Inspection_Params.prcSP, 
		//	m_Inspection_Params.absMB, 
		//	m_Inspection_Params.minwp,
		//	m_Inspection_Params.absSP, 
		//	m_Inspection_Params.dlen,
		//	m_pFWM->pDefMB, 
		//	m_pFWM->pDefMBprc,
		//	m_pFWM->pXdefMB, 
		//	m_pFWM->pYdefMB, 
		//	m_pFWM->pDefSP, 
		//	m_pFWM->pDefSPprc,
		//	m_pFWM->pXdefSP, 
		//	m_pFWM->pYdefSP, 
		//	&m_pFWM->MBnum, &m_pFWM->SPnum);
		float stdwidth = m_ho_FWMpar.D();


		//free(m_pFWM->pmedo);
		//m_pFWM->pmedo = (float *)malloc(MEDOSIZE);

		HTuple WG_tuple(/*(float*)*/pWG, Np * 1);
		HTuple W_tuple((float*)m_pFWM->pWidth, Np * 1);

		HTuple hv_widewpprc, hv_narrowwpprc, hv_narrowwp, hv_widewp;
		hv_widewpprc = 30;
		hv_narrowwpprc = 40;
		SetNarrowWide(W_tuple, WG_tuple, hv_widewpprc, hv_narrowwpprc,
			&hv_narrowwp, &hv_widewp);
		narrowwp = (int)hv_narrowwp.D();
		widewp = (int)hv_widewp.D();

		if (WRTEST)
		{
			WriteTuple(WG_tuple, "C:\\Temp\\WG_tuple.tup");
			WriteTuple(W_tuple, "C:\\Temp\\W_tuple.tup");
		}

		m_pFWM->FWM_PostProcCPUNew(m_pFWM->pWidth, pWG, m_pFWM->pmedo, m_pFWM->pPInt1x, m_pFWM->pPInt1y,/*pP3x, pP3y,*/ NaC, Np,
			pPaddrC,
			m_Inspection_Params.sz1,
			m_Inspection_Params.rmskip,
			m_Inspection_Params.prcMB,
			m_Inspection_Params.prcSP,
			m_Inspection_Params.absMB,
			m_Inspection_Params.minwp,
			m_Inspection_Params.narrowwp,
			m_Inspection_Params.widewp,
			stdwidth,
			m_Inspection_Params.absSP,
			m_Inspection_Params.dlen,
			m_pFWM->pDefMB,
			m_pFWM->pDefMBprc,
			m_pFWM->pXdefMB,
			m_pFWM->pYdefMB,
			m_pFWM->pDefSP,
			m_pFWM->pDefSPprc,
			m_pFWM->pXdefSP,
			m_pFWM->pYdefSP,
			
			&m_pFWM->MBnum, &m_pFWM->SPnum);

		if (WRTEST)
		{
			int nmid = pPaddrC[1];
			HTuple Nmid = nmid;
			WriteTuple(Nmid, "C:\\Temp\\Nmid.tup");
			WriteObject(m_ho_SkeletonsFWM, "C:\\Temp\\m_ho_SkeletonsFWM.hobj");
			WriteObject(m_pFWM->ho_ImIRBP, "C:\\Temp\\ho_ImIRBP");
			HTuple PWidthP_tuple((float*)m_pFWM->pWidth, Np * 1);
			WriteTuple(PWidthP_tuple, "C:\\Temp\\PWidthP_tuple.tup");

			HTuple PmedoP_tuple((float*)m_pFWM->pmedo, Np * 1);
			WriteTuple(PmedoP_tuple, "C:\\Temp\\PmedoP_tuple0.tup");

			HTuple PmedoP_tuple1((float*)m_pFWM->pmedo + MEDOSIZE / 2, Np * 1);
			WriteTuple(PmedoP_tuple1, "C:\\Temp\\PmedoP_tuple1.tup");

			HTuple PIntP1y_tuple((float*)m_pFWM->pPInt1y, Np * 1);
			HTuple PIntP1x_tuple((float*)m_pFWM->pPInt1x, Np * 1);
			HTuple PIntP2y_tuple((float*)m_pFWM->pPInt2y, Np * 1);
			HTuple PIntP2x_tuple((float*)m_pFWM->pPInt2x, Np * 1);

			HObject ho_RInt1, ho_RInt2;
			GenRegionPoints(&ho_RInt1, PIntP1y_tuple, PIntP1x_tuple);
			GenRegionPoints(&ho_RInt2, PIntP2y_tuple, PIntP2x_tuple);

			WriteObject(ho_RInt1, "C:\\Temp\\ho_RInt1");
			WriteObject(ho_RInt2, "C:\\Temp\\ho_RInt2");
			//WriteTuple(PWidthP1y_tuple, "C:\\Temp\\PWidthP_tuple.tup");
			
			//AfxMessageBox(_T("Done...PWidthS_tuple.tup saved!"));

		}

		/*m_pFWM->FWM_PostProcCPUI(m_pFWM->pWidth, pWG, m_pFWM->pmedo, ((m_pFWM->pPInt1x) + (m_pFWM->pPInt2x))/2., pP3y, Np,
			m_Inspection_Params.sz1,
			m_Inspection_Params.rmskip,
			m_Inspection_Params.prcMB,
			m_Inspection_Params.prcSP,
			m_Inspection_Params.absMB,
			m_Inspection_Params.absSP,
			m_Inspection_Params.dlen,
			m_pFWM->pDefMB,
			m_pFWM->pXdefMB,
			m_pFWM->pYdefMB,
			m_pFWM->pDefSP,
			m_pFWM->pXdefSP,
			m_pFWM->pYdefSP,
			&m_pFWM->MBnum, &m_pFWM->SPnum);*/

		if (WRTEST)
		{
			//WriteObject(m_ho_ImCLC, "C:\\Temp\\m_ho_ImCLC");
			//WriteObject(m_pFWM->ho_ImIRBP, "C:\\Temp\\ho_ImIRBP");
			HTuple pXdefSP_tuple((float*)m_pFWM->pXdefSP, m_pFWM->SPnum * 1);
			WriteTuple(pXdefSP_tuple, "C:\\Temp\\pXdefSP.tup");
			HTuple pYdefSP_tuple((float*)m_pFWM->pYdefSP, m_pFWM->SPnum * 1);
			WriteTuple(pYdefSP_tuple, "C:\\Temp\\pYdefSP.tup");

			HTuple pXdefMB_tuple((float*)m_pFWM->pXdefMB, m_pFWM->MBnum * 1);
			WriteTuple(pXdefMB_tuple, "C:\\Temp\\pXdefMB.tup");
			HTuple pYdefMB_tuple((float*)m_pFWM->pYdefMB, m_pFWM->MBnum * 1);
			WriteTuple(pYdefMB_tuple, "C:\\Temp\\pYdefMB.tup");
			//AfxMessageBox(_T("Done...PWidthS_tuple.tup saved!"));

		}


		// -------------------------------------------------------------------

		//FloatToTuple(m_pFWM->pDefMBprc, m_pFWM->MBnum, m_hv_pDefMBprc);
		//FloatToTuple(m_pFWM->pDefSPprc, m_pFWM->SPnum, m_hv_pDefSPprc);
		//FloatToTuple(m_pFWM->pDefMB, m_pFWM->MBnum, m_hv_pDefMB);
		//FloatToTuple(m_pFWM->pDefSP, m_pFWM->SPnum, m_hv_pDefSP);

		/// --------------------------------------------------------------------
		/// TODO: defect attributes --------------------------------------------
		/// --------------------------------------------------------------------
		/// Attributes: m_hv_pDefMBprc, m_hv_pDefSPprc, m_hv_pDefMB, m_hv_pDefSP

		m_hv_pDefMBprc = HTuple(m_pFWM->pDefMBprc, m_pFWM->MBnum);
		m_hv_pDefSPprc = HTuple(m_pFWM->pDefSPprc, m_pFWM->SPnum);
		m_hv_pDefMB = HTuple(m_pFWM->pDefMB, m_pFWM->MBnum);
		m_hv_pDefSP = HTuple(m_pFWM->pDefSP, m_pFWM->SPnum);

		// -------------------------------------------------------------------

		{
			HTuple PMBx_tuple;
			HTuple PMBy_tuple;
			FloatToTuple(m_pFWM->pXdefMB, m_pFWM->MBnum, PMBx_tuple);
			FloatToTuple(m_pFWM->pYdefMB, m_pFWM->MBnum, PMBy_tuple);

			GenEmptyObj(&m_ho_RegionsMB);
			//HTuple  hv_nMB = HTuple(m_pFWM->MBnum);
			TupleSelectMask(PMBy_tuple, PMBy_tuple, &PMBy_tuple);
			TupleSelectMask(PMBx_tuple, PMBx_tuple, &PMBx_tuple);
			HTuple  hv_nMB;
			TupleLength(PMBx_tuple, &hv_nMB);
			if (hv_nMB > 0)
			{
				HTuple hv_TR;
				TupleGenConst(hv_nMB, 30, &hv_TR);
				GenCircle(&m_ho_RegionsMB, PMBy_tuple, PMBx_tuple, hv_TR);
			}

		}

		// -------------------------------------------------------------------
		{
			HTuple PSPx_tuple;
			HTuple PSPy_tuple;
			FloatToTuple(m_pFWM->pXdefSP, m_pFWM->SPnum, PSPx_tuple);
			FloatToTuple(m_pFWM->pYdefSP, m_pFWM->SPnum, PSPy_tuple);

			GenEmptyObj(&m_ho_RegionsSP);
			//HTuple  hv_nSP = HTuple(m_pFWM->SPnum);
			TupleSelectMask(PSPy_tuple, PSPy_tuple, &PSPy_tuple);
			TupleSelectMask(PSPx_tuple, PSPx_tuple, &PSPx_tuple);
			HTuple  hv_nSP;
			TupleLength(PSPx_tuple, &hv_nSP);

			

			if (hv_nSP > 0)
			{
				HTuple hv_TR;
				TupleGenConst(hv_nSP, 30, &hv_TR);
				//DistancePr(m_ho_RegNoProc, PSPy_tuple, PSPx_tuple, &hv_DistNmin, &hv_DistNmax);
				GenCircle(&m_ho_RegionsSP, PSPy_tuple, PSPx_tuple, hv_TR);

				VerifySPregnoproc(m_ho_RegNoProc, m_pFWM->ho_RegionInoD, m_ho_RegionsSP,
					&m_ho_RegionsSP, 20);
			}
		}



		// -------------------------------------------------------------------

		//INT64 *Px = ConvertFloatToINT64(m_pFWM->pXdefMB, m_pFWM->MBnum);
		//INT64 *Py = ConvertFloatToINT64(m_pFWM->pYdefMB, m_pFWM->MBnum);
		//HTuple PMBx_tuple;
		//HTuple PMBy_tuple;
		////for (int i = 0; i < m_pFWM->MBnumS; i++)
		////{
		////	PMBSx_tuple[i] = (HTuple)*(PSx + i);
		////	PMBSy_tuple[i] = (HTuple)*(PSy + i);
		////}
		//PMBx_tuple = HTuple(Px, m_pFWM->MBnum);
		//PMBy_tuple = HTuple(Py, m_pFWM->MBnum);


		//FloatToTuple(m_pFWM->pDefMBprc, m_pFWM->MBnum, m_hv_pDefMBprc);
		//FloatToTuple(m_pFWM->pDefSPprc, m_pFWM->SPnum, m_hv_pDefSPprc);

		//HTuple hv_nRPv, hv_RadRPv;
		//HObject h_RP, h_RPvalid;
		//GenRegionPoints(&h_RP, PMBy_tuple, PMBx_tuple);
		//Intersection(h_RP, m_ho_SkeletonsFWMvalid, &h_RPvalid);
		//Union1(h_RPvalid, &h_RPvalid);
		//Connection(h_RPvalid, &h_RPvalid);
		//CountObj(h_RPvalid, &hv_nRPv);
		//if (hv_nRPv.L() > 0 && m_pFWM->MBnum>0)
		//{
		//	//TupleGenConst(hv_nRPv, 15, &hv_RadRPv);
		//	//DilationCircle(h_RPvalid, &m_ho_RegionsMB, hv_RadRPv);
		//	DilationCircle(h_RPvalid, &m_ho_RegionsMB, 15);
		//}
		//else
		//	GenEmptyObj(&m_ho_RegionsMB);
		
		//GenEmptyObj(&m_ho_RegionsSP);

		if (WRTEST)
		{
			//WriteObject(m_ho_ImCLC, "C:\\Temp\\m_ho_ImCLC");
			WriteObject(m_ho_RegionsMB, "C:\\Temp\\m_ho_RegionsSP");
			WriteObject(m_ho_RegNoProc, "C:\\Temp\\m_ho_RegNoProc");
			WriteObject(m_pFWM->ho_RegionInoD, "C:\\Temp\\ho_RegionInoD");
			HTuple pXdefSP_tuple((float*)m_pFWM->pXdefSP, m_pFWM->SPnum * 1);
			WriteTuple(pXdefSP_tuple, "C:\\Temp\\pXdefSP.tup");
			HTuple pYdefSP_tuple((float*)m_pFWM->pYdefSP, m_pFWM->SPnum * 1);
			WriteTuple(pYdefSP_tuple, "C:\\Temp\\pYdefSP.tup");
			//WriteObject(h_RP, "C:\\Temp\\h_RP");
		}
		//m_pFWM->skipbegP - m_pFWM->skipendP


		
		/*HTuple  hv_nMB = HTuple(m_pFWM->MBnum);
		if (hv_nMB > 0)
		{
			HTuple hv_TR;
			TupleGenConst(hv_nMB, 30, &hv_TR);
			GenCircle(&m_ho_RegionsMB, PMBy_tuple, PMBx_tuple, hv_TR);
		}
*/
		// ---------
		/*INT64 *PSPx = ConvertFloatToINT64(m_pFWM->pXdefSP, m_pFWM->SPnum);
		INT64 *PSPy = ConvertFloatToINT64(m_pFWM->pYdefSP, m_pFWM->SPnum);

		HTuple PSPx_tuple;
		HTuple PSPy_tuple;
		
		PSPx_tuple = HTuple(PSPx, m_pFWM->SPnum);
		PSPy_tuple = HTuple(PSPy, m_pFWM->SPnum);*/

		/*GenRegionPoints(&h_RP, PSPy_tuple, PSPx_tuple);
		Intersection(h_RP, m_ho_SkeletonsFWMvalid, &h_RPvalid);
		Union1(h_RPvalid, &h_RPvalid);
		HTuple hv_a, hv_r, hv_c;
		AreaCenter(h_RPvalid, &hv_a, &hv_r, &hv_c);*/
		//if (hv_a > 0)
		//{
		//	Connection(h_RPvalid, &h_RPvalid);
		//	CountObj(h_RPvalid, &hv_nRPv);
		//	if (hv_nRPv > 0)
		//	{
		//		/*			TupleGenConst(hv_nRPv, 15, &hv_RadRPv);
		//					DilationCircle(h_RPvalid, &m_ho_RegionsSP, hv_RadRPv);*/
		//		DilationCircle(h_RPvalid, &m_ho_RegionsSP, 15);
		//	}
		//}
		//
		//else
		//	GenEmptyObj(&m_ho_RegionsSP);

		/*GenEmptyObj(&m_ho_RegionsSP);
		HTuple  hv_nSP = HTuple(m_pFWM->SPnum);
		if (hv_nSP > 0)
		{
			HTuple hv_TR;
			TupleGenConst(hv_nSP, 30, &hv_TR);
			GenCircle(&m_ho_RegionsSP, PSPy_tuple, PSPx_tuple, hv_TR);
		}*/

		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- Proc_FWM() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time %0*.4f s",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

		return 0;
	}

	int CInspect_Call::Proc_CBM_Meander()
	{
		m_inspect_status = "Proc_CBM_Meander()";

		DWORD time_start = GetTickCount();

		HObject ho_MeanderDefect, ho_MeanderDefects, ho_Rect;
		HTuple hv_DOut, hv_nObj;
		HTuple ht1, ht3, ht2, ht;

		CountObj(m_ho_ContoursGrsm, &hv_nObj);
		int nObj = hv_nObj.I();

		HTuple m_hv_meander;
		HObject m_ho_CG, m_ho_CI;
		HTuple m_hv_thr, m_hw_absmb, m_hw_abssp, m_hv_ctype, m_fsz;

		HTuple HCnt;

		m_hv_thr = HTuple(m_Inspection_Params.threashold);
		m_hw_absmb = HTuple(m_Inspection_Params.fCBM_AbsMB_Meander);
		m_hw_abssp = HTuple(m_Inspection_Params.fCBM_AbsSP_Meander);
		m_hv_ctype = HTuple(true);
		m_fsz = HTuple(m_Inspection_Params.iCBM_Rmsize_Meander);

		GenEmptyObj(&ho_MeanderDefects);

		for (int i = 1; i <= nObj; i++)
		{
			m_hv_meander = (HTuple)i;

			GenEmptyObj(&ho_MeanderDefect);

			m_pCBM->ProcessMeander(m_local_Im, m_ho_ContoursGrsm, m_ho_MeanderRects, /*m_ho_Gi*/m_ho_Gi_CbmTest,
				&ho_MeanderDefect, &ho_Rect, &m_ho_CG, &m_ho_CI,
				m_hv_thr, m_hv_meander, m_hw_absmb, m_hw_abssp, m_hv_ctype, m_fsz,
				&hv_DOut);

			HCnt = 0;
			CountObj(ho_MeanderDefect, &HCnt);
			if (HCnt>0)
				ConcatObj(ho_MeanderDefects, ho_MeanderDefect, &ho_MeanderDefects);
		}

		HTuple DefMnum;
		CountObj(ho_MeanderDefects, &DefMnum);

		if (DefMnum > 0)
		{
			m_ho_RegionsMeanderMB = ho_MeanderDefects;
			GenEmptyObj(&m_ho_RegionsMeanderSP);
		}
		else
		{
			GenEmptyObj(&m_ho_RegionsMeanderMB);
			GenEmptyObj(&m_ho_RegionsMeanderSP);
		}

		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- Proc_CBM_Meander() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time %0*.4f s",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

		return 0;
	}

	int CInspect_Call::Proc_CBM_Pad()
	{
		m_inspect_status = "Proc_CBM_Pad()";

		DWORD time_start = GetTickCount();

		HObject ho_PadIDefect, ho_PadIDefects, ho_Rect;
		HTuple hv_DOut, hv_nObj;
		HTuple ht1, ht3, ht2, ht;

		CountObj(m_ho_ContoursGrsmPi, &hv_nObj);
		int nObj = hv_nObj.I();

		HTuple m_hv_meander;
		HObject m_ho_CG, m_ho_CI;
		HTuple m_hv_thr, m_hw_absmb, m_hw_abssp, m_hv_ctype, m_fsz;

		HTuple HCnt;

		m_hv_thr = HTuple(m_Inspection_Params.threashold);
		m_hw_absmb = HTuple(m_Inspection_Params.fCBM_AbsMB_Pad);
		m_hw_abssp = HTuple(m_Inspection_Params.fCBM_AbsSP_Pad);
		m_hv_ctype = HTuple(true);
		m_fsz = HTuple(m_Inspection_Params.iCBM_Rmsize_Pad);

		GenEmptyObj(&ho_PadIDefects);

		for (int i = 1; i <= nObj; i++)
		{
			m_hv_meander = (HTuple)i;

			GenEmptyObj(&ho_PadIDefect);

			m_pCBM->ProcessMeander(m_local_Im, m_ho_ContoursGrsmPi, m_ho_RectanglesPadsIsol, /*m_ho_Gi*/m_ho_Gi_CbmTest,
				&ho_PadIDefect, &ho_Rect, &m_ho_CG, &m_ho_CI,
				m_hv_thr, m_hv_meander, m_hw_absmb, m_hw_abssp, m_hv_ctype, m_fsz,
				&hv_DOut);

			HCnt = 0;
			CountObj(ho_PadIDefect, &HCnt);
			if (HCnt > 0)
				ConcatObj(ho_PadIDefects, ho_PadIDefect, &ho_PadIDefects);
		}

		HTuple DefMnum;
		CountObj(ho_PadIDefects, &DefMnum);

		if (DefMnum > 0)
		{
			m_ho_RegionsPadMB = ho_PadIDefects;
			GenEmptyObj(&m_ho_RegionsPadSP);
		}
		else
		{
			GenEmptyObj(&m_ho_RegionsPadMB);
			GenEmptyObj(&m_ho_RegionsPadSP);
		}

		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- Proc_CBM_Pad() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time %0*.4f s",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

		return 0;
	}

	int CInspect_Call::Proc_CBM_ConnPad()
	{
		m_inspect_status = "Proc_CBM_ConnPad()";

		DWORD time_start = GetTickCount();

		HObject ho_PadCDefect, ho_PadCDefects, ho_Rect;
		HTuple hv_DOut, hv_nObj;
		HTuple ht1, ht3, ht2, ht;

		CountObj(m_ho_ContoursGrsmPc, &hv_nObj);
		int nObj = hv_nObj.I();

		HTuple m_hv_meander;
		HObject m_ho_CG, m_ho_CI;
		HTuple m_hv_thr, m_hw_absmb, m_hw_abssp, m_hv_ctype, m_fsz;

		HTuple HCnt;

		m_hv_thr = HTuple(m_Inspection_Params.threashold);
		m_hw_absmb = HTuple(m_Inspection_Params.fCBM_AbsMB_ConnPad);
		m_hw_abssp = HTuple(m_Inspection_Params.fCBM_AbsSP_ConnPad);
		m_hv_ctype = HTuple(true);
		m_fsz = HTuple(m_Inspection_Params.iCBM_Rmsize_ConnPad);

		GenEmptyObj(&ho_PadCDefects);

		for (int i = 1; i <= nObj; i++)
		{
			m_hv_meander = (HTuple)i;

			GenEmptyObj(&ho_PadCDefect);

			m_pCBM->ProcessMeander(m_local_Im, m_ho_ContoursGrsmPc, m_ho_RectanglesPadsCon, /*m_ho_Gi*/m_ho_Gi_CbmTest,
				&ho_PadCDefect, &ho_Rect, &m_ho_CG, &m_ho_CI,
				m_hv_thr, m_hv_meander, m_hw_absmb, m_hw_abssp, m_hv_ctype, m_fsz,
				&hv_DOut);

			HCnt = 0;
			CountObj(ho_PadCDefect, &HCnt);
			if (HCnt > 0)
				ConcatObj(ho_PadCDefects, ho_PadCDefect, &ho_PadCDefects);
		}

		HTuple DefMnum;
		CountObj(ho_PadCDefects, &DefMnum);

		if (DefMnum > 0)
		{
			m_ho_RegionsConnPadMB = ho_PadCDefects;
			GenEmptyObj(&m_ho_RegionsConnPadSP);
		}
		else
		{
			GenEmptyObj(&m_ho_RegionsConnPadMB);
			GenEmptyObj(&m_ho_RegionsConnPadSP);
		}

		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- Proc_CBM_ConnPad() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time %0*.4f s",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

		return 0;
	}

	int CInspect_Call::Proc_CBM_WireAngle()
	{
		m_inspect_status = "Proc_CBM_WireAngle()";

		DWORD time_start = GetTickCount();

		HObject ho_WADefect, ho_WADefects, ho_Rect;
		HTuple hv_DOut, hv_nObj;
		HTuple ht1, ht3, ht2, ht;

		CountObj(m_ho_ContoursWireAngles, &hv_nObj);
		int nObj = hv_nObj.I();

		HTuple m_hv_meander;
		HObject m_ho_CG, m_ho_CI;
		HTuple m_hv_thr, m_hw_absmb, m_hw_abssp, m_hv_ctype, m_fsz;

		HTuple HCnt;

		m_hv_thr = HTuple(m_Inspection_Params.threashold);
		m_hw_absmb = HTuple(m_Inspection_Params.fCBM_AbsMB_WireAngle);
		m_hw_abssp = HTuple(m_Inspection_Params.fCBM_AbsSP_WireAngle);
		m_hv_ctype = HTuple(true);
		m_fsz = HTuple(m_Inspection_Params.iCBM_Rmsize_WireAngle);

		GenEmptyObj(&ho_WADefects);

		for (int i = 1; i <= nObj; i++)
		{
			m_hv_meander = (HTuple)i;

			GenEmptyObj(&ho_WADefect);

			m_pCBM->ProcessMeander(m_local_Im, m_ho_ContoursWireAngles, m_ho_RectanglesWireAngles, /*m_ho_Gi*/m_ho_Gi_CbmTest,
				&ho_WADefect, &ho_Rect, &m_ho_CG, &m_ho_CI,
				m_hv_thr, m_hv_meander, m_hw_absmb, m_hw_abssp, m_hv_ctype, m_fsz,
				&hv_DOut);

			HCnt = 0;
			CountObj(ho_WADefect, &HCnt);
			if (HCnt > 0)
				ConcatObj(ho_WADefects, ho_WADefect, &ho_WADefects);
		}

		HTuple DefMnum;
		CountObj(ho_WADefects, &DefMnum);

		if (DefMnum > 0)
		{
			m_ho_RegionsWireAngleMB = ho_WADefects;
			GenEmptyObj(&m_ho_RegionsWireAngleSP);
		}
		else
		{
			GenEmptyObj(&m_ho_RegionsWireAngleMB);
			GenEmptyObj(&m_ho_RegionsWireAngleSP);
		}

		DWORD time_end = GetTickCount();
		Queue_Add(m_queue_log, NULL, "--- Proc_CBM_WireAngle() - OK (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time %0*.4f s",
			inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

		return 0;
	}
	
	void CInspect_Call::Inspect(InspectParam_t* inspect_paramI)
	{
		DWORD time_start = GetTickCount();
		try
		{
			inspect_param = *inspect_paramI;
			m_total_defects = 1;

			if (!Alignment())
			{
				return;
			}


			if (m_Inspection_Params.inspect_Auto_Thr)
			{
				HTuple hv_thr;
				Auto_Threshold2(m_ho_SkeletonsFWMS, m_local_Im, &hv_thr);

				if (hv_thr == 0)
				{
					DWORD time_end = GetTickCount();
					Queue_Add(m_queue_log, NULL, "*** CInspect_Call error. Low image contrast(bad focus): (FrameNo=%d, UnitNo=%d, Thread=%d) threshold=%d: Lead Time: %0*.4f s",
						inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, threshold, 3, (double)(time_end - time_start) / 1000.);
					return;
				}

				threshold = hv_thr.ToDArr()[0];
			}
			else
			{
				threshold = m_Inspection_Params.threashold;
			}

			GenEmptyObj(&m_ho_RegionPH);
			GenEmptyObj(&m_ho_RegionsIsl);
			GenEmptyObj(&m_ho_RegionsOp);
			GenEmptyObj(&m_ho_RegionsShi);
			GenEmptyObj(&m_ho_RegionsLG);
			GenEmptyObj(&m_ho_RegionsLG_P);
			GenEmptyObj(&m_ho_RegionsMBS);
			GenEmptyObj(&m_ho_RegionsSPS);
			GenEmptyObj(&m_ho_RegionsMB);
			GenEmptyObj(&m_ho_RegionsSP);

			GenEmptyObj(&m_ho_RegionsMeanderMB);
			GenEmptyObj(&m_ho_RegionsMeanderSP);
			GenEmptyObj(&m_ho_RegionsPadMB);
			GenEmptyObj(&m_ho_RegionsPadSP);
			GenEmptyObj(&m_ho_RegionsConnPadMB);
			GenEmptyObj(&m_ho_RegionsConnPadSP);
			GenEmptyObj(&m_ho_RegionsWireAngleMB);
			GenEmptyObj(&m_ho_RegionsWireAngleSP);

			GenEmptyObj(&m_ho_RegionsDT_S);
			GenEmptyObj(&m_ho_RegionsDT_P);

			Pinhole_Island();

			OpenShort();

			Build_IRB();

			Low_Gray(threshold);

			if (m_Inspection_Params.inspect_FWMS)
			{
				//m_Inspection_Params.sz1 = 25;
				//m_Inspection_Params.sz2 = 25;			// Running median sizes for FWM and FWMS
				Proc_FWMS();
			}
			if (m_Inspection_Params.inspect_FWM)
			{
				//m_Inspection_Params.sz1 = 75;
				//m_Inspection_Params.sz2 = 75;			// Running median sizes for FWM and FWMS
				Proc_FWM();
			}


			DWORD time_end = GetTickCount();
			Queue_Add(m_queue_log, NULL, "*-- CInspect_Call completed: (FrameNo=%d, UnitNo=%d, Thread=%d): Lead Time: %0*.4f s",
				inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num, 3, (double)(time_end - time_start) / 1000.);

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

			Queue_Add(m_queue_log, NULL, "*** Halcon Exception [%s] : <%s>%s (FrameNo=%d, UnitNo=%d, Thread=%d)",
				m_inspect_status, sOperatorName, sErrMsg, inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num);
			return;
		}
		catch (const std::exception& e) {
			Queue_Add(m_queue_log, NULL, "*** System Exception [%s] : %s (FrameNo=%d, UnitNo=%d, Thread=%d)",
				m_inspect_status, e.what(), inspect_param.iFrameNo, inspect_param.iUnitNo, m_thread_num);
		}
	}

	void CInspect_Call::LGEnhance2(HObject ho_Image, HObject *ho_ImageResult, HTuple hv_mid, HTuple hv_range,
		HTuple hv_lutexp, HTuple hv_off)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_hrange, hv_lth, hv_hth, hv_low, hv_high;
		HTuple  hv_low1, hv_high1, hv_Function, hv_Hseq, hv_LUTf;
		HTuple  hv_LUT;

		hv_hrange = hv_range * hv_lutexp;
		//mid := (mid+range)/2.
		hv_lth = 5;
		hv_hth = 240;
		hv_low = hv_mid - hv_range;
		if (0 != (hv_low < hv_lth))
		{
			hv_low = hv_lth;
		}
		hv_high = hv_mid + hv_range;
		hv_low1 = hv_mid - hv_hrange;

		if (0 != (hv_low1 < hv_lth))
		{
			hv_low1 = hv_lth;
			//high1 := low+lutexp*range*1.
			hv_high1 = hv_mid + hv_hrange;
		}
		else
		{

			hv_high1 = hv_mid + hv_hrange;
		}

		if (0 != (hv_high1 > hv_hth))
		{
			hv_high1 = hv_hth;
		}

		if (0 != (hv_high > hv_hth))
		{
			hv_high = hv_hth;
		}

		CreateFunct1dPairs(((HTuple(0).TupleConcat(hv_low)).TupleConcat(hv_high)).TupleConcat(255),
			((HTuple(0).TupleConcat(hv_low1)).TupleConcat(hv_high1)).TupleConcat(255),
			&hv_Function);

		TupleGenSequence(0, 255, 1, &hv_Hseq);
		GetYValueFunct1d(hv_Function, hv_Hseq, "constant", &hv_LUTf);
		hv_LUT = hv_LUTf;
		if (0 != hv_off)
		{
			(*ho_ImageResult) = ho_Image;
		}
		else
		{
			LutTrans(ho_Image, &(*ho_ImageResult), hv_LUT);
		}

		return;
	}




	/* ************************************************************************** */
	/* Section: File Scope or Global Data                                         */
	/* ************************************************************************** */

	// -------------------------------------------------------------------
	// --- Teach data ----------------------------------------------------
	// -------------------------------------------------------------------
	TSQueueSM<CString> m_main_queue_log;

	CSlice_Teach m_Slice_Teach;


	/// for FWM ***
	int Np;		// number of MSs
	int Na;		// number of addresses (for all SkeletonsFWM regions)
	int NaC;		// number contours
	int NaR;		// number of regions
	// cpu memory pointers for FWM /
	float *pP1x = 0, *pP1y = 0, *pP2x = 0, *pP2y = 0, *pP3x = 0, *pP3y = 0, *pWi = 0;
	int *pPnum = 0, *pPnum2 = 0, *pWG = 0;
	int *pPaddrR = 0, *pPaddrC = 0;
	int *pPuseint = 0;

	/// for FWMS ***
	int NpS;		// number of MSs
	int NaS;		// number of addresses (for all SkeletonsFWM regions)
	int NaCS;		// number contours
	int NaRS;		// number of regions
	float absNarS; // narrow space
	// cpu memory pointers for FWM /
	float *pP1Sx = 0, *pP1Sy = 0, *pP2Sx = 0, *pP2Sy = 0, *pP3Sx = 0, *pP3Sy = 0, *pWiS = 0;
	int *pPnumS = 0, *pPnum2S = 0, *pWGS = 0;
	int *pPaddrRS = 0, *pPaddrCS = 0;

	int *pPuseintS;

	HObject m_ho_Gi;
	CString m_Path_Model;

	HObject m_ho_RegionGS, m_ho_RectangleCut, m_ho_RegionG, m_ho_RegionsEMPTY, m_ho_RegionsGND, m_ho_RegionsNFWM, m_ho_RegionFWM, m_ho_RegionsPNi, m_ho_RegionsPNPi,
		m_ho_RegionsPNSi, m_ho_RegNoProc, m_ho_SkeletonFWMi, m_ho_SkeletonG, m_ho_SkeletonGS, m_ho_SkeletonPartsP, m_ho_SkeletonsFWM, m_ho_SkeletonsFWMvalid, m_ho_SkeletonsFWMS, m_ho_SkeletonsFWMSvalid, m_ho_SkeletonsG;
	HObject m_ho_SkeletonsFWMScut, m_ho_SkelSEP, m_ho_RegionCut;
	HObject m_ho_SkeletonsFWMcut;

	HObject m_ho_DirImageP, m_ho_ImageIR, m_ho_ImageIR16, m_ho_ImageIRS, m_ho_ImDisplacement, m_ho_WidthImageP, m_ho_WidthImageS;
	HObject m_ho_DirImageS;
	HObject m_ho_ImCLC;

	UINT16 /**pImIRBP, *pImIRBS, */*pImCLC /**pImCLCS*/;// image pointers

	HObject m_ho_GiDomain;
	HObject m_ho_RegProc;

	HTuple m_ho_FWMpar;

	/// for CBM ***
	HObject m_ho_Gi_CbmTest;
	HObject m_ho_ContoursGrsm, m_ho_MeanderRects;
	HObject m_ho_ContoursGrsmPi, m_ho_RectanglesPadsIsol;
	HObject m_ho_ContoursGrsmPc, m_ho_RectanglesPadsCon;	
	HObject m_ho_ContoursWireAngles, m_ho_RectanglesWireAngles;

	/// for Dynthres Test ***
	HObject m_ho_CadSpaceRegion_DynThresTest;
	HObject m_ho_CadPatternRegion_DynThresTest;

	// --- Params ---
	struct Inspection_Params m_Inspection_Params;

	int m_num_teach = 0;

	/* ************************************************************************** */
	/* Section: Function				                                          */
	/* ************************************************************************** */



	/** ---------------------------------------------------------------------------
	* @Summary
	*	Конвертирование Туплы в BIN
	*/
	bool Convert_To_Bin(CString path_teach, bool replace)
	{
		try
		{
			// --- Конвертируем данные 

			// --- for Slice
			inspect_utils::SaveTupleToBinInt(path_teach, "Cols_Sens", "Cols_Sens");
			inspect_utils::SaveTupleToBinInt(path_teach, "Pointer_Sens", "Pointer_Sens");
			inspect_utils::SaveTupleToBinInt(path_teach, "Rows_Sens", "Rows_Sens");
			inspect_utils::SaveTupleToBinInt(path_teach, "Pointer_Skel", "Pointer_Skel");

			inspect_utils::SaveTupleToBinInt(path_teach, "Cols_Sens_P", "Cols_Sens_P");
			inspect_utils::SaveTupleToBinInt(path_teach, "Pointer_Sens_P", "Pointer_Sens_P");
			inspect_utils::SaveTupleToBinInt(path_teach, "Rows_Sens_P", "Rows_Sens_P");
			inspect_utils::SaveTupleToBinInt(path_teach, "Pointer_Skel_P", "Pointer_Skel_P");


			// --- for FWM ---
			inspect_utils::SaveTupleToBinInt(path_teach, "PaddrC", "PaddrC");
			inspect_utils::SaveTupleToBinInt(path_teach, "PaddrR", "PaddrR");

			inspect_utils::SaveTupleToBinInt(path_teach, "Pnum", "Pnum");
			inspect_utils::SaveTupleToBinInt(path_teach, "Pnum2", "Pnum2");
			inspect_utils::SaveTupleToBinInt(path_teach, "PnumR", "PnumR");
			inspect_utils::SaveTupleToBinInt(path_teach, "PcontR", "PcontR");
			inspect_utils::SaveTupleToBinInt(path_teach, "PCinRnum", "PCinRnum");

			inspect_utils::SaveTupleToBinFloat(path_teach, "X1", "X1");
			inspect_utils::SaveTupleToBinFloat(path_teach, "Y1", "Y1");
			inspect_utils::SaveTupleToBinFloat(path_teach, "X2", "X2");
			inspect_utils::SaveTupleToBinFloat(path_teach, "Y2", "Y2");
			inspect_utils::SaveTupleToBinFloat(path_teach, "XC", "XC");
			inspect_utils::SaveTupleToBinFloat(path_teach, "YC", "YC");
			inspect_utils::SaveTupleToBinInt(path_teach, "WG", "WG");
			inspect_utils::SaveTupleToBinInt(path_teach, "PuseintP", "Puseint");

			inspect_utils::SaveTupleToBinInt(path_teach, "PregC", "PregC");

			// --- for FWMS ---
			inspect_utils::SaveTupleToBinInt(path_teach, "PaddrCS", "PaddrCS");
			inspect_utils::SaveTupleToBinInt(path_teach, "PaddrRS", "PaddrRS");

			inspect_utils::SaveTupleToBinInt(path_teach, "PnumS", "PnumS");
			inspect_utils::SaveTupleToBinInt(path_teach, "Pnum2S", "Pnum2S");
			//inspect_utils::SaveTupleToBinInt(path_teach, "PnumSR", "PnumSR");
			inspect_utils::SaveTupleToBinInt(path_teach, "PcontRS", "PcontRS");
			inspect_utils::SaveTupleToBinInt(path_teach, "PCinRnumS", "PCinRnumS");

			inspect_utils::SaveTupleToBinFloat(path_teach, "X1S", "X1S");
			inspect_utils::SaveTupleToBinFloat(path_teach, "Y1S", "Y1S");
			inspect_utils::SaveTupleToBinFloat(path_teach, "X2S", "X2S");
			inspect_utils::SaveTupleToBinFloat(path_teach, "Y2S", "Y2S");
			inspect_utils::SaveTupleToBinFloat(path_teach, "YCS", "YCS");
			inspect_utils::SaveTupleToBinFloat(path_teach, "XCS", "XCS");
			inspect_utils::SaveTupleToBinInt(path_teach, "WGS", "WGS");

			inspect_utils::SaveTupleToBinInt(path_teach, "PregCS", "PregCS");

			printf("*--- Convert_To_Bin - OK");
		}
		catch (HException &except)
		{
			HTuple HExp;
			HTuple HOperatorName, HErrMsg;
			except.ToHTuple(&HExp);
			except.GetExceptionData(HExp, "operator", &HOperatorName);
			except.GetExceptionData(HExp, "error_message", &HErrMsg);

			CString sOperatorName, sErrMsg;
			sOperatorName = HOperatorName.S();
			sErrMsg = HErrMsg.S();

			printf("*** Halcon Exception*** Convert_To_Bin <%s>, <%s>", sOperatorName.GetBuffer(), sErrMsg.GetBuffer());
			return false;
		}
		catch (...)
		{
			printf("*** Exception*** Convert_To_Bin");
			return false;
		}
		return true;

	}
	
	bool Load_Params(CString szModelPath, int iVersion, Inspection_Params* params, TSQueueSM<CString>* queue_log)
	{
		bool ret = true;
		try
		{
			CParams *alg_params;
			alg_params = new CParams(nullptr);
			ret = alg_params->Load_Params(szModelPath, iVersion, &inspect_call::m_Inspection_Params);

			Queue_Add(queue_log, NULL, "--- Loading the parameters file Json - OK");
		}
		catch (...)
		{
			Queue_Add(queue_log, NULL, "***Exception*** Load_Params %s", szModelPath);
			return false;
		}
		return ret;

	}

	bool Save_Params(CString szModelPath, Inspection_Params* params, TSQueueSM<CString>* queue_log)
	{
		bool ret = true;
		try
		{
			CParams *alg_params;
			alg_params = new CParams(nullptr);
			bool ret = alg_params->Save_Params(szModelPath, &m_Inspection_Params);
			Queue_Add(queue_log, NULL, "--- Save the parameters file Json - OK");
		}
		catch (...)
		{
			Queue_Add(queue_log, NULL, "***Exception*** Save_Params %s", szModelPath);
			return false;
		}
		return ret;
	}

	/** ---------------------------------------------------------------------------
	* @Summary
	*	Загрузка обучения для FWM
	*/
	bool Load_Model(CString path_model)
	{
		try
		{
			SetCheck("give_error");		// Use exception handle
			SetSystem("store_empty_region", "true");
			SetSystem("no_object_result", "true");
			SetSystem("empty_region_result", "true");
			SetSystem("current_runlength_number", 100000);
			SetSystem("neighborhood", 8);
			SetSystem("clock_mode", "elapsed_time");
			SetSystem("max_connection", 0);


			//SetSystem("image_cache_capacity", 268435455);
			//HTuple thread_num;
			//GetSystem("thread_num", &thread_num);
			//int _num = thread_num.ToLArr()[0];
			//SetSystem("thread_num", 2);
			//SetSystem("tsp_thread_num", 2);
			//SetSystem("thread_pool", "false");
			//////////////////////////////////////////////////////////////////////////
			// Use Parallel Halcon
			SetSystem("parallelize_operators", "false");
			SetSystem("reentrant", "true");
			SetSystem("clip_region", "false");
			SetSystem("global_mem_cache", "exclusive");

			//SetSystem("tsp_temporary_mem_cache", "shared");
			SetSystem("temporary_mem_cache", "shared");

			ReadImage(&m_ho_Gi, HTuple(path_model) + "Gi.tif");
			GetDomain(m_ho_Gi, &m_ho_GiDomain);
			SmallestRectangle1(m_ho_GiDomain, &alig2::hv_Crop_Row1, &alig2::hv_Crop_Col1, &alig2::hv_Crop_Row2, &alig2::hv_Crop_Col2);

			bool ret = alig2::Alig2_Read(path_model);
			if (!ret)
			{
				Queue_Add(&m_main_queue_log, NULL, "**** Error loading models alignment! %s", path_model);
			}

			Queue_Add(&m_main_queue_log, NULL, "*-- Load_Model() - OK. %s", path_model);
		}
		catch (HException &except)
		{
			HTuple HExp;
			HTuple HOperatorName, HErrMsg;
			except.ToHTuple(&HExp);
			except.GetExceptionData(HExp, "operator", &HOperatorName);
			except.GetExceptionData(HExp, "error_message", &HErrMsg);

			CString sOperatorName, sErrMsg;
			sOperatorName = HOperatorName.S();
			sErrMsg = HErrMsg.S();

			Queue_Add(&m_main_queue_log, NULL, "***Halcon Exception*** Load_Model <%s>, <%s>. %s", sOperatorName.GetBuffer(), sErrMsg.GetBuffer(), path_model);
			return false;
		}
		catch (...)
		{
			Queue_Add(&m_main_queue_log, NULL, "***Exception*** Load_Model %s", path_model);
			return false;
		}
		return true;


	}
	bool Load_Teach(CString path_model)
	{
		CString path_teach = path_model + "Teach\\";

		bool ret = true;

		try
		{

			// --- Read HOBJ files ---------------------------------------------
			HCall(ret, Read_HObject(&m_ho_RegionFWM, HTuple(path_teach) + "RegionFWM"));
			HCall(ret, Read_HObject(&m_ho_SkeletonsFWMScut, HTuple(path_teach) + "SkeletonsFWMScut"));
			HCall(ret, Read_HObject(&m_ho_SkelSEP, HTuple(path_teach) + "SkelSEP"));
			HCall(ret, Read_HObject(&m_ho_RegionCut, HTuple(path_teach) + "RegionCut"));

			HCall(ret, Read_HObject(&m_ho_SkeletonsFWMcut, HTuple(path_teach) + "SkeletonsFWMcut"));

			HCall(ret, Read_HObject(&m_ho_RegionGS, HTuple(path_teach) + "RegionGS"));
			//ReadObject(&m_ho_RectangleCut, HTuple(path_teach) + "RectangleCut");
			HCall(ret, Read_HObject(&m_ho_RegionG, HTuple(path_teach) + "RegionG"));
			HCall(ret, Read_HObject(&m_ho_RegionsEMPTY, HTuple(path_teach) + "RegionsEMPTY"));
			HCall(ret, Read_HObject(&m_ho_RegionsGND, HTuple(path_teach) + "RegionsGND"));
			//ReadObject(&m_ho_RegionsNFWM, HTuple(path_teach) + "RegionsNFWM");
			//ReadObject(&m_ho_RegionsPNi, HTuple(path_teach) + "RegionsPNi");
			HCall(ret, Read_HObject(&m_ho_RegionsPNPi, HTuple(path_teach) + "RegionsPNPi"));
			HCall(ret, Read_HObject(&m_ho_RegionsPNSi, HTuple(path_teach) + "RegionsPNSi"));
			HCall(ret, Read_HObject(&m_ho_RegNoProc, HTuple(path_teach) + "RegNoProc"));
			//ReadObject(&m_ho_SkeletonFWMi, HTuple(path_teach) + "SkeletonFWMi");
			HCall(ret, Read_HObject(&m_ho_SkeletonG, HTuple(path_teach) + "SkeletonG"));
			HCall(ret, Read_HObject(&m_ho_SkeletonGS, HTuple(path_teach) + "SkeletonGS"));
			//ReadObject(&m_ho_SkeletonPartsP, HTuple(path_teach) + "SkeletonPartsP");
			HCall(ret, Read_HObject(&m_ho_SkeletonsFWM, HTuple(path_teach) + "SkeletonsFWM"));
			HCall(ret, Read_HObject(&m_ho_SkeletonsFWMS, HTuple(path_teach) + "SkeletonsFWMS"));
			//ReadObject(&m_ho_SkeletonsG, HTuple(path_teach) + "SkeletonsG");

			// 
			//SelectShape(m_ho_SkeletonsFWM, &m_ho_SkeletonsFWM, "area", "and", 1500, 99999);
			//SelectShape(m_ho_SkeletonsFWM, &m_ho_SkeletonsFWMvalid, "area", "and", 1500, 99999);
			GenSkeletonFWMvalid(m_ho_SkeletonsFWM, // функция, убирающая внутр. и внешний витки
				&m_ho_SkeletonsFWMvalid,
				//1300, 900);  // длина внешн. и внутр. витков
				1, 1);  // длина внешн. и внутр. витков

				//m_pFWM->skipbegP - m_pFWM->skipendP
			// --- Read IMAGE files ---------------------------------------------
			HCall(ret, Read_HImage(&m_ho_ImCLC, HTuple(path_teach) + "ImCLC.tif"));
			HCall(ret, Read_HImage(&m_ho_DirImageP, HTuple(path_teach) + "DirImageP.tif"));
			HCall(ret, Read_HImage(&m_ho_DirImageS, HTuple(path_teach) + "DirImageS.tif"));
			//ReadImage(&m_ho_ImageIR, HTuple(path_teach) + "ImageIR.tif");
			//ReadImage(&m_ho_ImageIR16, HTuple(path_teach) + "ImageIR16.tif");
			//ReadImage(&m_ho_ImageIRS, HTuple(path_teach) + "ImageIRS.tif");
			//ReadImage(&m_ho_ImDisplacement, HTuple(path_teach) + "ImDisplacement.tif");
			HCall(ret, Read_HImage(&m_ho_WidthImageP, HTuple(path_teach) + "WidthImageP.tif"));
			HCall(ret, Read_HImage(&m_ho_WidthImageS, HTuple(path_teach) + "WidthImageS.tif"));

			GetFWMStandardWidth(m_ho_SkeletonsFWM, m_ho_WidthImageP, &m_ho_FWMpar);
			//ReadTuple(HTuple(path_teach) + "FWMpar.tup", &m_ho_FWMpar);

			// --- Read BIN files ---------------------------------------------
			CString path_bin;
			path_bin.Format("%s\\Tuples\\Bin\\", path_teach);
			if (Check_Path(path_bin + "X1.dat"))
			{
				// --- for Slice
				m_Slice_Teach.Read_Teach(CStringA(path_model));

				//if (Check_Path(path_bin + "Cols_Sens.dat"))
				//{
				//	inspect_utils::Read_Bin_File(path_bin + "Cols_Sens.dat", &CSlice::m_Cols_Sens);
				//	inspect_utils::Read_Bin_File(path_bin + "Pointer_Sens.dat", &CSlice::m_Pointer_Sens);
				//	inspect_utils::Read_Bin_File(path_bin + "Rows_Sens.dat", &CSlice::m_Rows_Sens);
				//	inspect_utils::Read_Bin_File(path_bin + "Pointer_Skel.dat", &CSlice::m_Pointer_Skel);
				//	CSlice::m_Readed = true;
				//}
				//else
				//{
				//	CSlice::m_Readed = false;
				//}
				//if (Check_Path(path_bin + "Cols_Sens_P.dat"))
				//{
				//	inspect_utils::Read_Bin_File(path_bin + "Cols_Sens_P.dat", &CSlice::m_Cols_Sens_P);
				//	inspect_utils::Read_Bin_File(path_bin + "Pointer_Sens_P.dat", &CSlice::m_Pointer_Sens_P);
				//	inspect_utils::Read_Bin_File(path_bin + "Rows_Sens_P.dat", &CSlice::m_Rows_Sens_P);
				//	inspect_utils::Read_Bin_File(path_bin + "Pointer_Skel_P.dat", &CSlice::m_Pointer_Skel_P);
				//	CSlice::m_Readed_P = true;
				//}
				//else
				//{
				//	CSlice::m_Readed_P = false;
				//}

				// --- for FWM ---
				Np = inspect_utils::Read_Bin_File(path_bin + "X1.dat", &pP1x);
				inspect_utils::Read_Bin_File(path_bin + "Y1.dat", &pP1y);
				inspect_utils::Read_Bin_File(path_bin + "X2.dat", &pP2x);
				inspect_utils::Read_Bin_File(path_bin + "Y2.dat", &pP2y);
				inspect_utils::Read_Bin_File(path_bin + "XC.dat", &pP3x);
				inspect_utils::Read_Bin_File(path_bin + "YC.dat", &pP3y);

				inspect_utils::Read_Bin_File(path_bin + "Pnum.dat", &pPnum);
				inspect_utils::Read_Bin_File(path_bin + "Pnum2.dat", &pPnum2);
				inspect_utils::Read_Bin_File(path_bin + "WG.dat", &pWG);
				inspect_utils::Read_Bin_File(path_bin + "Puseint.dat", &pPuseint);

				Na = inspect_utils::Read_Bin_File(path_bin + "PaddrR.dat", &pPaddrR);
				NaC = inspect_utils::Read_Bin_File(path_bin + "PaddrC.dat", &pPaddrC);

				// --- for FWMS ---
				NpS = inspect_utils::Read_Bin_File(path_bin + "X1S.dat", &pP1Sx);
				inspect_utils::Read_Bin_File(path_bin + "Y1S.dat", &pP1Sy);
				inspect_utils::Read_Bin_File(path_bin + "X2S.dat", &pP2Sx);
				inspect_utils::Read_Bin_File(path_bin + "Y2S.dat", &pP2Sy);
				inspect_utils::Read_Bin_File(path_bin + "XCS.dat", &pP3Sx);
				inspect_utils::Read_Bin_File(path_bin + "YCS.dat", &pP3Sy);

				inspect_utils::Read_Bin_File(path_bin + "PnumS.dat", &pPnumS);
				inspect_utils::Read_Bin_File(path_bin + "Pnum2S.dat", &pPnum2S);
				inspect_utils::Read_Bin_File(path_bin + "WGS.dat", &pWGS);

				NaS = inspect_utils::Read_Bin_File(path_bin + "PaddrRS.dat", &pPaddrRS);
				NaCS = inspect_utils::Read_Bin_File(path_bin + "PaddrCS.dat", &pPaddrCS);

				Difference(m_ho_GiDomain, m_ho_RegNoProc, &m_ho_RegProc);
			}
			else
			{
				Queue_Add(&m_main_queue_log, NULL, "**** Error loading BIN files! %s", path_bin);
			}


			m_num_teach += 1;
			Queue_Add(&m_main_queue_log, NULL, "*-- Load_Teach() - OK. %s", path_model);
		}
		catch (HException &except)
		{
			HTuple HExp;
			HTuple HOperatorName, HErrMsg;
			except.ToHTuple(&HExp);
			except.GetExceptionData(HExp, "operator", &HOperatorName);
			except.GetExceptionData(HExp, "error_message", &HErrMsg);

			CString sOperatorName, sErrMsg;
			sOperatorName = HOperatorName.S();
			sErrMsg = HErrMsg.S();

			Queue_Add(&m_main_queue_log, NULL, "***Halcon Exception*** Load_Teach_FWM <%s>, <%s>. %s", sOperatorName.GetBuffer(), sErrMsg.GetBuffer(), path_model);
			return false;
		}
		catch (...)
		{
			Queue_Add(&m_main_queue_log, NULL, "***Exception*** Load_Teach_FWM %s", path_model);
			return false;
		}
		return ret;
	}

	bool Load_Teach3(CString path_model, int &iNoMeander, int &iNoPad, int &iNoConnPad, int &iNoWireAngle)
	{
		CString path_teach = path_model + "Teach3\\";

		bool ret = true;

		try
		{
			// --- Read HOBJ files ---------------------------------------------

			HCall(ret, Read_HObject(&m_ho_Gi_CbmTest, HTuple(path_teach) + "Gi"));

			HCall(ret, Read_HObject(&m_ho_ContoursGrsm, HTuple(path_teach) + "ContoursGrsmc"));
			HCall(ret, Read_HObject(&m_ho_MeanderRects, HTuple(path_teach) + "MeanderRects"));

			HCall(ret, Read_HObject(&m_ho_ContoursGrsmPi, HTuple(path_teach) + "ContoursGrsmPi"));
			HCall(ret, Read_HObject(&m_ho_RectanglesPadsIsol, HTuple(path_teach) + "RectanglesPadsIsol"));

			HCall(ret, Read_HObject(&m_ho_ContoursGrsmPc, HTuple(path_teach) + "ContoursGrsmPc"));
			HCall(ret, Read_HObject(&m_ho_RectanglesPadsCon, HTuple(path_teach) + "RectanglesPadsCon"));

			HCall(ret, Read_HObject(&m_ho_ContoursWireAngles, HTuple(path_teach) + "ContoursWireAngles"));
			HCall(ret, Read_HObject(&m_ho_RectanglesWireAngles, HTuple(path_teach) + "RectanglesWireAngles"));

			HTuple tnum;

			CountObj(m_ho_MeanderRects, &tnum);
			iNoMeander = tnum.I();

			CountObj(m_ho_RectanglesPadsIsol, &tnum);
			iNoPad = tnum.I();

			CountObj(m_ho_RectanglesPadsCon, &tnum);
			iNoConnPad = tnum.I();

			CountObj(m_ho_ContoursWireAngles, &tnum);
			iNoWireAngle = tnum.I();

			m_num_teach += 1;
			Queue_Add(&m_main_queue_log, NULL, "*-- Load_Teach3() - OK. %s", path_model);
		}
		catch (HException &except)
		{
			HTuple HExp;
			HTuple HOperatorName, HErrMsg;
			except.ToHTuple(&HExp);
			except.GetExceptionData(HExp, "operator", &HOperatorName);
			except.GetExceptionData(HExp, "error_message", &HErrMsg);

			CString sOperatorName, sErrMsg;
			sOperatorName = HOperatorName.S();
			sErrMsg = HErrMsg.S();

			Queue_Add(&m_main_queue_log, NULL, "***Halcon Exception*** Load_Teach3 <%s>, <%s>. %s", sOperatorName.GetBuffer(), sErrMsg.GetBuffer(), path_model);
			return false;
		}
		catch (...)
		{
			Queue_Add(&m_main_queue_log, NULL, "***Exception*** Load_Teach3 %s", path_model);
			return false;
		}
		return ret;
	}

	bool Load_Teach_DT(CString path_model)
	{
		CString path_teach = path_model + "LowGrayTeach\\";

		bool ret = true;

		try
		{
			// --- Read HOBJ files ---------------------------------------------

			HObject HCadImage;
			HCall(ret, Read_HImage(&HCadImage, HTuple(path_teach) + "Gi.tif"));

			Threshold(HCadImage, &m_ho_CadSpaceRegion_DynThresTest, 0, 128);
			Threshold(HCadImage, &m_ho_CadPatternRegion_DynThresTest, 128, 255);

			Queue_Add(&m_main_queue_log, NULL, "*-- Load_Teach_DT() - OK. %s", path_model);
		}
		catch (HException &except)
		{
			HTuple HExp;
			HTuple HOperatorName, HErrMsg;
			except.ToHTuple(&HExp);
			except.GetExceptionData(HExp, "operator", &HOperatorName);
			except.GetExceptionData(HExp, "error_message", &HErrMsg);

			CString sOperatorName, sErrMsg;
			sOperatorName = HOperatorName.S();
			sErrMsg = HErrMsg.S();

			Queue_Add(&m_main_queue_log, NULL, "***Halcon Exception*** Load_Teach_DT <%s>, <%s>. %s", sOperatorName.GetBuffer(), sErrMsg.GetBuffer(), path_model);
			return false;
		}
		catch (...)
		{
			Queue_Add(&m_main_queue_log, NULL, "***Exception*** Load_Teach_DT %s", path_model);
			return false;
		}
		return ret;
	}

	// -------------------------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------

	void Union_Defects(HObject ho_ROI1, HObject ho_ROI2, HObject *ho_ROI1_Ret, HObject *ho_ROI2_Ret,
		HTuple hv_Defect_Dist)
	{
		// Local iconic variables
		HObject  ho_ROI1_C, ho_ROI2_C, ho_ROI1_Res, ho_ROI2_Res;
		HObject  ho_ObjSel1, ho_ObjSel2, ho_Skeleton1, ho_RegionDilation1;
		HObject  ho_Skeleton2, ho_RegionDilation2, ho_RegionIntersection;

		// Local control variables
		HTuple  hv_Cnt1, hv_Cnt2, hv_I1, hv_I2, hv_Area;
		HTuple  hv_Row, hv_Col;

		Union1(ho_ROI1, &ho_ROI1);
		Union1(ho_ROI2, &ho_ROI2);

		Connection(ho_ROI1, &ho_ROI1_C);
		Connection(ho_ROI2, &ho_ROI2_C);
		CountObj(ho_ROI1_C, &hv_Cnt1);
		CountObj(ho_ROI2_C, &hv_Cnt2);

		if (0 != (HTuple(hv_Cnt1 == 0).TupleOr(hv_Cnt2 == 0)))
		{
			(*ho_ROI1_Ret) = ho_ROI1;
			(*ho_ROI2_Ret) = ho_ROI2;
			return;
		}

		ho_ROI1_Res = ho_ROI1;
		ho_ROI2_Res = ho_ROI2;
		Connection(ho_ROI1_Res, &ho_ROI1_Res);
		Connection(ho_ROI2_Res, &ho_ROI2_Res);

		{
			HTuple end_val19 = hv_Cnt1;
			HTuple step_val19 = 1;
			for (hv_I1 = 1; hv_I1.Continue(end_val19, step_val19); hv_I1 += step_val19)
			{
				SelectObj(ho_ROI1_C, &ho_ObjSel1, hv_I1);
				//area_center (ObjSel1, Area1, Row1, Col1)

				{
					HTuple end_val23 = hv_Cnt2;
					HTuple step_val23 = 1;
					for (hv_I2 = 1; hv_I2.Continue(end_val23, step_val23); hv_I2 += step_val23)
					{
						SelectObj(ho_ROI2_C, &ho_ObjSel2, hv_I2);
						//area_center (ObjSel2, Area2, Row2, Col2)

						Skeleton(ho_ObjSel1, &ho_Skeleton1);
						DilationCircle(ho_Skeleton1, &ho_RegionDilation1, hv_Defect_Dist);
						Skeleton(ho_ObjSel2, &ho_Skeleton2);
						DilationCircle(ho_Skeleton2, &ho_RegionDilation2, hv_Defect_Dist);

						Intersection(ho_RegionDilation1, ho_RegionDilation2, &ho_RegionIntersection
						);
						AreaCenter(ho_RegionIntersection, &hv_Area, &hv_Row, &hv_Col);

						if (0 != (hv_Area > 0))
						{
							ObjDiff(ho_ROI1_Res, ho_ObjSel1, &ho_ROI1_Res);
							ObjDiff(ho_ROI2_Res, ho_ObjSel2, &ho_ROI2_Res);
							Union2(ho_ROI1_Res, ho_ObjSel1, &ho_ROI1_Res);
						}

						//*         DRow := abs(Row1 - Row2)
						//*         DCol := abs(Col1 - Col2)
						//*         if (DRow < Defect_Dist and DCol < Defect_Dist)
						//union2 (ROI1_Res, ObjSel2, ROI1_Res)
						//obj_diff (ROI1_Res, ObjSel1, ROI1_Res)
						//obj_diff (ROI2_Res, ObjSel2, ROI2_Res)
						//*             gen_circle(Circle, Row1, Col1, Defect_Dist / 2)
						//*             union2(ROI1_Res, Circle, ROI1_Res)
						//*         endif
					}
				}
			}
		}

		//DilationCircle(ho_ROI1_Res, ho_ROI1_Ret, hv_Defect_Dist);

		// (*ho_ROI1_Ret) = ho_ROI1_Res;
		(*ho_ROI2_Ret) = ho_ROI2_Res;
		return;
	}


	void GenSkeletonFWMvalid(HObject ho_SkeletonsFWM, HObject *ho_SkeletonFWMvalid,
		HTuple hv_cutfrombeg, HTuple hv_cutfromend)
	{

		// Local iconic variables
		HObject  ho_S1, ho_Contour1, ho_S1c;

		// Local control variables
		HTuple  hv_nFWM, hv_j, hv_Rows, hv_Columns, hv_Row;
		HTuple  hv_Col, hv_l, hv_Row2, hv_lc, hv_Col2;

		CountObj(ho_SkeletonsFWM, &hv_nFWM);
		GenEmptyObj(&(*ho_SkeletonFWMvalid));
		{
			HTuple end_val2 = hv_nFWM;
			HTuple step_val2 = 1;
			for (hv_j = 1; hv_j.Continue(end_val2, step_val2); hv_j += step_val2)
			{
				SelectObj(ho_SkeletonsFWM, &ho_S1, hv_j);
				GetRegionPoints(ho_S1, &hv_Rows, &hv_Columns);
				GenContourRegionXld(ho_S1, &ho_Contour1, "center");

				GenContoursSkeletonXld(ho_S1, &ho_Contour1, 1, "filter");
				GetContourXld(ho_Contour1, &hv_Row, &hv_Col);
				TupleLength(hv_Row, &hv_l);
				TupleSelectRange(hv_Row, hv_cutfrombeg, hv_l - hv_cutfromend, &hv_Row2);
				TupleLength(hv_Col, &hv_lc);
				TupleSelectRange(hv_Col, hv_cutfrombeg, hv_lc - hv_cutfromend, &hv_Col2);
				GenRegionPoints(&ho_S1c, hv_Row2, hv_Col2);
				Union2((*ho_SkeletonFWMvalid), ho_S1c, &(*ho_SkeletonFWMvalid));
			}
		}
		return;
	}


	bool Read_HObject(HObject* Object, const HTuple& FileName)
	{
		try
		{
			ReadObject(Object, FileName);
			return true;
		}
		catch (HException &except)
		{
			HTuple HExp;
			HTuple HOperatorName, HErrMsg;
			except.ToHTuple(&HExp);
			except.GetExceptionData(HExp, "operator", &HOperatorName);
			except.GetExceptionData(HExp, "error_message", &HErrMsg);

			CString sOperatorName, sErrMsg;
			sOperatorName = HOperatorName.S();
			sErrMsg = HErrMsg.S();

			Queue_Add(&m_main_queue_log, NULL, "***Halcon Exception*** Object loading error <%s>, <%s>. %s", sOperatorName.GetBuffer(), sErrMsg.GetBuffer(), FileName);

			CString str;
			str.Format("Object loading error <%s>", FileName);
			AfxMessageBox(str, MB_ICONERROR | MB_SYSTEMMODAL);
			return false;
		}
	}
	bool Read_HImage(HObject* Image, const HTuple& FileName)
	{
		try
		{
			ReadImage(Image, FileName);
			return true;
		}
		catch (HException &except)
		{
			HTuple HExp;
			HTuple HOperatorName, HErrMsg;
			except.ToHTuple(&HExp);
			except.GetExceptionData(HExp, "operator", &HOperatorName);
			except.GetExceptionData(HExp, "error_message", &HErrMsg);

			CString sOperatorName, sErrMsg;
			sOperatorName = HOperatorName.S();
			sErrMsg = HErrMsg.S();

			Queue_Add(&m_main_queue_log, NULL, "***Halcon Exception*** Image loading error <%s>, <%s>. %s", sOperatorName.GetBuffer(), sErrMsg.GetBuffer(), FileName);

			CString str;
			str.Format("Image loading error <%s>", FileName);
			AfxMessageBox(str, MB_ICONERROR | MB_SYSTEMMODAL);
			return false;
		}
	}




	//std::mutex m_mutex_total_defects;
	//int Get_Defect_NO()
	//{
	//	std::unique_lock<std::mutex> lock(m_mutex_total_defects);
	//	int ret = m_total_defects;
	//	m_total_defects += 1;
	//	return ret;
	//}
	bool CheckParamBool(json key)
	{
		bool value;
		try { value = (bool)key; }
		catch (const std::exception&) { return false; }
		return true;
	}
	bool GetParamBool(json key, bool default)
	{
		bool value;
		try { value = (bool)key; }
		catch (const std::exception&) { value = default; }
		return value;
	}

	int GetParamInt(json key, int default)
	{
		int value;
		try { value = (int)key; }
		catch (const std::exception&) { value = default; }
		return value;
	}

	double GetParamDouble(json key, double default)
	{
		double value;
		try { value = (double)key; }
		catch (const std::exception&) { value = default; }
		return value;
	}

	string GetParamString(json key, string default)
	{
		string value;
		try { value = (string)key; }
		catch (const std::exception&) { value = default; }
		return value;
	}

	void Auto_Threshold(HObject ho_RegProc, HObject ho_Im, HTuple *hv_thr)
	{

		// Local iconic variables
		HObject  ho_RegionTh5;

		// Local control variables
		HTuple  hv_AbsoluteHisto, hv_FAbsoluteHisto, hv_FAbsoluteHistosm;
		HTuple  hv_Min2, hv_Max2;

		//gray_histo_abs (RegProc, Im, 1, AbsoluteHisto)
		//create_funct_1d_array (AbsoluteHisto, FAbsoluteHisto)
		//smooth_funct_1d_mean (FAbsoluteHisto, 9, 3, FAbsoluteHistosm)
		//local_min_max_funct_1d (FAbsoluteHisto, 'strict_min_max', 'true', Min1, Max1)
		//* tuple_mean (Min1, thr1)
		//* tuple_mean (Max1, thr2)

		//threshold (Im, RegionTh3, 60, 210)
		//* reduce_domain(Im, RegionTh3, Imp)
		//* gray_histo_abs(RegionTh3, Imp, 1, AbsoluteHisto)


		GrayHistoAbs(ho_RegProc, ho_Im, 1, &hv_AbsoluteHisto);
		CreateFunct1dArray(hv_AbsoluteHisto, &hv_FAbsoluteHisto);
		SmoothFunct1dMean(hv_FAbsoluteHisto, 9, 3, &hv_FAbsoluteHistosm);
		LocalMinMaxFunct1d(hv_FAbsoluteHistosm, "strict_min_max", "true", &hv_Min2, &hv_Max2);
		//tuple_median (Min2, thr1)
		//tuple_mean (Min2, thr2)
		//tuple_median (Max2, thr11)
		TupleMean(hv_Max2, &(*hv_thr));
		Threshold(ho_Im, &ho_RegionTh5, (*hv_thr), 255);
		return;
	}
	void Auto_Threshold2(HObject ho_SkeletonsFWMS, HObject ho_Im, HTuple *hv_thr)
	{
		// Local iconic variables
		HObject  ho_SkeletonFWMS, ho_SkeletonFWMSd, ho_Imp;

		// Local control variables
		HTuple  hv_AbsoluteHisto1, hv_Middle, hv_Space_T;
		HTuple  hv_FSpace_T, hv_MinS, hv_MaxS, hv_Idx, hv_thrS;
		HTuple  hv_Patern_T, hv_FPatern_T, hv_MinP, hv_MaxP, hv_thrP;

		Union1(ho_SkeletonsFWMS, &ho_SkeletonFWMS);
		DilationCircle(ho_SkeletonFWMS, &ho_SkeletonFWMSd, 9.5);
		ReduceDomain(ho_Im, ho_SkeletonFWMSd, &ho_Imp);

		GrayHistoAbs(ho_SkeletonFWMSd, ho_Imp, 1, &hv_AbsoluteHisto1);
		//create_funct_1d_array (AbsoluteHisto1, FAbsoluteHisto1)
		//* smooth_funct_1d_mean(FAbsoluteHisto1, 9, 3, FAbsoluteHistosm1)
		//local_min_max_funct_1d (FAbsoluteHistosm1, 'strict_min_max', 'true', Min21, Max21)


		hv_Middle = 180;
		TupleFirstN(hv_AbsoluteHisto1, hv_Middle - 1, &hv_Space_T);
		CreateFunct1dArray(hv_Space_T, &hv_FSpace_T);
		SmoothFunct1dMean(hv_FSpace_T, 9, 3, &hv_FSpace_T);
		LocalMinMaxFunct1d(hv_FSpace_T, "plateaus_center", "true", &hv_MinS, &hv_MaxS);

		Max_Index(hv_MaxS, hv_FSpace_T, &hv_Idx);
		hv_thrS = HTuple(hv_MaxS[hv_Idx]);



		TupleLastN(hv_AbsoluteHisto1, hv_Middle, &hv_Patern_T);
		CreateFunct1dArray(hv_Patern_T, &hv_FPatern_T);
		SmoothFunct1dMean(hv_FPatern_T, 9, 3, &hv_FPatern_T);
		LocalMinMaxFunct1d(hv_FPatern_T, "plateaus_center", "true", &hv_MinP, &hv_MaxP);

		Max_Index(hv_MaxP, hv_FPatern_T, &hv_Idx);
		hv_thrP = HTuple(hv_MaxP[hv_Idx]) + hv_Middle;

		TupleMean(hv_thrS.TupleConcat(hv_thrP), &(*hv_thr));

		return;
	}

	void Auto_Threshold3(HObject ho_RegionFWM, HObject ho_ImAlignment,
		HTuple *hv_thr)
	{

		// Local iconic variables
		HObject ho_RegionFWMd, ho_RegionFWMe;
		// Local control variables
		HTuple  hv_AbsoluteHisto, hv_RelativeHisto, hv_FAbsoluteHisto;
		HTuple  hv_FAbsoluteHistoSm, hv_Min, hv_MaxV, hv_Vmin, hv_Vmax;
		HTuple  hv_nM, hv_maxl127, hv_maxg127, hv_j, hv_jg, hv_jl;
		HTuple  hv_tmax1, hv_tmax2;

		DilationCircle(ho_RegionFWM, &(ho_RegionFWMd), 5.5);
		ErosionCircle(ho_RegionFWMd, &(ho_RegionFWMe), 5.5);
		GrayHisto((ho_RegionFWMe), ho_ImAlignment, &hv_AbsoluteHisto, &hv_RelativeHisto);
		CreateFunct1dArray(hv_AbsoluteHisto, &hv_FAbsoluteHisto);
		SmoothFunct1dMean(hv_FAbsoluteHisto, 5, 3, &hv_FAbsoluteHistoSm);
		LocalMinMaxFunct1d(hv_FAbsoluteHistoSm, "plateaus_center", "true", &hv_Min, &hv_MaxV);
		GetYValueFunct1d(hv_FAbsoluteHistoSm, hv_Min, "constant", &hv_Vmin);
		GetYValueFunct1d(hv_FAbsoluteHistoSm, hv_MaxV, "constant", &hv_Vmax);
		TupleLength(hv_MaxV, &hv_nM);
		hv_maxl127 = 255;
		hv_maxg127 = 0;
		{
			HTuple end_val10 = hv_nM - 1;
			HTuple step_val10 = 1;
			for (hv_j = 0; hv_j.Continue(end_val10, step_val10); hv_j += step_val10)
			{
				if (0 != (HTuple(hv_MaxV[hv_j]) > 127))
				{
					if (0 != (HTuple(hv_Vmax[hv_j]) > hv_maxg127))
					{
						hv_maxg127 = HTuple(hv_Vmax[hv_j]);
						hv_jg = hv_j;
					}
				}
				else
				{
					if (0 != (HTuple(hv_Vmax[hv_j]) > hv_maxl127))
					{
						hv_maxl127 = HTuple(hv_Vmax[hv_j]);
						hv_jl = hv_j;
					}
				}
			}
		}
		hv_tmax1 = hv_maxl127;
		hv_tmax2 = hv_maxg127;
		(*hv_thr) = (HTuple(hv_MaxV[hv_jg]) + HTuple(hv_MaxV[hv_jl])) / 2.;
		return;
	}

	void GetFWMStandardWidth(HObject ho_SkeletonFWM, HObject ho_WidthImageP, HTuple *hv_stdWFWM)
	{

		HObject ho_SkeletonFWMi;
		Union1(ho_SkeletonFWM, &ho_SkeletonFWMi);

		// Local control variables
		HTuple  hv_RowsFWM, hv_ColumnsFWM, hv_GWFWM;

		//** GetFWMStandardWidth
		//250228
		GetRegionPoints(ho_SkeletonFWMi, &hv_RowsFWM, &hv_ColumnsFWM);
		GetGrayval(ho_WidthImageP, hv_RowsFWM, hv_ColumnsFWM, &hv_GWFWM);
		TupleMedian(hv_GWFWM, &(*hv_stdWFWM));
		return;
	}

	void VerifySPregnoproc(HObject ho_RegNoProc, HObject ho_RegionsInoD, HObject ho_RegionsSP,
		HObject *ho_RegionsSPv, HTuple hv_mA)
	{

		// Local iconic variables
		HObject  ho_RegNoProcb, ho_RegionsInoDb, ho_RSP;
		HObject  ho_RegionIntersection1, ho_RegionIntersection2;
		HObject  ho_RegionIntersection3, ho_RegionIntersection3c;

		// Local control variables
		HTuple  hv_nSP, hv_j, hv_Areas3, hv_Rows3, hv_Columns3;
		HTuple  hv_maxA3;

		//*** VerifySPregnoproc - removing SPs caused by RegNoProc corners
		//* 15.04.2025

		Boundary(ho_RegNoProc, &ho_RegNoProcb, "inner");
		Boundary(ho_RegionsInoD, &ho_RegionsInoDb, "outer");

		CountObj(ho_RegionsSP, &hv_nSP);
		GenEmptyObj(&(*ho_RegionsSPv));

		{
			HTuple end_val9 = hv_nSP;
			HTuple step_val9 = 1;
			for (hv_j = 1; hv_j.Continue(end_val9, step_val9); hv_j += step_val9)
			{
				SelectObj(ho_RegionsSP, &ho_RSP, hv_j);
				Intersection(ho_RSP, ho_RegNoProcb, &ho_RegionIntersection1);
				Intersection(ho_RSP, ho_RegionsInoDb, &ho_RegionIntersection2);
				Intersection(ho_RegionIntersection1, ho_RegionIntersection2, &ho_RegionIntersection3
				);
				Connection(ho_RegionIntersection3, &ho_RegionIntersection3c);
				AreaCenter(ho_RegionIntersection3c, &hv_Areas3, &hv_Rows3, &hv_Columns3);
				TupleMax(hv_Areas3, &hv_maxA3);
				if (0 != (hv_maxA3 < hv_mA))
				{
					ConcatObj((*ho_RegionsSPv), ho_RSP, &(*ho_RegionsSPv));
				}
			}
		}
		// stop(...); only in hdevelop
		return;
	}

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

			Queue_Add(&m_main_queue_log, NULL, "*-- BlobUnion() Halcon Exception: <%s>, <%s>",
				sOperatorName, sErrMsg);

			return FALSE;
		}
	}

}

/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

