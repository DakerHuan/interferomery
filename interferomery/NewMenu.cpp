#include "stdafx.h"
#include "NewMenu.h"


NewMenu::NewMenu()
{
}


NewMenu::~NewMenu()
{
}
void NewMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 25;//项高
	lpMeasureItemStruct->itemWidth = 220;//项宽
}

void NewMenu::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rect = lpDrawItemStruct->rcItem;
	//CRect rect(20,0,50,100);
	rect.BottomRight().x += 200;
	if (lpDrawItemStruct->itemID==32800)
	{
		rect.TopLeft().x += 80;
	}
	//rect.left += 300;
	//ClientToScreen(this, &rect);
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	dc.FillSolidRect(rect, RGB(0, 0, 0));
	CFont Font;
	Font.CreatePointFont(100, _T("宋体"));//创建字体
	dc.SelectObject(&Font);
	CString *pText = (CString *)lpDrawItemStruct->itemData;
	if (lpDrawItemStruct->itemState&ODS_SELECTED)
		dc.FillSolidRect(rect, RGB(0, 0, 0));//菜单被选中
	dc.SetTextColor(RGB(255, 255, 255));//设置文本颜色
	dc.DrawText(*pText, rect, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	dc.Detach();

}

void NewMenu::ChangeMenuItem(CMenu *pMenu)
{
	int itemCount = pMenu->GetMenuItemCount();
	for (int i = 0; i < itemCount; i++)
	{
		CString *pText = new CString;
		UINT itemID = pMenu->GetMenuItemID(i);//获取菜单项ID号
		pMenu->GetMenuString(i, *pText, MF_BYPOSITION);//获取菜单文本

													   //ModifyMenu函数最后一个参数对应DRAWITEMSTRUCT结构里的itemData变量
		pMenu->ModifyMenu(i, MF_OWNERDRAW | MF_BYPOSITION | MF_STRING, itemID, (LPCTSTR)pText);
		//if (itemID == -1)//如果是一个弹出式菜单
		//{
		//	ChangeMenuItem(pMenu->GetSubMenu(i));
		//}
	}
}