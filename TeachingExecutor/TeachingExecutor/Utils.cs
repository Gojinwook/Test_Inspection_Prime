using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;
using Microsoft.Win32;
using HalconDotNet;

using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Reflection;

namespace TeachingExecutor
{
    public static class Utils
    {
        public static readonly String ProjectName = "TeachingExecutor";

        public static String Reg_Get_Values(String name, String val_default)
        {
            RegistryKey key = Registry.CurrentUser;
            RegistryKey project_key = key.CreateSubKey("Software\\" + ProjectName);
            String ret = project_key.GetValue(name, val_default).ToString();
            project_key.Close();
            return ret;
        }
        public static bool Reg_Get_Values(String name, bool val_default)
        {
            RegistryKey key = Registry.CurrentUser;
            RegistryKey project_key = key.CreateSubKey("Software\\" + ProjectName);
            String ret = project_key.GetValue(name, val_default).ToString();
            project_key.Close();
            return ret.ToUpper() == "TRUE" ? true: false;
        }

        public static bool Reg_Set_Value(String name, String value)
        {
            try
            {
                RegistryKey key = Registry.CurrentUser;
                RegistryKey project_key = key.CreateSubKey("Software\\" + ProjectName);
                project_key.SetValue(name, value);
                project_key.Close();
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }
        public static bool Reg_Set_Value(String name, bool value)
        {
            try
            {
                RegistryKey key = Registry.CurrentUser;
                RegistryKey project_key = key.CreateSubKey("Software\\" + ProjectName);
                project_key.SetValue(name, value);
                project_key.Close();
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }

        public static bool FolderCheck(String folder)
        {
            if (!Directory.Exists(folder))
            {
                Directory.CreateDirectory(folder);
                return false;
            }
            else
            {
                return true;
            }
        }

        public static void DevDisplayShapeMatchingResults(HTuple hv_ModelID,
            HTuple hv_Row, HTuple hv_Column, HTuple hv_Angle, HTuple hv_ScaleR, HTuple hv_ScaleC,
            HTuple hv_Model, out HObject ho_ContoursAffinTrans)
        {
            // Local iconic variables 
            HObject ho_ModelContours = null;
            ho_ContoursAffinTrans = null;

            // Local control variables 
            HTuple hv_NumMatches = null, hv_Index = new HTuple();
            HTuple hv_Match = new HTuple(), hv_HomMat2DIdentity = new HTuple();
            HTuple hv_HomMat2DScale = new HTuple(), hv_HomMat2DRotate = new HTuple();
            HTuple hv_HomMat2DTranslate = new HTuple();
            HTuple hv_Model_COPY_INP_TMP = hv_Model.Clone();
            HTuple hv_ScaleC_COPY_INP_TMP = hv_ScaleC.Clone();
            HTuple hv_ScaleR_COPY_INP_TMP = hv_ScaleR.Clone();

            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_ModelContours);
            //This procedure displays the results of Shape-Based Matching.
            //
            hv_NumMatches = new HTuple(hv_Row.TupleLength());
            if ((int)(new HTuple(hv_NumMatches.TupleGreater(0))) != 0)
            {
                if ((int)(new HTuple((new HTuple(hv_ScaleR_COPY_INP_TMP.TupleLength())).TupleEqual(
                    1))) != 0)
                {
                    HOperatorSet.TupleGenConst(hv_NumMatches, hv_ScaleR_COPY_INP_TMP, out hv_ScaleR_COPY_INP_TMP);
                }
                if ((int)(new HTuple((new HTuple(hv_ScaleC_COPY_INP_TMP.TupleLength())).TupleEqual(
                    1))) != 0)
                {
                    HOperatorSet.TupleGenConst(hv_NumMatches, hv_ScaleC_COPY_INP_TMP, out hv_ScaleC_COPY_INP_TMP);
                }
                if ((int)(new HTuple((new HTuple(hv_Model_COPY_INP_TMP.TupleLength())).TupleEqual(
                    0))) != 0)
                {
                    HOperatorSet.TupleGenConst(hv_NumMatches, 0, out hv_Model_COPY_INP_TMP);
                }
                else if ((int)(new HTuple((new HTuple(hv_Model_COPY_INP_TMP.TupleLength()
                    )).TupleEqual(1))) != 0)
                {
                    HOperatorSet.TupleGenConst(hv_NumMatches, hv_Model_COPY_INP_TMP, out hv_Model_COPY_INP_TMP);
                }
                for (hv_Index = 0; (int)hv_Index <= (int)((new HTuple(hv_ModelID.TupleLength())) - 1); hv_Index = (int)hv_Index + 1)
                {
                    ho_ModelContours.Dispose();
                    HOperatorSet.GetShapeModelContours(out ho_ModelContours, hv_ModelID.TupleSelect(
                        hv_Index), 1);
                    //if (HDevWindowStack.IsOpen())
                    //{
                    //    HOperatorSet.SetColor(HDevWindowStack.GetActive(), hv_Color.TupleSelect(
                    //        hv_Index % (new HTuple(hv_Color.TupleLength()))));
                    //}
                    HTuple end_val18 = hv_NumMatches - 1;
                    HTuple step_val18 = 1;
                    for (hv_Match = 0; hv_Match.Continue(end_val18, step_val18); hv_Match = hv_Match.TupleAdd(step_val18))
                    {
                        if ((int)(new HTuple(hv_Index.TupleEqual(hv_Model_COPY_INP_TMP.TupleSelect(
                            hv_Match)))) != 0)
                        {
                            HOperatorSet.HomMat2dIdentity(out hv_HomMat2DIdentity);
                            HOperatorSet.HomMat2dScale(hv_HomMat2DIdentity, hv_ScaleR_COPY_INP_TMP.TupleSelect(
                                hv_Match), hv_ScaleC_COPY_INP_TMP.TupleSelect(hv_Match), 0, 0, out hv_HomMat2DScale);
                            HOperatorSet.HomMat2dRotate(hv_HomMat2DScale, hv_Angle.TupleSelect(hv_Match),
                                0, 0, out hv_HomMat2DRotate);
                            HOperatorSet.HomMat2dTranslate(hv_HomMat2DRotate, hv_Row.TupleSelect(
                                hv_Match), hv_Column.TupleSelect(hv_Match), out hv_HomMat2DTranslate);

                            HOperatorSet.GenEmptyObj(out ho_ContoursAffinTrans);
                            ho_ContoursAffinTrans.Dispose();
                            HOperatorSet.AffineTransContourXld(ho_ModelContours, out ho_ContoursAffinTrans,
                                hv_HomMat2DTranslate);
                            //if (HDevWindowStack.IsOpen())
                            //{
                            //    HOperatorSet.DispObj(ho_ContoursAffinTrans, HDevWindowStack.GetActive());
                            //}
                        }
                    }
                }
            }
            //ho_ModelContours.Dispose();
            //ho_ContoursAffinTrans.Dispose();

            return;
        }


        public static void Get_FM_Center(
            HObject ho_Reg1, HTuple hv_Idx, 
            HTuple hv_Row01, HTuple hv_Col01, HTuple hv_Row02, HTuple hv_Col02, 
            out HTuple hv_Row_Ret, out HTuple hv_Col_Ret)
        {
            // Local iconic variables 
            HObject ho_ObjSel;

            // Local control variables 
            HTuple hv_Row11 = null, hv_Col11 = null, hv_Row12 = null;
            HTuple hv_Col12 = null, hv_RowDif1 = null, hv_ColDif1 = null;
            HTuple hv_RowDif2 = null, hv_ColDif2 = null, hv_Area = null;
            HTuple hv_Row = new HTuple(), hv_Col = new HTuple(), hv_RowDif = null;
            HTuple hv_ColDif = null;
            // Initialize local and output iconic variables 
            HOperatorSet.GenEmptyObj(out ho_ObjSel);
            hv_Row_Ret = -1;
            hv_Col_Ret = -1;

            ho_ObjSel.Dispose();
            HOperatorSet.SelectObj(ho_Reg1, out ho_ObjSel, hv_Idx);
            HOperatorSet.SmallestRectangle1(ho_ObjSel, out hv_Row11, out hv_Col11, out hv_Row12,
                out hv_Col12);

            HOperatorSet.TupleFabs(hv_Row01 - hv_Row11, out hv_RowDif1);
            HOperatorSet.TupleFabs(hv_Col01 - hv_Col11, out hv_ColDif1);
            HOperatorSet.TupleFabs(hv_Row02 - hv_Row12, out hv_RowDif2);
            HOperatorSet.TupleFabs(hv_Col02 - hv_Col12, out hv_ColDif2);
            if ((int)((new HTuple((new HTuple((new HTuple(hv_RowDif1.TupleGreater(2))).TupleAnd(
                new HTuple(hv_ColDif1.TupleGreater(2))))).TupleAnd(new HTuple(hv_RowDif2.TupleGreater(
                2))))).TupleAnd(new HTuple(hv_ColDif2.TupleGreater(2)))) != 0)
            {
                HOperatorSet.AreaCenter(ho_ObjSel, out hv_Area, out hv_Row, out hv_Col);
                hv_Row_Ret = hv_Row.Clone();
                hv_Col_Ret = hv_Col.Clone();
                ho_ObjSel.Dispose();

                return;
            }

            HOperatorSet.TupleFabs(hv_Row01 - hv_Row11, out hv_RowDif);
            HOperatorSet.TupleFabs(hv_Col01 - hv_Col11, out hv_ColDif);
            if ((int)((new HTuple(hv_RowDif.TupleGreater(2))).TupleAnd(new HTuple(hv_ColDif.TupleGreater(
                2)))) != 0)
            {
                hv_Row_Ret = hv_Row11.Clone();
                hv_Col_Ret = hv_Col11.Clone();
                ho_ObjSel.Dispose();

                return;
            }

            HOperatorSet.TupleFabs(hv_Row02 - hv_Row12, out hv_RowDif);
            HOperatorSet.TupleFabs(hv_Col02 - hv_Col12, out hv_ColDif);
            if ((int)((new HTuple(hv_RowDif.TupleGreater(2))).TupleAnd(new HTuple(hv_ColDif.TupleGreater(
                2)))) != 0)
            {
                hv_Row_Ret = hv_Row12.Clone();
                hv_Col_Ret = hv_Col12.Clone();
                ho_ObjSel.Dispose();

                return;
            }

            HOperatorSet.TupleFabs(hv_Row01 - hv_Row11, out hv_RowDif);
            HOperatorSet.TupleFabs(hv_Col02 - hv_Col12, out hv_ColDif);
            if ((int)((new HTuple(hv_RowDif.TupleGreater(2))).TupleAnd(new HTuple(hv_ColDif.TupleGreater(
                2)))) != 0)
            {
                hv_Row_Ret = hv_Row11.Clone();
                hv_Col_Ret = hv_Col12.Clone();
                ho_ObjSel.Dispose();

                return;
            }

            HOperatorSet.TupleFabs(hv_Row02 - hv_Row12, out hv_RowDif);
            HOperatorSet.TupleFabs(hv_Col01 - hv_Col11, out hv_ColDif);
            if ((int)((new HTuple(hv_RowDif.TupleGreater(2))).TupleAnd(new HTuple(hv_ColDif.TupleGreater(
                2)))) != 0)
            {
                hv_Row_Ret = hv_Row12.Clone();
                hv_Col_Ret = hv_Col11.Clone();
                ho_ObjSel.Dispose();

                return;
            }

            HOperatorSet.AreaCenter(ho_ObjSel, out hv_Area, out hv_Row, out hv_Col);
            hv_Row_Ret = hv_Row.Clone();
            hv_Col_Ret = hv_Col.Clone();

            ho_ObjSel.Dispose();

            return;
        }

        public sealed class CustomizedBinder : SerializationBinder
        {
            public override Type BindToType(string assemblyName, string typeName)
            {
                Type tyType = null;

                string assemblyName1 = Assembly.GetExecutingAssembly().FullName;
                string sShortAssemblyName = assemblyName1.Split(',')[0];

                Assembly[] ayAssemblies = AppDomain.CurrentDomain.GetAssemblies();

                foreach (Assembly ayAssembly in ayAssemblies)
                {
                    string str = ayAssembly.FullName.Split(',')[0];
                    if (sShortAssemblyName == str)
                    {
                        tyType = ayAssembly.GetType("ODB_Creator.Alignment+FiducialMarkerList");
                        //tyType = ayAssembly.GetType(typeName);
                        break;
                    }
                }
                return tyType;

                //Type returntype = null;
                ////string sharedAssemblyName = "AOI2, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null";
                //assemblyName = Assembly.GetExecutingAssembly().FullName;
                ////typeName = typeName.Replace(sharedAssemblyName, assemblyName);
                //typeName = typeName.Replace("AOI2", "ODB_Creator");
                ////returntype = Type.GetType(String.Format("{0}, {1}", typeName, assemblyName));
                //returntype = Type.GetType(String.Format("{0}, {1}", typeName, assemblyName));

                //return returntype;
            }

            public override void BindToName(Type serializedType, out string assemblyName, out string typeName)
            {
                base.BindToName(serializedType, out assemblyName, out typeName);
                assemblyName = "AOI2, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null";
            }
        }

        public static void Mirror_Points(HTuple hv_Row1C, HTuple hv_Col1C, HTuple hv_Row2C, HTuple hv_Col2C, ref HTuple hv_Rows, ref HTuple hv_Cols)
        {
            //// Определим ось зеркалирования для панели
            //HOperatorSet.GenRegionPolygonFilled(out HObject ho_Region, hv_Rows, hv_Cols);
            //HOperatorSet.SmallestRectangle1(ho_Region, out HTuple hv_Row01, out HTuple hv_Col01, out HTuple hv_Row02, out HTuple hv_Col02);

            // Зеркалируем все точки
            HOperatorSet.HomMat2dIdentity(out HTuple hv_HomMat2D);
            HOperatorSet.HomMat2dReflect(hv_HomMat2D, hv_Row1C, hv_Col1C, 0, hv_Col1C, out hv_HomMat2D);
            HOperatorSet.HomMat2dTranslate(hv_HomMat2D, 0, (hv_Col2C - hv_Col1C) + 1, out hv_HomMat2D);
            HOperatorSet.AffineTransPixel(hv_HomMat2D, hv_Rows, hv_Cols, out hv_Rows, out hv_Cols);
        }
        public static void Rotate_Points(int angle, HTuple hv_Row1C, HTuple hv_Col1C, HTuple hv_Row2C, HTuple hv_Col2C, ref HTuple hv_Rows, ref HTuple hv_Cols)
        {
            // Определим центр поворота
            //HOperatorSet.GenRegionPolygonFilled(out HObject ho_Region, hv_Rows, hv_Cols);
            //HOperatorSet.SmallestRectangle1(ho_Region, out HTuple hv_Row01, out HTuple hv_Col01, out HTuple hv_Row02, out HTuple hv_Col02);
            HTuple hv_Rot_Row = hv_Row1C + ((hv_Row2C - hv_Row1C) / 2);
            HTuple hv_Rot_Col = hv_Col1C + ((hv_Col2C - hv_Col1C) / 2);


            // Поворачиваем все точки
            HOperatorSet.TupleRad(angle * -1, out HTuple hv_Rad);
            HOperatorSet.HomMat2dIdentity(out HTuple hv_HomMat2D);
            HOperatorSet.HomMat2dRotate(hv_HomMat2D, hv_Rad, hv_Rot_Row, hv_Rot_Col, out hv_HomMat2D);
            HOperatorSet.AffineTransPixel(hv_HomMat2D, hv_Rows, hv_Cols, out hv_Rows, out hv_Cols);
        }
        public static void Trans_Points(HTuple hv_RowT, HTuple hv_ColT, ref HTuple hv_Rows, ref HTuple hv_Cols)
        {
            // Смещаем все точки
            HOperatorSet.HomMat2dIdentity(out HTuple hv_HomMat2D);
            HOperatorSet.HomMat2dTranslate(hv_HomMat2D, hv_RowT * -1, hv_ColT * -1, out hv_HomMat2D);
            HOperatorSet.AffineTransPixel(hv_HomMat2D, hv_Rows, hv_Cols, out hv_Rows, out hv_Cols);
        }

        public static T DeepCopy<T>(this T source) where T : new()
        {
            if (!typeof(T).IsSerializable)
            {
                // fail
                return source;
            }

            try
            {
                object result = null;
                using (var ms = new MemoryStream())
                {
                    var formatter = new BinaryFormatter();
                    formatter.Serialize(ms, source);
                    ms.Position = 0;
                    result = (T)formatter.Deserialize(ms);
                    ms.Close();
                }

                return (T)result;
            }
            catch (Exception)
            {
                // fail
                return new T();
            }
        }

        public static void CopyDirectory(string sourceDir, string destinationDir, bool recursive)
        {
            // Get information about the source directory
            var dir = new DirectoryInfo(sourceDir);

            // Check if the source directory exists
            if (!dir.Exists)
                throw new DirectoryNotFoundException($"Source directory not found: {dir.FullName}");

            // Cache directories before we start copying
            DirectoryInfo[] dirs = dir.GetDirectories();

            // Create the destination directory
            Directory.CreateDirectory(destinationDir);

            // Get the files in the source directory and copy to the destination directory
            foreach (FileInfo file in dir.GetFiles())
            {
                string targetFilePath = Path.Combine(destinationDir, file.Name);
                file.CopyTo(targetFilePath);
            }

            // If recursive and copying subdirectories, recursively call this method
            if (recursive)
            {
                foreach (DirectoryInfo subDir in dirs)
                {
                    string newDestinationDir = Path.Combine(destinationDir, subDir.Name);
                    CopyDirectory(subDir.FullName, newDestinationDir, true);
                }
            }
        }


        public static void Image_Diff(HObject image1, HObject image2, int width, int height, out HTuple hv_Err)
        {
            HOperatorSet.TupleGenSequence(0, width - 1, 1, out HTuple hv_Cols);

            hv_Err = 0;
            HTuple end_val15 = height - 1;
            HTuple step_val15 = 1;
            for (HTuple hv_Row = 0; hv_Row.Continue(end_val15, step_val15); hv_Row = hv_Row.TupleAdd(step_val15))
            {
                HOperatorSet.TupleGenConst(width, hv_Row, out HTuple hv_Rows);

                HOperatorSet.GetGrayval(image1, hv_Rows, hv_Cols, out HTuple hv_Gray);
                HOperatorSet.GetGrayval(image2, hv_Rows, hv_Cols, out HTuple hv_Gray_D);
                HTuple hv_Gray_Test = hv_Gray_D - hv_Gray;
                HOperatorSet.TupleAbs(hv_Gray_Test, out hv_Gray_Test);
                HOperatorSet.TupleSum(hv_Gray_Test, out hv_Gray_Test);
                hv_Err = hv_Err + hv_Gray_Test;
            }

        }


    }
}
