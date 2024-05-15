/*
	CTextForm.cpp - a C++ wrapper for the Ncurses TextForm library.
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

#include "CTextForm.h"

namespace std {

CTextForm::CTextForm(int startx/*=0*/, int starty/*=0*/, int width/*=-1*/, int height/*=-1*/, CTextField **pcChoices/*=NULL*/, int iNumChoices/*=0*/, char *lpszMenuName/*=NULL*/) :
	m_apsFields(NULL),
	m_psMyForm(NULL),
	m_iNumFields(iNumChoices),
	m_iStartX(startx),
	m_iStartY(starty),
	m_iWidth(width),
	m_iHeight(height),
	m_paFormFields(NULL),
	m_psMyFormWindow(NULL),
	m_iOldCursor(-1)
{
	if ( NULL!=pcChoices && m_iNumFields ){
		m_apsFields = (FIELD **)calloc(m_iNumFields+1, sizeof(FIELD *));
		m_paFormFields = (CTextField **)calloc(m_iNumFields, sizeof(CTextField *));
		/* Create form items */
		for (int i=0 ; i<m_iNumFields; ++i){
			m_paFormFields[i] = pcChoices[i];

			m_apsFields[i] = m_paFormFields[i]->m_psField;

			(void)set_field_userptr(m_apsFields[i], (char *)m_paFormFields[i]);

		}

		m_apsFields[m_iNumFields] = NULL;

		/* Create the form and post it */
		m_psMyForm = new_form(m_apsFields);

		/* Possibly, calculate the area required for the form */
		if ( -1==m_iHeight || -1==m_iWidth )
			(void)scale_form(m_psMyForm, &m_iHeight, &m_iWidth);

		/* Create the window to be associated with the form */
		m_psMyFormWindow = newwin(m_iHeight+4, m_iWidth+4, 4, 4);

		keypad(m_psMyFormWindow, TRUE);
		nodelay(m_psMyFormWindow, TRUE);

		/* Set main window and sub window */
		set_form_win(m_psMyForm, m_psMyFormWindow);
		set_form_sub(m_psMyForm, derwin(m_psMyFormWindow, m_iHeight, m_iWidth, 2, 2));

		/* Print a border around the main window and print a title */
		box(m_psMyFormWindow, 0, 0);
		CTextWindow::print_in_middle(m_psMyFormWindow, 0, 0, m_iWidth, lpszMenuName, COLOR_PAIR(1));
		CTextWindow::print_in_middle(m_psMyFormWindow, m_iHeight-1, 0, m_iWidth, "(Use UP, DOWN arrow keys to switch between fields.)", COLOR_PAIR(1));

		m_iOldCursor = curs_set(CTextWindow::CURSOR_VISIBLE);

		/* Start at the first field. */
		(void)form_driver(m_psMyForm, REQ_FIRST_FIELD);

		post_form(m_psMyForm);
		wrefresh(m_psMyFormWindow);


		refresh();



	}
}

CTextForm::~CTextForm() {

	if ( NULL!=m_psMyForm){
		unpost_form(m_psMyForm);
		free_form(m_psMyForm);
		(void)delwin(m_psMyFormWindow);
		m_psMyFormWindow = NULL;
		m_psMyForm = NULL;
		// endwin();
		(void)curs_set(m_iOldCursor);
		m_iOldCursor = -1;
	}

	if ( NULL!=m_apsFields ){
		free(m_apsFields);
		m_apsFields = NULL;
	}
	if ( NULL!=m_paFormFields ){
		free(m_paFormFields);
		m_paFormFields = NULL;
	}
}

// Very suitable for threading:
void CTextForm::vHandleForm(void){
	/* Loop through to get user requests */
	int c = -1;
	while( c != KEY_F(2) ){
		c = wgetch(m_psMyFormWindow);
		switch(c)
		{	case KEY_DOWN:{
				/* Go to next field */
				form_driver(m_psMyForm, REQ_NEXT_FIELD);
				/* Go to the end of the present buffer */
				/* Leaves nicely at the last character */
				form_driver(m_psMyForm, REQ_END_LINE);
			}
			break;
			case KEY_UP:{
				/* Go to previous field */
				form_driver(m_psMyForm, REQ_PREV_FIELD);
				form_driver(m_psMyForm, REQ_END_LINE);
			}
			break;
			case KEY_LEFT:{
				form_driver(m_psMyForm, REQ_PREV_CHAR);
			}
			break;
			case KEY_RIGHT:{
				form_driver(m_psMyForm, REQ_NEXT_CHAR);
			}
			break;
			case KEY_BACKSPACE:{
				form_driver(m_psMyForm, REQ_DEL_PREV);
			}
			break;
			case KEY_DC:{
				form_driver(m_psMyForm, REQ_DEL_CHAR);
			}
			break;
			case KEY_IC:
			default:
				/* If this is a normal character, it gets */
				/* Printed				  */
				form_driver(m_psMyForm, c);
				break;
		}
	}
}

} /* namespace std */
