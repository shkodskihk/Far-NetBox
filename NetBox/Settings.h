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

#pragma once


//! Plugin settings
class CSettings
{
public:
	CSettings();

	/**
	 * Load settings
	 */
	void Load();

	/**
	 * Configure settings
	 */
	void Configure();
	
	//Accessors
	bool AddToPanelMenu() const			{ return _AddToPanelMenu; }
	bool AddToDiskMenu() const			{ return _AddToDiskMenu; }
	const wchar_t* CmdPrefix() const	{ return _CmdPrefix.c_str(); }
	bool AltPrefix() const				{ return _AltPrefix; }
	bool UseOwnKey() const				{ return _UseOwnKey; }
	unsigned long Timeout() const		{ return _Timeout; }
	const wchar_t* SessionPath() const	{ return _SessionPath.c_str(); }

	/**
	 * Get session path
	 * \return session path
	 */
	wstring GetSessionPath() const;
private:
	/**
	 * Save settings
	 */
	void Save() const;

private:
	//Settings variables
	bool			_AddToPanelMenu;	///< Add plugin to the panel plugin menu flag
	bool			_AddToDiskMenu;		///< Add plugin to the disk menu flag
	wstring			_CmdPrefix;			///< Plugin command prefix
	bool			_AltPrefix;			///< Hande additional preffix flag (ftp, sftp etc)
	bool			_UseOwnKey;			///< Use own encryption key flag
	unsigned long	_Timeout;			///< Default timeout in seconds
	wstring			_SessionPath;		///< Session folder path
};

extern CSettings _Settings;
