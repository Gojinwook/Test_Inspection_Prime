/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
		Slice.cpp
 *
 * @Summary
 *
 * @Description
 *		
 *
 * @authors Iurii Kim
 * @version 1.0
 * @date 4/3/2024 6:37:06 AM
 */

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
//#include "pch.h"

#include "Inspect_Common.h"

#include "Slice.h"


/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
//bool CSlice::m_Readed;
//int *CSlice::m_Cols_Sens;
//int *CSlice::m_Rows_Sens;
//int *CSlice::m_Pointer_Sens;
//int *CSlice::m_Pointer_Skel;
//
//bool CSlice::m_Readed_P;
//int *CSlice::m_Cols_Sens_P;
//int *CSlice::m_Rows_Sens_P;
//int *CSlice::m_Pointer_Sens_P;
//int *CSlice::m_Pointer_Skel_P;
/* ************************************************************************** */
/* Section: Function				                                          */
/* ************************************************************************** */


/** ---------------------------------------------------------------------------
* @Summary
*
*/
void CSlice::Convert_To_Bin(CString path_teach)
{
	inspect_utils::SaveTupleToBinInt(path_teach, "Cols_Sens", "Cols_Sens");
	inspect_utils::SaveTupleToBinInt(path_teach, "Pointer_Sens", "Pointer_Sens");
	inspect_utils::SaveTupleToBinInt(path_teach, "Rows_Sens", "Rows_Sens");
	inspect_utils::SaveTupleToBinInt(path_teach, "Pointer_Skel", "Pointer_Skel");
}
void CSlice::Convert_To_Bin_P(CString path_teach)
{
	inspect_utils::SaveTupleToBinInt(path_teach, "Cols_Sens_P", "Cols_Sens_P");
	inspect_utils::SaveTupleToBinInt(path_teach, "Pointer_Sens_P", "Pointer_Sens_P");
	inspect_utils::SaveTupleToBinInt(path_teach, "Rows_Sens_P", "Rows_Sens_P");
	inspect_utils::SaveTupleToBinInt(path_teach, "Pointer_Skel_P", "Pointer_Skel_P");
}
void CSlice::Init()
{
	if (m_Count_Skel > 0)
	{
		delete(m_Widt_S);
		delete(m_Depth_S);
		delete(m_Depth_Ppr_S);
		delete(m_Rows_Cent);
		delete(m_Cols_Cent);
		delete(m_Gray);
		delete(m_Gray_Avg);
		delete(m_Loc_Min);
		delete(m_Loc_Min_Val);
		delete(m_Thr_Step);
		delete(m_Intersect);
	}

	/// TODO: Расчитать размер максимального скелетона из m_Pointer_Sens
	m_size = 500000; 

	m_Widt_S = (float*)malloc(m_size * sizeof(float));
	m_Depth_S = (float*)malloc(m_size * sizeof(float));
	m_Depth_Ppr_S = (float*)malloc(m_size * 2 * sizeof(float));
	m_Rows_Cent = (double*)malloc(m_size * sizeof(double));
	m_Cols_Cent = (double*)malloc(m_size * sizeof(double));

	m_Gray = (UINT8*)malloc(1000 * sizeof(UINT8));
	m_Gray_Avg = (UINT8*)malloc(1000 * sizeof(UINT8));
	m_Loc_Min = (int*)malloc(1000 * sizeof(int));
	m_Loc_Min_Val = (int*)malloc(1000 * sizeof(int));


	// 5-я версия алгоритма 
	m_Itr = 10;
//	m_Itr = 20;
	m_Thr_Step = (double*)malloc(m_Itr * sizeof(double));
	for (int i = 0; i < m_Itr; i++)
	{
		m_Thr_Step[i] = pow(0.85, i);
//		m_Thr_Step[i] = pow(0.9, i);
	}
	m_Intersect = (double*)malloc(100 * sizeof(double));



}
void Get_Gray_Val(UINT8 *pIm, int im_w, int im_h, int *rows, int *cols, int size, UINT8 *gray)
{
	for (int i = 0; i < size; i++)
	{
		int x = cols[i];
		int y = rows[i];

		gray[i] = *(pIm + y * im_w + x);
	}
}
void Moving_Avg(UINT8 *gray, int t_len, int move_avg_h, UINT8 *gray_avg)
{
	int sum_mov = gray[0] * move_avg_h;
	for (int k = 0; k < t_len + move_avg_h / 2; k++)
	{
		if (k >= move_avg_h / 2)
		{
			gray_avg[k - move_avg_h / 2] = sum_mov / move_avg_h;
		}
		if (k >= move_avg_h)
		{
			sum_mov = sum_mov - gray[k - move_avg_h];
		}
		else
		{
			sum_mov = sum_mov - gray[0];
		}

		if (k < t_len)
		{
			sum_mov = sum_mov + gray[k];
		}
		else
		{
			sum_mov = sum_mov + gray[t_len - 1];
		}
	}
}
int Loc_Min(UINT8 *c_Gray, int c_Gray_Len, int *loc_min, int *loc_min_val)
{
	int pos = 0;
	int c_Fl = 1;
	for (int c_I = 1; c_I < c_Gray_Len; c_I++)
	{
		int c_I0 = c_I - 1;
		if (c_Gray[c_I0] > c_Gray[c_I])
		{
			c_Fl = 1;
		}

		if ((c_Gray[c_I0] < c_Gray[c_I] && c_Fl == 1) || (c_I == c_Gray_Len - 1 && c_Fl == 1))
		{
			loc_min[pos] = c_I0;
			loc_min_val[pos] = c_Gray[c_I0];
			pos += 1;
			c_Fl = 0;
		}


	}

	return pos;
}

void CSlice::Proc(UINT8 *pIm, int im_w, int im_h, 
	HTuple *hv_Def_Rows, HTuple *hv_Def_Cols, 
	HTuple &hv_PrcM_Min, HTuple &hv_PrcM_Max, HTuple &hv_PrcM_Median)
{
	int type = 5;

	hv_PrcM_Min = HTuple();
	hv_PrcM_Max = HTuple();
	hv_PrcM_Median = HTuple();
	*hv_Def_Rows = HTuple();
	*hv_Def_Cols = HTuple();


	int Point_S, Point_E;
	int sens_len;
	int Pos_Skel_E;
	int Pos_Skel_S = 0;
	int *c_Rows, *c_Cols;
	int c_Bott;

	for (int I_Skel = 0; I_Skel < m_Count_Skel; I_Skel++)
	{
		Pos_Skel_E = teach->m_Pointer_Skel[I_Skel];

		int Depth_Pos = 1;
		if (type == 5)
		{
			Depth_Pos = 0;
		}
		int Point;
		for (Point = Pos_Skel_S; Point <= Pos_Skel_E; Point++)
		{
			// Начало и конец сенсора
			Point_S = teach->m_Pointer_Sens[Point];
			Point_E = teach->m_Pointer_Sens[Point + 1] - 1;
			sens_len = Point_E - Point_S + 1;
			// Указатели на начало сенсора
			c_Rows = &teach->m_Rows_Sens[Point_S];
			c_Cols = &teach->m_Cols_Sens[Point_S];
			// Получить срез
			Get_Gray_Val(pIm, im_w, im_h, c_Rows, c_Cols, sens_len, m_Gray);
			m_SensLenD2 = sens_len / 2;

			if (type == 5)
			{
				//UINT8 *cv_Thr_Min = std::min_element(m_Gray, m_Gray + sens_len);
				//if (*cv_Thr_Min >= m_Threshold)
				//{
				//	continue;
				//}
				UINT8 cv_Thr_Min = 0;
				UINT8 cv_Thr_Span = m_Threshold - cv_Thr_Min;

				double cv_Square = 0.0;
				for (int it = 0; it < m_Itr; it++)
				{
					double cv_Thr = (double)cv_Thr_Min + ((double)cv_Thr_Span * m_Thr_Step[it]);

					//Идем вдоль линии порога и ищем пересечение
					int cv_Even = 0;
					double cv_Xp, cv_Pr;
					int intersect_pos = 0;
					for (int i = 0; i < sens_len; i++)
					{
						//Расчет субпиксельной координаты X пересечения порога
						if (m_Gray[i] < cv_Thr && cv_Even == 0)
						{
							//Пересечение вниз
							if (i == 0)
							{
								cv_Xp = 0;
							}
							else
							{
								double cv_T1 = m_Gray[i];
								double cv_T2 = m_Gray[i - 1];
								cv_Pr = (cv_T1 - cv_Thr) / (cv_T1 - cv_T2);
								cv_Xp = i - cv_Pr;
							}
							m_Intersect[intersect_pos++] = cv_Xp;
							cv_Even = 1;
						}
						else if (m_Gray[i] > cv_Thr && cv_Even == 1)
						{
							//Пересечение вверх
							if (i == sens_len - 1)
							{
								cv_Xp = sens_len - 1;
							}
							else
							{
								double cv_T1 = m_Gray[i];
								double cv_T2 = m_Gray[i - 1];
								cv_Pr = (cv_T1 - cv_Thr) / (cv_T1 - cv_T2);
								cv_Xp = i - cv_Pr;
							}
							m_Intersect[intersect_pos++] = cv_Xp;
							cv_Even = 0;
						}
					}

					if (cv_Even == 1)
					{
						m_Intersect[intersect_pos++] = sens_len - 1;
					}


					//Расчет ширины
					double cv_Width = 0;
					{
						for (int i = 0; i < intersect_pos; i += 2)
						{
							double cv_Wd = m_Intersect[i + 1] - m_Intersect[i];
							cv_Width += cv_Wd;
						}
					}
					cv_Square += cv_Width;
				}

				//** --------------------------------------------------
				m_Depth_S[Depth_Pos] = cv_Square;

				//** --------------------------------------------------
				//Определим положение максимума под порогом

				int cv_Gray_Max = 0;
				int cv_Cent_Pos = sens_len / 2;

				//for (int i = 0; i < sens_len; i++)
				//{
				//	if (m_Gray[i] < m_Threshold && m_Gray[i] > cv_Gray_Max)
				//	{
				//		cv_Gray_Max = m_Gray[i];
				//		cv_Cent_Pos = i;
				//	}
				//}


				m_Rows_Cent[Depth_Pos] = c_Rows[cv_Cent_Pos];
				m_Cols_Cent[Depth_Pos] = c_Cols[cv_Cent_Pos];

			}
			else if (type == 1)
			{
				//inspect_utils::Moving_Avg(m_Gray, sens_len, 10, m_Gray_Avg);

				//if (Point == 12334 || Point == 13706)
				//{
				//	HTuple hv_m_Gray;
				//	HTuple hv_m_Gray_Avg;
				//	for (int t = 0; t < sens_len; t++)
				//	{
				//		hv_m_Gray[t] = m_Gray[t];
				//		hv_m_Gray_Avg[t] = m_Gray_Avg[t];

				//	}
				//	WriteTuple(hv_m_Gray, "C:\\Temp\\hv_m_Gray.tup");
				//	WriteTuple(hv_m_Gray_Avg, "C:\\Temp\\hv_m_Gray_Avg.tup");
				//}

				//** Находим минимальный ближе к центру - это дно Space
				int min_count = Loc_Min(m_Gray, sens_len, m_Loc_Min, m_Loc_Min_Val);

				// --- Low Gray --------------------------------------------------------------------------
				//* Найти минимальный минимум.
				//* С допуском просматриваем все минимумы и выбираем, который ближе к центу
				c_Bott = 0;
				int *c_Min_Val = std::min_element(m_Loc_Min_Val, m_Loc_Min_Val + min_count);
				int c_Diff = 1000000;
				for (int c_I = 0; c_I < min_count; c_I++)
				{
					if (m_Loc_Min_Val[c_I] <= *c_Min_Val + 1)
					{
						if (c_Diff > abs(m_SensLenD2 - m_Loc_Min[c_I]))
						{
							c_Bott = m_Loc_Min[c_I];
							c_Diff = abs(m_SensLenD2 - m_Loc_Min[c_I]);
						}
					}
				}

				if (Depth_Pos == 1)
				{
					m_Depth_S[Depth_Pos - 1] = 0;
					m_Rows_Cent[Depth_Pos - 1] = c_Rows[c_Bott];
					m_Cols_Cent[Depth_Pos - 1] = c_Cols[c_Bott];
				}

				int c_Depth = m_Gray[c_Bott];
				m_Depth_S[Depth_Pos] = c_Depth;
				m_Rows_Cent[Depth_Pos] = c_Rows[c_Bott];
				m_Cols_Cent[Depth_Pos] = c_Cols[c_Bott];
			}
			else if (type == 2)
			{
				int cv_Avg_Gray = m_SensLenD2;

				//Сильно сглаживаем. Убираем соседние патерны
				//Формируем признак для текущей Gray линии
				//Максимальное значение усредненого Gray
				inspect_utils::Moving_Avg(m_Gray, sens_len, cv_Avg_Gray, m_Gray_Avg);

				UINT8 *cv_Depth = std::min_element(m_Gray_Avg, m_Gray_Avg + sens_len);
				m_Depth_S[Depth_Pos] = *cv_Depth;

				m_Rows_Cent[Depth_Pos] = c_Rows[m_SensLenD2];
				m_Cols_Cent[Depth_Pos] = c_Cols[m_SensLenD2];

				if (Depth_Pos == 2)
				{
					m_Depth_S[Depth_Pos - 2] = *cv_Depth;
					m_Rows_Cent[Depth_Pos - 2] = c_Rows[m_SensLenD2];
					m_Cols_Cent[Depth_Pos - 2] = c_Cols[m_SensLenD2];
				}

			}

			Depth_Pos += 1;

			int test = 0;
			if (test == 1)
			{
				HTuple hv_Gray;
				for (int t = 0; t <= sens_len; t++)
				{
					hv_Gray[t] = m_Gray[t];


				}
				WriteTuple(hv_Gray, "C:\\Temp\\hv_Gray.tup");

			}


			// ---------------------------------------------------------------------------------------
			/// TODO: Другие алгоритмы








			// ---------------------------------------------------------------------------------------



		}

		// Добавлении заднего фронта.Для определения граничных дефектов
		m_Depth_S[Depth_Pos] = m_Depth_S[Depth_Pos - 2];
		m_Rows_Cent[Depth_Pos] = 0;
		m_Cols_Cent[Depth_Pos] = 0;

		inspect_utils::SrunmedSLIExtNoS(m_Depth_S, m_Depth_Ppr_S + m_size, m_Depth_Ppr_S, Depth_Pos, (m_szw + 1) * 2, 0, 10); // Running median with Extension at the ends, skip and linear interp (with skip at ends).

		for (int i = 0; i < Depth_Pos; i++)
		{
			float Wch = (float)m_Depth_S[i] - m_Depth_Ppr_S[i];
			float ppr_S = Wch * 100 / (m_Depth_Ppr_S[i] + 0.001);
			m_Depth_Ppr_S[i] = ppr_S;
		}

		int test = 0;
		if (test)
		{
			HTuple hv_Depth_S;
			HTuple hv_Depth_Ppr_S;
			for (int t = 0; t <= Depth_Pos; t++)
			{
				hv_Depth_S[t] = m_Depth_S[t];
				hv_Depth_Ppr_S[t] = m_Depth_Ppr_S[t];


			}
			WriteTuple(hv_Depth_S, "C:\\Temp\\hv_Depth_S.tup");
			WriteTuple(hv_Depth_Ppr_S, "C:\\Temp\\hv_Depth_Ppr_S.tup");
		}


		//HTuple hv_Depth_S = HTuple(m_Depth_S, Depth_Pos);
		//HTuple hv_Depth_Ppr_S = HTuple(m_Depth_Ppr_S, Depth_Pos);



		Defect_Post(
			m_Depth_Ppr_S, m_Rows_Cent, m_Cols_Cent, Depth_Pos,
			m_prsLG,
			*hv_Def_Rows, *hv_Def_Cols, hv_PrcM_Min, hv_PrcM_Max, hv_PrcM_Median);

		if (test)
		{
			HTuple hv_Depth_S;
			HTuple hv_Depth_Ppr_S;
			for (int t = 0; t <= Depth_Pos; t++)
			{
				hv_Depth_Ppr_S[t] = m_Depth_Ppr_S[t];
			}
			WriteTuple(hv_Depth_Ppr_S, "C:\\Temp\\hv_Depth_Ppr_S1.tup");
		}


		//for (int i = 0; i < Depth_Pos; i++)
		//{

		//	float Wch = (float)m_Depth_S[i] - m_Depth_Ppr_S[i];
		//	float ppr_S = Wch * 100 / (m_Depth_Ppr_S[i] + 0.001);
		//	m_Depth_Ppr_S[i] = ppr_S;

		//	if (abs(ppr_S) > m_prsLG)
		//	{
		//		TupleConcat(*hv_Def_Rows, m_Rows_Cent[i], hv_Def_Rows);
		//		TupleConcat(*hv_Def_Cols, m_Cols_Cent[i], hv_Def_Cols);
		//		TupleConcat(*hv_Prc, abs(ppr_S), hv_Prc);
		//	}
		//}

		Pos_Skel_S = Pos_Skel_E + 1;
		//HTuple hv_m_Gray(m_Depth_Ppr_S, Depth_Pos);
		//WriteTuple(hv_m_Gray, "C:\\Temp\\m_Depth_Ppr_S");
	}
}

void CSlice::Defect_Post(
	float *depth_prc, double *rows_cent, double *cols_cent, int len,
	double cv_Proc_Thr,
	HTuple &hv_Def_Rows, HTuple &hv_Def_Cols, HTuple &hv_PrcM_Min, HTuple &hv_PrcM_Max, HTuple &hv_PrcM_Median)
{
	HTuple hv_Prc = HTuple(depth_prc, len);
	HTuple hv_Rows_Cent = HTuple(rows_cent, len);
	HTuple hv_Cols_Cent = HTuple(cols_cent, len);

	//hv_PrcM_Min = HTuple();
	//hv_PrcM_Max = HTuple();
	//hv_PrcM_Median = HTuple();
	//hv_Def_Rows = HTuple();
	//hv_Def_Cols = HTuple();

	HTuple hv_PrcM_Sel, hv_Min, hv_Max, hv_Median;

	int cv_SPos = -1;
	int cv_Count = 0;
	HTuple hv_Count;
	TupleLength(hv_PrcM_Min, &hv_Count);
	cv_Count = hv_Count;

	HTuple hv_PrcM = hv_Prc * -1;
	{
		for (int cv_I = 0; cv_I < len; cv_I++)
		{
			if (hv_PrcM[cv_I] > cv_Proc_Thr)
			{
				if (cv_SPos == -1)
				{
					cv_SPos = cv_I;
				}
			}
			else if (0 != (cv_SPos >= 0))
			{
				TupleSelectRange(hv_PrcM, cv_SPos, cv_I, &hv_PrcM_Sel);
				TupleMin(hv_PrcM_Sel, &hv_Min);
				TupleMax(hv_PrcM_Sel, &hv_Max);
				TupleMedian(hv_PrcM_Sel, &hv_Median);
				hv_PrcM_Min[cv_Count] = hv_Min;
				hv_PrcM_Max[cv_Count] = hv_Max;
				hv_PrcM_Median[cv_Count] = hv_Median;

				cv_SPos += (cv_I - cv_SPos) / 2;
				hv_Def_Rows[cv_Count] = hv_Rows_Cent[cv_SPos];
				hv_Def_Cols[cv_Count] = hv_Cols_Cent[cv_SPos];

				cv_Count += 1;
				cv_SPos = -1;
			}
		}
	}



}

void CSlice::Proc_P(UINT8 *pIm, int im_w, int im_h, HTuple *hv_Def_Rows, HTuple *hv_Def_Cols, 
	HTuple &hv_PrcM_Min, HTuple &hv_PrcM_Max, HTuple &hv_PrcM_Median)
{
	//// *** Параметры инспекции----------
	//int m_Avg_Par = 30;
	//int m_szw_P = 50;
	//int m_prsLG_P = 20;
	// *** ------------------------------
	hv_PrcM_Min = HTuple();
	hv_PrcM_Max = HTuple();
	hv_PrcM_Median = HTuple();
	*hv_Def_Rows = HTuple();
	*hv_Def_Cols = HTuple();


	int Point_S, Point_E;
	int sens_len;
	int Pos_Skel_E;
	int Pos_Skel_S = 0;
	int *c_Rows, *c_Cols;
	int c_Bott;

	for (int I_Skel = 0; I_Skel < m_Count_Skel; I_Skel++)
	{
		Pos_Skel_E = teach->m_Pointer_Skel_P[I_Skel];

		int Depth_Pos = 0;
		int cent_pos;

		int Point;
		for (Point = Pos_Skel_S; Point <= Pos_Skel_E; Point++)
		{
			// Начало и конец сенсора
			Point_S = teach->m_Pointer_Sens_P[Point];
			Point_E = teach->m_Pointer_Sens_P[Point + 1] - 1;
			sens_len = Point_E - Point_S + 1;
			// Указатели на начало сенсора
			c_Rows = &teach->m_Rows_Sens_P[Point_S];
			c_Cols = &teach->m_Cols_Sens_P[Point_S];
			// Получить срез
			Get_Gray_Val(pIm, im_w, im_h, c_Rows, c_Cols, sens_len, m_Gray);

			
			inspect_utils::Moving_Avg(m_Gray, sens_len, m_Avg_Par, m_Gray_Avg);

			static int test_point = -1;
			if (Point == test_point)
			{
				HTuple hv_m_Gray;
				HTuple hv_m_Gray_Avg;
				for (int t = 0; t < sens_len; t++)
				{
					hv_m_Gray[t] = m_Gray[t];
					hv_m_Gray_Avg[t] = m_Gray_Avg[t];

				}
				WriteTuple(hv_m_Gray, "C:\\Temp\\hv_m_Gray.tup");
				WriteTuple(hv_m_Gray_Avg, "C:\\Temp\\hv_m_Gray_Avg.tup");
			}


			//UINT8 gray_MV_max;
			//int max_pos;
			//inspect_utils::Max_Buff(m_Gray_Avg, sens_len, gray_MV_max, max_pos);
			//m_Depth_S[Depth_Pos] = gray_MV_max;

			int cv_Point_Val = (sens_len / 2);
			int hv_Devs = (sens_len / 4);

			UINT8 *cv_Depth = std::max_element(m_Gray_Avg + (cv_Point_Val - hv_Devs), m_Gray_Avg + (cv_Point_Val + hv_Devs));

			m_Depth_S[Depth_Pos] = *cv_Depth;
			//m_Depth_S[Depth_Pos] = m_Gray_Avg[cv_Point_Val];

			cent_pos = sens_len / 2;
			m_Rows_Cent[Depth_Pos] = c_Rows[cent_pos];
			m_Cols_Cent[Depth_Pos] = c_Cols[cent_pos];

			Depth_Pos += 1;
		}

		// Добавлении заднего и переднего фронта. Для определения граничных дефектов
		//m_Depth_S[0] = 0;
		//m_Depth_S[Depth_Pos] = 0;
		//m_Rows_Cent[0] = c_Rows[cent_pos];
		//m_Cols_Cent[0] = c_Cols[cent_pos];
		//m_Rows_Cent[Depth_Pos] = c_Rows[cent_pos];
		//m_Cols_Cent[Depth_Pos] = c_Cols[cent_pos];

		inspect_utils::SrunmedSLIExtNoS(m_Depth_S, m_Depth_Ppr_S + m_size, m_Depth_Ppr_S, Depth_Pos, (m_szw_P + 1) * 2, 0, 10);


		for (int i = 0; i < Depth_Pos; i++)
		{
			float Wch = (float)m_Depth_S[i] - m_Depth_Ppr_S[i];
			float ppr_S = Wch * 100 / (m_Depth_Ppr_S[i] + 0.001);
			m_Depth_Ppr_S[i] = ppr_S;
		}
		//HTuple hv_Depth_S = HTuple(m_Depth_S, Depth_Pos);
		//HTuple hv_Depth_Ppr_S = HTuple(m_Depth_Ppr_S, Depth_Pos);

		Defect_Post(
			m_Depth_Ppr_S, m_Rows_Cent, m_Cols_Cent, Depth_Pos,
			m_prsLG_P,
			*hv_Def_Rows, *hv_Def_Cols, hv_PrcM_Min, hv_PrcM_Max, hv_PrcM_Median);


		//for (int i = 0; i < Depth_Pos; i++)
		//{
		//	float Wch = m_Depth_S[i] - m_Depth_Ppr_S[i];
		//	float ppr_S = abs(Wch * 100 / (m_Depth_Ppr_S[i] + 0.001));
		//	m_Depth_Ppr_S[i] = ppr_S;

		//	if (ppr_S > m_prsLG_P)
		//	{
		//		TupleConcat(*hv_Def_Rows, m_Rows_Cent[i], hv_Def_Rows);
		//		TupleConcat(*hv_Def_Cols, m_Cols_Cent[i], hv_Def_Cols);
		//		TupleConcat(*hv_Prc, abs(ppr_S), hv_Prc);
		//	}
		//}

		int test = -1;
		if (test > 0)
		{
			HTuple hv_m_Gray(m_Depth_Ppr_S, Depth_Pos);
			WriteTuple(hv_m_Gray, "C:\\Temp\\m_Depth_Ppr_S");
		}


		Pos_Skel_S = Pos_Skel_E + 1;
	}


}

bool CSlice_Teach::Read_Teach(const char* path_model)
{
	CString path_teach = CString(path_model) + "Teach\\";
	CString path_bin;
	path_bin.Format("%s\\Tuples\\Bin\\", path_teach);

	if (inspect_utils::Check_Path(path_bin + "Cols_Sens.dat"))
	{
		// --- for Slice
		if (inspect_utils::Check_Path(path_bin + "Cols_Sens.dat"))
		{
			int iNoCols_Sens = inspect_utils::Read_Bin_File(path_bin + "Cols_Sens.dat", &m_Cols_Sens);
			int iNoPointer_Sens = inspect_utils::Read_Bin_File(path_bin + "Pointer_Sens.dat", &m_Pointer_Sens);
			int iNoRows_Sens = inspect_utils::Read_Bin_File(path_bin + "Rows_Sens.dat", &m_Rows_Sens);
			int iNoPointer_Skel = inspect_utils::Read_Bin_File(path_bin + "Pointer_Skel.dat", &m_Pointer_Skel);
			m_iNoSkel = iNoPointer_Skel;
			m_Readed = true;
		}
		else
		{
			m_iNoSkel = 0;
			m_Readed = false;
		}
		if (inspect_utils::Check_Path(path_bin + "Cols_Sens_P.dat"))
		{
			inspect_utils::Read_Bin_File(path_bin + "Cols_Sens_P.dat", &m_Cols_Sens_P);
			inspect_utils::Read_Bin_File(path_bin + "Pointer_Sens_P.dat", &m_Pointer_Sens_P);
			inspect_utils::Read_Bin_File(path_bin + "Rows_Sens_P.dat", &m_Rows_Sens_P);
			int iNoPointer_Skel_P = inspect_utils::Read_Bin_File(path_bin + "Pointer_Skel_P.dat", &m_Pointer_Skel_P);
			m_iNoSkel_P = iNoPointer_Skel_P;
			m_Readed_P = true;
		}
		else
		{
			m_iNoSkel_P = 0;
			m_Readed_P = false;
		}
	}
	return m_Readed && m_Readed_P;
}


/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

