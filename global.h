#ifndef _GLOBALH_
#define _GLOBALH_

const char *PName = "qBittorrent CLI Monitor v2";
char *_URL = "http://localhost:8079";     /* url to test site */
char *ref = "";
char **name,***status,***info,**hash,**categories,**server,*sbuf;

char *sta_t[]={"DL(kb/s)","UL(kb/s)","Prog(%)","Ratio", "ULSize"};
char *inf_t[]={"State","Size","Balance","upload","ETA","Seed","Leeach","Category"};
char *srv_t[]={"Alltime DL","Alltime UL","DHT Nodes","Global Ratio"};

const int Max_Name_Length_Default = 40;
const int Interval_Default = 20;
const int Column_Width_Default = 9;
const int Column_Width_Max = 15;
const int Title = 1;
const int Menu = 2;
const int Edge_Offset = 1;

int Width, Height, N_Column, torrent_size, categories_size;
int Name_Window_Width, Status_Window_Width, Status_Window_Start_X, Window_Height, Name_Window_Start_X = 0;
int Max_Name_Length, Interval, Column_Width;
int Name_Highlight = 0, Named_Selected = 0;
int PrintList;

WINDOW *Name_Window, *Status_Window, *Pop_Window;

/* structure with bit fields */
struct {
   unsigned int Init : 1;
   unsigned int Color : 1;
   unsigned int Update : 1;
   unsigned int ECode : 3;
   unsigned int Downloading : 1;
   unsigned int Downloading_Ref : 1;
   unsigned int Completed : 1;
   unsigned int Completed_Ref : 1;
   unsigned int Categories_Error : 1;
   unsigned int Server_Error : 1;
   unsigned int Torrent_Error : 1;
   unsigned int Selected : 1;
   unsigned int Setting_Drawer : 1;
   unsigned int Action_Drawer : 1;
   unsigned int Interval_Action : 1;
   unsigned int Statistic_Action : 1;
   unsigned int Edit_Drawer : 1;
   unsigned int Category_Action : 1;
   unsigned int Category_Selected : 3;
} Flag;

#endif
