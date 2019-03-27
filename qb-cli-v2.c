/* qbittorrent command line version */
/* libcurl4-gnutls-dev libjson-c-dev libncurses5-dev libncursesw5-dev */
/* gcc -L/usr/lib/x86_64-linux-gnu qb-cli-v2.c -o qb-cli-v2 -lcurl -ljson-c -lncursesw */
/* valgrind --leak-check=full --track-origins=yes ./qb-cli-v2 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <locale.h>
#include "global.h"
#include "util.c"
#include "net.c"
#include "disp.c"

int main() {
   char TimeBuf[18];
   char EState[50];
   char Input[1];
   int Key;
   Flag.Color = 1;   /* set color screen */

   setlocale(LC_ALL, "en_SG.UTF-8");
   initscr();  /* Initialize ncurses */
   noecho();  /* Disable echo */
   curs_set(FALSE);  /* Hide cursor */
   keypad(stdscr, TRUE);  /* Enable keyboard */

   if(Flag.Color == 0) {
      start_color();
      use_default_colors();
      init_pair(1, COLOR_WHITE, COLOR_BLUE);
      init_pair(2, COLOR_WHITE, COLOR_RED);
   }

   while(1) {
      TimeNow(TimeBuf); /* Get time */
      Interval = (Flag.Init == 0) ? Interval_Default : Interval;
      halfdelay((Flag.Init != 0) ? Interval : 1); /* Set interval timming */
      Flag.ECode = GetData(EState); /* Download data */
      Key = getch();  /* Get keyboard */
      switch(Key) {
         case 'q':	/* quit */
            goto QUIT;
            break;
         case 's':
            Flag.Setting_Drawer = (Flag.Selected == 0 && Flag.Action_Drawer == 0 && Flag.Setting_Drawer == 0) ? 1 : 0;
            break;
         case 'n':
            if(Flag.Setting_Drawer == 1) { Flag.Interval_Action = 1; Flag.Setting_Drawer = 0; }
            break;
         case 'i':
            if(Flag.Setting_Drawer == 1) { Flag.Statistic_Action = 1; Flag.Setting_Drawer = 0; }
            break;
         case 'a':
            Flag.Action_Drawer = (Flag.Selected == 0 && Flag.Setting_Drawer == 0 && Flag.Action_Drawer == 0) ? 1 : 0;
            break;
         case 'p':
            if(Flag.Selected == 0){
               if(Flag.Action_Drawer == 1) { post_pauseAll(); Flag.Action_Drawer = 0; }
            }else{
               if(Flag.Edit_Drawer == 1) { post_pause(Named_Selected); Flag.Edit_Drawer = 0; }
            }
            break;
         case 'r':
            if(Flag.Selected == 0){
               if(Flag.Action_Drawer == 1) { post_resumeAll(); Flag.Action_Drawer = 0; }
            }else{
               if(Flag.Edit_Drawer == 1) { post_resume(Named_Selected); Flag.Edit_Drawer = 0; }
            }
            break;
         case 'e':
            if(Flag.Selected != 0) {
               Flag.Edit_Drawer = (Flag.Edit_Drawer == 0) ? 1 : 0;
            }
            break;
         case 'c':
            if (Flag.Selected != 0 && Flag.Edit_Drawer == 1) {
               Flag.Category_Action = 1;
               Flag.Edit_Drawer = 0;
            }
            break;
         case 'd':
            if (Flag.Selected != 0 && Flag.Edit_Drawer == 1) {
               post_delete(Flag.Selected);
               Flag.Edit_Drawer = 0;
               Flag.Selected = 0;
            }
            break;
         case 'x':
            if(Flag.Selected != 0) {
               if(Flag.Edit_Drawer == 1) { Flag.Edit_Drawer = 0; }
               Flag.Selected = 0;
            }
            Flag.Statistic_Action = (Flag.Statistic_Action == 1) ? 0 : Flag.Statistic_Action;
            break;
         case KEY_RESIZE:  /* Resize */
            Flag.Update = 1;
            break;
         case KEY_UP:
            if( torrent_size != 0 && Flag.Selected == 0) { Name_Highlight = (Name_Highlight == 0) ? 0 : --Name_Highlight; }
            break;
         case KEY_DOWN:
            if( torrent_size != 0 && Flag.Selected == 0) { Name_Highlight = (Name_Highlight == (torrent_size - 1)) ? (torrent_size - 1) : ++Name_Highlight; }
            break;
         case KEY_LEFT:
            if(Flag.Category_Action == 1) { Flag.Category_Selected = (Flag.Category_Selected == 0) ? Flag.Category_Selected = categories_size : --Flag.Category_Selected; }
            break;
         case KEY_RIGHT:
            if(Flag.Category_Action == 1) { Flag.Category_Selected = (Flag.Category_Selected == categories_size) ? Flag.Category_Selected = 0 : ++Flag.Category_Selected; }
            break;
         case 10:
            if( torrent_size != 0) { Named_Selected = Name_Highlight; Flag.Selected = (Flag.Selected != 1) ? 1 : 0; }
            break;
         default:
            break;
      }

      /* update for screen resize */
      if(Flag.Update == 1 || Flag.Init == 0) {
         GetScreen();
         delwin(Name_Window);
         delwin(Status_Window);
         delwin(Pop_Window);

         wclear(stdscr); /* Clear Window */

         Name_Window = newwin(Window_Height, Name_Window_Width, Title, Name_Window_Start_X);
         wclear(Name_Window); /* Clear Window */

         if(Flag.Color == 0) { wbkgd(Name_Window, COLOR_PAIR(1)); }  /* Set background color */

         if(Flag.ECode == 0) {
            Status_Window = newwin(Window_Height, Status_Window_Width, Title, Status_Window_Start_X);
            if(Flag.Color == 0) { wbkgd(Status_Window, COLOR_PAIR(2)); }  /* Set background color */
            wclear(Name_Window); /* Clear Window */
         }

         Flag.Update = 0; /* Reset flag */
         Flag.Init = 1; /* Flag after first initial run */
      }

      /* print program name with bold and underline */
      attron(A_BOLD | A_UNDERLINE);
      mvprintw(0, 0,"%-s",PName);
      attroff(A_BOLD | A_UNDERLINE);
      /* print time */
      mvprintw(0, strlen(PName), "%*s", (Width - strlen(PName) - Edge_Offset), TimeBuf);

      /* Check data status */
      if(Flag.ECode != 0){
         mvwprintw(Name_Window, 1, 0, "%s", EState);
         Print_menu('q');
      }else{
         /* Calculate upper, lower rng and offset */
         int URANGE = (PrintList > torrent_size) ? (torrent_size - 1) : (PrintList - 1);
         int LRANGE = (PrintList > torrent_size) ? URANGE - (torrent_size - 1) : URANGE - (PrintList - 1);
         int OFFSET = (Name_Highlight >= URANGE) ? Name_Highlight - (PrintList - 1) : (Name_Highlight <= LRANGE) ? Name_Highlight : 0;

         Print_Window_Header();
         Print_menu('s');
         Print_Name_Window(PrintList, OFFSET, Name_Highlight);
         Print_Stat_Window(PrintList, OFFSET, Named_Selected);
      }

      wnoutrefresh(stdscr);
      wnoutrefresh(Name_Window);
      wnoutrefresh(Status_Window);

      if(Flag.Setting_Drawer == 1) {
         Print_Setting();
         wnoutrefresh(Pop_Window);
      }
      if(Flag.Action_Drawer == 1) {
         Print_Action();
         wnoutrefresh(Pop_Window);
      }
      if(Flag.Interval_Action == 1){
         echo();
         Print_menu('n');
         getstr(Input);
         if( atoi(Input) > 0 ) { Interval = atoi(Input) * 10; }
         noecho();
         Flag.Interval_Action = 0;
      }
      if(Flag.Statistic_Action == 1) {
         Print_menu('a');
         Print_Statistic();
         wnoutrefresh(stdscr);
         wnoutrefresh(Pop_Window);
      }
      if(Flag.Edit_Drawer == 1) {
         Print_Edit();
         wnoutrefresh(Pop_Window);
      }
      if(Flag.Category_Action == 1) {
         Print_menu('c');
      }
      if(Flag.ECode == 0) {
         freeAll();
      }
      doupdate();
   }

   QUIT:
   endwin();
   exit(0);
}
