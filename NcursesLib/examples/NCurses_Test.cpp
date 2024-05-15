/*
	NCurses_Test.cpp - a C++ test file for my Ncurses c++ library.
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
#include <ncurses.h>
#include "CTextForm.h"
#include "CTextMenu.h"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4
namespace std {

extern "C" {
	int main(int argc, char *argv[]);
}

static int testMenu();

static int testForm();

static int testWin();

int main(int argc, char *argv[]){

#if 0
	CTextWindow *pcTextWindow = new CTextWindow(0, 0, -1, -1, "Text Window");
	pcTextWindow->vHandleWindow();
	delete pcTextWindow;
	pcTextWindow = NULL;

	// testWin();
	// testForm();
#else
	// (void)testForm();

	CTextField 		sfChc1(1, 25, 1, 1, "Wally's Nickname");
	CNumericField 	sfChc2(1, 25, 3, 1, " Wally's Weight ", 0, 0, INT_MAX);

	CTextField *aptfcChoices[] = {
		&sfChc1,
		(CTextField *)&sfChc2
	};

	CTextFormMenuItem cChc8("Wally's Attributes", NULL, 10, 5, -1, -1, aptfcChoices, 2);

	CTextMenuItem
		scChc1("Barney"),
		scChc2("Daphne"),
		scChc3("Buzzer"),
		scChc4("Kinsey"),
		scChc5("Lessa "),
		scChc6("Rhetta"),
		scChc7("Wally "),
		scChc8("Lucy  ");

		CTextMenuItem *apscChoices[] = {
			&scChc1,
			&scChc2,
			&scChc3,
			&scChc4,
			&scChc5,
			&scChc6,
			&scChc7,
			&scChc8
		};

	CSubMenuItem
		cChc7("Cats", NULL, apscChoices, 8, 5, 5, 22, 15);

	CTextWindowMenuItem
		cChc6("Analog Values", NULL, 5, 5, 20, 10);
	CTextMenuItem
		cChc1("1. AD9858 (WFG) Test  "),
		cChc2("2. AD9858 (ADC) Test  "),
		cChc3("3. AD9854 (CAL) Test  "),
		cChc4("4. Enclosure Temp/Humi"),
		cChc5("5. Complete WFG Test  ");

	CTextMenuItem *apcChoices[9] = {
		&cChc1,
		&cChc2,
		&cChc3,
		&cChc4,
		&cChc5,
		(CTextMenuItem *)&cChc6,
		(CTextMenuItem *)&cChc7,
		(CTextMenuItem *)&cChc8
	};



	CTextWindow *pcTextWindow = new CTextWindow(0, 0, -1, -1, "Test Window");

	CTextMenu *pCTM = new CTextMenu(pcTextWindow, apcChoices, 8, "Test Menu");

	pCTM->vHandleMenu();

	delete pcTextWindow;
	pcTextWindow = NULL;

	delete pCTM;
	pCTM = NULL;
#endif

	return 0;
}

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

char *choices[] =
{
	"Choice 1",
	"Choice 2",
	"Choice 3",
	"Choice 4",
	"Exit",
};

static int testMenu()
{
	ITEM **my_items;
	int c;
	MENU *my_menu;
	int n_choices, i;
	ITEM *cur_item;

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	n_choices = ARRAY_SIZE(choices);
	my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));

	for(i = 0; i < n_choices; ++i)
	        my_items[i] = new_item(choices[i], choices[i]);
	my_items[n_choices] = (ITEM *)NULL;

	my_menu = new_menu((ITEM **)my_items);
	mvprintw(LINES - 2, 0, "F1 to Exit");
	post_menu(my_menu);
	refresh();

	while((c = getch()) != KEY_F(1))
	{   switch(c)
	    {	case KEY_DOWN:
		        menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
		}
	}

	free_item(my_items[0]);
	free_item(my_items[1]);
	free_menu(my_menu);
	endwin();
	return 0;
}

static void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

static int testForm()
{
	FIELD *field[3];
	FORM  *my_form;
	WINDOW *my_form_win;
	int ch, rows, cols;

	/* Initialize curses */
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	/* Initialize few color pairs */
   	init_pair(1, COLOR_RED, COLOR_BLACK);

	/* Initialize the fields */
	field[0] = new_field(1, 10, 6, 1, 0, 0);
	field[1] = new_field(1, 10, 8, 1, 0, 0);
	field[2] = NULL;

	/* Set field options */
	set_field_back(field[0], A_UNDERLINE);
	field_opts_off(field[0], O_AUTOSKIP); /* Don't go to next field when this */
					      /* Field is filled up 		*/
	set_field_back(field[1], A_UNDERLINE);
	field_opts_off(field[1], O_AUTOSKIP);

	// This field type accepts a decimal number. It is set up as follows:
	int padding = 0;
	int vmin=80000000, vmax=160000000;
	int iResult = set_field_buffer (field[0] , 0, "120000000");

	iResult = set_field_type(
		field[0],          /* field to alter */
	    TYPE_NUMERIC,     	/* type to associate */
	    padding,           /* # places of precision */
	    vmin, vmax);   		/* valid range */

	/* Create the form and post it */
	my_form = new_form(field);

	/* Calculate the area required for the form */
	scale_form(my_form, &rows, &cols);

	/* Create the window to be associated with the form */
	my_form_win = newwin(rows + 4, cols + 4, 4, 4);
	keypad(my_form_win, TRUE);

	/* Set main window and sub window */
	set_form_win(my_form, my_form_win);
	set_form_sub(my_form, derwin(my_form_win, rows, cols, 2, 2));

	/* Print a border around the main window and print a title */
    box(my_form_win, 0, 0);
	print_in_middle(my_form_win, 1, 0, cols + 4, "My Form", COLOR_PAIR(1));

	post_form(my_form);
	wrefresh(my_form_win);

	mvprintw(LINES - 2, 0, "Use UP, DOWN arrow keys to switch between fields");
	/* Start at the first field. */
	(void)form_driver(my_form, REQ_FIRST_FIELD);
	refresh();

	/* Loop through to get user requests */
	while((ch = wgetch(my_form_win)) != KEY_F(2))
	{	switch(ch)
		{	case KEY_DOWN:
				/* Go to next field */
				form_driver(my_form, REQ_NEXT_FIELD);
				/* Go to the end of the present buffer */
				/* Leaves nicely at the last character */
				form_driver(my_form, REQ_END_LINE);
				break;
			case KEY_UP:
				/* Go to previous field */
				form_driver(my_form, REQ_PREV_FIELD);
				form_driver(my_form, REQ_END_LINE);
				break;
			case KEY_LEFT:
				form_driver(my_form, REQ_PREV_CHAR);
				break;
			case KEY_RIGHT:
				form_driver(my_form, REQ_NEXT_CHAR);
				break;
			default:
				/* If this is a normal character, it gets */
				/* Printed				  */
				form_driver(my_form, ch);
				break;
		}
	}

	/* Un post form and free the memory */
	unpost_form(my_form);
	free_form(my_form);
	free_field(field[0]);
	free_field(field[1]);

	endwin();
	return 0;
}

static void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{	int length, x, y;
	float temp;

	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;

	length = strlen(string);
	temp = (width - length)/ 2;
	x = startx + (int)temp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);
	refresh();
}

static WINDOW *create_newwin(int height, int width, int starty, int startx);
static void destroy_win(WINDOW *local_win);

static int testWin(void)
{	WINDOW *my_win;
	int startx, starty, width, height;
	int ch;

	initscr();			/* Start curses mode 		*/
	cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

	height = 3;
	width = 10;
	starty = (LINES - height) / 2;	/* Calculating for a center placement */
	startx = (COLS - width) / 2;	/* of the window		*/
	printw("Press F1 to exit");
	refresh();
	my_win = create_newwin(height, width, starty, startx);

	while((ch = getch()) != KEY_F(2))
	{	switch(ch)
		{	case KEY_LEFT:
				destroy_win(my_win);
				my_win = create_newwin(height, width, starty,--startx);
				break;
			case KEY_RIGHT:
				destroy_win(my_win);
				my_win = create_newwin(height, width, starty,++startx);
				break;
			case KEY_UP:
				destroy_win(my_win);
				my_win = create_newwin(height, width, --starty,startx);
				break;
			case KEY_DOWN:
				destroy_win(my_win);
				my_win = create_newwin(height, width, ++starty,startx);
				break;
		}
	}

	endwin();			/* End curses mode		  */
	return 0;
}

static WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

static void destroy_win(WINDOW *local_win)
{
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners
	 * and so an ugly remnant of window.
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window
	 * 3. rs: character to be used for the right side of the window
	 * 4. ts: character to be used for the top side of the window
	 * 5. bs: character to be used for the bottom side of the window
	 * 6. tl: character to be used for the top left corner of the window
	 * 7. tr: character to be used for the top right corner of the window
	 * 8. bl: character to be used for the bottom left corner of the window
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
}



} /* namespace std */
