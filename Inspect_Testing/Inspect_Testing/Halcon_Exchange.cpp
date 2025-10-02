/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
		Halcon_Exchange.cpp
 *
 * @Summary
 *
 * @Description
 *		
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 8/29/2024 8:53:40 AM
 */

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include "Common.h"
#include "Inspect_Common.h"
#include "Inspect_Call.h"

#include "Halcon_Exchange.h"

#include "Our_Algorithms.h"

namespace fs = std::filesystem;


namespace Halcon_Exchange
{

	/* ************************************************************************** */
	/* Section: File Scope or Global Data                                         */
	/* ************************************************************************** */
	HANDLE hThread_Exchange;
	DWORD dwThreadId_Exchange;
	CString m_Data_Exchange_Path;
	/* ************************************************************************** */
	/* Section: Function				                                          */
	/* ************************************************************************** */
	/** ---------------------------------------------------------------------------
	* @Summary
	*		Процесс ожидания команды
	*/
	void Region_Threshold_SubPix_Thread()
	{
		CString file_name_param = "Region_Threshold_SubPix.ser";

		// Читаем параметры
		HTuple hv_FileHandleIn, hv_SerializedItemHandle, hv_Func_Pars;
		OpenFile((HTuple)m_Data_Exchange_Path + file_name_param, "input_binary", &hv_FileHandleIn);
		FreadSerializedItem(hv_FileHandleIn, &hv_SerializedItemHandle);
		DeserializeTuple(hv_SerializedItemHandle, &hv_Func_Pars);
		CloseFile(hv_FileHandleIn);
		HTuple thr = hv_Func_Pars[0];
		HTuple hv_Params2 = hv_Func_Pars[1];

		// Читаем необходимые объекты и туплы
		HObject ho_Im, ho_RegSel;
		ReadImage(&ho_Im, (HTuple)m_Data_Exchange_Path + "Im");
		ReadObject(&ho_RegSel, (HTuple)m_Data_Exchange_Path + "RegSel");


		// Выполняем функцию
		double *rows_sub = (double*)malloc(POINTS_SUB_LEN * sizeof(double));
		double *cols_sub = (double*)malloc(POINTS_SUB_LEN * sizeof(double));
		int len = Region_Threshold_SubPix(ho_Im, ho_RegSel, thr, rows_sub, cols_sub);


		// Сохраняем файлы
		HTuple hv_Rows_Sub = HTuple(rows_sub, len);
		HTuple hv_Cols_Sub = HTuple(cols_sub, len);
		HObject  ho_Reg_Sub;
		GenContourPolygonXld(&ho_Reg_Sub, hv_Rows_Sub, hv_Cols_Sub);
		WriteObject(ho_Reg_Sub, (HTuple)m_Data_Exchange_Path + "ho_Reg_Sub");

		fs::path path1 = fs::path((LPCTSTR)(m_Data_Exchange_Path + file_name_param));
		fs::path path2 = fs::path((LPCTSTR)(m_Data_Exchange_Path + file_name_param + ".old"));
		fs::remove_all(path2);
		fs::rename(path1, path2);
	}

	/** ---------------------------------------------------------------------------
	* @Summary
	*		Процесс ожидания команды
	*/
	DWORD WINAPI Thread_Exchange(LPVOID lpParam)
	{
		bool check_file;

		while (true)
		{
			// --- Region_Threshold_SubPix ------------------------------------------ 
			check_file = inspect_utils::Check_Path(m_Data_Exchange_Path + "Region_Threshold_SubPix.ser");
			if (check_file)
			{
				Region_Threshold_SubPix_Thread();
			}







			Sleep(10);
		}
		return 0;
	}



	/** ---------------------------------------------------------------------------
	* @Summary
	*		Инициализация. Запуск процесса ожидания команды
	*/
	void Init_Exchange()
	{
		m_Data_Exchange_Path = "C:/Temp/Data_Exchange/";
		inspect_utils::ConfirmFolder(m_Data_Exchange_Path);

		hThread_Exchange = CreateThread(
			NULL,									// default security attributes
			0,										// use default stack size  
			&Thread_Exchange,						// thread function name
			NULL,									// argument to thread function 
			0,										// use default creation flags 
			&dwThreadId_Exchange);					// returns the thread identifier 

	}

}

/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

