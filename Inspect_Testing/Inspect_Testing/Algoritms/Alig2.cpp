/**
 * \copyright (C) Synapse Imaging 2023
 *
 * @File Name
 *      Alig2.cpp
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 2023.08.08
 */
#include "Inspect_Common.h"
#include "Alig2.h"

using namespace inspect_utils;

namespace alig2
{
#define FM_REF_COL			0
#define FM_REF_ROW			1
#define FM_METRIC			2
#define FM_MIN_SCOPRE		3
#define FM_NUM_LEVEL1		4
#define FM_NUM_LEVEL2		5
#define FM_INTERPOLATION	6
	class FM_Params
	{
	public:
		HTuple hv_Params;
		HTuple hv_ModelID;
		HObject ho_ROI_FM;
		HObject ho_ROI_Find;
		HTuple ho_NumLevel;

	public:
		void GetNumLevel()
		{
			if (hv_Params[FM_NUM_LEVEL1] == hv_Params[FM_NUM_LEVEL2])
			{
				ho_NumLevel = hv_Params[FM_NUM_LEVEL1];
			}
			else
			{
				TupleGenConst(2, 0, &ho_NumLevel);
				ho_NumLevel[0] = hv_Params[FM_NUM_LEVEL1];
				ho_NumLevel[1] = hv_Params[FM_NUM_LEVEL2];
			}
		}
	};

	/* ************************************************************************** */
	/* Section: File Scope or Global Data                                         */
	/* ************************************************************************** */
	HTuple  hv_path, hv_i_Width, hv_i_Height, hv_G_Width;
	HTuple  hv_G_Height, hv_Models_Count, hv_Max_Bias1, hv_Max_Bias2;
	HTuple  hv_Min_Scope, hv_ROI_1_Row1, hv_ROI_1_Col1, hv_ROI_1_Row2;
	HTuple  hv_ROI_1_Col2, hv_ROI_Row1, hv_ROI_Row2, hv_ROI_Col1;
	HTuple  hv_ROI_Col2, hv_ModelID_1, hv_Row_Ref_1, hv_Col_Ref_1;
	HTuple  hv_Row_Ref_2, hv_Col_Ref_2, hv_ModelID_2, hv_Alig_Param;
	HTuple  hv_ModelID, hv_alig2;

	HTuple  hv_Row_Ref, hv_Col_Ref;

	HTuple hv_Crop_Row1, hv_Crop_Col1, hv_Crop_Row2, hv_Crop_Col2;

	int m_iVersion_Alignment = 0;

	FM_Params m_FM_Params[5];
	FM_Params m_FM_Params_Lite[1];
	bool m_Model_Available;
	bool m_Model_Available_Lite;


	/* ************************************************************************** */
	/* Section: Functions                                                         */
	/* ************************************************************************** */

	// Procedures 
	// Short Description: Switch dev_update_pc, dev_update_var and dev_update_window to 'off'. 
	void dev_update_off()
	{

		//This procedure sets different update settings to 'off'.
		//This is useful to get the best performance and reduce overhead.
		//
		// dev_update_pc(...); only in hdevelop
		// dev_update_var(...); only in hdevelop
		// dev_update_window(...); only in hdevelop
		return;
	}

	void Alig2(HObject ho_Im, HObject *ho_Img_ALG, HTuple hv_Сombination, HTuple *hv_Shift_Row, HTuple *hv_Shift_Col, HTuple *hv_alig2_ok, HTuple *hv_ReturnHomMat2D)
	{
		// Local iconic variables
		HObject  ho_Img_ROI_1, ho_Img_ALG_1, ho_Img_ALG_1_ROI;

		// Local control variables
		HTuple  hv_Row, hv_Column, hv_Angle, hv_Score;
		HTuple  hv_matrix_trans, hv_Rows_Alig, hv_Cols_Alig, hv_Row_Ref_Alig;
		HTuple  hv_Col_Ref_Alig, hv_Length;
		HTuple  hv_HomMat2D, hv_Covariance;

		if (m_Model_Available == false)
		{
			*ho_Img_ALG = ho_Im;
			(*hv_alig2_ok) = 0;
			return;
		}

		//-- Сombination -----------------------------------------------------------
		if (hv_Сombination == 1)
		{
			ReduceDomain(ho_Im, m_FM_Params[0].ho_ROI_Find, &ho_Img_ROI_1);
			FindShapeModel(
				ho_Img_ROI_1,
				m_FM_Params[0].hv_ModelID, 0, 0, 0.1, 1, 0, "least_squares",
				m_FM_Params[0].ho_NumLevel,
				0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);

			*hv_Shift_Row = m_FM_Params[0].hv_Params[FM_REF_ROW] - hv_Row;
			*hv_Shift_Col = m_FM_Params[0].hv_Params[FM_REF_COL] - hv_Column;

			HomMat2dIdentity(&hv_matrix_trans);
			HomMat2dTranslate(
				hv_matrix_trans,
				*hv_Shift_Row,
				*hv_Shift_Col,
				&hv_matrix_trans);
			AffineTransImage(
				ho_Im, &ho_Img_ALG_1, hv_matrix_trans,
				m_FM_Params[0].hv_Params[FM_INTERPOLATION],
				"true");

		}
		else
		{
			ho_Img_ALG_1 = ho_Im;
		}

		//-- Alignment -------------------------------------------------------------
		hv_Rows_Alig = HTuple();
		hv_Cols_Alig = HTuple();
		hv_Row_Ref_Alig = HTuple();
		hv_Col_Ref_Alig = HTuple();
		for (int i = 1; i < 5; i++)
		{
			ReduceDomain(ho_Img_ALG_1, m_FM_Params[i].ho_ROI_Find, &ho_Img_ALG_1_ROI);
			FindShapeModel(
				ho_Img_ALG_1_ROI,
				m_FM_Params[i].hv_ModelID, 0, 0, 0.1, 1,
				0.5, "least_squares",
				m_FM_Params[i].ho_NumLevel,
				0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);

			//TupleLength(hv_Score, &hv_Score_Len);
			if (hv_Score > m_FM_Params[i].hv_Params[FM_MIN_SCOPRE])
			{
				TupleConcat(hv_Rows_Alig, hv_Row, &hv_Rows_Alig);
				TupleConcat(hv_Cols_Alig, hv_Column, &hv_Cols_Alig);

				TupleConcat(hv_Row_Ref_Alig, m_FM_Params[i].hv_Params[FM_REF_ROW], &hv_Row_Ref_Alig);
				TupleConcat(hv_Col_Ref_Alig, m_FM_Params[i].hv_Params[FM_REF_COL], &hv_Col_Ref_Alig);
			}
		}

		TupleLength(hv_Rows_Alig, &hv_Length);
		if (hv_Length == 4)
		{
			VectorToHomMat2d(hv_Rows_Alig, hv_Cols_Alig, hv_Row_Ref_Alig, hv_Col_Ref_Alig, &hv_HomMat2D);
			VectorToHomMat2d(hv_Row_Ref_Alig, hv_Col_Ref_Alig, hv_Rows_Alig, hv_Cols_Alig, hv_ReturnHomMat2D);
			AffineTransImage(
				ho_Img_ALG_1, &(*ho_Img_ALG), hv_HomMat2D,
				"constant", "true");

			CropRectangle1(
				*ho_Img_ALG, ho_Img_ALG,
				hv_Crop_Row1, hv_Crop_Col1, hv_Crop_Row2, hv_Crop_Col2);

			(*hv_alig2_ok) = 1;
		}
		else
		{
			*ho_Img_ALG = ho_Im;
			(*hv_alig2_ok) = 0;
		}

		return;
	}

	void Alig2_Lite(HObject ho_Im, HObject *ho_Img_ALG, HTuple hv_Сombination, HTuple *hv_Shift_Row, HTuple *hv_Shift_Col, HTuple *hv_alig2_ok, HTuple *hv_ReturnHomMat2D)
	{
		// Local iconic variables
		HObject  ho_Img_ROI_1, ho_Img_ALG_1, ho_Img_ALG_1_ROI;

		// Local control variables
		HTuple  hv_Row, hv_Column, hv_Angle, hv_Score;
		HTuple  hv_matrix_trans, hv_Rows_Alig, hv_Cols_Alig, hv_Row_Ref_Alig;
		HTuple  hv_Col_Ref_Alig, hv_Length;
		HTuple  hv_HomMat2D, hv_Covariance;

		if (m_Model_Available_Lite == false)
		{
			*ho_Img_ALG = ho_Im;
			(*hv_alig2_ok) = 0;
			return;
		}

		//-- Сombination -----------------------------------------------------------
		//ReduceDomain(ho_Im, m_FM_Params_Lite[0].ho_ROI_Find, &ho_Img_ROI_1);
		FindShapeModel(
			ho_Im, //ho_Img_ROI_1,
			m_FM_Params_Lite[0].hv_ModelID, 0, 0, 0.1, 1, 0, "least_squares",
			m_FM_Params_Lite[0].ho_NumLevel,
			0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);

		*hv_Shift_Row = m_FM_Params_Lite[0].hv_Params[FM_REF_ROW] - hv_Row;
		*hv_Shift_Col = m_FM_Params_Lite[0].hv_Params[FM_REF_COL] - hv_Column;

		HomMat2dIdentity(&hv_matrix_trans);
		HomMat2dTranslate(
			hv_matrix_trans,
			*hv_Shift_Row,
			*hv_Shift_Col,
			&hv_matrix_trans);
		AffineTransImage(
			ho_Im, ho_Img_ALG, hv_matrix_trans,
			m_FM_Params_Lite[0].hv_Params[FM_INTERPOLATION],
			"true");

		if (hv_Score > m_FM_Params_Lite[0].hv_Params[FM_MIN_SCOPRE])
		{
			CropRectangle1(
				*ho_Img_ALG, ho_Img_ALG,
				hv_Crop_Row1, hv_Crop_Col1, hv_Crop_Row2, hv_Crop_Col2);


			(*hv_alig2_ok) = 1;
		}
		else
		{
			*ho_Img_ALG = ho_Im;
			(*hv_alig2_ok) = 0;
		}

		return;
	}


	bool Alig2_Read(CString path)
	{
		HTuple hv_Crop_Row1, hv_Crop_Col1, hv_Crop_Row2, hv_Crop_Col2;

		// --- Read alignments objects ---- -------------
		HTuple alig_Path = HTuple(path + "\\Alig2\\");
		HTuple alig_Path_Lite = HTuple(path + "\\Alig2\\Lite\\");
		HTuple hv_Alig_Param, hv_Length;

		if (!Check_Path(path + "\\Alig2\\" + "Alig_Param.tup"))
		{
			return false;
		}


		ReadTuple(alig_Path + "Alig_Param.tup", &hv_Alig_Param);
		TupleLength(hv_Alig_Param, &hv_Length);

		m_iVersion_Alignment = 2;

		if (hv_Length > 5)
		{
			// New Alignment
			m_iVersion_Alignment = hv_Alig_Param[4];
		}

		m_Model_Available = false;
		m_Model_Available_Lite = false;
		if (m_iVersion_Alignment == 3 || m_iVersion_Alignment == 4)
		{
			if (Check_Path(path + "\\Alig2\\FM_0\\" + "Params.tup"))
			{
				for (int i = 0; i < 5; i++)
				{
					HTuple fm_Path = alig_Path + "FM_" + i + "\\";
					ReadTuple(fm_Path + "Params.tup", &m_FM_Params[i].hv_Params);
					ReadShapeModel(fm_Path + "ModelID.sbm", &m_FM_Params[i].hv_ModelID);
					ReadObject(&m_FM_Params[i].ho_ROI_Find, fm_Path + "ROI_Find.hobj");
					ReadObject(&m_FM_Params[i].ho_ROI_FM, fm_Path + "ROI_FM.hobj");
					m_FM_Params[i].GetNumLevel();
				}

				m_Model_Available = true;
			}

			if (Check_Path(path + "\\Alig2\\Lite\\" + "Alig_Param.tup"))
			{
				int i = 0;
				HTuple fm_Path = alig_Path_Lite + "FM_" + i + "\\";
				ReadTuple(fm_Path + "Params.tup", &m_FM_Params_Lite[i].hv_Params);
				ReadShapeModel(fm_Path + "ModelID.sbm", &m_FM_Params_Lite[i].hv_ModelID);
				ReadObject(&m_FM_Params_Lite[i].ho_ROI_Find, fm_Path + "ROI_Find.hobj");
				ReadObject(&m_FM_Params_Lite[i].ho_ROI_FM, fm_Path + "ROI_FM.hobj");
				m_FM_Params_Lite[i].GetNumLevel();

				m_Model_Available_Lite = true;
			}

		}


		return true;
	}

}
/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */


