/*
 * CTextWindow.h
 *
 *  Created on: Aug 2, 2013
 *      Author: kirby
 */

#ifndef CTEXTWINDOW_H_
#define CTEXTWINDOW_H_
#include <stdio.h>
#include <fcntl.h>      /* open */
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

namespace std {

class CTextWindow {
public:
	static const int CURSOR_INVISIBLE 	= 0;	// invisible
	static const int CURSOR_NORMAL 		= 1;	// normal
	static const int CURSOR_VISIBLE		= 2;	// very visible
	CTextWindow(int startx=0, int starty=0, int width=-1, int height=-1, char *name=NULL);
	virtual ~CTextWindow();
	inline void vMoveTextWindow(int height, int width, int starty, int startx){
		if ( NULL!=m_pWin ){
			destroy_win(m_pWin);
			m_pWin = NULL;
		}
		m_pWin = create_newwin(height, width, starty, startx);
	}
	static void print_in_middle(WINDOW *win=NULL, int starty=-1, int startx=-1, int width=-1, char *string=NULL, chtype color=0);
	static int kbhit(void);
	void vHandleWindow(void);
	inline WINDOW *getWinInst(void){
		return m_pWin;
	}
protected:
	virtual void vPrintInformation(void){;}
	chtype m_ColorType1;
	chtype m_ColorType2;
	chtype m_ColorType3;
	chtype m_ColorType4;
private:
	WINDOW *m_pWin;
	int m_iStartX, m_iStartY, m_iWidth, m_iHeight;
	WINDOW *create_newwin(int height, int width, int starty, int startx);
	void destroy_win(WINDOW *local_win);
};

} /* namespace std */
#endif /* CTEXTWINDOW_H_ */
