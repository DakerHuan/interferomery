#include <stdlib.h>
#include <iostream>

/************************************/
//����Դ�ļ���ֱ�Ӱ���BasicExcel.cpp��
//					  BasicExcel.hpp��
//					  ExcelFormat.cpp��
//					  ExcelFormat.h
//�����������а���	  ExcelFormat.h
//�������			  StdAfx.h
//�������error LNK2038: ��⵽��_ITERATOR_DEBUG_LEVEL���Ĳ�ƥ�����debug��releaseģʽ��ԭ��
//��������ο�https://blog.csdn.net/wzsy/article/details/70599654
/************************************/
#include "StdAfx.h"
#include "ExcelFormat.h"
#define filePath "myTry.xls" //���ɵ�excel�ļ�����·��
using namespace ExcelFormat;

bool main(int argc, char **argv)
{
	/****�����ǽӿڲ���****/
	int mySize = 100; //���ڴ���Excel���ĵ���������
	vector<float> myData; //���ڴ���Excel�������ݡ�
	vector<string> myName; //���ڴ���Excel�����������ơ�
	/****�����н��������ƺ����ݷֱ���myName��myData****/
	for (int i = 1; i < mySize+1; ++i)
	{
		float myRand = (float)(100*rand())/(float)(100*rand());
		myData.push_back(myRand);
		char dataName[20];
		sprintf(dataName,"My Data%d",i);
		myName.push_back(dataName);
	}
	
	
	
	
	//����myName��myData��С�Ƿ���ͬ
	if (myName.size() != myData.size())
	{
		std::cout<<"��������������������"<<std::endl;
		return false;
	}
	//����sheet1����ȡ��ع������ָ��
	BasicExcel xls;
	xls.New(1);
	BasicExcelWorksheet* sheet = xls.GetWorksheet(0);
	XLSFormatManager fmt_mgr(xls);
	//����excel����е�����
	ExcelFont font_bold;
	font_bold._weight = FW_BOLD; //����
	CellFormat fmt_bold(fmt_mgr);
	fmt_bold.set_font(font_bold);
	int col = 0, row = 0;
	//�ڵ�һ��д���������ƣ�Ҳ���Ǳ�ͷ
	for (row = 0; row < myName.size(); row++)
	{
		BasicExcelCell* cell = sheet->Cell(row, col);

		cell->Set(myName[row].c_str());
		cell->SetFormat(fmt_bold);
	}
	//�ڵڶ���д������
	col = 1;
	for (row = 0; row < myData.size(); row++)
	{
		sheet->Cell(row, col)->Set(myData[row]);
	}
	xls.SaveAs(filePath);
	
	
	
	return true;
}