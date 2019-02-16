#pragma once
#include "afxwin.h"
class NewMenu :
	public CMenu
{
public:
	NewMenu();
	~NewMenu();

	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	void ChangeMenuItem(CMenu *pMenu);
	
};

