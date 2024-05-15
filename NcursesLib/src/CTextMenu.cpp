/*
	CTextMenu.cpp - a C++ wrapper for the Ncurses TextMenu library.
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

#include <string.h>
#include <stdio.h>      /* printf, scanf, NULL 	*/
#include <ctype.h>		/* for isascii		   	*/
#include <time.h>
#include <unistd.h>		/* for usleep			*/
#include <stdlib.h>
#include <stdio.h>
#include "CTextMenu.h"

namespace std {

int CTextMenu::m_iNumMenues = 0;

CTextMenu::CTextMenu(int startx/*=0*/, int starty/*=0*/, int width/*=COLS*/, int height/*=LINES*/, CTextMenuItem **pcChoices/*=NULL*/, int iNumChoices/*=0*/, char *lpszMenuName/*=NULL*/) :
	m_ppsMyItems(NULL),
	m_psMyMenu(NULL),
	m_psMyMenuWin(NULL),
	m_iNumChoices(iNumChoices),
	m_iStartX(startx),
	m_iStartY(starty),
	m_iWidth(width),
	m_iHeight(height)
{
	if ( NULL==lpszMenuName )
		bzero(m_achMenuName, sizeof(m_achMenuName));
	else
		(void)strncpy(m_achMenuName, lpszMenuName, sizeof(m_achMenuName));


	if ( 0<m_iNumChoices && NULL!=pcChoices ){
		/* If needed, initialize curses */
		m_iNumMenues++;
		if ( 1==m_iNumMenues ){
			initscr();
			start_color();
			cbreak();
			noecho();
			curs_set(CTextWindow::CURSOR_INVISIBLE);
			keypad(stdscr, TRUE);
			init_pair(1, COLOR_RED, 	COLOR_WHITE);
			init_pair(2, COLOR_BLUE, 	COLOR_WHITE);
			init_pair(3, COLOR_GREEN, 	COLOR_BLACK);
			init_pair(4, COLOR_WHITE,	COLOR_BLACK);
		}

		vCreateWindow();

		/* Create menu items */
		m_ppsMyItems = (ITEM **)calloc(m_iNumChoices+1, sizeof(ITEM *));
		for (int i=0 ; i<m_iNumChoices; ++i){
			m_pacMenuItems[i] = pcChoices[i];
			m_ppsMyItems[i] = new_item(pcChoices[i]->lpszChoice(), pcChoices[i]->lpszDesc());
			set_item_userptr(m_ppsMyItems[i], (void *)pcChoices[i] );
		}

		m_ppsMyItems[m_iNumChoices] = NULL;

		/* Create menu */
		m_psMyMenu = new_menu(m_ppsMyItems);

		// keypad(m_psMyMenuWin, TRUE);
		// nodelay(m_psMyMenuWin, TRUE);

		keypad(stdscr, TRUE);
		nodelay(stdscr, TRUE);

		/* Set main window and sub window */
		set_menu_win(m_psMyMenu, m_psMyMenuWin);
		set_menu_sub(m_psMyMenu, derwin(m_psMyMenuWin, m_iStartX+2, m_iWidth-2, 3, 1));

		// Since we are hightlighting, no menu mark.
		set_menu_mark(m_psMyMenu, "");

		/* Print a border around the main window and print a title */
	    box(m_psMyMenuWin, 0, 0);
	    CTextWindow::print_in_middle(m_psMyMenuWin, 0, 0, m_iWidth, m_achMenuName, COLOR_PAIR(2));

		mvwhline(m_psMyMenuWin, 2, 0, ACS_HLINE, m_iWidth);
		mvwaddch(m_psMyMenuWin, 2, m_iWidth-1, ACS_RTEE);
	    mvwaddch(m_psMyMenuWin, 2, 0, ACS_LTEE);

		mvprintw(m_iStartY+m_iHeight-2, m_iStartX+2, "F2 to exit");

		refresh();

		/* Post the menu */
		post_menu(m_psMyMenu);
		wrefresh(m_psMyMenuWin);
	}

}


CTextMenu::CTextMenu(CTextMenuItem **pcChoices/*=NULL*/, int iNumChoices/*=0*/, char *lpszMenuName/*=NULL*/) :
	m_ppsMyItems(NULL),
	m_psMyMenu(NULL),
	m_psMyMenuWin(NULL),
	m_iNumChoices(iNumChoices),
	m_iStartX(-1),
	m_iStartY(-1),
	m_iWidth(-1),
	m_iHeight(-1)
{
	if ( NULL==lpszMenuName )
		bzero(m_achMenuName, sizeof(m_achMenuName));
	else
		(void)strncpy(m_achMenuName, lpszMenuName, sizeof(m_achMenuName));

	if ( 0<m_iNumChoices && NULL!=pcChoices ){
		/* If needed, initialize curses. */
		m_iNumMenues++;
		if ( 1==m_iNumMenues ) {
			initscr();
			start_color();
			cbreak();
			noecho();
			curs_set(CTextWindow::CURSOR_INVISIBLE);
			keypad(stdscr, TRUE);
			init_pair(1, COLOR_RED, 	COLOR_WHITE);
			init_pair(2, COLOR_BLUE, 	COLOR_WHITE);
			init_pair(3, COLOR_GREEN, 	COLOR_BLACK);
			init_pair(4, COLOR_WHITE,	COLOR_BLACK);
		}

		m_psMyMenuWin = stdscr;
		nodelay(stdscr, TRUE);

		m_iStartY=m_psMyMenuWin->_begy;
		m_iStartX=m_psMyMenuWin->_begx;
		m_iHeight=m_psMyMenuWin->_maxy-m_psMyMenuWin->_begy;
		m_iWidth=m_psMyMenuWin->_maxx-m_psMyMenuWin->_begx;

		int max_len = 0;

		/* Create menu items */
		m_ppsMyItems = (ITEM **)calloc(m_iNumChoices+1, sizeof(ITEM *));
		for (int i=0 ; i<m_iNumChoices; ++i){
			int len = 0;
			m_pacMenuItems[i] = pcChoices[i];
			m_ppsMyItems[i] = new_item(pcChoices[i]->lpszChoice(), pcChoices[i]->lpszDesc());
			set_item_userptr(m_ppsMyItems[i], (void *)pcChoices[i] );
			len = strlen(pcChoices[i]->lpszDesc());
			if ( len>max_len )
				max_len=len;
		}

		m_ppsMyItems[m_iNumChoices] = NULL;

		/* Create menu */
		m_psMyMenu = new_menu(m_ppsMyItems);

		// Have the current window capture the keyboard.
		keypad(m_psMyMenuWin, TRUE);

		/* Set main window and sub window */
		set_menu_win(m_psMyMenu, m_psMyMenuWin);
		set_menu_sub(m_psMyMenu, derwin(m_psMyMenuWin, m_iNumChoices+1, m_iWidth - 2, m_iStartY+3, m_iStartX+1));

		set_menu_mark(m_psMyMenu, " ->");

		/* Print a border around the main window and print a title */
	    box(m_psMyMenuWin, 0, 0);
	    CTextWindow::print_in_middle(m_psMyMenuWin, m_iStartY+1, m_iStartX, m_iWidth, m_achMenuName, COLOR_PAIR(2));

		mvwhline(m_psMyMenuWin, m_iStartY+2, m_iStartX, ACS_HLINE, m_iWidth);
		mvwaddch(m_psMyMenuWin, m_iStartY+2, m_iWidth, ACS_RTEE);
	    mvwaddch(m_psMyMenuWin, m_iStartY+2, m_iStartX, ACS_LTEE);

		mvprintw(LINES - 2, 2, "F2 to exit");
		refresh();

		/* Post the menu */
		post_menu(m_psMyMenu);
		wrefresh(m_psMyMenuWin);
	}

}

CTextMenu::CTextMenu(CTextWindow *pWin/*=NULL*/, CTextMenuItem **pcChoices/*=NULL*/, int iNumChoices/*=0*/,  char *lpszMenuName/*=NULL*/) :
			m_ppsMyItems(NULL),
			m_psMyMenu(NULL),
			m_psMyMenuWin(NULL),
			m_pcMyMenuWinInst(NULL),
			m_iNumChoices(iNumChoices),
			m_iStartX(-1),
			m_iStartY(-1),
			m_iWidth(-1),
			m_iHeight(-1)
{
	if ( NULL==lpszMenuName )
		bzero(m_achMenuName, sizeof(m_achMenuName));
	else
		(void)strncpy(m_achMenuName, lpszMenuName, sizeof(m_achMenuName));


	if ( 0<m_iNumChoices && NULL!=pcChoices ){
		/* If needed, initialize curses */
		m_iNumMenues++;
		if ( 1==m_iNumMenues ){
			initscr();
			start_color();
			cbreak();
			noecho();
			curs_set(CTextWindow::CURSOR_INVISIBLE);
			init_pair(1, COLOR_RED, 	COLOR_WHITE);
			init_pair(2, COLOR_BLUE, 	COLOR_WHITE);
			init_pair(3, COLOR_GREEN, 	COLOR_BLACK);
			init_pair(4, COLOR_WHITE,	COLOR_BLACK);
		}

		if ( NULL!=pWin ){
			m_pcMyMenuWinInst = pWin;
			m_psMyMenuWin	=  m_pcMyMenuWinInst->getWinInst();
			wrefresh(m_psMyMenuWin);
		}
		else {
			m_pcMyMenuWinInst = NULL;
			vCreateWindow();
		}

		m_iStartX		= m_psMyMenuWin->_begx;
		m_iStartY 		= m_psMyMenuWin->_begy;
		m_iWidth 		= m_psMyMenuWin->_maxx - m_iStartX;
		m_iHeight 		= m_psMyMenuWin->_maxy - m_iStartY;

		/* Create menu items */
		m_ppsMyItems = (ITEM **)calloc(m_iNumChoices+1, sizeof(ITEM *));
		for (int i=0 ; i<m_iNumChoices; ++i){
			m_pacMenuItems[i] = pcChoices[i];
			m_ppsMyItems[i] = new_item(pcChoices[i]->lpszChoice(), pcChoices[i]->lpszDesc());
			set_item_userptr(m_ppsMyItems[i], (void *)pcChoices[i] );
		}

		m_ppsMyItems[m_iNumChoices] = NULL;

		/* Create menu */
		m_psMyMenu = new_menu(m_ppsMyItems);

		/* Set main window and sub window */
		set_menu_win(m_psMyMenu, m_psMyMenuWin);
		set_menu_sub(m_psMyMenu, derwin(m_psMyMenuWin, m_iNumChoices+1, m_iWidth - 2, m_iStartY+3, m_iStartX+1));

		// Since we are highlighting, no menu mark.
		set_menu_mark(m_psMyMenu, "");

		// Have the current window capture the keyboard.
		// keypad(m_psMyMenuWin, TRUE);
		// nodelay(m_psMyMenuWin, TRUE);

		keypad(stdscr, TRUE);
		nodelay(stdscr, TRUE);

		/* Print a border around the main window and print a title */
	    box(m_psMyMenuWin, 0, 0);
	    CTextWindow::print_in_middle(m_psMyMenuWin, m_iStartY+1, m_iStartX, m_iWidth, m_achMenuName, COLOR_PAIR(2));

		mvwhline(m_psMyMenuWin, m_iStartY+2, m_iStartX, ACS_HLINE, m_iWidth);
		mvwaddch(m_psMyMenuWin, m_iStartY+2, m_iWidth, ACS_RTEE);
	    mvwaddch(m_psMyMenuWin, m_iStartY+2, m_iStartX, ACS_LTEE);

		mvprintw(m_iStartY+m_iHeight-2, m_iStartX+2, "F2 to exit");

		refresh();

		/* Post the menu */
		post_menu(m_psMyMenu);
		wrefresh(m_psMyMenuWin);
	}
}

void CTextMenu::vPrintInformation(void){
    time_t current_time = 0;

    /* Obtain current time as seconds elapsed since the Epoch. */
    current_time = time(NULL);

    if (current_time == ((time_t)-1)){
        (void) fprintf(stderr, "Failure to compute the current time.");
        return;
    }

    char buffer[80];
    bzero(buffer, sizeof(buffer));

    /* Convert to local time format. */
    char * c_time_string = ctime_r(&current_time, buffer);

    if (c_time_string == NULL){
        (void) fprintf(stderr, "Failure to convert the current time.");
        return;
    }

    char *old_line = strchr(buffer, '\n');
    if ( NULL!=old_line )
    	*old_line = '\0';

    CTextWindow::print_in_middle(m_psMyMenuWin, m_psMyMenuWin->_maxy, 0, m_iWidth, buffer, COLOR_PAIR(3));
    wrefresh(m_psMyMenuWin);
}

CTextMenu::~CTextMenu() {
	if ( NULL!=m_psMyMenu ){
		unpost_menu(m_psMyMenu);
		free_menu(m_psMyMenu);
		m_psMyMenu = NULL;
	}
	for(int i = 0; i < m_iNumChoices; ++i){
		free_item(m_ppsMyItems[i]);
	}
	if ( NULL!=m_ppsMyItems ){
		free(m_ppsMyItems);
		m_ppsMyItems = NULL;
	}
	if ( stdscr!=m_psMyMenuWin )
		vDestroyWindow();

	m_iNumMenues--;
	if ( 0==m_iNumMenues ) {
		endwin();
	}
	m_psMyMenuWin = NULL;
}

// Very suitable for threading:
void CTextMenu::vHandleMenu(void){
	int c = -1;
	while( KEY_F(2) != c){
		if ( CTextWindow::kbhit() )
			c = wgetch(m_psMyMenuWin);
		else {
			vPrintInformation();
			usleep(10000);
			continue;
		}

		ITEM *psI = current_item (m_psMyMenu);
		switch(c){
		case KEY_RIGHT:
		case KEY_DOWN:{
			int i=-1;
			for ( i=0; i<m_iNumChoices ; i++ ){
				ITEM *pcItem = m_ppsMyItems[i];
				if ( psI==pcItem ){
					break;
				}
			}
			if ( i>=(m_iNumChoices-1))
				set_current_item (m_psMyMenu, m_ppsMyItems[0]);
			else
				menu_driver(m_psMyMenu, REQ_DOWN_ITEM);
		}
		break;
		case KEY_LEFT:
		case KEY_UP:{
			int i=-1;
			for ( i=0; i<m_iNumChoices ; i++ ){
				ITEM *pcItem = m_ppsMyItems[i];
				if ( psI==pcItem ){
					break;
				}
			}
			if ( i==0 )
				set_current_item (m_psMyMenu, m_ppsMyItems[m_iNumChoices-1]);
			else
				menu_driver(m_psMyMenu, REQ_UP_ITEM);
		}
		break;
		case KEY_ENTER:{
			for ( int i=0; i<m_iNumChoices ; i++ ){
				CTextMenuItem *pcMenuItem = m_pacMenuItems[i];
				if ( psI->userptr==pcMenuItem ){
					pcMenuItem->vHandleItem();
					(void)wrefresh(m_psMyMenuWin);
					redrawwin(m_psMyMenuWin);
					break;
				}
			}


			if ( !strncmp("Exit", psI->name.str , 4 )){
				c = KEY_F(2);
				continue;
			}
		}
		break;
		case KEY_EXIT:
		case KEY_CLOSE:{
			c = KEY_F(2);
			continue;
		}
		break;
		case KEY_F(2):
			continue;
		default:
			if ( isalnum(c) ){
				c = toupper(c);
				for ( int i=0; i<m_iNumChoices ; i++ ){
					CTextMenuItem *pcMenuItem = m_pacMenuItems[i];
					int ch = *pcMenuItem->lpszChoice();
					ch = toupper(ch);
					if ( c==ch ){
						set_current_item (m_psMyMenu, m_ppsMyItems[i]);
						break;
					}
				}
			}
			break;
		}
		wrefresh(m_psMyMenuWin);
	}
}

void CTextMenu::vCreateWindow(void){
	if ( -1==m_iStartY )
		m_iStartY = 0;
	if ( -1==m_iStartX )
		m_iStartX = 0;
	if ( -1==m_iHeight ){
		m_iHeight = LINES;
	}
	if ( -1==m_iWidth ){
		m_iWidth = COLS;
	}
	m_psMyMenuWin = newwin(m_iHeight, m_iWidth, m_iStartY, m_iStartX);
	// box(m_psMyMenuWin, 0 , 0);
	wrefresh(m_psMyMenuWin);
}

void CTextMenu::vDestroyWindow(void){
	wborder(m_psMyMenuWin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(m_psMyMenuWin);
	delwin(m_psMyMenuWin);
}

void CTextWindowMenuItem::vHandleItem(void){
	m_pcTextWindow = new CTextWindow(m_iStartX, m_iStartY, m_iWidth, m_iHeight, lpszChoice() );
	m_pcTextWindow->vHandleWindow();
	delete m_pcTextWindow;
	m_pcTextWindow = NULL;
}

void CSubMenuItem::vHandleItem(void){
	m_pcSubMenu = new CTextMenu(m_iStartX, m_iStartY, m_iWidth, m_iHeight, m_pcChoices, m_iNumChoices, lpszChoice());
	m_pcSubMenu->vHandleMenu();
	delete m_pcSubMenu;
	m_pcSubMenu = NULL;
}

void CTextFormMenuItem::vHandleItem(void){
	m_pcTextForm = new CTextForm(m_iStartX, m_iStartY, m_iWidth, m_iHeight, m_pcChoices, m_iNumChoices, lpszChoice());
	m_pcTextForm->vHandleForm();
	delete m_pcTextForm;
	m_pcTextForm = NULL;
}

} /* namespace std */
