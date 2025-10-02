/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
		Common.cpp
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 2/28/2024 11:49:48 AM
 */

 /* ************************************************************************** */
 /* Section: Included Files                                                    */
 /* ************************************************************************** */
 //#include "stdafx.h"
#include "Inspect_Common.h"
//namespace fs = std::filesystem;

namespace inspect_utils
{
	/* ************************************************************************** */
	/* Section: File Scope or Global Data                                         */
	/* ************************************************************************** */
	FILE* fp_console;

	/* ************************************************************************** */
	/* Section: Function				                                          */
	/* ************************************************************************** */
	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Adding a Formatted String to the Logging Queue
	 */
	void Init_Console()
	{
		if (!AllocConsole())
		{
			AfxMessageBox(_T("Failed AllocConsole()"));
		}
		freopen_s(&fp_console, "CONOUT$", "w", stdout);
		freopen_s(&fp_console, "CONIN$", "r", stdin);
		freopen_s(&fp_console, "CONOUT$", "w", stderr);
		std::cout << "Starting console .... Ok" << std::endl;
	}

	bool SaveTupleToBinInt(CString path_teach, CString name, CString save_name)
	{
		CFileFind finder;
		CString file_name_tuple = path_teach + "Tuples\\" + name + ".tup";
		if (finder.FindFile((LPCTSTR)file_name_tuple) == TRUE)
		{
			HTuple hv_Tuple;
			ReadTuple(HTuple(file_name_tuple), &hv_Tuple);

			int* data = ConvertTupleToInt(hv_Tuple);

			HTuple tl;
			TupleLength(hv_Tuple, &tl);
			int len = (int)tl.L();

			FILE* file;
			CString file_name_bin = path_teach + "Tuples\\Bin\\" + save_name + ".dat";
			errno_t err = fopen_s(&file, file_name_bin, "wb");
			if (err != 0)
			{
				printf("***Fail File Open*** Convert_To_Bin - %s", file_name_tuple.GetBuffer());
				return false;
			}
			fwrite(data, sizeof(int), len, file);
			fclose(file);

			delete data;

			return true;
		}
		else
		{
			printf("***File not found*** Convert_To_Bin - %s", file_name_tuple.GetBuffer());
			return false;
		}
	}
	bool SaveTupleToBinFloat(CString path_teach, CString name, CString save_name)
	{
		CFileFind finder;
		CString file_name_tuple = path_teach + "Tuples\\" + name + ".tup";
		if (finder.FindFile((LPCTSTR)file_name_tuple) == TRUE)
		{
			HTuple hv_Tuple;
			ReadTuple(HTuple(file_name_tuple), &hv_Tuple);
			float* data = ConvertTupleToFloat(hv_Tuple);

			HTuple tl;
			TupleLength(hv_Tuple, &tl);
			int len = (int)tl.L();

			FILE* file;
			CString file_name_bin = path_teach + "Tuples\\Bin\\" + save_name + ".dat";
			errno_t err = fopen_s(&file, file_name_bin, "wb");
			if (err != 0)
			{
				printf("***Fail File Open*** Convert_To_Bin - %s", file_name_tuple.GetBuffer());
				return false;
			}
			fwrite(data, sizeof(float), len, file);
			fclose(file);

			delete data;
			return true;
		}
		else
		{
			printf("***File not found*** Convert_To_Bin - %s", file_name_tuple.GetBuffer());
			return false;
		}
	}

	int* ConvertTupleToInt(HTuple T)
	{
		int id;
		Hlong *hlptr;
		//float f;
		HTuple tl;
		int *ip;

		TupleLength(T, &tl);
		int itl = (int)tl.L();

		hlptr = T.ToLArr();
		ip = (int *)malloc(itl * sizeof(int));
		for (int i = 0; i < itl; i++)
		{
			id = (int)*(hlptr + i);
			*(ip + i) = id;
		}
		return ip;
	}

	double Tuple2Double(HTuple hv_Any)
	{
		HTuple hv_Double;
		TupleReal(hv_Any, &hv_Double);
		double ret = hv_Double.DArr()[0];
		return ret;
	}

	float* ConvertTupleToFloat(HTuple T)
	{
		double *dptr, d;
		HTuple tl;
		float *dp;

		TupleLength(T, &tl);
		int itl = (int)tl.L();

		dptr = T.ToDArr();
		dp = (float *)malloc(itl * sizeof(float));
		for (int i = 0; i < itl; i++)
		{
			d = *(dptr + i);
			*(dp + i) = (float)d;
		}
		return dp;
	}

	int Read_Bin_File(CString file_Name, float **buff)
	{
		FILE* file;
		clock_t start, end;
		start = clock();
		while (1)
		{
			auto err = fopen_s(&file, file_Name, "rb");
			end = clock();
			if (end - start > 15000)
			{
				CString str;
				str.Format("file loading error <%s>", file_Name);
				AfxMessageBox(str, MB_ICONERROR | MB_SYSTEMMODAL);
				fclose(file);
				return false;
			}
			fclose(file);
			if (file == NULL)
				continue;
			else
				break;
		}
		fopen_s(&file, file_Name, "rb");
		fseek(file, 0, SEEK_END);
		int size = ftell(file) / sizeof(float);
		if (*buff != 0) free(*buff);
		*buff = (float*)malloc(size * 1 * sizeof(float));
		fseek(file, 0, SEEK_SET);
		fread(*buff, sizeof(int), size, file);
		fclose(file);
		return size;
	}
	int Read_Bin_File(CString file_Name, int **buff)
	{
		FILE* file;
		clock_t start, end;
		start = clock();
		while (1)
		{
			auto err = fopen_s(&file, file_Name, "rb");
			end = clock();
			if (end - start > 15000)
			{
				CString str;
				str.Format("file loading error <%s>", file_Name);
				AfxMessageBox(str, MB_ICONERROR | MB_SYSTEMMODAL);
				fclose(file);
				return false;
			}
			if (file == NULL)
				continue;
			else
				break;
		}
		fseek(file, 0, SEEK_END);
		int size = ftell(file) / sizeof(int);
		if (*buff != 0) free(*buff);
		*buff = (int*)malloc(size * 1 * sizeof(int));
		fseek(file, 0, SEEK_SET);
		fread(*buff, sizeof(int), size, file);
		fclose(file);
		return size;
	}

	INT64 * ConvertFloatToINT64(float *dp, int len)
	{
		INT64 *dptr;
		int d;
		//float f;
		HTuple tl;
		//INT64 *dp;

		//TupleLength(T, &tl);
		int itl = len;

		//dptr = tl.ToDArr();
		//iptr = hv_tst.ToIArr();
		dptr = (INT64 *)malloc(itl * sizeof(INT64));
		for (int i = 0; i < itl; i++)
		{
			d = (int)*(dp + i);
			*(dptr + i) = d;
		}
		return dptr;
	}

	void FloatToTuple(float *dp, int len, HTuple &tuple)
	{
		tuple = HTuple(dp, len);
		TupleInt(tuple, &tuple);

		//INT64* ptr = ConvertFloatToINT64(dp, len);
		//tuple = HTuple(ptr, len);
	}
	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Adding a Formatted String to the Logging Queue
	 */
	void Queue_Add(TSQueueSM<CString> *queue, CString *out_str, LPCSTR pszFormat, ...)
	{
		if (queue->Size() > 100000)
			return;

		ATLASSERT(AtlIsValidString(pszFormat));

		va_list argList;
		va_start(argList, pszFormat);

		CString str;
		str.FormatV(pszFormat, argList);
		queue->Push(str);

		va_end(argList);

		//THEAPP.m_pDebugDlg->AddText("%s\n", str);
		//CONSOLE(printf("%s\n", str));

		if (out_str != NULL)
		{
			*out_str = str;
		}
	}

	/** -----------------------------------------------------------------------
	 * @Summary
	 *      Save a string to the registry
	 */
	 //SetRegString("Batch_Inspection_Path", "C:123-123");
	 //CString bath_path, bath_path1;
	 //GetRegString("Batch_Inspection_Path", &bath_path1);
	bool SetRegBool(CString value_name, bool value)
	{
		if (value)
		{
			SetRegString(value_name, "TRUE");
		}
		else
		{
			SetRegString(value_name, "FALSE");
		}
		return true;
	}

	bool SetRegString(CString value_name, CString value)
	{
		CString sKeyName = "Software\\Synapse\\Inspection";
		CRegKey regKey;
		LONG result = regKey.Create(HKEY_CURRENT_USER, sKeyName);
		if (result != ERROR_SUCCESS)
		{
			return false;
		}

		result = regKey.SetStringValue(value_name, value);
		if (result != ERROR_SUCCESS)
		{
			return false;
		}

		return true;
	}
	/** -----------------------------------------------------------------------
	 * @Summary
	 *      Read a string from the registry
	 */
	bool GetRegBool(CString value_name, bool *value)
	{
		CString str_value;
		bool ret = GetRegString(value_name, &str_value);
		if (ret && str_value == "TRUE")
		{
			*value = true;
			return true;
		}
		else
		{
			*value = false;
			return false;
		}
	}
	bool GetRegString(CString value_name, CString *value, CString default_value)
	{
		bool ret = GetRegString(value_name, value);
		if (!ret)
		{
			*value = default_value;
		}

		return ret;
	}
	bool GetRegString(CString value_name, CString *value)
	{
		CString sKeyName = "Software\\Synapse\\Inspection";

		CRegKey regKey;
		//LONG result = regKey.Open(HKEY_CURRENT_USER, sKeyName, KEY_ALL_ACCESS);
		LONG result = regKey.Create(HKEY_CURRENT_USER, sKeyName);
		if (result != ERROR_SUCCESS)
		{
			return false;
		}

		TCHAR bufPath[MAX_PATH];
		ULONG len = sizeof(bufPath);
		result = regKey.QueryStringValue(value_name, bufPath, &len);
		if (result != ERROR_SUCCESS)
		{
			return false;
		}

		CString param(bufPath, len);
		*value = param;

		return true;
	}

	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Counting the number of files
	 */
	int Folder_Count_Files(CString sImageFileName)
	{
		HANDLE hFind;
		WIN32_FIND_DATA	ffd;
		int count_files = 0;

		hFind = FindFirstFile(sImageFileName, &ffd);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			return count_files;
		}

		do
		{
			count_files += 1;
		} while (FindNextFile(hFind, &ffd) == TRUE);
		FindClose(hFind);

		return count_files;
	}

	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Get file name
	 */
	bool Check_Path(CString files_path)
	{
		HANDLE hFind;
		WIN32_FIND_DATA	ffd;
		hFind = FindFirstFile(files_path, &ffd);

		return (hFind != INVALID_HANDLE_VALUE);
	}

	void Delete_All_Folders(CString folder)
	{
		//fs::remove_all((LPCTSTR)folder);
	}

	void ConfirmFolder(CString folder)
	{
		//bool exits = fs::exists((LPCTSTR)folder);
		//if (!exits)
		{
			//fs::create_directories((LPCTSTR)folder);
		}
	}


	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Get file name
	 */
	CString Get_Path_Image(int file_num, CString files_path)
	{
		CString sImageFileName;
		HANDLE hFind;
		WIN32_FIND_DATA	ffd;

		sImageFileName.Format("%s\\RawImage_%d.jpg", files_path, file_num + 1);
		hFind = FindFirstFile(sImageFileName, &ffd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			return sImageFileName;
		}
		sImageFileName.Format("%s\\RawImage_%d.bmp", files_path, file_num + 1);
		hFind = FindFirstFile(sImageFileName, &ffd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			return sImageFileName;
		}
		return "";
	}
	void GetAllFilesNames(CString files_path, CStringArray& files)
	{
		files.RemoveAll();

		CFileFind finder;
		BOOL bWorking = finder.FindFile(files_path + "\\*");
		while (bWorking)
		{
			bWorking = finder.FindNextFile();

			// skip . and .. files
			if (!finder.IsDots())
			{
				files.Add(finder.GetFileName());
			}
		}
	}
	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Inspect_Write_Log
	 */
	CString m_log_file_name;
	void Inspect_Init_Log(CString result_path, int pc_type)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		CreateDirectory(result_path, NULL);

		m_log_file_name.Format("%s\\Inspect_%04d%02d%02d_%02d%02d%02d-%d.txt", result_path, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, pc_type);
		HANDLE hLogFile = CreateFile(m_log_file_name, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(hLogFile);
	}
	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Inspect_Write_Log
	 */
	void Inspect_Write_Log(CString log)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		HANDLE hLogFile;
		hLogFile = CreateFile(m_log_file_name, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		SetFilePointer(hLogFile, NULL, NULL, FILE_END);

		CString log_line;
		log_line.Format("[%02d:%02d:%02d-%03d]: %s\n", sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds, log);
		WriteFile(hLogFile, log_line, log_line.GetLength(), NULL, NULL);
		CloseHandle(hLogFile);
	}

	void Max_Index(HTuple hv_Max21, HTuple hv_FAbsoluteHistosm1, HTuple *hv_Max_Index)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_Max_Pos, hv_Max_Val, hv_Length, hv_Index;
		HTuple  hv_Extr_Pos, hv_Extr_Val;

		hv_Max_Pos = 0;
		hv_Max_Val = 0;
		(*hv_Max_Index) = 0;
		TupleLength(hv_Max21, &hv_Length);
		{
			HTuple end_val4 = hv_Length - 1;
			HTuple step_val4 = 1;
			for (hv_Index = 0; hv_Index.Continue(end_val4, step_val4); hv_Index += step_val4)
			{

				hv_Extr_Pos = HTuple(hv_Max21[hv_Index]);
				TupleInt(hv_Extr_Pos, &hv_Extr_Pos);
				hv_Extr_Val = HTuple(hv_FAbsoluteHistosm1[hv_Extr_Pos]);
				if (0 != (hv_Extr_Val > hv_Max_Val))
				{
					hv_Max_Val = hv_Extr_Val;
					hv_Max_Pos = hv_Extr_Pos;
					(*hv_Max_Index) = hv_Index;
				}
			}
		}
		return;
	}

	// Prototype
	void SrunmedProto(float* y, float* smo, long n, int bw_tot, int skip) // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
	{
		/* Local variables */
		double rmed, rmin, temp, rnew, yout, yi;
		double rbe, rtb, rse, yin, rts, fa, fb;
		int imin, ismo, i, j, first, last, band2, kminus, kplus, shift, bw, step, num_meds, last_med, a, b, x, rad;

		step = skip + 1;
		bw = (((bw_tot - 1) / 2) / step) * 2 + 1;

		rad = ((bw - 1) / 2)*step;
		//std::cout << "new: bw=" << bw << ", radius=" << bw / 2 << ", step=" << step << "\n";

		if (n < bw_tot)
			return;
		//double *scrat = (double *)malloc(bw, sizeof(double));
		//was
		double *scrat = (double *)malloc((unsigned)bw * sizeof(double));

		//if ((bw-1)*step+1 > n)
		//	error(_("bandwidth/span of running medians is larger than n"));

		/* 1. Compute  'rmed' := Median of the first 'band' values
		   ======================================================== */

		shift = 0;

		for (i = 0; i < bw; ++i)
		{
			scrat[i] = y[i*step + shift];
		}

		/* find minimal value  rmin = scrat[imin] <= scrat[j] */
		rmin = scrat[0]; imin = 0;
		for (i = 1; i < bw; ++i)
			if (scrat[i] < rmin) {
				rmin = scrat[i]; imin = i;
			}
		/* swap scrat[0] <-> scrat[imin] */
		temp = scrat[0]; scrat[0] = rmin; scrat[imin] = temp;
		/* sort the rest of of scrat[] by bubble (?) sort -- */
		for (i = 2; i < bw; ++i) {
			if (scrat[i] < scrat[i - 1]) {/* find the proper place for scrat[i] */
				temp = scrat[i];
				j = i;
				do {
					scrat[j] = scrat[j - 1];
					--j;
				} while (scrat[j - 1] > temp); /* now:  scrat[j-1] <= temp */
				scrat[j] = temp;
			}
		}
		band2 = (bw - 1) / 2;
		rmed = scrat[band2];/* == Median( y[(1:band2)-1] ) */
		/* "malloc" had  free( (char*) scrat);*/ /*-- release scratch memory --*/

		/*
		if (shift == 0) {
			if (end_rule == 0) { //-- keep DATA at end values
				for (i = 0; i < band2*step; ++i)
					smo[i] = y[i];
			}
			else { // if(end_rule == 1)  copy median to CONSTANT end values
				for (i = 0; i < band2*step; ++i)
					smo[i] = rmed;
			}
		}
		*/

		smo[/*band2*step +*/ shift] = (float)rmed;
		num_meds = 1;
		band2++; /* = bw / 2 + 1*/;


		/* Big	FOR Loop: RUNNING median, update the median 'rmed'
		   ======================================================= */
		for (first = step + shift, last = bw * step + shift, ismo = band2 * step + shift;
			last < n;
			first += step, last += step, ismo += step) {

			yin = y[last];
			yout = y[first - step];
			rnew = rmed; /* New median = old one   in all the simple cases --*/

			if (yin < rmed) {
				if (yout >= rmed) {
					kminus = 0;
					if (yout > rmed) {/*	--- yin < rmed < yout --- */
						//if (print_level >= 2) REprintf(": yin < rmed < yout ");
						rnew = yin;/* was -rinf */
						for (i = first; i <= last; i += step) {
							yi = y[i];
							if (yi < rmed) {
								++kminus;
								if (yi > rnew)	rnew = yi;
							}
						}
						if (kminus < band2)		rnew = rmed;
					}
					else {/*		--- yin < rmed = yout --- */
						rse = rts = yin;/* was -rinf */
						for (i = first; i <= last; i += step) {
							yi = y[i];
							if (yi <= rmed) {
								if (yi < rmed) {
									++kminus;
									if (yi > rts)	rts = yi;
									if (yi > rse)	rse = yi;
								}
								else		rse = yi;

							}
						}
						rnew = (kminus == band2) ? rts : rse;
					}
				} /* else: both  yin, yout < rmed -- nothing to do .... */
			}
			else if (yin != rmed) { /* yin > rmed */
				if (yout <= rmed) {
					kplus = 0;
					if (yout < rmed) {/* -- yout < rmed < yin --- */
						rnew = yin; /* was rinf */
						for (i = first; i <= last; i += step) {
							yi = y[i];
							if (yi > rmed) {
								++kplus;
								if (yi < rnew)	rnew = yi;
							}
						}
						if (kplus < band2)	rnew = rmed;
					}
					else { /* -- yout = rmed < yin --- */
						rbe = rtb = yin; /* was rinf */
						for (i = first; i <= last; i += step) {
							yi = y[i];
							if (yi >= rmed) {
								if (yi > rmed) {
									++kplus;
									if (yi < rtb)	rtb = yi;
									if (yi < rbe)	rbe = yi;
								}
								else		rbe = yi;
							}
						}
						rnew = (kplus == band2) ? rtb : rbe;
					}
				} /* else: both  yin, yout > rmed --> nothing to do */
			} /* else: yin == rmed -- nothing to do .... */
			rmed = rnew;
			smo[ismo - rad] = (float)rmed;
			num_meds++;
		} /*     END FOR ------------ big Loop -------------------- */

		free(scrat);

		band2 = (bw - 1) / 2;
		last_med = band2 * step + (num_meds - 1)*step;


		/*if (end_rule == 0) { //-- keep DATA at end values
			for (i = last_med + 1; i < n; ++i)
				smo[i] = y[i];
		}
		else { // if(end_rule == 1)  copy median to CONSTANT end values
			for (i = last_med + 1; i < n; ++i)
				smo[i] = smo[last_med];
		}*/

		if (skip > 0) {
			for (i = 0; i < num_meds - 1; i++) {
				for (j = 1; j < step; j++) {
					x = (band2 + i)*step + j;
					a = (band2 + i)*step;
					b = (band2 + i + 1)*step;
					fa = smo[a - rad];
					fb = smo[b - rad];
					smo[x - rad] = (float)(fa + j * (fb - fa) / step); //b-a=step, x-a=j
				}
			}
		}
	}
	void SrunmedSLIExtNoS(float* yinp, float* y/*extended*/, float* smo, long n1, int bw_tot, /*int end_rule,*/ int skip, int noskipsz) // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
	{
		/*
			 *  Computes "Running Median" smoother ("Stuetzle" algorithm) with medians of 'band'

			 *  Input:
			 *	yinp(n1)	- responses in order of increasing predictor values
			 *	y(n)		- auxillary array; n = n1 + bw_tot - 1
			 *	n1		- number of observations
			 *	bw_tot		- span of running medians (MUST be ODD !!)
			 //*	end_rule 		-- 0: Keep original data at ends {j; j < b2 | j > n-b2}
			 *		     		-- 1: Constant ends = median(y[1],..,y[bw]) "robust"
			 *	skip		- how many data points to skip between two used points when doing interpolation
			 *	noskipsz	- length of the regions at the beginning and at the end where interpolation is not used; must be no greater than n1/2 and no lesser than bw_tot/2
			 *  Output:
			 *	smo(n)		- smoothed responses

			 //* NOTE:  The 'end' values are just copied !! this is fast but not too nice !
		*/

		/* Local variables */
		int i, rad, rad2n, step, dif;

		rad = (bw_tot - 1) / 2;

		step = skip + 1;
		rad2n = (((bw_tot - 1) / 2) / step) * step;

		dif = n1 - 2 * noskipsz - 1 - ((n1 - 2 * noskipsz - 1) / step)*step;


		for (i = 0; i < rad; i++)
			y[i] = yinp[0];
		//memcpy(y + bw_tot, yinp, n1 * sizeof(float));
		for (i = 0; i < n1; i++)
			y[rad + i] = yinp[i];
		for (i = 0; i < rad; i++)
			y[rad + n1 + i] = yinp[n1 - 1];


		SrunmedProto(y, smo /*- ((bw_tot - 1) / 2)*/, noskipsz + bw_tot - 1, bw_tot, 0);

		SrunmedProto(y + (n1 - noskipsz) - dif, smo + (n1 - noskipsz /*- (bw_tot - 1) / 2*/) - dif, noskipsz + bw_tot - 1 + dif, bw_tot, 0);

		SrunmedProto(y + noskipsz + rad - rad2n, smo + (noskipsz /*- (bw_tot - 1) / 2*/), n1 - 2 * noskipsz + 2 * rad2n, bw_tot, skip);

	} /* SrunmedSLIExt */

	void Max_Buff(UINT8 *buff, int size, UINT8 &max, int &max_pos)
	{
		max = 0;
		max_pos = 0;
		for (int k = 0; k < size; k++)
		{
			if (buff[k] > max)
			{
				max = buff[k];
				max_pos = k;
			}
		}
	}

	void Moving_Avg(UINT8 *buff, int size, int period, UINT8 *out_buff)
	{
		// *Скользящее среднее
		float sum_mov = buff[0] * period;
		int period_D2 = period / 2;
		for (int k = 0; k < size + period_D2; k++)
		{
			if (k >= period / 2)
			{
				out_buff[k - period_D2] = sum_mov / period;
			}

			if (k >= period)
			{
				sum_mov = sum_mov - buff[k - period];
			}
			else
			{
				sum_mov = sum_mov - buff[0];
			}

			if (k < size)
			{
				sum_mov = sum_mov + buff[k];
			}
			else
			{
				sum_mov = sum_mov + buff[size - 1];
			}
		}
	}

	// Short Description: Display the results of Shape-Based Matching. 
	void dev_display_shape_matching_results(HTuple hv_ModelID, HTuple hv_Color, HTuple hv_Row,
		HTuple hv_Column, HTuple hv_Angle, HTuple hv_ScaleR, HTuple hv_ScaleC, HTuple hv_Model)
	{
		return;
	}
}


/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

