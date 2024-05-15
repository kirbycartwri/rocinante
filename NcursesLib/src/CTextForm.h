/*
	CTextForm.h - a C++ wrapper for the Ncurses TextForm library.
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

#ifndef CTEXTFORM_H_
#define CTEXTFORM_H_
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <limits.h>
#include <curses.h>
#include <menu.h>
#include <form.h>
#include "CTextWindow.h"

namespace std {
class CTextForm;

class CTextField {
	friend class CTextForm;
public:
	CTextField(int height=-1, int width=-1, int starty=-1, int startx=-1, char *desc=NULL) :
		m_psField(NULL)
	{
		bzero(m_achDescription, sizeof(m_achDescription));
		if ( NULL!=desc )
			(void)strncpy(m_achDescription, desc, sizeof(m_achDescription));

		/* Initialize the fields */
		m_psField = new_field(height, width, starty, startx, 0, 0);

	    (void)set_field_just(m_psField, JUSTIFY_CENTER);
		/* Set the field options: 								*/
		(void)set_field_back(m_psField, A_NORMAL);
		(void)set_field_fore(m_psField, A_COLOR);
		/* Don't go to next field when this field is filled up. */
		(void)field_opts_off(m_psField, O_AUTOSKIP);
		/* Initialize field to default value.					*/
		(void)set_field_buffer (m_psField, 0, m_achDescription);

		/* Default to this field type (base class) accepting alphabetic data and digits; no blanks, no special characters. */
		(void)set_field_type( m_psField, TYPE_ALNUM, width);
	}

	inline char *lpszDesc(void){
		return m_achDescription;
	}
	virtual ~CTextField(){
		if ( NULL!=m_psField )
			free_field(m_psField);
		m_psField = NULL;
		bzero(m_achDescription, sizeof(m_achDescription));
	}
protected:
	FIELD *m_psField;
	char m_achDescription[25];
	virtual void vHandleField(void){
		(void)strncpy( m_achDescription, field_buffer (m_psField, 0), sizeof(m_achDescription));
	}
private:
};

class CStaticField : public CTextField {
	friend class CTextForm;
public:
	CStaticField(int height=-1, int width=-1, int starty=-1, int startx=-1, char *desc=NULL) :
		CTextField(height, width, starty, startx, desc)
	{
		(void)set_field_just(m_psField, JUSTIFY_RIGHT);
		(void)field_opts_off(m_psField, O_EDIT);
		(void)set_field_back(m_psField, A_NORMAL);
		(void)set_field_fore(m_psField, A_BOLD);
	}
	virtual ~CStaticField(){
		;
	}
protected:
private:
};

class CNumericField : public CTextField {
	friend class CTextForm;
public:
	CNumericField(int height, int width, int starty, int startx, char *desc=NULL, int padding=0, int vmin=0, int vmax=INT_MAX) :
		CTextField(height, width, starty, startx, desc),
		m_dValue(0.0)
	{
		(void)set_field_type( m_psField, TYPE_NUMERIC, padding, vmin, vmax);
		if ( NULL!=desc )
			m_dValue = atof(desc);
	}
	virtual ~CNumericField(){
		;
	}
	inline double dGetValue(void){
		return m_dValue;
	}
protected:
	virtual void vHandleField(void){
		CTextField::vHandleField();
		m_dValue = atof(m_achDescription);
	}
private:
	double m_dValue;
};

class CTextForm {
public:
	CTextForm(int startx=0, int starty=0, int width=-1, int height=-1, CTextField **pcChoices=NULL, int iNumChoices=0, char *lpszMenuName=NULL);
	virtual ~CTextForm();
	void vHandleForm(void);
protected:
private:
	FIELD **m_apsFields;
	FORM  *m_psMyForm;
	int m_iNumFields;
	int m_iStartX, m_iStartY, m_iWidth, m_iHeight;
	CTextField **m_paFormFields;
	char m_achFormName[25];
	WINDOW *m_psMyFormWindow;
	int m_iOldCursor;
};

} /* namespace std */

#endif /* CTEXTFORM_H_ */
