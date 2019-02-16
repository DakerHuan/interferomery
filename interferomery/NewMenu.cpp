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
	lpMeasureItemStruct->itemHeight = 25;//���
	lpMeasureItemStruct->itemWidth = 220;//���
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
	Font.CreatePointFont(100, _T("����"));//��������
	dc.SelectObject(&Font);
	CString *pText = (CString *)lpDrawItemStruct->itemData;
	if (lpDrawItemStruct->itemState&ODS_SELECTED)
		dc.FillSolidRect(rect, RGB(0, 0, 0));//�˵���ѡ��
	dc.SetTextColor(RGB(255, 255, 255));//�����ı���ɫ
	dc.DrawText(*pText, rect, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	dc.Detach();

}

void NewMenu::ChangeMenuItem(CMenu *pMenu)
{
	int itemCount = pMenu->GetMenuItemCount();
	for (int i = 0; i < itemCount; i++)
	{
		CString *pText = new CString;
		UINT itemID = pMenu->GetMenuItemID(i);//��ȡ�˵���ID��
		pMenu->GetMenuString(i, *pText, MF_BYPOSITION);//��ȡ�˵��ı�

													   //ModifyMenu�������һ��������ӦDRAWITEMSTRUCT�ṹ���itemData����
		pMenu->ModifyMenu(i, MF_OWNERDRAW | MF_BYPOSITION | MF_STRING, itemID, (LPCTSTR)pText);
		//if (itemID == -1)//�����һ������ʽ�˵�
		//{
		//	ChangeMenuItem(pMenu->GetSubMenu(i));
		//}
	}
}