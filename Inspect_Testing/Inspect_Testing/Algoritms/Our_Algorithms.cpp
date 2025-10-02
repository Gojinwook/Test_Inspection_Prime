/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
		Our_Algorithms.cpp
 *
 * @Summary
 *
 * @Description
 *		
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 8/27/2024 6:51:41 AM
 */

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#ifndef USE_DLL
#include "Inspect_Common.h"
#endif

#include "Our_Algorithms.h"


/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */

/* ************************************************************************** */
/* Section: Function				                                          */
/* ************************************************************************** */
#if !defined(USE_DLL) || defined(FWMDLL_EXPORTS)
/** ---------------------------------------------------------------------------
* @Summary
*	Выделение памяти для субпиксельных координат
*	Для выделения памяти уходит много ресурсов. 
*	Поэтому, выделяем память один раз и много
*/
FWMDLL_API void Alloc_Sub(sub_buff_t *m_buff)
{
	m_buff->reg_count = 0;
	if (m_buff->len_sub != NULL)
	{
		// Инициализация
		return;
	}
	m_buff->rows_sub = (double*)malloc(REGION_SUB_CNT * POINTS_SUB_LEN * sizeof(double));
	m_buff->cols_sub = (double*)malloc(REGION_SUB_CNT * POINTS_SUB_LEN * sizeof(double));
	m_buff->len_sub = (int*)malloc(REGION_SUB_CNT * sizeof(int));
}
/** ---------------------------------------------------------------------------
* @Summary
*	Получение значения яркости пикселя
*/
UINT8 Get_Gray_Val(UINT8 *pIm, int im_w, int im_h, int y, int x)
{
	return *(pIm + y * im_w + x);
}
/** ---------------------------------------------------------------------------
* @Summary
*	Проверка, является ли регион дыркой
*/
bool Test_Hole(UINT8 *pIm, int im_w, int im_h, UINT64* rows, UINT64* cols, int count_len, int thr)
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
*	Интерполяция. Определение субпиксельной координаты. 
*/
bool Interpolation_Sub(bool reg_hole, bool direct, UINT8 gray0, UINT8 gray1, UINT8 gray2, int col_curr, int row_curr, int thr, double &xR, double &yR)
{
	int x0 = -1, x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	// * Идем вниз.Сравниваем с полем справа
	// * Идем влево. Сравниваем с полем снизу
	if (direct && ((reg_hole && gray1 > thr) || (!reg_hole && gray1 < thr)))
	{
		x0 = thr;
		x1 = gray0;
		x2 = gray1;
		y1 = col_curr;
	}
	// * Идем вверх. Сравниваем с полем слева
	// * Идем вправо. Сравниваем с полем сверху
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
/** ---------------------------------------------------------------------------
* @Summary
*	Формирование субпиксельных координат для региона
*/
int Region_Threshold_SubPix(HObject ho_Im, HObject ho_RegSel, int thr, double *rows_sub, double *cols_sub)
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

	// Тест на дырку
	bool reg_hole = Test_Hole(pIm, im_w, im_h, rows, cols, count_len, thr);

	int pos;
	for (pos = 0; pos < count_len; pos++)
	{
		// * Окружение исследуемого пикселя
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

		// За пределами изображения
		if (row_curr >= im_h - 1 || col_curr >= im_w - 1 || row_curr == 0 || col_curr == 0)
		{
			continue;
		}

		// * Направление
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

		// *--- Собираем результат ------------------------------------------------------
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

#endif
/** ---------------------------------------------------------------------------
* @Summary
*	Пример использования
*/
void Example_Sub(HTuple file_name_Im, HTuple file_name_save)
{
	HObject ho_Im, ho_Im_Reg, ho_Im_RegN, ho_RegSel;

	ReadImage(&ho_Im, file_name_Im);

	struct sub_buff_t m_buff;
	Alloc_Sub(&m_buff);

	HTuple hv_t1, hv_t2, hv_time;
	CountSeconds(&hv_t1);

	int thr = 150;
	Threshold(ho_Im, &ho_Im_Reg, thr, 255);
	Threshold(ho_Im, &ho_Im_RegN, 0, thr);
	Connection(ho_Im_Reg, &ho_Im_Reg);
	Connection(ho_Im_RegN, &ho_Im_RegN);

	int idx = 0;
	SelectObj(ho_Im_RegN, &ho_RegSel, 1);
	m_buff.len_sub[idx] = Region_Threshold_SubPix(ho_Im, ho_RegSel, thr, m_buff.rows_sub, m_buff.cols_sub);

	CountSeconds(&hv_t2);
	hv_time = hv_t2 - hv_t1;

	Hlong num = m_buff.len_sub[idx];
	double* row_ptr = &m_buff.rows_sub[idx * POINTS_SUB_LEN];
	double* col_ptr = &m_buff.cols_sub[idx * POINTS_SUB_LEN];
	HTuple hv_Rows_Sub = HTuple(row_ptr, num);
	HTuple hv_Cols_Sub = HTuple(col_ptr, num);
	HObject  ho_Reg_Sub;
	GenContourPolygonXld(&ho_Reg_Sub, hv_Rows_Sub, hv_Cols_Sub);

	WriteObject(ho_Reg_Sub, file_name_save);
}

/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

