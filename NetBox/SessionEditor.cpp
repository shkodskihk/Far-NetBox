/**************************************************************************
 *  NetBox plugin for FAR 2.0 (http://code.google.com/p/farplugs)         *
 *  Copyright (C) 2011 by Artem Senichev <artemsen@gmail.com>             *
 *  Copyright (C) 2011 by Michael Lukashov <michael.lukashov@gmail.com>   *
 *                                                                        *
 *  This program is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#include "stdafx.h"
#include "SessionEditor.h"
#include "Session.h"
#include "Strings.h"


CSessionEditor::CSessionEditor(CSession* session, const int width, const int height)
	: CFarDialog(width, height),
	  _IdEditName(0), _IdEditURL(0), _IdEditUser(0), _IdEditPswHide(0), _IdEditPswShow(0), _IdChBxPromtpPsw(0), _IdChBxShowPsw(0), _IdBtnOK(0), _IdBtnCancel(0),
	  _EditMode(false), _Session(session)
{
	assert(_Session);
}


bool CSessionEditor::EditSession()
{
	_EditMode = (wcslen(_Session->GetSessionName()) > 0);
	_Title = CFarPlugin::GetString(_EditMode ? StringEdEdtTitle : StringEdCrtTitle);
	_Title += L" (";
	_Title += CSession::GetProtocolName(_Session->GetProtocolId());
	_Title += L')';
	SetTitle(_Title.c_str());

	wstring url = _Session->GetURL();
	if (!_EditMode) {
		url = CSession::GetDefaultScheme(_Session->GetProtocolId());
		url += L"://";
	}

	CreateText(GetLeft(), GetTop() + 0, CFarPlugin::GetString(StringEdName));
	_IdEditName = CreateEdit(GetLeft(), GetTop() + 1, MAX_SIZE, _Session->GetSessionName());
	CreateText(GetLeft(), GetTop() + 2, CFarPlugin::GetString(StringEdURL));
	_IdEditURL = CreateEdit(GetLeft(), GetTop() + 3, MAX_SIZE, url.c_str());

	CreateSeparator(GetTop() + 4, CFarPlugin::GetString(StringEdAuth));
	CreateText(GetLeft(), GetTop() + 5, CFarPlugin::GetString(StringEdAuthUser));
	_IdEditUser = CreateEdit(GetLeft(), GetTop() + 6, MAX_SIZE, _Session->GetUserName());
	CreateText(GetLeft(), GetTop() + 7, CFarPlugin::GetString(StringEdAuthPsw));
	_IdEditPswShow = CreateEdit(GetLeft(), GetTop() + 8, MAX_SIZE, _Session->GetPassword());
	_IdEditPswHide = CreateDlgItem(DI_PSWEDIT, GetLeft(), GetWidth(), GetTop() + 8, GetTop() + 8, _Session->GetPassword());

	_IdChBxPromtpPsw = CreateCheckBox(GetLeft(), GetTop() + 9, CFarPlugin::GetString(StringEdAuthPromtpPsw), _Session->GetPromptPwd());
	_IdChBxShowPsw = CreateCheckBox(GetLeft(), GetTop() + 10, CFarPlugin::GetString(StringEdAuthShowPsw), false);

	OnPrepareDialog();

	CreateSeparator(GetHeight() - 2);
	_IdBtnOK = CreateButton(0, GetHeight() - 1, CFarPlugin::GetString(StringOK), DIF_CENTERGROUP);
	_IdBtnCancel = CreateButton(0, GetHeight() - 1, CFarPlugin::GetString(StringCancel), DIF_CENTERGROUP);

	const int ret = DoModal();
	if (ret < 0 || ret == _IdBtnCancel)
		return false;

	_Session->SetSessionName(GetText(_IdEditName).c_str());
	_Session->SetURL(GetText(_IdEditURL).c_str());
	_Session->SetUserName(GetText(_IdEditUser).c_str());
	_Session->SetPassword(GetText(_IdEditPswHide).c_str());
	_Session->SetPromptPwd(GetCheckState(_IdChBxPromtpPsw));

	OnSave();
	return true;
}


int CSessionEditor::CreateCodePageControl(const int topPos, const UINT current)
{
	CreateText(GetLeft(), topPos, CFarPlugin::GetString(StringEdCP));

	//Avialable codepages
	static vector<wstring> codePages;
	static vector<FarListItem> farListItems;
	static FarList farList;
	if (codePages.empty()) {
		CPINFOEX cpInfoEx;
		if (GetCPInfoEx(CP_UTF8, 0, &cpInfoEx))
			codePages.push_back(cpInfoEx.CodePageName);
		if (GetCPInfoEx(CP_OEMCP, 0, &cpInfoEx))
			codePages.push_back(cpInfoEx.CodePageName);
		if (GetCPInfoEx(CP_ACP, 0, &cpInfoEx))
			codePages.push_back(cpInfoEx.CodePageName);
		if (GetCPInfoEx(20866, 0, &cpInfoEx))	//KOI8-r
			codePages.push_back(cpInfoEx.CodePageName);

		const size_t avialCP = codePages.size();
		farListItems.resize(avialCP);
		ZeroMemory(&farListItems[0], avialCP * sizeof(FarListItem));
		for (size_t i = 0; i < avialCP; ++i)
			farListItems[i].Text = codePages[i].c_str();
		farList.Items = &farListItems.front();
		farList.ItemsNumber = static_cast<int>(farListItems.size());
	}

	bool cpFound = false;
	for (size_t i = 0; i < farListItems.size(); ++i) {
		if (static_cast<UINT>(_wtoi(farListItems[i].Text)) != current)
			farListItems[i].Flags = 0;
		else {
			cpFound = true;
			farListItems[i].Flags = LIF_SELECTED;
		}
	}

	FarDialogItem* dlgItemidCPList;
	const int id = CreateDlgItem(DI_COMBOBOX, GetLeft(), GetWidth() - 1, topPos + 1, topPos + 1, NULL, DIF_LISTWRAPMODE, &dlgItemidCPList);
	dlgItemidCPList->ListItems = &farList;

	if (!cpFound) {
		static wchar_t num[32];
		_itow_s(current, num, 10);
		dlgItemidCPList->PtrData = num;
	}

	return id;
}


LONG_PTR CSessionEditor::DialogMessageProc(int msg, int param1, LONG_PTR param2)
{
	if (msg == DN_INITDIALOG) {
		ShowDlgItem(_IdEditPswShow, false);
		ShowDlgItem(_IdEditPswHide, true);
	}
	else if (msg == DN_CLOSE && param1 >= 0 && param1 != _IdBtnCancel && !Validate())
		return FALSE;
	else if (msg == DN_BTNCLICK && param1 == _IdChBxShowPsw) {
		const bool showPwd = (param2 == 1);
		ShowDlgItem(_IdEditPswShow, showPwd);
		ShowDlgItem(_IdEditPswHide, !showPwd);
	}
	else if (msg == DN_EDITCHANGE && (param1 == _IdEditPswHide || param1 == _IdEditPswShow)) {
		COORD coord;
		CFarPlugin::GetPSI()->SendDlgMessage(_Dlg, DM_GETCURSORPOS, param1, reinterpret_cast<LONG_PTR>(&coord));
		if (param1 == _IdEditPswHide)
			SetText(_IdEditPswShow, GetText(_IdEditPswHide).c_str());
		else
			SetText(_IdEditPswHide, GetText(_IdEditPswShow).c_str());
		CFarPlugin::GetPSI()->SendDlgMessage(_Dlg, DM_SETCURSORPOS, param1, reinterpret_cast<LONG_PTR>(&coord));
	}

	return CFarDialog::DialogMessageProc(msg, param1, param2);
}


bool CSessionEditor::Validate() const
{
	wstring url = GetText(_IdEditURL);
	if (url.empty()) {
		CFarPlugin::MessageBox(_Title.c_str(), CFarPlugin::GetString(StringEdErrURLEmpty), FMSG_MB_OK | FMSG_WARNING);
		return false;
	}

	//Check URL for valid form
	wstring schemeName, hostName;
	ParseURL(url.c_str(), &schemeName, NULL, NULL, NULL, NULL, NULL, NULL);
	if (schemeName.empty()) {
		url = CSession::GetDefaultScheme(_Session->GetProtocolId()) + url;
		SetText(_IdEditURL, url.c_str());
	}
	ParseURL(url.c_str(), NULL, &hostName, NULL, NULL, NULL, NULL, NULL);
	if (hostName.empty()) {
		CFarPlugin::MessageBox(_Title.c_str(), CFarPlugin::GetString(StringEdErrURLInvalid), FMSG_MB_OK | FMSG_WARNING);
		return false;
	}

	wstring name = GetText(_IdEditName);
	if (name.empty()) {
		if (!_EditMode) {
			name = hostName;
			SetText(_IdEditName, name.c_str());
		}
		else {
			CFarPlugin::MessageBox(_Title.c_str(), CFarPlugin::GetString(StringEdErrNameEmpty), FMSG_MB_OK | FMSG_WARNING);
			return false;
		}
	}
	else {
		static const wchar_t* restrictedSymbols = L"<>:\"/\\|?*";
		if (name.find_first_of(restrictedSymbols) != string::npos) {
			CFarPlugin::MessageBox(_Title.c_str(), CFarPlugin::GetString(StringEdErrNameInvalid), FMSG_MB_OK | FMSG_WARNING);
			return false;
		}
	}

	return OnValidate();
}
