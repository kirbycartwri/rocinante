/*
	CTextMenu.h - a C++ wrapper for the Ncurses TextMenu library.
	Copyright (c) 2024 Kirby W. Cartwright. All right reserved.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef CTEXTMENU_H_
#define CTEXTMENU_H_
#include <string.h>
#include <syslog.h>
#include <curses.h>
#include <menu.h>
#include <form.h>
#include "CTextForm.h"

namespace std {

class CTextMenu;

class CTextMenuItem {
public:
	CTextMenuItem(char *chc=NULL, char *desc=NULL){
		if ( NULL!=chc )
			(void)strncpy(m_achChoice, chc, sizeof(m_achChoice));
		else
			bzero(m_achChoice, sizeof(m_achChoice));
		if ( NULL!=desc )
			(void)strncpy(m_achDescription, desc, sizeof(m_achDescription));
		else
			bzero(m_achDescription, sizeof(m_achDescription));
	}
	virtual ~CTextMenuItem(){
		bzero(m_achChoice, sizeof(m_achChoice));
	}
	inline char *lpszChoice(void){
		return m_achChoice;
	}
	inline char *lpszDesc(void){
		return m_achDescription;
	}
	virtual void vHandleItem(void){;}
protected:

private:
	char m_achChoice[25];
	char m_achDescription[25];
};

class CTextFormMenuItem : public CTextMenuItem {
public:
	CTextFormMenuItem(char *chc=NULL, char *desc=NULL, int startx=0, int starty=0, int width=COLS, int height=LINES, CTextField **pcChoices=NULL, int iNumChoices=0)
	: CTextMenuItem(chc, desc),
	m_pcTextForm(NULL),
	m_iStartX(startx), m_iStartY(starty), m_iWidth(width), m_iHeight(height),
	m_iNumChoices(iNumChoices),
	m_pcChoices(pcChoices)
	{
		;
	}
	virtual ~CTextFormMenuItem(){
		if ( NULL!=m_pcTextForm ){
			delete m_pcTextForm;
			m_pcTextForm = NULL;
		}
	}

	virtual void vHandleItem(void);

protected:

private:
	CTextForm *m_pcTextForm;
	int m_iStartX, m_iStartY, m_iWidth, m_iHeight, m_iNumChoices;
	CTextField **m_pcChoices;
};

class CTextWindowMenuItem : public CTextMenuItem {
public:
	CTextWindowMenuItem(char *chc=NULL, char *desc=NULL, int startx=0, int starty=0, int width=-1, int height=-1) : CTextMenuItem(chc, desc),
	m_pcTextWindow(NULL),
	m_iStartX(startx), m_iStartY(starty), m_iWidth(width), m_iHeight(height)
	{
		;
	}
	virtual ~CTextWindowMenuItem(){
		if ( NULL!=m_pcTextWindow ){
			delete m_pcTextWindow;
			m_pcTextWindow = NULL;
		}
	}

	virtual void vHandleItem(void);

protected:

private:
	CTextWindow *m_pcTextWindow;
	int m_iStartX, m_iStartY, m_iWidth, m_iHeight;
};

class CSubMenuItem : public CTextMenuItem {
public:
	CSubMenuItem(char *chc=NULL, char *desc=NULL, CTextMenuItem **pcChoices=NULL, int numchoices=0, int startx=0, int starty=0, int width=-1, int height=-1) : CTextMenuItem(chc, desc),
	m_pcSubMenu(NULL),
	m_pcChoices(pcChoices), m_iNumChoices(numchoices),
	m_iStartX(startx), m_iStartY(starty), m_iWidth(width), m_iHeight(height)
	{
		;
	}
	virtual ~CSubMenuItem(){
		if ( NULL!=m_pcSubMenu ){
			delete m_pcSubMenu;
			m_pcSubMenu = NULL;
		}
	}

	virtual void vHandleItem(void);
protected:

private:
	class CTextMenu *m_pcSubMenu;
	CTextMenuItem **m_pcChoices;
	int m_iStartX, m_iStartY, m_iWidth, m_iHeight, m_iNumChoices;
};

class CTextMenu {
public:
	CTextMenu(int startx=0, int starty=0, int width=COLS, int height=LINES, CTextMenuItem **pcChoices=NULL, int iNumChoices=0, char *lpszMenuName=NULL);
	CTextMenu(CTextWindow *pWin=NULL, CTextMenuItem **pcChoices=NULL, int iNumChoices=0,  char *lpszMenuName=NULL);
	CTextMenu(CTextMenuItem **pcChoices=NULL, int iNumChoices=0, char *lpszMenuName=NULL);
	virtual ~CTextMenu();
	void vHandleMenu(void);
protected:
	void vCreateWindow(void);
	void vDestroyWindow(void);
	virtual void vPrintInformation(void);
private:
	ITEM **m_ppsMyItems;
	MENU *m_psMyMenu;
	WINDOW *m_psMyMenuWin;
	CTextWindow *m_pcMyMenuWinInst;
	int m_iNumChoices;
	int m_iStartX, m_iStartY, m_iWidth, m_iHeight;
	CTextMenuItem *m_pacMenuItems[25];
	char m_achMenuName[25];
	static int m_iNumMenues;
};

} /* namespace std */
#endif /* CTEXTMENU_H_ */
