/**
 * \copyright (C) Synapse Imaging 2023
 *
 * @File Name
 *      Alig2.h
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
#pragma once

 /* ************************************************************************** */
 /* Section: Included Files                                                    */
 /* ************************************************************************** */

namespace alig2
{
	/* ************************************************************************** */
	/* Section: File Scope or Global Data                                         */
	/* ************************************************************************** */
	//extern HTuple  hv_path, hv_i_Width, hv_i_Height, hv_G_Width;
	//extern HTuple  hv_G_Height, hv_Models_Count, hv_Max_Bias1, hv_Max_Bias2;
	//extern HTuple  hv_Min_Scope, hv_ROI_1_Row1, hv_ROI_1_Col1, hv_ROI_1_Row2;
	//extern HTuple  hv_ROI_1_Col2, hv_ROI_Row1, hv_ROI_Row2, hv_ROI_Col1;
	//extern HTuple  hv_ROI_Col2, hv_ModelID_1, hv_Row_Ref_1, hv_Col_Ref_1;
	//extern HTuple  hv_Row_Ref_2, hv_Col_Ref_2, hv_ModelID_2, hv_Alig_Param;
	//extern HTuple  hv_ModelID, hv_alig2;

	//extern HTuple  hv_Row_Ref, hv_Col_Ref;

	extern HTuple hv_Crop_Row1, hv_Crop_Col1, hv_Crop_Row2, hv_Crop_Col2;

	/* ************************************************************************** */
	/* Section: Prototype functions                                               */
	/* ************************************************************************** */

	// Procedure declarations 
	// Chapter: Develop
	// Short Description: Switch dev_update_pc, dev_update_var and dev_update_window to 'off'. 
	void dev_update_off();
	// Local procedures 
	void Alig2(HObject ho_Im, HObject *ho_Img_ALG, HTuple hv_Сombination, HTuple *hv_Shift_Row, HTuple *hv_Shift_Col, HTuple *hv_alig2_ok, HTuple *hv_ReturnHomMat2D);
	void Alig2_Lite(HObject ho_Im, HObject *ho_Img_ALG, HTuple hv_Сombination, HTuple *hv_Shift_Row, HTuple *hv_Shift_Col, HTuple *hv_alig2_ok, HTuple *hv_ReturnHomMat2D);
	bool Alig2_Read(CString path);


}
/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */
