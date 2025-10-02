/**
 * \copyright (C) Synapse Imaging 2025
 *
 * @File Name
 *      Open_Short_2.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Iurii Kim
 * @version 1.0
 * @date 1/13/2025 11:49:01 AM
 */

#pragma once
//#include <numeric>      // std::iota
//#include <algorithm>    // std::sort
//#include <iostream>     // std::cout
//#include <functional>   // std::bind
//
//#include <cstring>
//#include <string.h>
//#include <stdio.h>
//#include <basetsd.h>
//#include <memory.h>
//#include <set>
//#include <stdlib.h>
//#include <filesystem>
//#include <algorithm>
//
//#include <iostream>
//#include <vector>
//#include <atlstr.h>
//#include <fstream>
//#include <cmath>
//
//
//#include "HalconCpp.h"

//using namespace HalconCpp;
//using namespace std;

#ifdef ALGORITHMSUB_EXPORTS
#define ALGORITHMSUB_API __declspec(dllexport)
#else
#define ALGORITHMSUB_API __declspec(dllimport)
#endif

#define OP2_PARENT_GRAY			1
#define OP2_SPACE_GRAY			0


/** ---------------------------------------------------------------------------
* @Summary
*		Параметры инспекции
*/
struct Open_Short_Param_t
{
	int model_loaded;

	// --- Обучение -----------------------------------------------------------
	// Допустимый разрыв серого при обучении
	int cv_Gray_Gap_Teach;				// 20
	// Размер буфера для Points_Buff. Размер изображения?
	int buff_size;						// 100000000
	// Размер инкремента буффера для Cross_Gray
	int alloc_step;						// 1000000 

	// --- Инспекция ----------------------------------------------------------
	// Порог
	double cv_Thr_Inspect;
	double cv_Thr_Open;
	double cv_Thr_Short;

	// ----------------------------------------------------------------------------

	// Минмальная допустимая площадь региона CAD для идентификации номеров закрашивания
	int cv_Min_CAD_Area_Open;				
	int cv_Min_CAD_Area_Short;				
	// TODO: Если div_Min_CAD_Area > 0, то берем минимальный размер из обучения и делим на этот делитель
	int div_Min_CAD_Area_Open;				// 3
	int div_Min_CAD_Area_Short;				// 3
	// Допустимый разрыв серого
	int cv_Gray_Gap_Open;					
	int cv_Gray_Gap_Short;					
	// Максимальная длина дефекта
	//int cv_Max_Len_Open;
	//int cv_Max_Len_Short;

	// ----------------------------------------------------------------------------

	//// Минмальная допустимая площадь региона CAD для идентификации номеров закрашивания
	//int cv_Min_CAD_Area;				// 100
	//// Если div_Min_CAD_Area > 0, то берем минимальный размер из обучения и делим на этот делитель
	//int div_Min_CAD_Area;				// 3
	//// Допустимый разрыв серого
	//int cv_Gray_Gap;					// 60
	// Проверять диагональные пиксели 
	int check_diagonals;				// 1
	//// Максимальная длина дефекта
	////int cv_Max_Len;
	//// ----------------------------------------------------------------------------



	// Проверять оверкиллы Open/Short
	int check_overkills_Open;
	int check_overkills_Short;

	int save_test_file;
	CString path_test;					// "C:\\WORK_CURR\\ALGORITMS\\Inspection_SUB\\Halcon\\Model3\\";

	// --- Статистические данные ----------------------------------------------
	int count_defects_open;
	int count_defects_short;
	int count_overkill_open;
	int count_overkill_short;




};
/** ---------------------------------------------------------------------------
* @Summary
*		Класс объектов разрешенных переходов Cross_Gray
*/
class Cross_Gray
{
#define CROSS_ALOC_STEP		500000
public:
	UINT32 *m_B;
	UINT32 *m_E;
	int* m_Idx_B;
	int m_Pos;
	int m_Size;
	int cv_Gray_Gap;
	int param_1;
	int param_2;
	int m_test;
public:
	~Cross_Gray();
	Cross_Gray(int test);
	void Alloc();
	/// Удалить дубликаты
	void Delete_DUB();
	/// Скопировать данные в тупл
	int Get_Tuple(HTuple &hv_SKL_Cross_B, HTuple &hv_SKL_Cross_E, HTuple &hv_SKL_Cross_B_IDX);
	/// Скопировать данные из Тупла
	void From_Tuple(HTuple hv_SKL_Cross_B, HTuple hv_SKL_Cross_E);
	/// Скопировать данные из Тупла
	void From_Tuple(HTuple hv_SKL_Cross_B, HTuple hv_SKL_Cross_E, HTuple hv_SKL_Cross_B_IDX);
	void Insert(UINT32 cross_B, UINT32 cross_E);
	// Индексация
	void Creating_Index(bool mirror);
	void Creating_Index();
	// Проверяем gray1 и gray2 на допустимую пару 
	bool Сompare(UINT32 gray1, UINT32 gray2, int param);
	void Сompare1(UINT32 gray1, UINT32 gray2);
	bool Сompare2(UINT32 gray1, UINT32 gray2, int cv_Gary_Gap);
};
/** ---------------------------------------------------------------------------
* @Summary
*		Класс объектов заливок
*/
class Points_Buff
{
public:
	int *m_pX;
	int *m_pY;
	int m_Pos_S;
	int m_Pos_E;
	int m_Length;
	int m_Size;

public:
	~Points_Buff();
	Points_Buff(int length);

	void Inc_Pos(int &pos);
	void Dec_Pos(int &pos);
	bool Insert_Back(int pX, int pY);
	bool Insert_First(int pX, int pY);
	bool Get_First(int &pX, int &pY);
	bool Get_Last(int &pX, int &pY);
	int Length();
	bool Empty();
	bool Full();
	void Clear();
};

class ALGORITHMSUB_API Open_Short_2
{
public:
	Open_Short_Param_t params;
	HObject ho_Inspect_ID;
	HObject ho_Open, ho_Short;
	HObject m_ho_RegionsIsl, m_ho_RegionPH;

private:
	HObject ho_SH_Im_Cont, ho_Im_Cont_ID, ho_SH_Gi_Reg_Proc, ho_OP_Im_Cont, ho_OP_Gi_Reg_Proc;
	HTuple hv_SH_SKL_Cross_B, hv_SH_SKL_Cross_E, hv_SH_SKL_Cross_B_IDX, hv_OP_SKL_Cross_B, hv_OP_SKL_Cross_E, hv_OP_SKL_Cross_B_IDX, hv_Min_Region_Area;
	Cross_Gray *m_Cross_Open;
	Cross_Gray *m_Cross_Short;



public:
	bool Check_Path(HTuple hv_folder);
	void ConfirmFolder(HTuple hv_folder);
	Open_Short_2();
	~Open_Short_2();

public:
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Обучение
	*/
	bool Init_Teaching();
	void Teaching_Open_Short(HObject ho_Gi, HObject ho_RegNoProc, HTuple path_model);

	bool Check_Path(CString files_path);

	/** ---------------------------------------------------------------------------
	* @Summary
	*		Инспектирование
	*/
	bool Init_Inspection(HTuple path_model);
	bool Read_Teaching_2(HTuple path_model);
	void Inspect_Open_Short(
		HObject ho_Im, HObject ho_Gi_Reg, HObject ho_RegNoProc);

	void Remove_Null_Objects(HObject & ho_RegionDefect);


// --------------------------------------------------------------------------------
// --- Инспекция ------------------------------------------------------------------
// --------------------------------------------------------------------------------
//private:
public:
	// ----------------------------------------------------------------------------
	// Минмальная допустимая площадь региона CAD для идентификации номеров закрашивания
	int cv_Min_CAD_Area;				// 100
	// Если div_Min_CAD_Area > 0, то берем минимальный размер из обучения и делим на этот делитель
	int div_Min_CAD_Area;				// 3
	// Допустимый разрыв серого
	int cv_Gray_Gap;					// 60
	//// Проверять диагональные пиксели 
	//int check_diagonals;				// 1
	// Максимальная длина дефекта
	//int cv_Max_Len;
	// ----------------------------------------------------------------------------

	/** ---------------------------------------------------------------------------
	* @Summary
	*		Удаление оверкиллов
	*/
	int Remove_Overkills(
		HObject &ho_Reg_Defects,				// Регион с дефектами
		HObject ho_Im_Reg);						// Регион изображения. Для Open патерн, для Short space
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Получение атрибутов дефектов
	*/
	void Get_Defect_Attribute(
		HObject ho_Im,
		HObject ho_Reg_Defects,													// Регион с дефектами
		HTuple &hv_Gray_Min, HTuple &hv_Gray_Max, HTuple &hv_Gray_Median);		// Атрибуты
		/** ---------------------------------------------------------------------------
	* @Summary
	*		Стадии инспекции
	*/
	void Inspect_Stage(HObject ho_Gi_Reg_E, HObject ho_Im_Cont, HObject ho_Im_Reg_E, HObject ho_Im_Domain,
		Cross_Gray &cv_Defect,
		Cross_Gray &cross, HTuple hv_Im_W, HTuple hv_Im_H);

	void Inspect_OPEN(HObject ho_Im, HObject ho_Gi_Reg_E_S, HObject ho_RegNoProc,
		HObject ho_OP_Im_Cont, Cross_Gray &cross, HObject &ho_Open);
	
	void Inspect_SHORT(HObject ho_Im, HObject ho_Gi_Reg_E_P, HObject ho_RegNoProc,
		HObject ho_SH_Im_Cont, Cross_Gray &cross, HObject &ho_Short);

	bool Read_Auto_Params(HTuple path_model, int *cv_Min_CAD_Area_Short, int *cv_Min_CAD_Area_Open, int *cv_Gray_Gap_Short, int *cv_Gray_Gap_Open);

public:
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Стадии инспекции ID
	*/
	void Inspect_ID_Stage(
		HObject ho_Gi_Reg_E, HObject ho_Im_Cont, HObject ho_Im_Reg_E,
		HTuple hv_Im_W, HTuple hv_Im_H,
		HObject &ho_Inspect_ID);

private:
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Функции заполнения
	*/
	void Paint_Check_Inspect_1(UINT32 *pIm_Inspect, UINT32 *pIm_Cont, int set_X, int set_Y, int q_X, int q_Y, Points_Buff &buf_Q, Points_Buff &buf_Q0,
		int cv_Cont_W, int cv_Cont_H, int cv_Inspect_W, int cv_Inspect_H);
	void Paint_Check_Inspect_2(UINT32 *pIm_Inspect, int set_X, int set_Y, UINT32 i_Set_Gary, Points_Buff &buf_Q, Cross_Gray &cv_Short, Cross_Gray &cross,
		int cv_Inspect_W, int cv_Inspect_H);
	void Paint_Check_Inspect_2(
		UINT16 *pIm_Inspect, int set_X, int set_Y, UINT16 i_Set_Gary, Points_Buff &buf_Q,
		int cv_Inspect_W, int cv_Inspect_H);

	///** ---------------------------------------------------------------------------
	//* @Summary
	//*		Стадии инспекции ID
	//*/
	//void Inspect_ID_Stage(
	//	HObject ho_Gi_Reg_E, HObject ho_Im_Cont, HObject ho_Im_Reg_E,
	//	HTuple hv_Im_W, HTuple hv_Im_H,
	//	HObject &ho_Inspect_ID);

	/** ---------------------------------------------------------------------------
	* @Summary
	*		Формирование ID изображения
	*/
	void Inspect_Image_ID(
		HObject ho_Im, HObject ho_RegNoProc,
		HObject ho_Gi_Reg_Proc, HObject ho_Im_Cont_ID, HObject &ho_Inspect_ID);

	void Inspect_OPEN_SHORT(
		HObject ho_Gi_Reg, HObject ho_Im, HObject ho_RegNoProc,
		HObject ho_OP_Gi_Reg_Proc, HObject ho_OP_Im_Cont, Cross_Gray &cross_Open, HObject &ho_Open,
		HObject ho_SH_Gi_Reg_Proc, HObject ho_SH_Im_Cont, Cross_Gray &cross_Short, HObject &ho_Short);

// --------------------------------------------------------------------------------
// --- Обучение -------------------------------------------------------------------
// --------------------------------------------------------------------------------
private:
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Заполнение изображением серым скелетоном
	*/
	void Fill_SKL(
		HTuple hv_Rows, HTuple hv_Cols,
		UINT32* co_Im_Cont, UINT16* co_Im_Cont_ID, UINT32 &cv_SKL_GRAY, Cross_Gray &cross_gray, Points_Buff &buf_Q, UINT16 reg_ID,
		int img_W, int img_H);

	/** ---------------------------------------------------------------------------
	* @Summary
	*		Обучение общее для всех инспекций
	*/
	void Teach_Stage(
		HObject ho_Gi_Reg_Proc,
		HObject ho_Im_Cont, HObject ho_Im_Cont_ID, Cross_Gray &cross_gray,
		int img_W, int img_H);
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Обучение для Open
	*/
	void Teach_OPEN(
		HObject ho_Gi_Reg_Proc,
		HObject &ho_Im_Cont, Cross_Gray &cross_gray,
		int hv_Im_W, int hv_Im_H);
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Обучение для Short
	*/
	void Teach_SHORT(
		HObject ho_Gi_Reg_Proc,
		HObject &ho_Im_Cont, HObject &ho_Im_Cont_ID, Cross_Gray &cross_gray,
		HTuple hv_Im_W, HTuple hv_Im_H);
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Обучение для Open/Short
	*/
	void Teach_OPEN_SHORT(
		HObject ho_Gi, HObject ho_RegNoProc,
		HObject &ho_Im_Cont_S, Cross_Gray &cross_gray_S, HObject &ho_Gi_Reg_Proc_S,
		HObject &ho_Im_Cont, HObject &ho_Im_Cont_ID, HObject &ho_Gi_Reg_Proc, Cross_Gray &cross_gray);


	/** ---------------------------------------------------------------------------
	* @Summary
	*		Функции работы с изображениями
	*/
	UINT32 Get_Gray_OS(UINT32 *img, int pos_X, int pos_Y, int img_W, int img_H);
	UINT16 Get_Gray_OS(UINT16 *img, int pos_X, int pos_Y, int img_W, int img_H);
	void Set_Gray_OS(UINT32 *img, int pos_X, int pos_Y, int img_W, int img_H, UINT32 gray);
	void Set_Gray_OS(UINT16 *img, int pos_X, int pos_Y, int img_W, int img_H, UINT16 gray);
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Функция проверки пикселя
	*/
	void Paint_Check_Teach(
		UINT32 *pImg, UINT16 *pImg_ID, int set_X, int set_Y, UINT32 set_gray, UINT32 &get_gray, UINT16 set_id,
		Cross_Gray &cross, Points_Buff &buf_Q, int im_w, int im_h);

};

//ALGORITHMSUB_API void Read_Auto_Params(HTuple path_model, int *cv_Min_CAD_Area_Short, int *cv_Min_CAD_Area_Open, int *cv_Gray_Gap_Short, int *cv_Gray_Gap_Open);


#ifdef __cplusplus
extern "C" {
#endif
extern __declspec(dllexport) Open_Short_2* Create_Open_Short_2();
extern __declspec(dllexport) void Teaching_Open_Short_2(
	Open_Short_2* op, int gray_gap, int buff_size, const char *path_model);

#ifdef __cplusplus
}
#endif

/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */
