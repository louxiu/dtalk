#include <ncurses.h>

typedef struct _win_border_struct {
	chtype 	ls, rs, ts, bs, 
	 	tl, tr, bl, br;
}WIN_BORDER;

typedef struct _WIN_struct {

	int startx, starty;
	int height, width;
	WIN_BORDER border;
}WIN;

void init_win_params(WIN *p_win, int height, int width, int starty, int startx);
void print_win_params(WIN *p_win);
void create_box(WIN *win, bool flag);

int main(int argc, char *argv[])
{
    WIN win_rooms, win_input, win_chat;
	int ch;

	initscr();			/* Start curses mode 		*/
	start_color();	    /* Start the color functionality */
	/* cbreak();			/\* Line buffering disabled, Pass on */
    /*                      * everty thing to me 		*\/ */
	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/
	/* noecho(); */
	init_pair(1, COLOR_CYAN, COLOR_BLACK);

	/* Initialize the rooms window parameters */
	init_win_params(&win_rooms, LINES-3, COLS/6, 1, 0);
	print_win_params(&win_rooms);

	attron(COLOR_PAIR(1));
	printw("Press F1 to exit");
	refresh();
	attroff(COLOR_PAIR(1));
	
	create_box(&win_rooms, TRUE);

    /* Initalize the chat window parameters */
	init_win_params(&win_chat, LINES-3-2, COLS/6*5-1, 1, COLS/6);
	print_win_params(&win_chat);

	/* attron(COLOR_PAIR(1)); */
	/* printw("Press F1 to exit"); */
	/* refresh(); */
	/* attroff(COLOR_PAIR(1)); */
	
	create_box(&win_chat, TRUE);
    
    /* Initalize the input window parameters */
	init_win_params(&win_input, 2, COLS/6*5-1, LINES-3-1, COLS/6);
	print_win_params(&win_input);

	/* attron(COLOR_PAIR(1)); */
	/* printw("Press F1 to exit"); */
	/* refresh(); */
	/* attroff(COLOR_PAIR(1)); */
	
	create_box(&win_input, TRUE);

    
	while((ch = getch()) != KEY_F(1))
	{	switch(ch)
		{	case KEY_LEFT:
				/* create_box(&win_rooms, FALSE); */
				/* --win_rooms.startx; */
				/* create_box(&win_rooms, TRUE); */
                refresh();                
				break;
			case KEY_RIGHT:
				/* create_box(&win_rooms, FALSE); */
				/* ++win_rooms.startx; */
				/* create_box(&win_rooms, TRUE); */
                refresh();                                
				break;
			case KEY_UP:
				/* create_box(&win_rooms, FALSE); */
				/* --win_rooms.starty; */
				/* create_box(&win_rooms, TRUE); */
                refresh();                                
				break;
			case KEY_DOWN:
				/* create_box(&win_rooms, FALSE); */
				/* ++win_rooms.starty; */
				/* create_box(&win_rooms, TRUE); */
                refresh();
				break;
            default:
                printf("%c", ch);
		}
	}
	endwin();			/* End curses mode		  */
	return 0;
}
void init_win_params(WIN *p_win, int height, int width, int starty, int startx)
{
	/* p_win->height = LINES - 3; */
	/* p_win->width =  COLS/6; */

	/* p_win->starty = 1;	 */
	/* p_win->startx = 0; */

	p_win->height = height;
	p_win->width =  width;

	p_win->starty = starty;	
	p_win->startx = startx;
    
	p_win->border.ls = '|';
	p_win->border.rs = '|';
	p_win->border.ts = '-';
	p_win->border.bs = '-';
	p_win->border.tl = '+';
	p_win->border.tr = '+';
	p_win->border.bl = '+';
	p_win->border.br = '+';

}
void print_win_params(WIN *p_win)
{
#ifdef _DEBUG
	mvprintw(25, 0, "%d %d %d %d", p_win->startx, p_win->starty, 
             p_win->width, p_win->height);
	refresh();
#endif
}
void create_box(WIN *p_win, bool flag)
{	int i, j;
	int x, y, w, h;

	x = p_win->startx;
	y = p_win->starty;
	w = p_win->width;
	h = p_win->height;

	if(flag == TRUE)
	{	mvaddch(y, x, p_win->border.tl);
		mvaddch(y, x + w, p_win->border.tr);
		mvaddch(y + h, x, p_win->border.bl);
		mvaddch(y + h, x + w, p_win->border.br);
		mvhline(y, x + 1, p_win->border.ts, w - 1);
		mvhline(y + h, x + 1, p_win->border.bs, w - 1);
		mvvline(y + 1, x, p_win->border.ls, h - 1);
		mvvline(y + 1, x + w, p_win->border.rs, h - 1);

	}
	else
		for(j = y; j <= y + h; ++j)
			for(i = x; i <= x + w; ++i)
				mvaddch(j, i, ' ');
				
	refresh();

}
