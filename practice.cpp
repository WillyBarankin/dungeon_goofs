#include <random>
#include <curses.h>

using namespace std;

int getRandomNumber(int param_a, int param_b)
{
	random_device rnd_dv;
	default_random_engine random(rnd_dv());
	uniform_int_distribution<int> distrib(param_a, param_b);

	return(distrib(random));
}

int getSpaceLeft(WINDOW *win_nm, int bs_ch, int box_pos_y, int box_pos_x, int box_height, int box_length) 
{
	int ch_left = 0;
	 
	for (int g = 0; g < box_height; g++)
	{
		for (int k = 0; k < box_length; k++)
		{
			vector<int> ch_ar(box_length);
			ch_ar[k] = (mvwinch(win_nm, box_pos_y+g, box_pos_x+k) & A_CHARTEXT);

			if (ch_ar[k] == bs_ch)
			{
				ch_left++;
			}
		}
	}

	return (ch_left);
}

void launchGame(int row, int col)
{
	start_color();
	use_default_colors();

	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(2, -1, -1);
	init_pair(3, COLOR_WHITE, COLOR_BLUE);
	init_pair(4, COLOR_BLACK, COLOR_WHITE);
	init_pair(5, COLOR_BLACK, COLOR_GREEN);

	int box_length = getRandomNumber(6, col-1);
	int box_height = getRandomNumber(5, row-3);

	WINDOW * g_winPtr = newwin(box_height, box_length, (row - box_height)/2, (col - box_length)/2);
	int g_win_sy, g_win_my, g_win_sx, g_win_mx;
	getparyx(g_winPtr, g_win_sy, g_win_sx);
	getmaxyx(g_winPtr, g_win_my, g_win_mx);

	box(g_winPtr, 1, 1);
	wattron(g_winPtr, COLOR_PAIR(1));
	wborder(g_winPtr, '#' ,'#' ,'#' ,'#' ,'#' ,'#' ,'#' ,'#');
	refresh();
	wrefresh(g_winPtr);

	int obsticles_n = (g_win_mx + g_win_my)/4;

	for (int i=0; i <= obsticles_n; i++)
	{
		wattron(g_winPtr, COLOR_PAIR(4));
		mvwhline(g_winPtr, getRandomNumber(g_win_sy+2, g_win_my-2), getRandomNumber(g_win_sx+2, g_win_mx-5), 'X', getRandomNumber(1, 4));
		mvwvline(g_winPtr, getRandomNumber(g_win_sy+2, g_win_my-4), getRandomNumber(g_win_sx+2, g_win_mx-2), 'X', getRandomNumber(1, 3));
		mvwvline(g_winPtr, getRandomNumber(g_win_sy+2, g_win_my-2), getRandomNumber(g_win_sx+2, g_win_mx-2), 'f' | COLOR_PAIR(5), 1);
		wrefresh(g_winPtr);
	}

	int unit_count  = obsticles_n/2;
	vector<int> char_pos_xx(unit_count);
	vector<int> char_pos_yy(unit_count);
	vector<int> ch(unit_count);

	for (int i = 0; i< unit_count; i++)
	{
		char_pos_yy[i] = getRandomNumber(g_win_sy+2, g_win_my-1);
		char_pos_xx[i] = getRandomNumber(g_win_sx+2, g_win_mx-1);
		ch[i] = (mvwinch(g_winPtr, char_pos_yy[i], char_pos_xx[i]) & A_CHARTEXT);

		while (ch[i] == 35 || ch[i] == 88 || ch[i] == 64 || ch[i] == 102) 
		{
			char_pos_yy[i] = getRandomNumber(g_win_sy+2, g_win_my-1);
			char_pos_xx[i] = getRandomNumber(g_win_sx+2, g_win_mx-1);
			ch[i] = (mvwinch(g_winPtr, char_pos_yy[i], char_pos_xx[i]) & A_CHARTEXT);
		}

		mvwaddch(g_winPtr, char_pos_yy[i], char_pos_xx[i], '@' | A_BOLD | COLOR_PAIR(3));
	}

	while (1)
	{
		for (int j = 0; j < unit_count; j++)
		{
			int old_pos_x = char_pos_xx[j];
			int old_pos_y = char_pos_yy[j];
			char_pos_xx[j] = char_pos_xx[j] + getRandomNumber(-1, 1);
			char_pos_yy[j] = char_pos_yy[j] + getRandomNumber(-1, 1);
			ch[j] = (mvwinch(g_winPtr, char_pos_yy[j], char_pos_xx[j]) & A_CHARTEXT);

			while ((old_pos_x == char_pos_xx[j] && old_pos_y == char_pos_yy[j]) || ch[j] == 88 || ch[j] == 35 || ch[j] == 64)
			{
				char_pos_xx[j] = old_pos_x + getRandomNumber(-1, 1);
				char_pos_yy[j] = old_pos_y + getRandomNumber(-1, 1);
				ch[j] = (mvwinch(g_winPtr, char_pos_yy[j], char_pos_xx[j]) & A_CHARTEXT);
			}

			mvwaddch(g_winPtr, char_pos_yy[j], char_pos_xx[j], '@' | A_BOLD | COLOR_PAIR(3));

			mvwaddch(g_winPtr, old_pos_y, old_pos_x, '.' | COLOR_PAIR(2));
		}

		timeout(150);

		int ch_left = getSpaceLeft(g_winPtr, 32, g_win_sy, g_win_sx, g_win_my, g_win_mx);
		move(row-1, 1);
		printw("Space left: %d", ch_left);
		clrtoeol();
		mvprintw(row-1, col-20, "Press 'q' to stop.", ch_left);
		clrtoeol();
 
		wrefresh(g_winPtr);

		char input = getch();
		if (input == 'q' || ch_left == 0)
		{
			timeout(-1);
			attron(COLOR_PAIR(1));
			mvprintw(row-1, col-20, "Press 'q' to quit.", ch_left);
			attron(COLOR_PAIR(2));
			getch();
			break;
		}
	}

	timeout(-1);
	clear();
}

int showMenu()
{
	char input;
	noecho();

	int y_row, x_col;
	getmaxyx(stdscr, y_row, x_col);

	int m_height, m_width, m_start_x, m_start_y;
	m_height = 6;
	m_width = 15;
	m_start_x = x_col/2-m_width/2;
	m_start_y = y_row/2-m_height/2;

	WINDOW * m_winPtr = newwin(m_height, m_width, m_start_y, m_start_x);

	box(m_winPtr, 0, 0);
	mvwprintw(m_winPtr, 1, 1, "Main menu:");
	mvwprintw(m_winPtr, 2, 1, "1)New game");
	mvwprintw(m_winPtr, 3, 1, "2)Options");
	mvwprintw(m_winPtr, 4, 1, "3)Quit");
	move(y_row-1, x_col-1);
	refresh();
	wrefresh(m_winPtr);

	input = getch();

	switch (input)
	{
		case '1':
			clear();
			mvprintw(y_row/2, x_col/2 - m_width/2, "There's no game.");
			refresh();
			char key;
			key = getch();
			if (key == 'j')
			{
				clear();
				launchGame(y_row, x_col);
				refresh();
				break;
			}
			clear();
			break;

		case '2':
			clear();
			mvprintw(y_row/2 + m_height/2, x_col/2 - m_width/2 - 3, "There are no options.");
			refresh();
			break;

		case '3':
			clear();
			mvprintw(y_row/2, x_col/2, "OK");
			refresh();
			return (0);

		default:
			clear();
			mvprintw(y_row/2 + m_height/2, x_col/2 - m_width/2 + 5, "ERROR");
			refresh();
	}

	showMenu();

	return(0);
}		

int main()
{
	initscr();
	curs_set(0);

	showMenu();
	getch();

	curs_set(1);
	endwin();

	return (0);
}		
