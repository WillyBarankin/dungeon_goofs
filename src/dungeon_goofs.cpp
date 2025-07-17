// dungeon_goofs.cpp
// Simple terminal game to demonstrate C++ and ncurses (PDCurses) usage.
// Goofs move around a randomly generated map, eat food, and spawn pseudogoofs.

#include <random>
#include <curses.h>

using namespace std;

// *************** Function Prototypes ************** //

// Returns a random integer between param_a and param_b (inclusive)
int getRandomNumber(int param_a, int param_b);

// Counts the number of spaces (matching bs_ch) left in a given window area
int getSpaceLeft(WINDOW *win_nm, int bs_ch, int box_pos_y, int box_pos_x, int box_height, int box_length);

// Reads the character at a given position in the window and stores it in ch[i]
void getChar(vector<int>& ch, int i, WINDOW* g_winPtr, vector<int>& char_pos_yy, vector<int>& char_pos_xx);

// Launches the main game loop
void launchGame (int row, int col);

// Displays the main menu and handles user input
int showMenu (void);

// ************************************************** //

int main()
{
	initscr(); // Initialize ncurses
	curs_set(0); // Hide cursor

	showMenu(); // Show main menu
	getch(); // Wait for user input before exiting

	curs_set(1); // Restore cursor
	endwin(); // End ncurses mode

	return (0);
} 

// Generate a random integer in the range [param_a, param_b]
int getRandomNumber(int param_a, int param_b)
{
	random_device rnd_dv;
	default_random_engine random(rnd_dv());
	uniform_int_distribution<int> distrib(param_a, param_b);

	return(distrib(random));
}

// Count the number of spaces (bs_ch) left in the specified window area
int getSpaceLeft(WINDOW *win_nm, int bs_ch, int box_pos_y, int box_pos_x, int box_height, int box_length) 
{
	int ch_left = 0;
	 
	for (int g = 0; g < box_height; g++)
	{
		for (int k = 0; k < box_length; k++)
		{
			vector<int> ch_ar(box_length);
			ch_ar[k] = mvwinch(win_nm, box_pos_y+g, box_pos_x+k) & A_CHARTEXT;

			if (ch_ar[k] == bs_ch)
				ch_left++;
		}
	}

	return (ch_left);
}

// Get the character at the specified position and store in ch[i]
void getChar(vector<int>& ch, int i, WINDOW* g_winPtr, vector<int>& char_pos_yy, vector<int>& char_pos_xx)
{
	ch[i] = mvwinch(g_winPtr, char_pos_yy[i], char_pos_xx[i]) & A_CHARTEXT;
}

// Main game logic: draws the map, places goofs, handles movement and food
void launchGame(int row, int col)
{
	start_color();
	use_default_colors();

	// Initialize color pairs for different elements
	init_pair(1, COLOR_BLACK, COLOR_RED);    // Map border
	init_pair(2, -1, -1);                    // Default
	init_pair(3, COLOR_WHITE, COLOR_BLUE);   // Goof
	init_pair(4, COLOR_BLACK, COLOR_WHITE);  // Obstacles
	init_pair(5, COLOR_BLACK, COLOR_GREEN);  // Food

	// Randomly determine map size
	int box_length = getRandomNumber(6, col);
	int box_height = getRandomNumber(5, row-1);

	// Create the game window (map)
	WINDOW * g_winPtr = newwin(box_height, box_length, (row - box_height)/2, (col - box_length)/2);
	int g_win_sy, g_win_my, g_win_sx, g_win_mx;
	getparyx(g_winPtr, g_win_sy, g_win_sx);
	getmaxyx(g_winPtr, g_win_my, g_win_mx);

	// Draw the border
	wattron(g_winPtr, COLOR_PAIR(1));
	wborder(g_winPtr, '#' ,'#' ,'#' ,'#' ,'#' ,'#' ,'#' ,'#');
	refresh();
	wrefresh(g_winPtr);
	
	// Generate obstacles and food
	int obstacles_n = (g_win_mx + g_win_my)/4;

	for (int i=0; i <= obstacles_n; i++)
	{
		wattron(g_winPtr, COLOR_PAIR(4));
		// Place horizontal and vertical obstacles (X)
		mvwhline(g_winPtr, getRandomNumber(g_win_sy+2, g_win_my-2), getRandomNumber(g_win_sx+2, g_win_mx-5), 'X', getRandomNumber(1, 4));
		mvwvline(g_winPtr, getRandomNumber(g_win_sy+2, g_win_my-4), getRandomNumber(g_win_sx+2, g_win_mx-2), 'X', getRandomNumber(1, 3));
		// Place food (f)
		mvwvline(g_winPtr, getRandomNumber(g_win_sy+2, g_win_my-2), getRandomNumber(g_win_sx+2, g_win_mx-2), 'f' | COLOR_PAIR(5), 1);
		wrefresh(g_winPtr);
	}

	// Place goofs (@)
	int unit_count  = obstacles_n/2;
	vector<int> char_pos_xx(unit_count);
	vector<int> char_pos_yy(unit_count);
	vector<int> ch(unit_count);

	for (int i = 0; i< unit_count; i++)
	{
		// Randomly find a free spot for each goof
		char_pos_yy[i] = getRandomNumber(g_win_sy+2, g_win_my-1);
		char_pos_xx[i] = getRandomNumber(g_win_sx+2, g_win_mx-1);
		getChar(ch, i, g_winPtr, char_pos_yy, char_pos_xx);

		// Ensure goof is not placed on obstacle, food, or another goof
		while (ch[i] == (int) '#' || ch[i] == (int) 'X' || ch[i] == (int) '@' || ch[i] == (int) 'f') 
		{
			char_pos_yy[i] = getRandomNumber(g_win_sy+2, g_win_my-1);
			char_pos_xx[i] = getRandomNumber(g_win_sx+2, g_win_mx-1);
			getChar(ch, i, g_winPtr, char_pos_yy, char_pos_xx);
		}

		// Place the goof
		mvwaddch(g_winPtr, char_pos_yy[i], char_pos_xx[i], '@' | A_BOLD | COLOR_PAIR(3));
	}

	// Main game loop: move goofs, handle food, spawn pseudogoofs
	while (1)
	{
		for (int i = 0; i < unit_count; i++)
		{
			int old_pos_x = char_pos_xx[i];
			int old_pos_y = char_pos_yy[i];
			// Move goof randomly by -1, 0, or 1 in each direction
			char_pos_xx[i] = char_pos_xx[i] + getRandomNumber(-1, 1);
			char_pos_yy[i] = char_pos_yy[i] + getRandomNumber(-1, 1);
			getChar(ch, i, g_winPtr, char_pos_yy, char_pos_xx);

			// Prevent goof from moving onto obstacles, other goofs, or pseudogoofs
			while (ch[i] == (int) '#' || ch[i] == (int) 'X' || ch[i] == (int) '@' || ch[i] == (int) '$')
			{
				char_pos_xx[i] = old_pos_x + getRandomNumber(-1, 1);
				char_pos_yy[i] = old_pos_y + getRandomNumber(-1, 1);
				getChar(ch, i, g_winPtr, char_pos_yy, char_pos_xx);
			}

			if ((mvwinch(g_winPtr, old_pos_y, old_pos_x) & A_CHARTEXT) == (int)'$')
			{
				// If leaving a pseudogoof, draw $ and clear old spot
				mvwaddch(g_winPtr, char_pos_yy[i], char_pos_xx[i], '$' | A_BOLD | COLOR_PAIR(3));
				mvwaddch(g_winPtr, old_pos_y, old_pos_x, '.' | COLOR_PAIR(2));
			}
			else if (ch[i] == (int)'f')
			{
				// If goof eats food, spawn a pseudogoof at old position
				mvwaddch(g_winPtr, old_pos_y, old_pos_x, '$' | A_BOLD | COLOR_PAIR(3));
				ch.push_back(unit_count++);
				char_pos_yy.push_back(old_pos_y);
				char_pos_xx.push_back(old_pos_x);
			}
			else
			{
				// Normal move: draw goof at new position, clear old
				mvwaddch(g_winPtr, char_pos_yy[i], char_pos_xx[i], '@' | A_BOLD | COLOR_PAIR(3));
				mvwaddch(g_winPtr, old_pos_y, old_pos_x, '.' | COLOR_PAIR(2));
			}

		}

		// Game status display
		timeout(150); // Delay for animation

		int ch_left = getSpaceLeft(g_winPtr, 32, g_win_sy, g_win_sx, g_win_my, g_win_mx); // Spaces left
		move(row-1, 1);
		printw("Space left: %d\tUnit count: %d", ch_left , unit_count);
		clrtoeol();
		mvprintw(row-1, col-20, "Press 'q' to stop.");
		clrtoeol();
 
		wrefresh(g_winPtr);

		char input = getch();
		if (input == 'q' || ch_left == 0)
		{
			timeout(-1);
			attron(COLOR_PAIR(1));
			mvprintw(row-1, col-20, "Press 'q' to quit.");
			attron(COLOR_PAIR(2));
			getch();
			break;
		}
	}

	timeout(-1);
	clear();
}

// Show the main menu and handle user input
int showMenu()
{
	char input;
	noecho(); // Don't echo user input

	int y_row, x_col;
	getmaxyx(stdscr, y_row, x_col); // Get terminal size

	// Menu window dimensions and position
	int m_height, m_width, m_start_x, m_start_y;
	m_height = 6;
	m_width = 15;
	m_start_x = x_col/2-m_width/2;
	m_start_y = y_row/2-m_height/2;

	WINDOW * m_winPtr = newwin(m_height, m_width, m_start_y, m_start_x);

	// Draw menu
	box(m_winPtr, 0, 0);
	mvwprintw(m_winPtr, 1, 1, "Main menu:");
	mvwprintw(m_winPtr, 2, 1, "1)New game");
	mvwprintw(m_winPtr, 3, 1, "2)Options");
	mvwprintw(m_winPtr, 4, 1, "3)Quit");
	refresh();
	move(y_row-1, x_col-1);
	wrefresh(m_winPtr);

	input = getch();

	switch (input)
	{
		case '1':
			clear();
			mvprintw(y_row/2, x_col/2 - 8, "There's no game.");
			refresh();
			char key;
			key = getch();
			if (key == 'j')
			{
				clear();
				launchGame(y_row, x_col); // Start the game
				refresh();
				break;
			}
			clear();
			break;

		case '2':
			clear();
			mvprintw(y_row/2 + m_height/2, x_col/2 - 10, "There are no options.");
			refresh();
			break;

		case '3':
			clear();
			mvprintw(y_row/2, x_col/2 - 1, "OK");
			refresh();
			return (0);

		default:
			clear();
			mvprintw(y_row/2 + m_height/2, x_col/2 - 2, "ERROR");
			refresh();
	}

	showMenu(); // Show menu again after action

	return(0);
} 
