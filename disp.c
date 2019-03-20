#include "global.h"

/* get screen size & cal printing variable */
void GetScreen() {
   getmaxyx(stdscr, Height, Width); /* <= slower & unreliable */
   N_Column = (Flag.Downloading == 1 && Flag.Completed == 1) ? 4 : 3;
   Column_Width = ((Max_Name_Length + (Column_Width_Default * N_Column)) < Width) ? ((Width - Max_Name_Length) / N_Column) : Column_Width_Default;
   Column_Width = (Column_Width <= Column_Width_Max) ? Column_Width : Column_Width_Max;
   Status_Window_Width = (Column_Width * N_Column) + Right_offset;
   Status_Window_Start_X = Width - Status_Window_Width;
   Name_Window_Width = Width;
   Window_Height = Height - Title - Menu;
}

/* print menu list */
void Print_menu(char m) {
   char main_sym[] = {'s','a','q'};
   char *main_desc[] = {"Setting","Action","Quit"};
   char info_sym[] = {'e','x'};
   char *info_desc[] = {"Edit","Close"};
   char statistic_sym[] = {'x'};
   char *statistic_desc[] = {"Close"};
   char *categories_sym[] = {"\xE2\xA5\xAE", "\xE2\xAE\xA0"};
   char *categories_desc[] = {"Select", "Enter"};
   char quit_sym[] ={'q'};
   char *quit_desc[] = {"Quit"};
   int y = Height - 1;

   /* Print Quit menu */
   if(m == 'q') {
      move((y - 2),0);
      wclrtobot(stdscr);
      attron(A_UNDERLINE);
      mvprintw((y - 1), 0, "%*s\n", Width - 1, " ");
      attroff(A_UNDERLINE);
      for(int i = 0; i < (sizeof(quit_sym)/sizeof(quit_sym[0])); ++i) {
         attron(A_REVERSE);
         mvaddch(y, (i * 15), quit_sym[i]);
         attroff(A_REVERSE);
         mvaddstr(y, ((i * 15) + 2), quit_desc[i]);
      }
   }

   /* Print main menu */
   if(m == 's') {
      move((y - 2),0);
      wclrtobot(stdscr);
      attron(A_UNDERLINE);
      mvprintw((y - 1), 0, "%*s\n", Width - 1, " ");
      attroff(A_UNDERLINE);
      for(int i = 0; i < (sizeof(main_sym)/sizeof(main_sym[0])); ++i) {
         attron(A_REVERSE);
         mvaddch(y, (i * 15), main_sym[i]);
         attroff(A_REVERSE);
         mvaddstr(y, ((i * 15) + 2), main_desc[i]);
      }
   }
   /* Print info menu */
   if(m == 'i') {
      move((y - 2), 0);
      wclrtobot(stdscr);
      attron(A_UNDERLINE);
      mvprintw((y - 1), 0, "%*s\n", Width - 1, " ");
      attroff(A_UNDERLINE);
      for(int i = 0; i < (sizeof(info_sym)/sizeof(info_sym[0])); ++i) {
         attron(A_REVERSE);
         mvaddch(y, (i * 15), info_sym[i]);
         attroff(A_REVERSE);
         mvaddstr(y, ((i * 15) + 2), info_desc[i]);
      }
   }
   /* Print Statistic menu */
   if(m == 'a') {
      move((y - 2), 0);
      wclrtobot(stdscr);
      attron(A_UNDERLINE);
      mvprintw((y - 1), 0, "%*s\n", Width - 1, " ");
      attroff(A_UNDERLINE);
      for(int i = 0; i < (sizeof(statistic_sym)/sizeof(statistic_sym[0])); ++i) {
         attron(A_REVERSE);
         mvaddch(y, (i * 15), statistic_sym[i]);
         attroff(A_REVERSE);
         mvaddstr(y, ((i * 15) + 2), statistic_desc[i]);
      }
   }
   /* Print Interval menu */
   if(m == 'n') {
      move((y - 2), 0);
      wclrtobot(stdscr);
      attron(A_UNDERLINE);
      mvprintw((y-2), 0, "%*s\n", Width - 1, " ");
      attroff(A_UNDERLINE);
      for(int i = 0; i < (sizeof(main_sym)/sizeof(main_sym[0])); ++i) {
         attron(A_REVERSE);
         mvaddch(y, (i * 15), main_sym[i]);
         attroff(A_REVERSE);
         mvaddstr(y, ((i * 15) + 2), main_desc[i]);
      }
      mvprintw((y - 1), 0, "Default[%d],Enter[1-9]: ", Interval/10);
   }
   /* Print Category menu */
   if(m == 'c') {
      move((y - 2), 0);
      wclrtobot(stdscr);
      attron(A_UNDERLINE);
      mvprintw((y - 2), 0, "%*s\n", Width - 1, " ");
      attroff(A_UNDERLINE);
      for(int i = 0; i < (sizeof(categories_sym)/sizeof(categories_sym[0])); ++i) {
         //attron(A_REVERSE);
         mvaddstr(y, (i * 15), categories_sym[i]);
         //attroff(A_REVERSE);
         mvaddstr(y, ((i * 15) + 2), categories_desc[i]);
      }
      mvprintw((y - 1), 0, "Category: %s", categories[Flag.Category_Selected]);
   }
}

/* Print Window Header */
void Print_Window_Header() {
   mvwprintw(Name_Window, 0, 0, "%-*s", Name_Window_Width, "Name");
   if(Flag.Selected == 0){
      if(Flag.Downloading == 1 && Flag.Completed == 1) {
         /* print 4 column */
         mvwprintw(Status_Window, 0, 0, "%-*s%*s%-*s%*s%-*s%*s%-*s", Column_Width, sta_t[0], 1, " ", Column_Width, sta_t[1], 1, " ", Column_Width, sta_t[2], 1, " ", Column_Width, sta_t[3]);
      }else if(Flag.Downloading == 0 && Flag.Completed == 1){
         /* print 3 column with completed */
         mvwprintw(Status_Window, 0, 0, "%*s%*s%*s%*s", Column_Width, sta_t[1], Column_Width, sta_t[3], Column_Width, sta_t[4], Right_offset, " ");
      }else{
         /* print 3 column with download */
         mvwprintw(Status_Window, 0, 0, "%-*s%*s%-*s%*s%-*s", Column_Width, sta_t[0], 1, " ", Column_Width, sta_t[1], 1, " ", Column_Width, sta_t[2]);
      }
   }
}

/* print name window */
void Print_Name_Window(int Selected) {
   if(torrent_size != 0) {
      for(int i = 0; i < torrent_size; ++i) {
         if(Selected == i){
            wattron(Name_Window, A_REVERSE);
            if((Width - Status_Window_Width - 1) < strlen(name[i])) {
               mvwprintw(Name_Window,(i + 1), 0, "%-.*s..", (Width - Status_Window_Width - 4), name[i]);
            }else{
               mvwprintw(Name_Window,(i + 1), 0, "%-s",name[i]);
            }
            wattroff(Name_Window,A_REVERSE);
         }else{
            if((Width - Status_Window_Width - 1) < strlen(name[i])) {
               mvwprintw(Name_Window, (i + 1), 0, "%-.*s..", (Width - Status_Window_Width - 4), name[i]);
            }else{
               mvwprintw(Name_Window, (i + 1), 0, "%-s", name[i]);
            }
         }
      }
   }
}

/* print status window */
void Print_Stat_Window(int Selected) {
   if(torrent_size != 0) {
      if(Flag.Selected == 0){
         for(int i = 0; i < torrent_size; ++i) {
            if(Flag.Downloading == 1 && Flag.Completed == 1) {
               /* print 4 column */
               mvwprintw(Status_Window, (i + 1), 0, "%-*s%*s%-*s%*s%-*s%*s%-*s", Column_Width, status[i][0], 1, " ", Column_Width, status[i][1], 1, " ", Column_Width, status[i][2], 1, " ", Column_Width, status[i][3]);
            }else if(Flag.Downloading == 0 && Flag.Completed == 1){
               /* print 3 column with completed */
               mvwprintw(Status_Window, (i + 1), 0, "%*s%*s%*s%*s", Column_Width, status[i][1], Column_Width, status[i][3], Column_Width, info[i][3], Right_offset, " ");
            }else{
               /* print 3 column with download */
               mvwprintw(Status_Window, (i + 1), 0, "%-*s%*s%-*s%*s%-*s", Column_Width, status[i][0], 1, " ", Column_Width, status[i][1], 1, " ", Column_Width, status[i][2]);
            }
         }
      }else{
         for(int s = 0; s < (sizeof(inf_t)/sizeof(inf_t[0])); ++s) {
            mvwprintw(Status_Window, (s + 1), 0, "%-*s : %-s", 8, inf_t[s], info[Selected][s]);
         }
         Print_menu('i');
      }
   }
}

/* Print Setting Drawer*/
void Print_Setting() {
	const char pop_sym[]={'n','i'};
	const char *pop_desc[]={"Interval","Statistics"};
	int s = (sizeof(pop_sym)/sizeof(pop_sym[0]));
	int y = Height - 2 - (s + 1);
	Pop_Window = newwin((s + 1), 20, y, 0);
	for(int i = 0; i < (sizeof(pop_sym)/sizeof(pop_sym[0])); ++i) {
		wattron(Pop_Window, A_REVERSE);
		mvwaddch(Pop_Window, i, 0, pop_sym[i]);
		wattroff(Pop_Window, A_REVERSE);
		mvwaddstr(Pop_Window, i, 2, pop_desc[i]);
	}
	mvwaddstr(Pop_Window, s, 0, "\xE2\x96\xBC\n");
}

/* Print Action Drawer */
void Print_Action() {
	const char pop_sym[]={'p','r'};
	const char *pop_desc[]={"Pause All","Resume All"};
	int s = (sizeof(pop_sym)/sizeof(pop_sym[0]));
	int y = Height - 2 - (s + 1);
	Pop_Window = newwin((s + 1), 20, y, 15);
	for(int i = 0; i < (sizeof(pop_sym)/sizeof(pop_sym[0])); ++i) {
		wattron(Pop_Window, A_REVERSE);
		mvwaddch(Pop_Window, i, 0, pop_sym[i]);
		wattroff(Pop_Window, A_REVERSE);
		mvwaddstr(Pop_Window, i, 2, pop_desc[i]);
	}
	mvwaddstr(Pop_Window, s, 0, "\xE2\x96\xBC\n");
}

/* Print Statistic */
void Print_Statistic() {
	const int sub_width = 30, sub_height = 10;
	int sub_center_x = (Width / 2) - (sub_width / 2);
	int sub_center_y = (Height / 2) - (sub_height / 2);
	Pop_Window = newwin(sub_height, sub_width, sub_center_y, sub_center_x);
	wborder(Pop_Window,ACS_VLINE,ACS_VLINE,ACS_HLINE,ACS_HLINE,ACS_ULCORNER,ACS_URCORNER,ACS_LLCORNER,ACS_LRCORNER);
	wattron(Pop_Window, A_UNDERLINE);
	mvwprintw(Pop_Window, 1, 1, "Statistic");
	wattroff(Pop_Window, A_UNDERLINE);
	for(int i = 0; i < (sizeof(srv_t)/sizeof(srv_t[0])); ++i) {
		mvwprintw(Pop_Window, ((i * 2) + 2), 1, "%-*s = %*s", 13, srv_t[i], 10, server[i]);
	}
}

/* Print Edit Drawer */
void Print_Edit() {
	char pop_sym[]={'p','r','d','c'};
	char *pop_desc[]={"Pause","Resume","Delete","Category"};
	int s = (sizeof(pop_sym)/sizeof(pop_sym[0]));
	int y = Height - 2 - (s + 1);
	Pop_Window = newwin((s + 1), 20, y, 0);
	for(int i = 0; i < (sizeof(pop_sym)/sizeof(pop_sym[0])); ++i) {
		wattron(Pop_Window, A_REVERSE);
		mvwaddch(Pop_Window, i, 0, pop_sym[i]);
		wattroff(Pop_Window, A_REVERSE);
		mvwaddstr(Pop_Window, i, 2, pop_desc[i]);
	}
	mvwaddstr(Pop_Window, s, 0, "\xE2\x96\xBC\n");
}
