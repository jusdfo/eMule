//this file is part of eMule
//Copyright (C)2002-2023 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / https://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include "stdafx.h"
#include "emule.h"
#include "DirectoryTreeCtrl.h"
#include "otherfunctions.h"
#include "Preferences.h"
#include "TitleMenu.h"
#include "UserMsgs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////
// written by robert rostek - tecxx@rrs.at //
/////////////////////////////////////////////

struct STreeItem
{
	CString strPath;
};


// CDirectoryTreeCtrl

IMPLEMENT_DYNAMIC(CDirectoryTreeCtrl, CTreeCtrl)

BEGIN_MESSAGE_MAP(CDirectoryTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnTvnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnTvnGetdispinfo)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnTvnDeleteItem)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

CDirectoryTreeCtrl::~CDirectoryTreeCtrl()
{
	// don't destroy the system's image list
	m_images.Detach();
}

void CDirectoryTreeCtrl::OnDestroy()
{
	// If a treeview control is created with TVS_CHECKBOXES, the application has to
	// delete the image list which was implicitly created by the control.
	CImageList *piml = GetImageList(TVSIL_STATE);
	if (piml)
		piml->DeleteImageList();

	CTreeCtrl::OnDestroy();
}

void CDirectoryTreeCtrl::OnTvnItemexpanding(LPNMHDR pNMHDR, LRESULT *pResult)
{
	CWaitCursor curWait;
	SetRedraw(FALSE);

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	// remove all sub-items
	HTREEITEM hRemove;
	while ((hRemove = GetChildItem(hItem)) != NULL)
		DeleteItem(hRemove);

	// fetch all subdirectories and add them to the node
	AddSubdirectories(hItem, GetFullPath(hItem));

	SetRedraw(TRUE);
	Invalidate();
	*pResult = 0;
}

void CDirectoryTreeCtrl::ShareSubDirTree(HTREEITEM hItem, BOOL bRecurse)
{
	CWaitCursor curWait;
	SetRedraw(FALSE);
	BOOL bCheck = !GetCheck(hItem);
	HTREEITEM hItemVisibleItem = GetFirstVisibleItem();
	CheckChanged(hItem, bCheck);
	if (bRecurse) {
		Expand(hItem, TVE_TOGGLE);
		for (HTREEITEM hChild = GetChildItem(hItem); hChild != NULL; hChild = GetNextSiblingItem(hChild))
			MarkChildren(hChild, bCheck);
		Expand(hItem, TVE_TOGGLE);
	}
	if (hItemVisibleItem)
		SelectSetFirstVisible(hItemVisibleItem);

	SetRedraw(TRUE);
	Invalidate();
}

void CDirectoryTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	//VQB adjustments to provide for sharing or unsharing of subdirectories when control key is Down
	UINT uHitFlags;
	HTREEITEM hItem = HitTest(point, &uHitFlags);
	if (hItem && (uHitFlags & TVHT_ONITEMSTATEICON))
		ShareSubDirTree(hItem, nFlags & MK_CONTROL);
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CDirectoryTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_SPACE) {
		HTREEITEM hItem = GetSelectedItem();
		if (hItem) {
			ShareSubDirTree(hItem, GetKeyState(VK_CONTROL) < 0);

			// if Ctrl+Space is passed to the tree control, it just beeps and does not check/uncheck the item!
			SetCheck(hItem, !GetCheck(hItem));
			return;
		}
	}

	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDirectoryTreeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// If we let any keystrokes which are handled by us - but not by the tree control -
	// be passed to the control, the user will hear a system event sound (Standard Error!)
	if (GetKeyState(VK_CONTROL) >= 0 || nChar != VK_SPACE)
		CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void CDirectoryTreeCtrl::MarkChildren(HTREEITEM hChild, bool mark)
{
	CheckChanged(hChild, mark);
	SetCheck(hChild, mark);
	Expand(hChild, TVE_TOGGLE); // VQB - make sure tree has entries
	for (HTREEITEM hChild2 = GetChildItem(hChild); hChild2 != NULL; hChild2 = GetNextSiblingItem(hChild2))
		MarkChildren(hChild2, mark);

	Expand(hChild, TVE_TOGGLE); // VQB - restore tree to initial disposition
}

void CDirectoryTreeCtrl::OnTvnGetdispinfo(LPNMHDR pNMHDR, LRESULT *pResult)
{
	reinterpret_cast<LPNMTVDISPINFO>(pNMHDR)->item.cChildren = 1;
	*pResult = 0;
}

void CDirectoryTreeCtrl::Init()
{
	// Win98: Explicitly set to Unicode to receive Unicode notifications.
	SendMessage(CCM_SETUNICODEFORMAT, TRUE);

	ShowWindow(SW_HIDE);
	DeleteAllItems();

	// START: added by FoRcHa /////////////
	SHFILEINFO shFinfo;

	// Get the system image list using a "path" which is available on all systems. [patch by bluecow]
	HIMAGELIST hImgList = (HIMAGELIST)::SHGetFileInfo(_T("."), 0, &shFinfo, sizeof(shFinfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	if (!hImgList) {
		TRACE(_T("Cannot retrieve the Handle of SystemImageList!"));
		//return;
	}

	m_images.m_hImageList = hImgList;
	SetImageList(&m_images, TVSIL_NORMAL);
	////////////////////////////////

	TCHAR drivebuffer[500];
	DWORD dwRet = GetLogicalDriveStrings(_countof(drivebuffer) - 1, drivebuffer);
	if (dwRet > 0 && dwRet < _countof(drivebuffer)) {
		drivebuffer[_countof(drivebuffer) - 1] = _T('\0');

		for (LPCTSTR pos = drivebuffer; *pos != _T('\0');) {
			// Copy drive name
			TCHAR drive[4];
			_tcsncpy(drive, pos, _countof(drive));
			drive[2] = _T('\0');
			AddChildItem(NULL, drive); // e.g. "C:"

			// Point to the next drive
			pos += _tcslen(pos) + 1;
		}
	}
	for (INT_PTR i = 0; i < m_lstUNC.GetCount(); ++i) {
		const CString &sUNC(m_lstUNC[i]);
		AddChildItem(NULL, sUNC.Left(sUNC.GetLength() - 1));
	}

	ShowWindow(SW_SHOW);
}

HTREEITEM CDirectoryTreeCtrl::AddChildItem(HTREEITEM hRoot, const CString &strText)
{
	CString strDir(GetFullPath(hRoot));
	ASSERT(strDir.IsEmpty() || strDir.Right(1) == _T("\\"));
	strDir += strText;
	slosh(strDir);

	TVINSERTSTRUCT itInsert = {};
	itInsert.hParent = hRoot;
	itInsert.hInsertAfter = hRoot ? TVI_SORT : TVI_LAST;
	itInsert.item.pszText = const_cast<LPTSTR>((LPCTSTR)strText);

	// START: changed by FoRcHa /////
	itInsert.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_TEXT | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	itInsert.item.stateMask = TVIS_BOLD | TVIS_STATEIMAGEMASK;
	// END: changed by FoRcHa ///////

	itInsert.item.state = HasSharedSubdirectory(strDir) ? TVIS_BOLD : 0;
	// used to display the + symbol next to each item
	itInsert.item.cChildren = HasSubdirectories(strDir) ? I_CHILDRENCALLBACK : 0;

	// START: added by FoRcHa ////////////////
	UINT nType = ::GetDriveType(strDir);
	if (DRIVE_REMOVABLE <= nType && nType <= DRIVE_RAMDISK)
		itInsert.item.iImage = nType;

	SHFILEINFO shFinfo;
	shFinfo.szDisplayName[0] = _T('\0');
	if (!::SHGetFileInfo(strDir, 0, &shFinfo, sizeof(shFinfo), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_DISPLAYNAME)) {
		TRACE(_T("Error getting SystemFileInfo!"));
		itInsert.itemex.iImage = 0; // :(
	} else {
		itInsert.itemex.iImage = shFinfo.iIcon;
		::DestroyIcon(shFinfo.hIcon);
		if (hRoot == NULL && shFinfo.szDisplayName[0] != _T('\0')) {
			STreeItem *pti = new STreeItem;
			pti->strPath = strText;
			itInsert.item.pszText = const_cast<LPTSTR>((LPCTSTR)pti->strPath);
			itInsert.item.mask |= TVIF_PARAM;
			itInsert.item.lParam = (LPARAM)pti;
		}
	}

	if (!::SHGetFileInfo(strDir, 0, &shFinfo, sizeof(shFinfo), SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON)) {
		TRACE(_T("Error Getting SystemFileInfo!"));
		itInsert.itemex.iImage = 0;
	} else {
		itInsert.itemex.iSelectedImage = shFinfo.iIcon;
		::DestroyIcon(shFinfo.hIcon);
	}
	// END: added by FoRcHa //////////////

	HTREEITEM hItem = InsertItem(&itInsert);
	if (IsShared(strDir))
		SetCheck(hItem);

	return hItem;
}

CString CDirectoryTreeCtrl::GetFullPath(HTREEITEM hItem)
{
	CString strDir;
	for (HTREEITEM hSearchItem = hItem; hSearchItem != NULL; hSearchItem = GetParentItem(hSearchItem)) {
		const STreeItem *pti = reinterpret_cast<STreeItem*>(GetItemData(hSearchItem));
		strDir.Insert(0, _T('\\')); //trailing backslash
		strDir.Insert(0, pti ? pti->strPath : GetItemText(hSearchItem));
	}
	return strDir;
}

void CDirectoryTreeCtrl::AddSubdirectories(HTREEITEM hRoot, const CString &strDir)
{
	ASSERT(strDir.Right(1) == _T("\\"));
	CFileFind finder;
	for (BOOL bFound = finder.FindFile(strDir + _T("*.*")); bFound;) {
		bFound = finder.FindNextFile();
		if (finder.IsDirectory() && !finder.IsDots() && !finder.IsSystem()) {
			CString strFilename(finder.GetFileName());
			int i = strFilename.ReverseFind(_T('\\'));
			if (i >= 0)
				strFilename.Delete(0, i + 1);
			AddChildItem(hRoot, strFilename);
		}
	}
}

bool CDirectoryTreeCtrl::HasSubdirectories(const CString &strDir)
{
	return ::HasSubdirectories(strDir);
}

void CDirectoryTreeCtrl::GetSharedDirectories(CStringList &list)
{
	list.AddTail(&m_lstShared);
}

void CDirectoryTreeCtrl::SetSharedDirectories(CStringList &list)
{
	m_lstShared.RemoveAll();

	for (POSITION pos = list.GetHeadPosition(); pos != NULL;) {
		const CString &sDir(list.GetNext(pos));
		m_lstShared.AddTail(sDir);
		if (::PathIsUNC(sDir)) {
			const CString &sShare(GetShareName(sDir));
			INT_PTR i = m_lstUNC.GetCount();
			if (!i) {
				m_lstUNC.Add(sShare);
				continue;
			}
			while (--i >= 0) {
				int cmp = sShare.CompareNoCase(m_lstUNC[i]);
				if (cmp >= 0) {
					if (cmp)
						m_lstUNC.InsertAt(i + 1, sShare);
					break;
				}
			}
		}
	}
	Init();
}

bool CDirectoryTreeCtrl::HasSharedSubdirectory(const CString &strDir)
{
	int iLen = strDir.GetLength();
	ASSERT(iLen > 0);
	bool bSlosh = (strDir[iLen - 1] == _T('\\'));
	for (POSITION pos = m_lstShared.GetHeadPosition(); pos != NULL;) {
		const CString &sDir(m_lstShared.GetNext(pos));
		if (_tcsnicmp(sDir, strDir, iLen) == 0 && iLen < sDir.GetLength() && (bSlosh || sDir[iLen] == _T('\\')))
			return true;
	}
	return false;
}

void CDirectoryTreeCtrl::CheckChanged(HTREEITEM hItem, bool bChecked)
{
	const CString &strDir(GetFullPath(hItem));
	if (bChecked)
		AddShare(strDir);
	else
		DelShare(strDir);

	UpdateParentItems(hItem);
	GetParent()->SendMessage(WM_COMMAND, UM_ITEMSTATECHANGED, reinterpret_cast<LPARAM>(m_hWnd));
}

bool CDirectoryTreeCtrl::IsShared(const CString &strDir)
{
	for (POSITION pos = m_lstShared.GetHeadPosition(); pos != NULL;)
		if (EqualPaths(m_lstShared.GetNext(pos), strDir))
			return true;

	return false;
}

void CDirectoryTreeCtrl::AddShare(const CString &strDir)
{
	ASSERT(strDir.Right(1) == _T('\\'));
	if (!IsShared(strDir) && thePrefs.IsShareableDirectory(strDir))
		m_lstShared.AddTail(strDir);
}

void CDirectoryTreeCtrl::DelShare(const CString &strDir)
{
	ASSERT(strDir.Right(1) == _T('\\'));
	for (POSITION pos = m_lstShared.GetHeadPosition(); pos != NULL;) {
		POSITION pos2 = pos;
		if (EqualPaths(m_lstShared.GetNext(pos), strDir)) {
			m_lstShared.RemoveAt(pos2);
			break;
		}
	}
}

void CDirectoryTreeCtrl::UpdateParentItems(HTREEITEM hChild)
{
	HTREEITEM hSearch = hChild;
	while ((hSearch = GetParentItem(hSearch)) != NULL)
		SetItemState(hSearch, (HasSharedSubdirectory(GetFullPath(hSearch)) ? TVIS_BOLD : 0), TVIS_BOLD);
}

void CDirectoryTreeCtrl::OnContextMenu(CWnd*, CPoint point)
{
	if (!PointInClient(*this, point)) {
		Default();
		return;
	}

	CPoint ptMenu(-1, -1);
	if (ptMenu != point) {
		ptMenu = point;
		ScreenToClient(&point);
	} else {
		HTREEITEM hSel = GetNextItem(TVI_ROOT, TVGN_CARET);
		if (hSel) {
			CRect rcItem;
			if (GetItemRect(hSel, &rcItem, TRUE)) {
				ptMenu = rcItem.TopLeft();
				ClientToScreen(&ptMenu);
			}
		} else {
			ptMenu.SetPoint(0, 0);
			ClientToScreen(&ptMenu);
		}
	}

	HTREEITEM hItem = HitTest(point);

	// create the menu
	CTitleMenu SharedMenu;
	SharedMenu.CreatePopupMenu();
	SharedMenu.AddMenuTitle(GetResString(IDS_SHAREDFOLDERS));
	bool bMenuIsEmpty = true;

	// add all shared directories
	UINT_PTR iCnt = 0;
	const CString &sView1(GetResString(IDS_VIEW1));
	CString sViewPath;
	for (POSITION pos = m_lstShared.GetHeadPosition(); pos != NULL; ++iCnt) {
		sViewPath.Format(_T("%s%s"), (LPCTSTR)sView1, (LPCTSTR)m_lstShared.GetNext(pos));
		SharedMenu.AppendMenu(MF_STRING, MP_SHAREDFOLDERS_FIRST + iCnt, sViewPath);
		bMenuIsEmpty = false;
	}

	// add right clicked folder, if any
	if (hItem) {
		m_strLastRightClicked = GetFullPath(hItem); //trailing backslash
		if (!IsShared(m_strLastRightClicked)) {
			sViewPath.Format(_T("%s%s%s"), (LPCTSTR)sView1, (LPCTSTR)m_strLastRightClicked, (LPCTSTR)GetResString(IDS_VIEW2));
			if (!bMenuIsEmpty)
				SharedMenu.AppendMenu(MF_SEPARATOR);
			SharedMenu.AppendMenu(MF_STRING, MP_SHAREDFOLDERS_FIRST - 1, sViewPath);
			bMenuIsEmpty = false;
		}
	}

	// display menu
	if (!bMenuIsEmpty)
		SharedMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMenu.x, ptMenu.y, this);
	VERIFY(SharedMenu.DestroyMenu());
}

void CDirectoryTreeCtrl::OnRButtonDown(UINT, CPoint)
{
	// catch WM_RBUTTONDOWN and do not route it the default way... otherwise we won't get a WM_CONTEXTMENU.
	//CTreeCtrl::OnRButtonDown(nFlags, point);
}

BOOL CDirectoryTreeCtrl::OnCommand(WPARAM wParam, LPARAM)
{
	if (wParam < MP_SHAREDFOLDERS_FIRST)
		ShellOpenFile(m_strLastRightClicked);
	else {
		POSITION pos = m_lstShared.FindIndex(wParam - MP_SHAREDFOLDERS_FIRST);
		if (pos)
			ShellOpenFile(m_lstShared.GetAt(pos));
	}

	return TRUE;
}

void CDirectoryTreeCtrl::OnTvnDeleteItem(LPNMHDR pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (pNMTreeView->itemOld.lParam)
		delete reinterpret_cast<STreeItem*>(pNMTreeView->itemOld.lParam);
	*pResult = 0;
}