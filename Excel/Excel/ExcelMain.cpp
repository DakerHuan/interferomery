#include <stdlib.h>
#include <iostream>

/************************************/
//请在源文件中直接包含BasicExcel.cpp、
//					  BasicExcel.hpp、
//					  ExcelFormat.cpp、
//					  ExcelFormat.h
//请在主程序中包含	  ExcelFormat.h
//建议包含			  StdAfx.h
//如果出现error LNK2038: 检测到“_ITERATOR_DEBUG_LEVEL”的不匹配项，是debug和release模式的原因，
//解决方法参考https://blog.csdn.net/wzsy/article/details/70599654
/************************************/
#include "StdAfx.h"
#include "ExcelFormat.h"
#define filePath "myTry.xls" //生成的excel文件名和路径
using namespace ExcelFormat;

bool main(int argc, char **argv)
{
	/****以下是接口部分****/
	int mySize = 100; //用于存于Excel表格的的数据量。
	vector<float> myData; //用于存于Excel表格的数据。
	vector<string> myName; //用于存于Excel表格的数据名称。
	/****请自行将数据名称和数据分别导入myName和myData****/
	for (int i = 1; i < mySize+1; ++i)
	{
		float myRand = (float)(100*rand())/(float)(100*rand());
		myData.push_back(myRand);
		char dataName[20];
		sprintf(dataName,"My Data%d",i);
		myName.push_back(dataName);
	}
	
	
	
	
	//检验myName和myData大小是否相同
	if (myName.size() != myData.size())
	{
		std::cout<<"数据名称与数据量不符"<<std::endl;
		return false;
	}
	//创建sheet1并获取相关工作表的指针
	BasicExcel xls;
	xls.New(1);
	BasicExcelWorksheet* sheet = xls.GetWorksheet(0);
	XLSFormatManager fmt_mgr(xls);
	//设置excel表格中的字体
	ExcelFont font_bold;
	font_bold._weight = FW_BOLD; //黑体
	CellFormat fmt_bold(fmt_mgr);
	fmt_bold.set_font(font_bold);
	int col = 0, row = 0;
	//在第一列写入数据名称，也就是表头
	for (row = 0; row < myName.size(); row++)
	{
		BasicExcelCell* cell = sheet->Cell(row, col);

		cell->Set(myName[row].c_str());
		cell->SetFormat(fmt_bold);
	}
	//在第二列写下数据
	col = 1;
	for (row = 0; row < myData.size(); row++)
	{
		sheet->Cell(row, col)->Set(myData[row]);
	}
	xls.SaveAs(filePath);
	
	
	
	return true;
}