#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

//setari predefinite pentru dimensiunea patratelelor si meniului
#define BOX_SIZE_X 9
#define BOX_SIZE_Y 5
#define MENU_X 30
#define MENU_Y 20

//setari predefinite pentru culori
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define	MAGENTA 5
#define CYAN 6
#define PURPLE 13
#define ORANGE 19
#define PINK 23
#define LIGHT_BLUE 31

//setarile culorilor pentru butoanele selectate si indisponibile
#define HIGHLIGHT_COLOR 200
#define UNAVAILABLE_COLOR 199

//setarile culorilor pentru modul botului
#define BOT_ON 4
#define BOT_OFF 5

#define SPACE_KEY 32


WINDOW *create_win(int size_y, int size_x, int starty, int startx);
void init_wins(WINDOW ***win, int TABLE_DIMENSION);
void init_playtable(int TABLE_DIMENSION);
void init_instructions(int bot_delay, int TABLE_DIMENSION, int color);
void update_color_scheme(int *color_scheme_binary, int color_scheme);
void update_score_window(WINDOW *win, int score);
void update_highscore_window(WINDOW *win, int score);
void convert_color_code(int *color_array, int color);
void print_menu(WINDOW *menu, int select, int resume_state);
void print_settings(WINDOW *win, int bot_delay, int TABLE_DIMENSION, int select, int direction, int color_scheme);
void select_menu(WINDOW *menu, int *select, int *resume_state, int *exit_state, int *pause_state, int *settings_selected);
void select_settings(int *bot_delay, int *TABLE_DIMENSION, int *color_scheme);
void start_game(int **resume_table, int *resume_state, int *pause_state, int *choice, int bot_delay, int TABLE_DIMENSION, int *score, int *highscore, char file_name[10]);
int free_space_check(int **table, int space_number, int TABLE_DIMENSION);
void update_boxes(WINDOW*** boxes, int **table, int TABLE_DIMENSION);
void copy_table(int **source, int **destination, int TABLE_DIMENSION);
int compare_table(int **table1, int **table2, int TABLE_DIMENSION);
int move_up(int **table, int TABLE_DIMENSION, int *score);
int move_down(int **table, int TABLE_DIMENSION, int *score);
int move_left(int **table, int TABLE_DIMENSION, int *score);
int move_right(int **table, int TABLE_DIMENSION, int *score);
int set_next_move(int **current_table, int **undo_table, int TABLE_DIMENSION, int *score);
void show_endgame_screen();

int main(){

	FILE *read_file, *write_file, *settings_file;
	WINDOW *main_menu;
	int color_selection[] =
		{RED, GREEN, YELLOW, BLUE, MAGENTA,
		CYAN, PURPLE, ORANGE, PINK, LIGHT_BLUE};
	int choice = 1, resume_state = 0, exit_selected = 0, settings_selected = 0;
	int game_paused = 1, bot_delay = 1, color_scheme = 0, *color_scheme_binary;
	int old_dimension = 4;
	int index_row;
	int TABLE_DIMENSION = 4;
	int score = 0, highscore = 0;
	int **resume_table, **empty_table;
	char dimension_string[3], file_name[10];

	//citesc setarile salvate
	if ((settings_file = fopen("settings.save", "rb")) != NULL) {
		fread(&TABLE_DIMENSION, sizeof(int), 1, settings_file);
		fread(&bot_delay, sizeof(int), 1, settings_file);
		fread(&color_scheme, sizeof(int), 1, settings_file);
		fclose(settings_file);
	}
	
	color_scheme_binary = (int*)calloc(6, sizeof(int));
	resume_table = (int**)calloc(TABLE_DIMENSION, sizeof(int*));
	empty_table = (int**)calloc(TABLE_DIMENSION, sizeof(int*));
	
	for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {
		resume_table[index_row] = (int*)calloc(TABLE_DIMENSION, sizeof(int));
		empty_table[index_row] = (int*)calloc(TABLE_DIMENSION, sizeof(int));	
	}

	//initializez modul ncurses si culorile, dezactivez cursorul etc.
	initscr();
	start_color();
	cbreak();
	raw();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	init_color(COLOR_BLUE, 300, 300, 300);

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(UNAVAILABLE_COLOR, COLOR_BLUE, COLOR_BLACK);
	init_pair(BOT_ON, COLOR_GREEN, COLOR_BLACK);
	init_pair(BOT_OFF, COLOR_RED, COLOR_BLACK);

	convert_color_code(color_scheme_binary, color_selection[color_scheme]);
	update_color_scheme(color_scheme_binary, color_scheme);

	main_menu = create_win(MENU_Y, MENU_X, 0, 0);
	/*
	stabilesc denumirea fisierului in care salvez/citesc resume + highscore
	pentru o anumita tabla de joc cu formatul "TABLE_DIMxTABLE_DIM.save"
	*/
	sprintf(dimension_string, "%dx%d", TABLE_DIMENSION, TABLE_DIMENSION);
	strcpy(file_name, dimension_string);
	strcat(file_name, ".save");

	//daca fisierul exista iau tabla de resume si scorul salvate
	if ((read_file = fopen(file_name, "rb")) != NULL) {

		fread(&score, sizeof(int), 1, read_file);
		fread(&highscore, sizeof(int), 1, read_file);
		for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {
			fread(resume_table[index_row], sizeof(int),
				  TABLE_DIMENSION, read_file);
		}
		if (compare_table(resume_table, empty_table, TABLE_DIMENSION) == 0) {
			resume_state = 1;
		}
		fclose(read_file);
	}
	
	old_dimension = TABLE_DIMENSION;

	while(exit_selected != 1){
		
		if (game_paused == 0) {
			start_game(resume_table, &resume_state, &game_paused,
					   &choice, bot_delay, TABLE_DIMENSION,
					   &score, &highscore, file_name);
			game_paused = 1;
		}

		else {

			if (settings_selected == 1) {
				select_settings(&bot_delay, &TABLE_DIMENSION, &color_scheme);
				settings_selected = 0;
			}
			else
				select_menu(main_menu, &choice, &resume_state, &exit_selected,
							&game_paused, &settings_selected);
			
			/*
			cand se modifica dimensiunea tablei de joc din setari, se
			redimensioneaza tabla de joc, se incarca fisierul cu salvarea
			corespunzatoare, si se realoca memoria pentru tabla de resume
			*/
			if (old_dimension != TABLE_DIMENSION) {

				sprintf(dimension_string, "%dx%d",
						old_dimension, old_dimension);
				strcpy(file_name, dimension_string);
				strcat(file_name, ".save");

				write_file = fopen(file_name, "wb");

					fwrite(&score, sizeof(int), 1, write_file);
					fwrite(&highscore, sizeof(int), 1, write_file);
					for (index_row = 0;
						 index_row < old_dimension;
						 index_row++) {
						fwrite(resume_table[index_row], sizeof(int),
							   old_dimension, write_file);
					}
					if (compare_table(resume_table,
									  empty_table, old_dimension) == 0) {
						resume_state = 1;
					}
					fclose(write_file);

				resume_state = 0;

				for (index_row = 0; index_row < old_dimension; index_row++) {
					free(empty_table[index_row]);
					free(resume_table[index_row]);
				}

				free(empty_table);
				free(resume_table);

				resume_table = (int**)calloc(TABLE_DIMENSION, sizeof(int*));
				empty_table = (int**)calloc(TABLE_DIMENSION, sizeof(int*));
				
				for (index_row = 0;
					 index_row < TABLE_DIMENSION;
					 index_row++) {
					resume_table[index_row] =
					(int*)calloc(TABLE_DIMENSION, sizeof(int));
					empty_table[index_row] =
					(int*)calloc(TABLE_DIMENSION, sizeof(int));	
				}

				sprintf(dimension_string, "%dx%d",
						TABLE_DIMENSION, TABLE_DIMENSION);
				strcpy(file_name, dimension_string);
				strcat(file_name, ".save");

				if ((read_file = fopen(file_name, "rb")) != NULL) {

					fread(&score, sizeof(int), 1, read_file);
					fread(&highscore, sizeof(int), 1, read_file);
					for (index_row = 0;
						 index_row < TABLE_DIMENSION;
						 index_row++) {
						fread(resume_table[index_row], sizeof(int),
							  TABLE_DIMENSION, read_file);
					}
					if (compare_table(resume_table, empty_table,
									  TABLE_DIMENSION) == 0) {
						resume_state = 1;
					}
					fclose(read_file);
				}
				
				else {
					score = 0;
					highscore = 0;
				}
				
				old_dimension = TABLE_DIMENSION;
			}
		}
	}

	sprintf(dimension_string, "%dx%d", old_dimension, old_dimension);
	strcpy(file_name, dimension_string);
	strcat(file_name, ".save");

	//salveaza tabla in fisierul cu numele specific
	write_file = fopen(file_name, "wb");

	fwrite(&score, sizeof(int), 1, write_file);
	fwrite(&highscore, sizeof(int), 1, write_file);
	for (index_row = 0; index_row < old_dimension; index_row++) {
		fwrite(resume_table[index_row],
			   sizeof(int),
			   old_dimension,
			   write_file);
	}
	fclose(write_file);

	//salveaza setarile in fisierul "settings.save"
	settings_file = fopen("settings.save", "wb");
	fwrite(&TABLE_DIMENSION, sizeof(int), 1, settings_file);
	fwrite(&bot_delay, sizeof(int), 1, settings_file);
	fwrite(&color_scheme, sizeof(int), 1, settings_file);
	fclose(settings_file);

	refresh();
	endwin();
	
	return 0;
}

//creez cele 16 patrate idin matricea boxes
void init_wins(WINDOW*** win, int TABLE_DIMENSION){

	int index_col, index_row;
	for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {

		for (index_col = 0; index_col < TABLE_DIMENSION; index_col++) {
			
			win[index_row][index_col] =
			create_win(BOX_SIZE_Y, BOX_SIZE_X,
					   index_row * (BOX_SIZE_Y + 1) + 1,
					   index_col * (BOX_SIZE_X + 1) + 1);
			wrefresh(win[index_row][index_col]);
		}
	}
}

//functia folosita pentru creearea diverselor tipuri de windowuri
WINDOW *create_win(int size_y, int size_x, int starty, int startx){

	WINDOW *local_win;
	local_win = newwin(size_y, size_x, starty, startx);
	box(local_win, 0 , 0); 
	wrefresh(local_win);
	return local_win;
}

//functia care initializeaza marginile tablei ce contin cele 16 patrate
void init_playtable(int TABLE_DIMENSION){
	
	WINDOW *big_table; 
	int index_row, index_col, width, height;
	
	big_table = create_win((BOX_SIZE_Y + 1) * TABLE_DIMENSION + 1,
						   (BOX_SIZE_X + 1) * TABLE_DIMENSION + 1,
							0,
							0);
	box(big_table, 0, 0);
	getmaxyx(big_table, height, width);
	
	for (index_row = 1; index_row < TABLE_DIMENSION; index_row++) {
		mvwaddch(big_table, index_row * (BOX_SIZE_Y + 1), 0, ACS_LTEE);
		mvwaddch(big_table, index_row * (BOX_SIZE_Y + 1), width - 1, ACS_RTEE);
		mvwaddch(big_table, 0, index_row * (BOX_SIZE_X + 1), ACS_TTEE);
		mvwaddch(big_table, height -1, index_row * (BOX_SIZE_X + 1), ACS_BTEE);
		wrefresh(big_table);
		
		for (index_col = 1; index_col < TABLE_DIMENSION; index_col++) {
			mvwaddch(big_table, index_row * (BOX_SIZE_Y + 1),
					 index_col * (BOX_SIZE_X + 1), ACS_PLUS);
			wrefresh(big_table);
		}
	}

	for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {

		for (index_col = 0; index_col < TABLE_DIMENSION; index_col++) {

			mvwvline(big_table, index_row * (BOX_SIZE_Y + 1) + 1,
					 index_col * (BOX_SIZE_X + 1),
					 ACS_VLINE, BOX_SIZE_Y);
			mvwhline(big_table, index_row * (BOX_SIZE_Y + 1),
					 index_col * (BOX_SIZE_X + 1) + 1,
					 ACS_HLINE, BOX_SIZE_X);
			wrefresh(big_table);
		}
	}
}

//initializarea windowului ce contine instructiunile din timpul jocului
void init_instructions(int bot_delay, int TABLE_DIMENSION, int color){

	WINDOW *instructions_win;
	instructions_win =
	create_win(15, 35, 0, (BOX_SIZE_X + 1) * TABLE_DIMENSION + 4);
	mvwprintw(instructions_win, 1, 1, "Press   to move squares up");
	mvwaddch(instructions_win, 1, 7, ACS_UARROW);
	mvwprintw(instructions_win, 2, 1, "Press   to move squares down");
	mvwaddch(instructions_win, 2, 7, 'v');
	mvwprintw(instructions_win, 3, 1, "Press   to move squares left");
	mvwaddch(instructions_win, 3, 7, ACS_LARROW);
	mvwprintw(instructions_win, 4, 1, "Press   to move squares right");
	mvwaddch(instructions_win, 4, 7, ACS_RARROW);
	mvwprintw(instructions_win, 6, 1, "Press Q to access the pause menu");
	mvwprintw(instructions_win, 7, 1, "Press U to undo one move");
	mvwprintw(instructions_win, 8, 1, "Press B to toggle the AI");
	wattron(instructions_win, COLOR_PAIR(color));
	mvwaddch(instructions_win, 8, 26, ACS_DIAMOND);
	wattron(instructions_win, COLOR_PAIR(1));
	if(color == 4){
		mvwprintw(instructions_win, 10, 1, "If you do not press any button");
		mvwprintw(instructions_win, 11, 1,
				  "in %d seconds, a move will", bot_delay / 10);
		mvwprintw(instructions_win, 12, 1, "be made by the computer");
	}
	wrefresh(instructions_win);

}

//functia care updateaza schema de culoare dupa un vector binar
void update_color_scheme(int *color_scheme_binary, int color_scheme){
	int color_index;

	init_color(HIGHLIGHT_COLOR,
			1000 - 1000 * !color_scheme_binary[0] -
			500 * color_scheme_binary[3] * color_scheme_binary[0],
			1000 - 1000 * !color_scheme_binary[1] -
			500 * color_scheme_binary[4] * color_scheme_binary[1],
			1000 - 1000 * !color_scheme_binary[2] -
			500 * color_scheme_binary[5] * color_scheme_binary[2]);

	for (color_index = 0; color_index <= 4; color_index++) {
		init_color(color_index + 10,
			1000 - 200 * color_index * !color_scheme_binary[0] -
			100 * color_index * color_scheme_binary[3] *
			color_scheme_binary[0],
			1000 - 200 * color_index * !color_scheme_binary[1] -
			100 * color_index * color_scheme_binary[4] *
			color_scheme_binary[1],
			1000 - 200 * color_index * !color_scheme_binary[2] -
			100 * color_index * color_scheme_binary[5] *
			color_scheme_binary[2]);

		init_color(color_index + 15,
			1000 - 1000 * !color_scheme_binary[0] -
			500 * color_scheme_binary[3] * color_scheme_binary[0] -
			100 * color_index * color_scheme_binary[0],
			1000 - 1000 * !color_scheme_binary[1] -
			500 * color_scheme_binary[4] * color_scheme_binary[1] -
			100 * color_index * color_scheme_binary[1],
			1000 - 1000 * !color_scheme_binary[2] -
			500 * color_scheme_binary[5] * color_scheme_binary[2] -
			100 * color_index * color_scheme_binary[2]);

		init_pair(color_index + 10, COLOR_BLACK, color_index + 10);
		if (color_scheme == 0 || color_scheme == 3 ||
			color_scheme == 6 || color_scheme == 9)
			init_pair(color_index + 15, COLOR_WHITE, color_index + 15);
		else
			init_pair(color_index + 15, COLOR_BLACK, color_index + 15);
	}
	
	init_pair(HIGHLIGHT_COLOR, HIGHLIGHT_COLOR, COLOR_BLACK);
	
	refresh();
}

//functia care tine la curent fereastra de scor in functie de variabile score
void update_score_window(WINDOW *win, int score){

	mvwprintw(win, 1, 1, "SCORE:");
	char int_to_string[10];
	sprintf(int_to_string, "%d", score);
	mvwprintw(win, 1, 6 + (17 - strlen(int_to_string))/2, int_to_string);
	wrefresh(win);
}

//ditto update_score_window dar pentru highscore
void update_highscore_window(WINDOW *win, int score){
	mvwprintw(win, 1, 1, "HIGHSCORE:");
	char int_to_string[10];
	sprintf(int_to_string, "%d", score);
	mvwprintw(win, 1, 10 + (13 - strlen(int_to_string))/2, int_to_string);
	wrefresh(win);
}

/*
functia care schimba codul unei culori in vectorul binar folosit de
functia update_color_scheme
*/
void convert_color_code(int *color_array, int color){

	int index = 0;

	while(color != 0){
		color_array[index] = color % 2;
		color = color / 2;
		index++;
	}
	while(index < 6){
		color_array[index]=0;
		index++;
	}
}

//afiseaza meniul cu cele 4 optiuni si avand una selectata
void print_menu(WINDOW *menu_win, int select, int resume_state){
	
	char *choices[] = {"New Game", "Resume", "Settings", "Exit"};
	int n_choices = 4;
	int x,y,index;
	
	box(menu_win, 0, 0);
	getmaxyx(menu_win, y, x);
	y = y / 2 - 2;
	wattron(menu_win,COLOR_PAIR(1));

	for (index = 0; index < n_choices; ++index) { 
		if (select == index + 1) {
			wattron(menu_win,COLOR_PAIR(HIGHLIGHT_COLOR));
			mvwaddch(menu_win, y,
					(x - strlen(choices[index])) / 2 - 2, ACS_BULLET);
			mvwprintw(menu_win, y,
					 (x - strlen(choices[index])) / 2 , "%s", choices[index]);
			mvwaddch(menu_win, y,
					 x - (x - strlen(choices[index])) / 2 + 1 , ACS_BULLET);
			wattron(menu_win,COLOR_PAIR(1));
		}
		else {
			if (index == 1 && resume_state == 0){
				wattron(menu_win,COLOR_PAIR(UNAVAILABLE_COLOR));
				mvwaddch(menu_win, y,
						(x - strlen(choices[index])) / 2 - 2, SPACE_KEY);
				mvwprintw(menu_win, y,
						 (x-strlen(choices[index]))/2, "%s", choices[index]);
				mvwaddch(menu_win, y,
						 x - (x - strlen(choices[index])) / 2 + 1 , SPACE_KEY);
				wattron(menu_win,COLOR_PAIR(1));
			}
			else {
				mvwaddch(menu_win, y,
						(x - strlen(choices[index])) / 2 - 2, SPACE_KEY);
				mvwprintw(menu_win, y,
						 (x-strlen(choices[index]))/2, "%s", choices[index]);
				mvwaddch(menu_win, y,
						 x - (x - strlen(choices[index])) / 2 + 1 , SPACE_KEY);
			}
		}
		y++;
	}
	wrefresh(menu_win);
}

//afiseaza meniul de setari cu cele 4 optiuni
void print_settings(WINDOW *win, int bot_delay, int TABLE_DIMENSION,
					int select, int direction, int color_scheme){

	char *choices[] = {"Table Size", "Automove Delay", "Color Scheme", "Back"};
	int y = MENU_Y, x = MENU_X;
	int index;
	y = y / 2 - 4;
	for (index = 0; index < 4; index++) {
		if (select == index + 1) {
			wattron(win,COLOR_PAIR(HIGHLIGHT_COLOR));
			mvwaddch(win, y,
					(x - strlen(choices[index])) / 2 - 6, ACS_BULLET);
			mvwprintw(win, y,
					 (x - strlen(choices[index])) / 2 - 4, "%s", choices[index]);
			wattron(win,COLOR_PAIR(1));
			if (index != 3) {
				if (direction == -1)
					wattron(win,COLOR_PAIR(HIGHLIGHT_COLOR));
				else
					wattron(win,COLOR_PAIR(1));
				mvwaddch(win, y, 22, ACS_LARROW);
				if (direction == 1)
					wattron(win,COLOR_PAIR(HIGHLIGHT_COLOR));
				else
					wattron(win,COLOR_PAIR(1));
				mvwaddch(win, y, 28, ACS_RARROW);
			}
			wattron(win,COLOR_PAIR(1));
		}
		else {
			mvwaddch(win, y,
					(x - strlen(choices[index])) / 2 - 6, SPACE_KEY);
			mvwprintw(win, y,
					 (x-strlen(choices[index]))/2 - 4, "%s", choices[index]);
			mvwaddch(win, y, 22, SPACE_KEY);
			mvwaddch(win, y, 28, SPACE_KEY);
		}
		if (index == 0)
			mvwprintw(win, y, 25, "%d ", TABLE_DIMENSION);
		if (index == 1)
			mvwprintw(win, y, 25, "%d ", bot_delay / 10);
		if (index == 2)
			mvwprintw(win, y, 25, "%d ", color_scheme + 1);
		y += 2;
		wrefresh(win);
	}
}

/*
functia prin care ofera functionabilitate meniului, care permite schimbarea
butonului selectat si disponibilitatea butonului de resume, precum si
indeplinirea functiilor butoanelor la apsarea tastei ENTER
*/
void select_menu(WINDOW *menu, int *select, int *resume_state, int *exit_state,
				int *pause_state, int *settings_selected){
	
	int ch;

	wrefresh(menu);
	refresh();
	
	do {
		print_menu(menu, *select, *resume_state);
		ch = getch();
		switch (ch) {

			case KEY_UP:
				if (*select == 1)
					break;
				else
					*select -= 1;
				if (*select == 2 && *resume_state == 0)
					*select -= 1;
				print_menu(menu, *select, *resume_state);
			break;

			case KEY_DOWN:
				if (*select == 4)
					break;
				else
					*select += 1;
				if (*select == 2 && *resume_state == 0)
					*select += 1;
				print_menu(menu, *select, *resume_state);
			break;

			case 10:
				if (*select == 1 || *select == 2) {
					*pause_state = 0;
					clear();
					refresh();
				}
				else
					if (*select == 4) {
						*exit_state = 1;
						clear();
						refresh();
					}
					else {
						*settings_selected = 1;
						clear();
						refresh();
					}
			break;

			case 27:
				*exit_state = 1;
				clear();
				refresh();
			break;
		}
	}while(ch != 10);
}

//ditto select_menu dar pentru windowul de setari
void select_settings(int *bot_delay, int *TABLE_DIMENSION, int *color_scheme){

	WINDOW *settings_win = create_win(MENU_Y, MENU_X, 0, 0);
	int choice = 1, direction = 0;
	int ch, exit_settings = 0;
	int color_scheme_binary[6]={0,0,0,0,0,0};
	int old_color_scheme = *color_scheme;
	int color_selection[] =
		{RED, GREEN, YELLOW, BLUE, MAGENTA,
		CYAN, PURPLE, ORANGE, PINK, LIGHT_BLUE};

	
	do {
		direction = 0;
		print_settings(settings_win, *bot_delay, *TABLE_DIMENSION,
					   choice, direction, *color_scheme);
		ch = getch();
		switch (ch) {

			case KEY_UP:
				if (choice == 1)
					break;
				else
					choice -= 1;
				print_settings(settings_win, *bot_delay, *TABLE_DIMENSION,
							   choice, direction, *color_scheme);
			break;

			case KEY_DOWN:
				if (choice == 4)
					break;
				else 
					choice += 1;
				print_settings(settings_win, *bot_delay, *TABLE_DIMENSION,
							   choice, direction, *color_scheme);
			break;

			case KEY_LEFT:
				direction = -1;
				switch (choice) {
					case 1:
						if (*TABLE_DIMENSION > 2)
							*TABLE_DIMENSION -= 1;
					break;

					case 2:
						if (*bot_delay > 1)
							*bot_delay -= 10;
					break;

					case 3:
						if (*color_scheme > 0) {
							*color_scheme -= 1;
						}
					break;
				}

				print_settings(settings_win, *bot_delay, *TABLE_DIMENSION,
							   choice, direction, *color_scheme);
				flushinp();
				usleep(100000);
			break;

			case KEY_RIGHT:
				direction = 1;
				switch (choice) {
					case 1:
						if (*TABLE_DIMENSION < 6)
							*TABLE_DIMENSION += 1;
					break;

					case 2:
						if (*bot_delay < 300)
							*bot_delay += 10;
					break;

					case 3:
						if (*color_scheme < 9){
							*color_scheme += 1;
						}
					break;
				}
				
				print_settings(settings_win, *bot_delay, *TABLE_DIMENSION,
							   choice, direction, *color_scheme);
				flushinp();
				usleep(100000);
			break;

			case 10:
				if (choice == 4)
					exit_settings = 1;
			break;

			case 'q':
				exit_settings = 1;
			break;

			case 27:
				exit_settings = 1;
			break;
		}

		if (old_color_scheme != *color_scheme) {
			
			convert_color_code(color_scheme_binary,
							   color_selection[*color_scheme]);
			update_color_scheme(color_scheme_binary, *color_scheme);
			old_color_scheme = *color_scheme;
		}

	}while (exit_settings == 0);
	clear();
	wrefresh(settings_win);
	refresh();
}

void start_game(int **resume_table, int *resume_state, int *pause_state,
				int *choice, int bot_delay, int TABLE_DIMENSION,
				int *score, int *highscore, char file_name[10]){

	WINDOW ***boxes;
	WINDOW *score_window =
	create_win(3, 23, 15, (BOX_SIZE_X + 1) * TABLE_DIMENSION + 4);
	WINDOW *highscore_window =
	create_win(3, 23, 18, (BOX_SIZE_X + 1) * TABLE_DIMENSION + 4);
	WINDOW *time_window =
	create_win(3, 23, 21, (BOX_SIZE_X + 1) * TABLE_DIMENSION + 4);

	int **current_table, **undo_table, **new_game_table;
	int ch, end_game = 0, random_number, correct_button;
	int box_x, box_y, index;
	int spaces = TABLE_DIMENSION * TABLE_DIMENSION;
	int free_spaces = 0, next_move = 1;
	int undo_score;
	int AI_toggle = 0;
	int alloc_index, index_row, index_col;

	struct tm *time_struct;
	time_t current_time, old_time;
	char time_string[80];

	current_time = time(NULL);
	old_time = current_time;
	time_struct = localtime(&current_time);
	strftime(time_string, 80, "%d %B %Y %H:%M", time_struct);
	mvwprintw(time_window, 1, 1, time_string);
	wrefresh(time_window);
	refresh();

	current_table = (int**)calloc(TABLE_DIMENSION, sizeof(int*));
	undo_table = (int**)calloc(TABLE_DIMENSION, sizeof(int*));
	new_game_table = (int**)calloc(TABLE_DIMENSION, sizeof(int*));
	boxes = (WINDOW***)calloc(TABLE_DIMENSION, sizeof(WINDOW**));

	for (alloc_index = 0; alloc_index < TABLE_DIMENSION; alloc_index++) {
		current_table[alloc_index] =
		(int*)calloc(TABLE_DIMENSION, sizeof(int));
		undo_table[alloc_index] =
		(int*)calloc(TABLE_DIMENSION, sizeof(int));
		new_game_table[alloc_index] =
		(int*)calloc(TABLE_DIMENSION, sizeof(int));
	}

	for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {

		boxes[index_row] = (WINDOW**)calloc(TABLE_DIMENSION, sizeof(WINDOW*));
		for (index_col = 0; index_col < TABLE_DIMENSION; index_col++) {

			boxes[index_row][index_col] =
			(WINDOW*)calloc(TABLE_DIMENSION, sizeof(WINDOW));

		}
	}

	init_playtable(TABLE_DIMENSION);
	init_instructions(bot_delay, TABLE_DIMENSION, BOT_OFF);
	init_wins(boxes, TABLE_DIMENSION);
	
	halfdelay(bot_delay);
	srand((int)time(NULL));

	if (*choice == 1) {
		
		*score = 0;
		undo_score = 0;
		update_score_window(score_window, *score);
		update_highscore_window(highscore_window, *highscore);

		*resume_state = 1;
		copy_table(new_game_table, current_table, TABLE_DIMENSION);
		
		random_number = rand()/(RAND_MAX / spaces + 1);
		box_x = random_number / TABLE_DIMENSION;
		box_y = random_number % TABLE_DIMENSION;

		current_table[box_x][box_y] = (rand()/(RAND_MAX / 2 + 1) + 1) * 2;
		update_boxes(boxes, current_table, TABLE_DIMENSION);

		random_number = rand()/(RAND_MAX / spaces + 1);
		
		while (free_space_check(current_table, random_number,
								TABLE_DIMENSION) == 0) {
			random_number = rand()/(RAND_MAX / spaces + 1);
		}

		box_x = random_number / TABLE_DIMENSION;
		box_y = random_number % TABLE_DIMENSION;

		current_table[box_x][box_y] = (rand()/(RAND_MAX / 2 + 1) + 1) * 2;
		update_boxes(boxes, current_table, TABLE_DIMENSION);
		free_spaces = TABLE_DIMENSION * TABLE_DIMENSION - 2;
	}
	
	if(*choice == 2) {
		
		copy_table(resume_table, current_table, TABLE_DIMENSION);

		undo_score = *score;
		update_score_window(score_window, *score);
		update_highscore_window(highscore_window, *highscore);
		update_boxes(boxes, current_table, TABLE_DIMENSION);

		for (index = 0; index < TABLE_DIMENSION * TABLE_DIMENSION; index++) {
			if (free_space_check(current_table, index, TABLE_DIMENSION) == 1)
				free_spaces++;
		}
	}

	while (end_game == 0 && *pause_state == 0) {

		current_time = time(NULL);
		if (old_time != current_time) {
			old_time = current_time;
			time_struct = localtime(&current_time);
			strftime(time_string, 80, "%d %B %Y %H:%M", time_struct);
			mvwprintw(time_window, 1, 1, time_string);
			wrefresh(time_window);
			refresh();
		}
		correct_button = 0;
		ch = getch();

		switch (ch) {
			
			case KEY_UP:
				undo_score = *score;
				copy_table(current_table, undo_table, TABLE_DIMENSION);
				free_spaces =
				move_up(current_table, TABLE_DIMENSION, score);
				update_boxes(boxes, current_table, TABLE_DIMENSION);
				correct_button = 1;
			break;
			
			case KEY_DOWN:
				undo_score = *score;
				copy_table(current_table, undo_table, TABLE_DIMENSION);
				free_spaces =
				move_down(current_table, TABLE_DIMENSION, score);
				update_boxes(boxes, current_table, TABLE_DIMENSION);
				correct_button = 1;
			break;

			case KEY_LEFT:
				undo_score = *score;
				copy_table(current_table, undo_table, TABLE_DIMENSION);
				free_spaces =
				move_left(current_table, TABLE_DIMENSION, score);
				update_boxes(boxes, current_table, TABLE_DIMENSION);
				correct_button = 1;
			break;

			case KEY_RIGHT:
				undo_score = *score;
				copy_table(current_table, undo_table, TABLE_DIMENSION);
				free_spaces =
				move_right(current_table, TABLE_DIMENSION, score);
				update_boxes(boxes, current_table, TABLE_DIMENSION);
				correct_button = 1;
			break;

			case 'q':
				*pause_state = 1;
				copy_table(current_table, resume_table, TABLE_DIMENSION);
				
			break;

			case 'u':
				*score = undo_score;
				copy_table(undo_table, current_table, TABLE_DIMENSION);
				update_boxes(boxes, current_table, TABLE_DIMENSION);
				correct_button = 1;
			break;

			case 'b':
				if (AI_toggle == 1) {
					halfdelay(-1);
					init_instructions(bot_delay, TABLE_DIMENSION, BOT_OFF);
					AI_toggle = 0;
				}
				else {
					halfdelay(bot_delay);
					init_instructions(bot_delay, TABLE_DIMENSION, BOT_ON);
					AI_toggle = 1;
				}
			break;

			case -1:

				if (AI_toggle == 1) {
					correct_button = 1;
					undo_score = *score;
					copy_table(current_table, undo_table, TABLE_DIMENSION);
					next_move =
					set_next_move(current_table, undo_table,
								  TABLE_DIMENSION, score);
				
					switch (next_move) {

						case 1: 
							free_spaces =
							move_up(current_table, TABLE_DIMENSION, score);
							update_boxes(boxes, current_table,
										 TABLE_DIMENSION);
						break;

						case 2:
							free_spaces =
							move_down(current_table, TABLE_DIMENSION, score);
							update_boxes(boxes, current_table,
										 TABLE_DIMENSION);
						break;

						case 3:
							free_spaces =
							move_left(current_table, TABLE_DIMENSION, score);
							update_boxes(boxes, current_table,
										 TABLE_DIMENSION);
						break;

						case 4:
							free_spaces =
							move_right(current_table, TABLE_DIMENSION, score);
							update_boxes(boxes, current_table,
										 TABLE_DIMENSION);
						break;

						default:
							end_game = 1;
						break;
					}
				}
			break;
		}
		if (*pause_state == 0 &&
			compare_table(current_table, undo_table, TABLE_DIMENSION) == 0 &&
			correct_button == 1) {

			random_number = rand()/(RAND_MAX / spaces + 1);
		
			while (free_space_check(current_table,random_number,
									TABLE_DIMENSION) == 0) {
				random_number = rand()/(RAND_MAX / spaces + 1);
			}

			box_x = random_number / TABLE_DIMENSION;
			box_y = random_number % TABLE_DIMENSION;

			current_table[box_x][box_y] = (rand()/(RAND_MAX / 2 + 1) + 1) * 2;
			update_boxes(boxes, current_table, TABLE_DIMENSION);
		}
		update_score_window(score_window, *score);
		if (*score > *highscore) {
			*highscore = *score;
			update_highscore_window(highscore_window, *highscore);
		}

		if (free_spaces == 0)
			end_game = 1;
		if (end_game == 1) {
			
				end_game = 1;
				*pause_state = 1;
				*resume_state = 0;
				*choice = 1;
				show_endgame_screen();
				while(ch != 10) {
					show_endgame_screen();
					refresh();
					ch = getch();
				}
			copy_table(new_game_table, resume_table, TABLE_DIMENSION);
		}
	}
	clear();
	for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {
							
		for (index_col = 0; index_col < TABLE_DIMENSION; index_col++) {
			
			delwin(boxes[index_row][index_col]);
		}
		free(boxes[index_row]);
	}
	free(boxes);
}

//functia tine la curent patratelele in functie de tabla de joc
void update_boxes(WINDOW*** boxes, int **table, int TABLE_DIMENSION){

	int width = BOX_SIZE_X, height = BOX_SIZE_Y;
	int index_row, index_col;
	char int_to_string[10], oversize_string[10];
	int string_length, is_oversized = 0;
	int clear_x, clear_y;

	for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {

		for (index_col = 0; index_col < TABLE_DIMENSION; index_col++) {
			
			if (table[index_row][index_col] != 0) {
				if (log2(table[index_row][index_col]) <= 10)
					wattron(boxes[index_row][index_col],
						COLOR_PAIR(log2(table[index_row][index_col])+9));
				else
					wattron(boxes[index_row][index_col], COLOR_PAIR(19));
				for (clear_y = 1; clear_y < BOX_SIZE_Y -1; clear_y++) {
					for (clear_x = 1; clear_x < BOX_SIZE_X - 1; clear_x++) {
						mvwaddch(boxes[index_row][index_col],
								 clear_y,
								 clear_x,
								 SPACE_KEY);
						wrefresh(boxes[index_row][index_col]);
					}
				}

				is_oversized = 0;
				sprintf(int_to_string, "%d", table[index_row][index_col]);
				string_length = strlen(int_to_string);
				
				if (string_length > BOX_SIZE_X - 2) {
					strcpy(oversize_string, int_to_string + string_length / 2);
					int_to_string[string_length / 2] = 0;
					string_length = string_length / 2;
					is_oversized = 1;
				}

				move(height / 2,(width - string_length) / 2);
				mvwprintw(boxes[index_row][index_col], height / 2,
						 (width - string_length) / 2, int_to_string);
				
				if (is_oversized == 1) {
					mvwprintw(boxes[index_row][index_col], height / 2 + 1,
							 (width - string_length) / 2, oversize_string);
				}

				wrefresh(boxes[index_row][index_col]);

			}

			else {
				wattron(boxes[index_row][index_col], COLOR_PAIR(1));
				for (clear_y = 1; clear_y < BOX_SIZE_Y -1; clear_y++) {
					for (clear_x = 1; clear_x < BOX_SIZE_X - 1; clear_x++) {
						mvwaddch(boxes[index_row][index_col],
								 clear_y,
								 clear_x,
								 SPACE_KEY);
						wrefresh(boxes[index_row][index_col]);
					}
				}
			}
		}
	}
}

//functia verifica daca casuta cu numarul space_number este libera
int free_space_check(int **table, int space_number, int TABLE_DIMENSION){

	int box_x = space_number / TABLE_DIMENSION;
	int box_y = space_number % TABLE_DIMENSION;

	if (table[box_x][box_y] != 0) return 0;
	return 1;
}

//functia copiaza matricea sursa in destinatie
void copy_table(int **source, int **destination, int TABLE_DIMENSION){
	
	int index_col, index_row;

	for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {

		for (index_col = 0; index_col < TABLE_DIMENSION; index_col++) {
			destination[index_row][index_col] = source[index_row][index_col];
		}
	}
}

//functia compara doua matrice si returneaza 1 daca sunt egale sau 0 daca nu
int compare_table(int **table1, int **table2, int TABLE_DIMENSION){

	int index_col, index_row;

	for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {

		for (index_col = 0; index_col < TABLE_DIMENSION; index_col++) {
			if (table1[index_row][index_col] != table2[index_row][index_col])
				return 0;
		}
	}
	return 1;
}

/*
functiile move muta sau imbina patratelele catre directia aleasa
si returneaza numarul de spatii libere ramase dupa realizarea acestei mutari
*/
int move_up(int **table, int TABLE_DIMENSION, int *score){
	
	int not_changed = 0, alloc_index;
	int index_row, index_col;
	int **switch_table;
	int free_spaces = 0;

	switch_table = calloc(TABLE_DIMENSION, sizeof(int*));
	for (alloc_index = 0; alloc_index < TABLE_DIMENSION; alloc_index++) {
		switch_table[alloc_index] = calloc(TABLE_DIMENSION, sizeof(int));
	}

	while(not_changed == 0){
		
		not_changed = 1;
		
		for(index_col = 0; index_col < TABLE_DIMENSION; index_col++){
			
			for(index_row = 1; index_row < TABLE_DIMENSION; index_row++){
				
				if(table[index_row][index_col] != 0){
					
					if(table[index_row - 1][index_col] == 0){
						
						table[index_row - 1][index_col] =
						table[index_row][index_col];
						table[index_row][index_col] = 0;
						
						not_changed = 0;
					}
					
					else {
						
						if (table[index_row - 1][index_col] ==
							table[index_row][index_col] &&
							switch_table[index_row - 1][index_col] == 0 &&
							switch_table[index_row][index_col] == 0) {
							
							table[index_row - 1][index_col] +=
							table[index_row][index_col];						
							table[index_row][index_col] = 0;

							switch_table[index_row - 1][index_col] = 1;
							
							not_changed = 0;
							*score += table[index_row - 1][index_col];
						}
					}
				}
			}
		}
	}
	for (index_col = 0; index_col < TABLE_DIMENSION; index_col++) {
			
		for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {

			if (table[index_row][index_col] == 0) {
				free_spaces++;
			}
		}
	}
	return free_spaces;
}

int move_down(int **table, int TABLE_DIMENSION, int *score){

	int not_changed = 0, alloc_index;
	int index_row, index_col;
	int **switch_table;
	int free_spaces = 0;

	switch_table = calloc(TABLE_DIMENSION, sizeof(int*));
	for (alloc_index = 0; alloc_index < TABLE_DIMENSION; alloc_index++) {
		switch_table[alloc_index] = calloc(TABLE_DIMENSION, sizeof(int));
	}

	while(not_changed == 0){
		
		not_changed = 1;
		
		for(index_col = 0; index_col < TABLE_DIMENSION; index_col++){
			
			for(index_row = TABLE_DIMENSION - 2; index_row >= 0; index_row--){
				
				if(table[index_row][index_col] != 0){
					
					if(table[index_row + 1][index_col] == 0){
						
						table[index_row + 1][index_col] =
						table[index_row][index_col];
						table[index_row][index_col] = 0;
						
						not_changed = 0;
					}
					
					else {
						
						if (table[index_row + 1][index_col] ==
							table[index_row][index_col] &&
							switch_table[index_row + 1][index_col] == 0 &&
							switch_table[index_row][index_col] == 0) {
							
							table[index_row + 1][index_col] +=
							table[index_row][index_col];
							table[index_row][index_col] = 0;

							switch_table[index_row + 1][index_col] = 1;
							
							not_changed = 0;
							*score += table[index_row + 1][index_col];
						}
					}
				}
			}
		}
	}
	for (index_col = 0; index_col < TABLE_DIMENSION; index_col++) {
			
		for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {
			
			if (table[index_row][index_col] == 0) {
				free_spaces++;
			}
		}
	}
	return free_spaces;
}

int move_left(int **table, int TABLE_DIMENSION, int *score){

	int not_changed = 0, alloc_index;
	int index_row, index_col;
	int **switch_table;
	int free_spaces = 0;

	switch_table = calloc(TABLE_DIMENSION, sizeof(int*));
	for (alloc_index = 0; alloc_index < TABLE_DIMENSION; alloc_index++) {
		switch_table[alloc_index] = calloc(TABLE_DIMENSION, sizeof(int));
	}

	while(not_changed == 0){
		
		not_changed = 1;
		
		for(index_row = 0; index_row < TABLE_DIMENSION; index_row++){
			
			for(index_col = 1; index_col < TABLE_DIMENSION; index_col++){
				
				if(table[index_row][index_col] != 0){
					
					if(table[index_row][index_col - 1] == 0){
						
						table[index_row][index_col - 1] =
						table[index_row][index_col];
						table[index_row][index_col] = 0;
						
						not_changed = 0;
					}
					
					else {
						
						if (table[index_row][index_col - 1] ==
							table[index_row][index_col] &&
							switch_table[index_row][index_col - 1] == 0 &&
							switch_table[index_row][index_col] == 0) {
							
							table[index_row][index_col - 1] +=
							table[index_row][index_col];
							table[index_row][index_col] = 0;

							switch_table[index_row][index_col - 1] = 1;
							
							not_changed = 0;
							*score += table[index_row][index_col - 1];
						}
					}
				}
			}
		}
	}

	for (index_col = 0; index_col < TABLE_DIMENSION; index_col++) {
			
		for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {
			
			if (table[index_row][index_col] == 0) {
				free_spaces++;
			}
		}
	}
	return free_spaces;
}

int move_right(int **table, int TABLE_DIMENSION, int *score){

	int not_changed = 0, alloc_index;
	int index_row, index_col;
	int **switch_table;
	int free_spaces = 0;

	switch_table = calloc(TABLE_DIMENSION, sizeof(int*));
	for (alloc_index = 0; alloc_index < TABLE_DIMENSION; alloc_index++) {
		switch_table[alloc_index] = calloc(TABLE_DIMENSION, sizeof(int));
	}

	while(not_changed == 0){
		
		not_changed = 1;
		
		for(index_row = 0; index_row < TABLE_DIMENSION; index_row++){
			
			for(index_col = TABLE_DIMENSION - 2; index_col >= 0; index_col--){
				
				if(table[index_row][index_col] != 0) {	
					if(table[index_row][index_col + 1] == 0){
						
						table[index_row][index_col + 1] =
						table[index_row][index_col];
						table[index_row][index_col] = 0;
						
						not_changed = 0;
					}
					
					else {
						
						if (table[index_row][index_col + 1] ==
							table[index_row][index_col] &&
							switch_table[index_row][index_col + 1] == 0 &&
							switch_table[index_row][index_col] == 0) {
							
							table[index_row][index_col + 1] +=
							table[index_row][index_col];
							table[index_row][index_col] = 0;

							switch_table[index_row][index_col + 1] = 1;
							
							not_changed = 0;
							*score += table[index_row][index_col + 1];
						}
					}
				}
			}
		}
	}
	for (index_col = 0; index_col < TABLE_DIMENSION; index_col++) {
			
		for (index_row = 0; index_row < TABLE_DIMENSION; index_row++) {
			
			if (table[index_row][index_col] == 0) {
				free_spaces++;
			}
		}
	}
	return free_spaces;
}

//verifica care este miscarea optima din punct de vedere al spatiilor libere
int set_next_move(int **current_table, int **undo_table,
				  int TABLE_DIMENSION, int *score){
	
	int free_spaces_next, choice = 0, max_free_spaces;
	int keep_score = *score;
	
	/*
	daca sunt mai multe miscari cu acelasi numar de casute libere
	ordinea de prioritate este UP, DOWN, LEFT, RIGHT, deci in general
	casutele mari se vor strange in coltul de stanga sus
	*/
	max_free_spaces = move_up(current_table, TABLE_DIMENSION, score);
	if (compare_table(current_table, undo_table, TABLE_DIMENSION) == 0)
		choice = 1;
	
	*score = keep_score;
	copy_table(undo_table, current_table, TABLE_DIMENSION);
	free_spaces_next = move_down(current_table, TABLE_DIMENSION, score);
	
	if (compare_table(current_table, undo_table, TABLE_DIMENSION) == 0) {
		if (max_free_spaces < free_spaces_next || choice == 0) {
			max_free_spaces = free_spaces_next;
			choice = 2;
		}
	}

	*score = keep_score;
	copy_table(undo_table, current_table, TABLE_DIMENSION);
	free_spaces_next = move_left(current_table, TABLE_DIMENSION, score);
	
	if (compare_table(current_table, undo_table, TABLE_DIMENSION) == 0) {
		if (max_free_spaces < free_spaces_next || choice == 0) {
			max_free_spaces = free_spaces_next;
			choice = 3;
		}
	}

	*score = keep_score;
	copy_table(undo_table, current_table, TABLE_DIMENSION);
	free_spaces_next = move_right(current_table, TABLE_DIMENSION, score);
	
	if (compare_table(current_table, undo_table, TABLE_DIMENSION) == 0) {
		if (max_free_spaces < free_spaces_next || choice == 0) {
			max_free_spaces = free_spaces_next;
			choice = 4;
		}
	}
	
	*score = keep_score;
	copy_table(undo_table, current_table, TABLE_DIMENSION);
	return choice;
}

//afiseaza fereastra de game over
void show_endgame_screen(){
	WINDOW *endgame_screen;
	int x, y;
	char string[] = "GAME OVER";
	getmaxyx(stdscr, y, x);
	endgame_screen = create_win(5, 20, (y - 5) / 2, (x - 20) / 2);
	mvwprintw(endgame_screen, 2, 5, string);
	wrefresh(endgame_screen);
	refresh();
}