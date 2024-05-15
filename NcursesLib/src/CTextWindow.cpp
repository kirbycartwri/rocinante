/*
	CTextWindow.cpp - a C++ wrapper for the Ncurses TextWindo library.
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

#include "CTextWindow.h"

namespace std {

CTextWindow::CTextWindow(int startx/*=0*/, int starty/*=0*/, int width/*=-1*/, int height/*=-1*/, char *name/*=NULL*/) :
	m_pWin(NULL),
	m_iStartX(startx),
	m_iStartY(starty),
	m_iWidth(width),
	m_iHeight(height)
{
	(void)initscr();					/* Start curses mode 									*/
	start_color();						/* Wally's wide wonderful world of color.				*/
	(void)cbreak();						/* Line buffering disabled, Pass on everything to me. 	*/
	(void)keypad(stdscr, TRUE);			/* I need that nifty F2 key in Gnome. 					*/
	(void)noecho();						/* Don't echo() while we do getch.						*/
	(void)curs_set(CURSOR_INVISIBLE);	/* No cursor displayed.									*/
	(void)refresh();
	init_pair(1, COLOR_RED, 	COLOR_WHITE);
	init_pair(2, COLOR_BLUE, 	COLOR_WHITE);
	init_pair(3, COLOR_GREEN, 	COLOR_BLACK);
	init_pair(4, COLOR_WHITE,	COLOR_BLACK);
	m_ColorType1 = COLOR_PAIR(1);
	m_ColorType2 = COLOR_PAIR(2);
	m_ColorType3 = COLOR_PAIR(3);
	m_ColorType4 = COLOR_PAIR(4);
	if ( -1==m_iWidth )
		m_iWidth = COLS;
	if ( -1== m_iHeight )
		m_iHeight = LINES;

	attron(m_ColorType4);
		m_pWin = create_newwin(m_iHeight, m_iWidth, m_iStartY, m_iStartX);
	attroff(m_ColorType4);

	if ( NULL!=name && NULL!=m_pWin ){
		print_in_middle(m_pWin, 0, 0, m_iWidth, name, m_ColorType4);
		// print_in_middle(m_pWin, m_iStartY+m_iHeight-1, m_iStartX, m_iWidth, "Hit F2 to exit", m_ColorType4);
		(void)wrefresh(m_pWin);
	}
}

CTextWindow::~CTextWindow() {
	if ( NULL!=m_pWin ){
		destroy_win(m_pWin);
		m_pWin = NULL;
	}
	endwin();
}

WINDOW *CTextWindow::create_newwin(int height, int width, int starty, int startx){
	WINDOW *local_win = NULL;
	local_win = newwin(height, width, starty, startx);
	(void)box(local_win, 0 , 0);
	(void)wrefresh(local_win);
	return local_win;
}

void CTextWindow::destroy_win(WINDOW *local_win){
	(void)wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	(void)wrefresh(local_win);
	(void)delwin(local_win);
}

int CTextWindow::kbhit(void){
    int ch = getch();
    if ( ch!=ERR ) {
    	(void)ungetch(ch);
        return 1;
    }
    else {
    	return 0;
    }
}

void CTextWindow::print_in_middle(WINDOW *win/*=NULL*/, int starty/*=-1*/, int startx/*=-1*/, int width/*=-1*/, char *string/*=NULL*/, chtype color/*=0*/){
	int length =-1,
		x = -1,
		y= -1;

	int temp = -1;

	if ( NULL==win )
		win = stdscr;

	getyx( win, y, x );

	if ( -1!=startx )
		x = startx;

	if ( -1!=starty )
		y = starty;

	if ( -1==width )
		width = win->_maxx - win->_begx;

	if ( NULL!=string )
		length = strlen(string);
	else
		length = 0;

	temp = (width-length)/2;

	x = startx + (int)temp;

	wattron(win, color);
		mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);

	(void)wrefresh(win);
}

// Very suitable for threading:
void CTextWindow::vHandleWindow(void){
	int c = -1;
	while( KEY_F(2) != c){
		if ( kbhit() )
			c = wgetch(m_pWin);
		else {
			vPrintInformation();
			usleep(10000);
			continue;
		}

		switch(c){
		case KEY_RIGHT:
		case KEY_DOWN:{

		}
		break;
		case KEY_LEFT:
		case KEY_UP:{
		}
		break;
		case KEY_ENTER:{

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
		default:{

			}
			break;
		}
		wrefresh(m_pWin);
	}
}


} /* namespace std */
