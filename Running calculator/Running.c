#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "stack.h"
#include <iup.h>
#include <iup_config.h>

/*****************************************************************#DEFINES*****************************************************************/
/*RUNNING CALCS DEFINITIONS*/
#define MINTOSEK 60.00                            // minute to seconds
#define HTOMIN 60.00                              // hour to minutes
#define HTOSEK 3600                               // hour to seconds
#define LIMIT 200.00                              // maximum distance in km
#define LIMITHOURS 24                             // maximum time in hours 
#define LIMITMINUTES 60                           // maximum time in minutes 
#define LIMITSECONDS 60                           // maximum time in seconds
#define TEMPSIZETIME 3


#define MAXSTACK 10
/*GENERAL DEFINITIONS*/
#define FILL  "30"                                // iupfill size 
#define RESULTLBLSIZE "60"                        // size of cell in database
#define RESULTLBLALIGN "ACENTER"                  // alignment of text within cell in database
#define PACECALCSIZE "60"                         // size of cell in pace calculations
#define PACECALCALIGN "ACENTER"                   // alignment of text within cell in pace calculations
#define TIMEINPSIZE "60"                          // size of cell in time input dialog
#define TIMEINPALIGN "ACENTER"                    // alignment of text within cell in time input dialog
#define MORERESSIZE "60"                          // size of cell in more results dialog
#define MORERESALIGN "ACENTER"                    // alignment of text within cell in more results dialog
#define TITLESIZE "21"                            // size of the title in database
#define TITLESIZE2 21                             // size of the title in database
#define DISTANCESIZE 8                            // size of the title in database
#define TIMESIZE 9                                // size of the title in database
#define SAVESTRSIZE 40                            // size of the string saved in database (roughly calculated by adding components)
#define SKIPLINESIZE 60                           // size of the string which is read from file and then skipped
#define DATEMRSTRSIZE 11                          // size of the title in database
#define TIMEMRSTRSIZE 9                          // size of the title in database
#define TIMEPERKMMRSTRSIZE 7                     // size of the title in database
#define SPEEDMRSTRSIZE 7                          // size of the title in database

/*CONFIG*/
#define FILLAREAPOS 0                             // containers order in database
#define POSITIONPOS 1
#define TITLEPOS 2
#define LENGTHPOS 3
#define TIMEPOS 4
#define MORERESPOS 5
#define MAPPOS 6
#define SELECTPOS 7
#define DELETEPOS 8
#define DBHBOXOFFSETSTART 6                       // offset to the first database hbox from the start of vbox
#define DBHBOXOFFSETEND 2                         // offset to the first database hbox from the end of vbox
#define MRHBOXOFFSETSTART 1                       // offset to the first more results hbox from the start of vbox
#define VBOXELCOUNTINI 8                          // offset to the first database hbox from the start of vbox
#define DATEMRPOS 0                               // containers order in database for more results dialog
#define TIMEMRPOS 1
#define TIMEPERKMMRPOS 2
#define SPEEDMRPOS 3
#define DELETEMRPOS 4

/*CREATION OF NEW ENTRIES*/
#define FILLAREA(X) fillarea ## x
#define POSITION(X) position ## x
#define TITLE(X) title ## x
#define LENGTH(X) length ## x
#define TIME(X) time ## x
#define MORERES(n) moreres ## x
#define MAP(X) map ## x
#define SELECTBTN(X) selectbtn ## x
#define DELETE(X) delete ## x 
#define HBOX(X) hbox ## x

/*CREATION OF NEW ENTRIES IN "MORE RESULTS" TAB*/
#define DATEMR(X,Y) datemr ## x ## y
#define TIMEMR(X,Y) timemr ## x ## y
#define TIMEPERKMMR(X,Y) timeperkmmr ## x ## y
#define SPEEDMR(X,Y) speedmr ## x ## y
#define DELETEMR(X,Y) deletemr ## x ## y 
#define HBOXMR(X,Y) hboxmr ## x ## y

/*CONTROL COMMENTS DEFINISIONS*/                  // if you want to activate particular check instruction just uncomment the appropriate position
#define ADRESSESSCHECK                         // print adressess
// #define DISTANCECHECK                      
// #define OPENFILESTR
#define DIRTYSTATUS
// #define DBLDBCHECK                             // Checking the db in case of doubles
// #define TIMEINPUTCHECK                         // Checking the caller status - button or keybord 
// #define STACKCONTENT                              // Checking the content of stacks
// #define BESTTIMECHECK

/*****************************************************************Prototypes of function*****************************************************************/
/*UTILITIES*/
char *read_file(char * st, FILE* file);          // reading data from file
void write_file(const char* filename, const char* str, unsigned int rowlen, FILE * file);      //saving data into file
bool save_file(Ihandle *self);                    // called before exit; save the database onto the file
bool renumberpostitl(Item id, Ihandle * vbox);    // renumber titles of position in the database
void set_besttime(Ihandle *besttimedb, Ihandle *mrvbox);      // function sets the best time among all times related to particular distance into appropriate container in db
void generatestack(Item id);                                  // function which is used for creation of new stack used for controlling amount of more results tab entries
void stackscontent(void);                                     // prints the content and addresses of the stacks - for control purposes

/*OPEN & EXIT PROGRAM*/
int open_cb(Ihandle *self, int state);            // procedures activated when the program is starting
int exit_cb(Ihandle *self);                       // procedures activated when user click close button

/*TOOLBAR OPTIONS*/
int item_new_action_cb(Ihandle *self);            // creates new file
int item_open_action_cb(Ihandle *self);           // open file
int item_saveas_action_cb(Ihandle *self);         // save as 

/*CREATION OF NEW ENTRIES*/
int item_createnew_cb(Ihandle *self);             // adding new item into the list
int create_newentry_cb(Ihandle *self);            // create new entry in database
int confirm_newentry_cb(Ihandle *self);           // confirm the correctness of given data while creating new entry
int close_newentry_cb(Ihandle* btncancel);        // close the create new entry dialog
int addtodb_cb(Ihandle *self);                    // adding data into the database from the pace calculator
int getddate(Ihandle *self);                        // function creates new dialog which allows for adding date to existing entry
int getdateandtitle(Ihandle *self);               // function creates new dialog which allows for adding date and title to entry
int calendar_cb(Ihandle* self);                   // function creates calendar which allows the user to pick up the date
int confirm_calendar_cb(Ihandle* self);           // function puts the selected date in appopriate container
int getback_addtodb_cb(Ihandle *self);            // function checks the whether the user input does exist and then hide the input dialog (returns control to addtodb_cb)

/*MANAGEMENT FUNCTIONS OF ENTRIES IN DATABASE*/
int item_select_cb(Ihandle *self);                // select item from the list and inserting its distance into the pace calculator
int item_delete_cb(Ihandle *self);                // deleting item from the list
int moreresults_cb(Ihandle *self);                // creates dialog containing more results related to given distance
int moreresults_closedia_cb(Ihandle *self);       // hides more result dialog
int moreres_createnew_cb(Ihandle *self);          // creates new row of data in more results tab
int show_moreresultsdlg_cb(Ihandle *self);        // display dialog containing more results
int moreres_delete_cb(Ihandle *self);             // delete entry in more results tab

/*PACE CALCULATIONS*/
int calculate_running_cb(Ihandle *self);          // calculate all running parameters

/*TIME INPUT DIALOG*/       
int timeinput_cb(Ihandle *self, int c);           // creates the time input dialog  
int timeinput_buttonok_cb(Ihandle *self);         // set the time input into the main dialog

/*CHECKS OF INPUT DATA*/
int checkdata_distance_cb(Ihandle *self);         // check the distance against the limits
int distance_dcmplcs_cb(Ihandle *self);           // setting appropriate format of distance
int checkdata_time_cb(Ihandle *self);             // check the time against the limits
bool label_check(Ihandle *self);                  // called before exit; check whether all title cells are filled; returns false when not
int nodouble_label_check(Ihandle *self);          // checks whether the title of entry one try to add is already in db;
unsigned int nodouble_db_check(char * dist, Ihandle *vboxmain, unsigned int lasthboxnr);                //check whether the new added parameters: length and time are already in the database 
bool nodouble_dbne_check(char * dist, char *nm, Ihandle *vboxmain, unsigned int latesthboxnr);    //check whether the new added parameters in create_newentry: title and length are already in the database 

/*ICONS*/
static Ihandle* create_image_addicon(void);       // create the icon image for Add
static Ihandle* create_image_locationicon(void);  // create the icon image for Location mark
static Ihandle* create_image_checkicon(void);     // create the icon image for Check 

/*****************************************************************OTHER*****************************************************************/
/*GLOBAL VARIABLES*/
Stack st;                                         // global variable, reference to stack with numbers - used for creation of sets of variables
Stack *stacktab[MAXSTACK];                        // table containing pointers to stacks used for controlling amount of more results tab entries

int main(int argc, char **argv)
{
  //running pace declarations
  Ihandle  *labelDistance, *labelTime, *labelTimeperkm, *labelSpeed, *distance, *time, *timeperkm, *speed, *btncalc, *btnadd, *fillarea1;
  //dialog declarations
  Ihandle *dlg, *hbox1, *hbox2, *vbox, *separatorhor, *signature, *hboxsignature;
  Ihandle *toolbar_head, *btn_new, *btn_open, *btn_saveas;
  //database declarations
  Ihandle *btn_add, *toolbar_db, *labelnumber, *labelname, *labellength, *labeltime, *labelmoreres, *labelmap, *labelselect, *labeldelete, *hboxreslab, *fillarea2, *fillarea3, *vboxfill; 
  Ihandle *config;

  /*********** startup commands *************/
  InitializeStack(&st);
  IupOpen(&argc, &argv);
  IupImageLibOpen();

  config = IupConfig();
  IupSetAttribute(config, "APP_NAME", "PACECALC");
  IupConfigLoad(config);

  create_image_addicon();   // creates Add image
  create_image_locationicon();
  create_image_checkicon();

  /*************************/
  /*TOOLBAR*/
  btn_new = IupButton(NULL, NULL);
  IupSetAttribute(btn_new, "IMAGE", "IUP_FileNew");
  IupSetAttribute(btn_new, "FLAT", "Yes");
  IupSetAttribute(btn_new, "TIP", "New (Ctrl+N)");
  IupSetAttribute(btn_new, "CANFOCUS", "No");
  IupSetCallback(btn_new, "ACTION", (Icallback)item_new_action_cb);

  btn_open = IupButton(NULL, NULL);
  IupSetAttribute(btn_open, "IMAGE", "IUP_FileOpen");
  IupSetAttribute(btn_open, "FLAT", "Yes");
  IupSetAttribute(btn_open, "TIP", "Open (Ctrl+O)");
  IupSetAttribute(btn_open, "CANFOCUS", "No");
  IupSetCallback(btn_open, "ACTION", (Icallback)item_open_action_cb);

  btn_saveas = IupButton(NULL, NULL);
  IupSetAttribute(btn_saveas, "IMAGE", "IUP_FileSave");
  IupSetAttribute(btn_saveas, "FLAT", "Yes");
  IupSetAttribute(btn_saveas, "TIP", "Save As (Ctrl+S)");
  IupSetAttribute(btn_saveas, "CANFOCUS", "No");
  IupSetCallback(btn_saveas, "ACTION", (Icallback)item_saveas_action_cb);

  toolbar_head = IupHbox(
    btn_new,
    btn_open,
    btn_saveas,
    NULL);

  IupSetAttribute(toolbar_head, "MARGIN", "5x5");
  IupSetAttribute(toolbar_head, "GAP", "2");

  /*HBOX1*/
  labelDistance = IupLabel("Distance\n[km]");
  IupSetAttribute(labelDistance, "SIZE", PACECALCSIZE);
  IupSetAttribute(labelDistance, "ALIGNMENT", PACECALCALIGN);

  labelTime = IupLabel("Time\n[hrs:min:sec]");
  IupSetAttribute(labelTime, "SIZE", PACECALCSIZE);
  IupSetAttribute(labelTime, "ALIGNMENT", PACECALCALIGN);

  labelTimeperkm = IupLabel("Time per km\n[min:sec/km]");
  IupSetAttribute(labelTimeperkm , "SIZE", PACECALCSIZE);
  IupSetAttribute(labelTimeperkm , "ALIGNMENT", PACECALCALIGN);

  labelSpeed = IupLabel("Speed\n[km/h]");
  IupSetAttribute(labelSpeed, "SIZE", PACECALCSIZE);
  IupSetAttribute(labelSpeed, "ALIGNMENT", PACECALCALIGN);

  fillarea1 = IupFill();
  IupSetAttribute(fillarea1, "SIZE", FILL);

  fillarea2 = IupFill();
  IupSetAttribute(fillarea2, "SIZE", "20"); // set by hand to match the position of containers in hbox2

  hbox1 = IupHbox(
    IupFill(),
    labelDistance,
    labelTime,
    fillarea1,
    labelTimeperkm,
    labelSpeed,
    fillarea2,
    IupFill(),
    NULL);

  IupSetAttribute(hbox1, "MARGIN", "10x0");
  IupSetAttribute(hbox1, "GAP", "0");
  IupSetAttribute(hbox1, "ALIGNMENT", "ACENTER");
  
   /*HBOX2*/
  btncalc = IupButton("Calc", NULL);
  IupSetAttribute(btncalc, "SIZE", "30");
  IupSetCallback(btncalc, "ACTION", (Icallback)calculate_running_cb);

  distance = IupText(NULL);
  IupSetAttribute(distance, "SIZE", PACECALCSIZE);
  IupSetAttribute(distance, "NAME", "DISTANCE");
  IupSetAttribute(distance, "MASK", IUP_MASK_FLOAT);
  IupSetCallback(distance, "CARET_CB", (Icallback)checkdata_distance_cb);
  IupSetAttribute(distance, "VALUE", "");
  IupSetCallback(distance, "KILLFOCUS_CB", (Icallback)distance_dcmplcs_cb);

  time = IupFlatButton("0:0:0");
  IupSetAttribute(time, "SIZE", PACECALCSIZE);
  IupSetAttribute(time, "ALIGNMENT", PACECALCALIGN);
  IupSetAttribute(time, "NAME", "TIME");
  IupSetCallback(time, "FLAT_ACTION", (Icallback)timeinput_cb);

  timeperkm = IupLabel("");
  IupSetAttribute(timeperkm, "SIZE", PACECALCSIZE);
  IupSetAttribute(timeperkm, "ALIGNMENT", PACECALCALIGN);
  IupSetAttribute(timeperkm, "NAME", "TIMEPERKM");
  IupSetAttribute(timeperkm, "ACTIVE", "NO");

  speed = IupLabel("");
  IupSetAttribute(speed, "SIZE", PACECALCSIZE);
  IupSetAttribute(speed, "ALIGNMENT", PACECALCALIGN);
  IupSetAttribute(speed, "NAME", "SPEED");
  IupSetAttribute(speed, "ACTIVE", "NO");

  btnadd = IupButton(NULL, NULL);
  IupSetAttribute(btnadd, "IMAGE", "IMGADD");
  IupSetAttribute(btnadd, "FLAT", "Yes");
  IupSetCallback(btnadd, "ACTION", (Icallback)addtodb_cb);
  IupSetAttribute(btnadd, "TIP", "Add new trail");
  IupSetAttribute(btnadd, "CANFOCUS", "No");
  
  hbox2 = IupHbox(
    IupFill(),
    distance,
    time,
    btncalc,
    timeperkm,
    speed,
    btnadd,
    IupFill(),
    NULL);

  IupSetAttribute(hbox2, "MARGIN", "10x0");
  IupSetAttribute(hbox2, "GAP", "0");
  IupSetAttribute(hbox2, "ALIGNMENT", "ACENTER");

 /*********** running database  *************/

 /*DB TOOLBAR*/
  btn_add = IupButton(NULL, NULL);
  IupSetAttribute(btn_add, "IMAGE", "IMGADD");
  IupSetAttribute(btn_add, "FLAT", "Yes");
  IupSetCallback(btn_add, "ACTION", (Icallback)create_newentry_cb);
  IupSetAttribute(btn_add, "TIP", "Add new trail");
  IupSetAttribute(btn_add, "CANFOCUS", "No");

  toolbar_db = IupHbox(
  btn_add,
  IupFill(),
  NULL);

  IupSetAttribute(toolbar_db, "MARGIN", "5x5");
  IupSetAttribute(toolbar_db, "GAP", "2");
  IupSetAttribute(toolbar_db, "VISIBLE", "YES");

 /*LABELS*/
  labelnumber = IupLabel("Nr.");
  IupSetAttribute(labelnumber, "SIZE", RESULTLBLSIZE);
  IupSetAttribute(labelnumber, "ALIGNMENT", RESULTLBLALIGN);

  labelname = IupLabel("Name");
  IupSetAttribute(labelname, "SIZE", RESULTLBLSIZE);
  IupSetAttribute(labelname, "ALIGNMENT", RESULTLBLALIGN);

  labellength = IupLabel("Distance\n[km]");
  IupSetAttribute(labellength , "SIZE", RESULTLBLSIZE);
  IupSetAttribute(labellength , "ALIGNMENT", RESULTLBLALIGN);

  labeltime = IupLabel("Best time\n[hrs:min:sec]");
  IupSetAttribute(labeltime, "SIZE",RESULTLBLSIZE);
  IupSetAttribute(labeltime, "ALIGNMENT", RESULTLBLALIGN);

  labelmoreres = IupLabel("More\nresults");
  IupSetAttribute(labelmoreres, "SIZE",RESULTLBLSIZE);
  IupSetAttribute(labelmoreres, "ALIGNMENT", RESULTLBLALIGN);

  labelmap = IupLabel("Map");
  IupSetAttribute(labelmap, "SIZE", RESULTLBLSIZE);
  IupSetAttribute(labelmap, "ALIGNMENT", RESULTLBLALIGN);

  labelselect = IupLabel("Select");
  IupSetAttribute(labelselect, "SIZE", RESULTLBLSIZE);
  IupSetAttribute(labelselect, "ALIGNMENT", RESULTLBLALIGN);

  labeldelete = IupLabel("Delete");
  IupSetAttribute(labeldelete, "SIZE", RESULTLBLSIZE);
  IupSetAttribute(labeldelete, "ALIGNMENT", RESULTLBLALIGN);

  fillarea3 = IupFill();
  IupSetAttribute(fillarea3, "SIZE", FILL);

  hboxreslab = IupHbox(
    fillarea3,
    labelnumber,
    labelname,
    labellength,
    labeltime,
    labelmoreres,
    labelmap,
    labelselect,
    labeldelete,
    NULL);

  IupSetAttribute(hboxreslab, "MARGIN", "10x0");
  IupSetAttribute(hboxreslab, "GAP", "0");
  IupSetAttribute(hboxreslab, "ALIGNMENT", "ACENTER");

  /*DATABASE*/
  
  // GENERATED IN item_createnew_cb

  /************** Dialog *************/
  
  separatorhor = IupLabel("");
  IupSetAttribute(separatorhor, "SEPARATOR", "HORIZONTAL");
  IupSetAttribute(separatorhor, "MARGIN", "10x10");
  IupSetAttribute(separatorhor, "GAP", "10");
  
  signature = IupLabel("Created by Rafal Nowak");
  IupSetAttribute(signature, "ALIGNMENT", "ARIGHT");
  IupSetAttribute(signature, "FONTSIZE", "10");
  IupSetAttribute(signature, "FONTFACE", "Bell MT");
  IupSetAttribute(signature, "FGCOLOR", "128 128 128");

  hboxsignature = IupHbox(
    IupFill(),
    signature,
    NULL);

  IupSetAttribute(hboxsignature, "MARGIN", "20x0");
  IupSetAttribute(hboxsignature, "ALIGNMENT", "ARIGHT");
  IupSetAttribute(hboxsignature, "NAME", "HBOXSIGNATURE");

  vboxfill = IupVbox(IupFill(), NULL);
  IupSetAttribute(vboxfill, "NAME", "VBOXFILL");

  vbox = IupVbox(
    toolbar_head,
    hbox1,
    hbox2,
    separatorhor,
    toolbar_db,
    hboxreslab,
    vboxfill,
    hboxsignature,
    NULL);
  
  IupSetAttribute(vbox, "NAME", "VBOX");

  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "Running pace calculator");
  IupSetAttribute(dlg, "NAME", "DIALOG");
  IupSetAttribute(dlg,"DIRTYOPEN","NO");
  IupSetAttribute(dlg,"DIRTYSAVE","NO");
  IupSetAttribute(dlg, "SIZE", "HALFxTHIRD");
  IupSetAttributeHandle(dlg, "DEFAULTENTER", btncalc);
  IupSetAttribute(dlg, "CONFIG", (char*)config);
  IupSetCallback(dlg, "CLOSE_CB", (Icallback)exit_cb);
  IupSetCallback(dlg, "SHOW_CB", (Icallback)open_cb);
  IupSetCallback(dlg, "K_cn", (Icallback)item_new_action_cb);
  IupSetCallback(dlg, "K_cO", (Icallback)item_open_action_cb);
  IupSetCallback(dlg, "K_cS", (Icallback)item_saveas_action_cb);

#ifdef ADRESSESSCHECK
printf("main: dlg adress - %p, vbox adress - %p\n", dlg, vbox);
unsigned int countrows = IupGetChildCount(vbox);
printf("main: Vbox rows = %u\n", countrows);
printf("main: btnadd adress - %p\n", btnadd);
#endif
   /* parent for pre-defined dialogs in closed functions (IupMessage) */
  IupSetAttributeHandle(NULL, "PARENTDIALOG", dlg);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  IupClose();
  return EXIT_SUCCESS;
}

/*UTILITIES*/
char * read_file(char * st, FILE* file)
{
  //function reads only one line from given file

  char * ret_val;                 // return value - string
  char * find;

  ret_val = fgets(st, SAVESTRSIZE, file);
  if (ret_val)
  {
    find = strchr(st, '\n');      // look for newline
    if (find)                     // if the address is not NULL,
        *find = '\0';             // place a null character there
    else
        while (getchar() != '\n')
            continue;
  }

#ifdef OPENFILESTR
printf("read_file: obtained string = %s\n", ret_val);  
#endif

  return ret_val;
}

void write_file(const char* filename, const char* str, unsigned int rowlen, FILE * file)
{
  //function writes only one line to given file

  fwrite(str, rowlen, 1, file);

  if (ferror(file))
    IupMessagef("Error", "Fail when writing to file: %s", filename);
}

bool save_file(Ihandle *self)
{   
  // function checks all pre-conditions and does actions when needed. Then the "for" loop gets appropriate containers from vbox and sends them to direct saving into file
 
  Ihandle *dlgmain, *vboxmain,*hboxcurrent, *title, *length, *time, *filedlg, *config;
  char *titleval, *lengthval, *timeval, *filename;
  char row[SAVESTRSIZE];                                                              // string which is created from the database entry parameters before saving
  bool fexistflag = false;                                                            // flag describing whether the filename does exist (comes from config file)
  
  dlgmain = IupGetDialog(self);
  int dirty = IupGetInt(dlgmain, "DIRTYSAVE");                                        // flag describing the origin of calling function, "0" means that there is a need of creation of new file for saving
#ifdef DIRTYSTATUS
printf("save_file: dirtysave = %d\n", dirty);
#endif
  config = (Ihandle*)IupGetAttribute(self, "CONFIG");
  if (dirty)                                                                          // there is a need to do two separate checks - one for dirty status and second for filename
    filename = NULL;
  else
  {
    strcpy(filename,IupConfigGetVariableStr(config, "MainDialog", "Filename"));
    // filename = IupConfigGetVariableStr(config, "MainDialog", "Filename");             // get name from config 
  }
  if (filename == NULL)                                                               // open file dialog
  {
    filedlg = IupFileDlg();
    IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");
    IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
    IupSetAttributeHandle(filedlg, "PARENTDIALOG", IupGetDialog(self));
    IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
    filename = IupGetAttribute(filedlg, "VALUE");
  }
  else
    fexistflag = true;                                                                // set flag for next condition
 
  if (fexistflag || IupGetInt(filedlg, "STATUS") != -1)                               // the order of conditions is important, IupGetInt(filedlg, "STATUS") != -1 is not checked when "dirty"
  {
    FILE* file = fopen(filename, "w+");
    if (!file)
    {
      IupMessagef("Error", "Can't save file: %s", filename);
      return false;
    }
                                                                       
    IupConfigSetVariableStr(config, "MainDialog", "Filename", filename);              // sets filename into config file

    vboxmain = IupGetDialogChild(self, "VBOX");                                       // getting vbox from main dialog

#ifdef ADRESSESSCHECK
printf("save_file: dlg adress - %p, vbox adress - %p\n", self, vboxmain);
#endif 

    unsigned int vboxelcount = IupGetChildCount(vboxmain);                            // amount of elements in vbox
    unsigned int firsthboxnr = DBHBOXOFFSETSTART;
    unsigned int latesthboxnr = vboxelcount - DBHBOXOFFSETEND;                        // calculating current position of new entry in database
    unsigned int rowlen = 0;                                                          // length of string which contains db entry (can be var. dependant of name and time)

    printf("save_file: vboxelcount = %u, firsthboxnr = %u, latesthboxnr = %u\n", vboxelcount, firsthboxnr, latesthboxnr);

    /*SAVING SCHEME*/
    char* info ="; [NAME]; [DISTANCE]; [BEST TIME];\n";                             // saving start strings
    char* info2 ="; [DATE]; [TIME]; [TIMEPERKM]; [SPEED];\n";                              
    rowlen = strlen(info);
    write_file(filename, info, rowlen, file);
    rowlen = strlen(info2);
    write_file(filename, info2, rowlen, file);

    for (unsigned int i = firsthboxnr ; i < latesthboxnr; i++)                        // saving row after the row (one hbox after the other one)
    {
      /*SAVING MAIN DIALOG*/
      hboxcurrent = IupGetChild(vboxmain, i);                                         // getting appropriate hbox from vbox
      title = IupGetChild(hboxcurrent,TITLEPOS);                                      // getting appropriate containers from hbox
      length = IupGetChild(hboxcurrent,LENGTHPOS); 
      time = IupGetChild(hboxcurrent,TIMEPOS); 

      titleval = IupGetAttribute(title, "VALUE");
      lengthval = IupGetAttribute(length, "VALUE");
      timeval = IupGetAttribute(time, "TITLE");                                       // getting title not value because element is a IUPLABEL

      sprintf(row,"* %s;%s;%s\n",titleval, lengthval, timeval);                         // creation of string for saving
      rowlen = strlen(row);
      write_file(filename, row, rowlen, file);

      /*SAVING "MORE RESULTS" TAB*/
      Ihandle* moreres = IupGetChild(hboxcurrent, MORERESPOS);                        // steps in order to get to the proper vbox
      Ihandle* mrdlg = IupGetAttributeHandle(moreres,"MRDLG");      
      Ihandle* mrvbox = IupGetDialogChild(mrdlg, "VBOXMORE");
      unsigned int countrows = IupGetChildCount(mrvbox);
      unsigned int firsthboxnrmr = MRHBOXOFFSETSTART;  
      unsigned int lasthboxnrmr = countrows - 1;

      Ihandle *hboxrow, *datemr,*timemr, *timeperkmmr, *speedmr;
      char *datemrval, *timemrval, *timeperkmmrval, *speedmrval;
      printf("save_file: hboxcurrent = %p, moreres = %p\n", hboxcurrent, moreres);
          getchar();
#ifdef ADRESSESSCHECK
printf("save_file: countrows = %u\n",countrows);
printf("save_file: moreres address = %p\n",moreres);
printf("save_file: mrdlg address = %p, mrvbox address = %p\n", mrdlg, mrvbox);
#endif

      for (unsigned int i = firsthboxnrmr; i <= lasthboxnrmr; i++)                   // creating strings and saving
      {
        hboxrow = IupGetChild(mrvbox, i);                                            // getting appropriate hbox and containers with data
        datemr = IupGetChild(hboxrow ,DATEMRPOS);
        timemr = IupGetChild(hboxrow ,TIMEMRPOS); 
        timeperkmmr = IupGetChild(hboxrow ,TIMEPERKMMRPOS);
        speedmr = IupGetChild(hboxrow ,SPEEDMRPOS);  

#ifdef ADRESSESSCHECK
printf("save_file: hboxcurrent address = %p, i = %u\n",hboxrow,i);
printf("save_file: datemr address = %p, timemr address = %p, timeperkmmr adress = %p, speedmr address = %p\n", datemr, timemr, timeperkmmr, speedmr);
#endif      

        datemrval = IupGetAttribute(datemr, "TITLE");                                // getting data from containers
        timemrval = IupGetAttribute(timemr, "TITLE");
        timeperkmmrval = IupGetAttribute(timeperkmmr, "TITLE");
        speedmrval =  IupGetAttribute(speedmr, "TITLE");

        sprintf(row,"# %s;%s;%s;%s\n",datemrval, timemrval, timeperkmmrval,speedmrval);         // creation of string for saving
        rowlen = strlen(row);
        write_file(filename, row, rowlen, file);
      }

      sprintf(row,"!\n");                                                  // adding separator - creation of string for saving
      rowlen = strlen(row);
      write_file(filename, row, rowlen, file);
    }
    
    if (fclose(file) != 0)
      fprintf(stderr,"Error closing file\n");

    IupSetAttribute(dlgmain,"DIRTYSAVE","NO");
    return true;
  }
  else
  {
    IupSetAttribute(dlgmain,"DIRTYSAVE","NO");
    return false;
  }

  IupConfigSave(config);
  IupDestroy(filedlg);
  return true;
}

bool renumberpostitl(Item id, Ihandle * vbox)
{
  // function renumbers the entries by setting the TITLE in appropriate containers in "for" loop

  Ihandle *hbox, *positionbox;
  unsigned int hboxnumber = DBHBOXOFFSETSTART;      // position of first hbox with results in database
  bool flag = false;                                // inform whether the deleted entry was already met
  
  for (unsigned int i = 1; i < MAXSTACK - AmountofPosition(&st) + 1; i++, hboxnumber++)
  {
    hbox = IupGetChild(vbox,hboxnumber);
    positionbox = IupGetChild(hbox,POSITIONPOS);
   
    if (flag == false)
      IupSetStrf(positionbox, "TITLE", "%u", i);
    else if (i == id && flag == false)              // case when the deleted item is met (renumbering takes place BEFORE the deletion)
    {
      flag = true;
      continue;
    }
    else if (flag == true)
      IupSetStrf(positionbox, "TITLE", "%u", i-1);
    else
      return false;
  }
  return true;
}

void set_besttime(Ihandle *besttimedb, Ihandle *mrvbox)
{
  // function sets best time into appropriate container in main dialog based on more results tab

  Ihandle *hboxrow, *timemr;
  unsigned int countrows = IupGetChildCount(mrvbox);
  unsigned int firsthboxnrmr = MRHBOXOFFSETSTART;
  unsigned int lasthboxnrmr = countrows - 1;                            // getting number of last row of data in more results tab
  char *besttimeval = "24:60:60";                                       // setting maximum allowed time for comparisons
  char *timemrval;

  for (unsigned int i = firsthboxnrmr; i <= lasthboxnrmr; i++)          // making comparisons row after row
  {
    hboxrow = IupGetChild(mrvbox, i);                                   // getting appropriate container and its value
    timemr = IupGetChild(hboxrow ,TIMEMRPOS);
    timemrval = IupGetAttribute(timemr ,"TITLE");

#ifdef BESTTIMECHECK
printf("set_besttime: countrows = %u, besttimeval = %s, timemrval = %s\n", countrows, besttimeval, timemrval);
#endif 

    if (strcmp(timemrval, besttimeval) < 0)                             // making comparisons and selecting the smallest
      besttimeval = timemrval;                                       
  }

  IupSetStrf(besttimedb, "TITLE", "%s", besttimeval);                   // setting the selected value in container

  if (countrows < 2)                                                    // no rows in more result tab
    IupSetAttribute(besttimedb, "TITLE", "0:0:0");  
}

void generatestack(Item id)
{
  // function which is used for creation of new stack used for controlling amount of more results tab entries

  stacktab[id] = (Stack *) malloc (sizeof(Stack));        // allocation of memory - released when the whole row of data in db is deleted
  if (stacktab[id] == NULL)
    puts("Memory allocation not succedded");
  InitializeStack(stacktab[id]);

#ifdef STACKSCONTENT
  for (unsigned int i = 0; i < MAXSTACK; i++)
  {
    // printf("generatestack: newstack.item[%u] = %u\n",i,newstack.item[i]);
    printf("generatestack: stacktab[%u].item[%u] = %u\n",id,i,stacktab[id]->item[i]); 
  }
  stackscontent();
#endif

}

void stackscontent(void)
{
  // prints the content and addresses of the stacks - for control purposes

  for (unsigned int i = 0; i < MAXSTACK; i++)
  {
    if(stacktab[i] == NULL)
      printf("stackscontent: stacktab[%u] = NULL\n",i);
    else
    {
      printf("stackscontent: stacktab[%u] = FULL\n",i);
      puts("stackscontent: content:");
      for(unsigned int j = 0; j < MAXSTACK; j++)
        printf("stackscontent: stacktab[%u].item[%u] = %u\n",i,j,stacktab[i]->item[j]); 
    }
  }

   for (unsigned int i = 0; i < MAXSTACK; i++)
  { 
      if(stacktab[i] == NULL)
        printf("stacksaddresses: stacktab[%u] = %p\n",i, stacktab[i]);
      else
        printf("stacksaddresses: stacktab[%u] = %p, * stacktab[%u] = %p\n",i, stacktab[i],i, &stacktab[i]);

  }
}   
  
/*OPEN & EXIT PROGRAM*/
int open_cb(Ihandle *self, int state)
{
  // function is automatically executed when program starts. It loads the database from the file if such file exists.
  // function is also called when open from toolbar is called - it opens another file then

  if (state == 0)                                                                     // state == 0 means that the program has started and main dialog appear
  {
    Ihandle *dlgmain, *vboxmain, *hboxcurrent, *title, *length, *time, *filedlg, *config;
    Ihandle *moreres, *mrdlg, *mrvbox, *hboxrow, *datemr, *timemr, *timeperkmmr, *speedmr;
    char row[SAVESTRSIZE], titlestr[TITLESIZE2], distancestr[DISTANCESIZE], timestr[TIMESIZE];           // creates strings for holding obtained data 
    char datemrstr[DATEMRSTRSIZE], timemrstr[TIMEMRSTRSIZE], timeperkmstr[TIMEPERKMMRSTRSIZE], speedstr[SPEEDMRSTRSIZE];  // creates strings for holding obtained data 
    char *filename;
    bool fexistflag = false;                                                           // flag describing whether the filename does exist (comes from config file )
    unsigned int vboxelcount, latesthboxnr;                                            // counters for db in main dialog
    unsigned int countrows, latesthboxnrmr;                                            // counters for entries in more result tab
    unsigned int j = 0, k = 0, l = 0, m = 0;                                           // indices for storing char in particular containers while dividing one row of data
    unsigned int count = 0;                                                            // flag ("0", "1", "2") describing particular containers while dividing one row of data 
    char marker;
    char skipline[SKIPLINESIZE];

    dlgmain = IupGetDialog(self);                                                      // flag describing the origin of calling function, "0" means that there is a need of creation of new file for saving
    int dirty = IupGetInt(dlgmain, "DIRTYOPEN");

    config = (Ihandle*)IupGetAttribute(self, "CONFIG");

    if(dirty)                                                                         // there is a need to do two separate checks - one for dirty status and second for filename
      filename = NULL;
    else
    {
      filename = IupConfigGetVariableStr(config, "MainDialog", "Filename");
      if(!filename)                                                                   // early exit - config empty
        return IUP_DEFAULT;
    }
    if (filename == NULL)
    {
      filedlg = IupFileDlg();
      IupSetAttribute(filedlg, "DIALOGTYPE", "OPEN");
      IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
      IupSetAttributeHandle(filedlg, "PARENTDIALOG", IupGetDialog(self)); 
      IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
      filename = IupGetAttribute(filedlg, "VALUE");
    }
    else    
      fexistflag = true;                                                              // setting flag for next condition

    if (fexistflag || IupGetInt(filedlg, "STATUS") != -1)                             // the order of conditions is important, IupGetInt(filedlg, "STATUS") != -1 is not checked when "dirty"
    {
      FILE* file = fopen(filename, "r");
      if (!file)
      {
        IupMessagef("Error", "Can't open file: %s", filename);
        return false;
      }

  // ************************************************
      if (dirty)                                                                      // there is a need to delete all data in db
      {
        Ihandle* delete;
        unsigned int firsthboxnr;
        
        vboxmain = IupGetDialogChild(self, "VBOX");                                   // getting vbox from main dialog

  #ifdef ADRESSESSCHECK
  printf("open_cb: dlg adress - %p, vbox adress - %p\n", self, vboxmain);
  #endif 

        vboxelcount = IupGetChildCount(vboxmain);                                     // amount of elements in vbox
        firsthboxnr = DBHBOXOFFSETSTART;
        latesthboxnr = vboxelcount - DBHBOXOFFSETEND;                                 // calculating current position of new entry in database
        
        for (unsigned int i = firsthboxnr ; i < latesthboxnr; i++)                    // delete row after the row (one hbox after the other one)
        {
          hboxcurrent = IupGetChild(vboxmain, firsthboxnr);                           // getting appropriate hbox from vbox
          delete = IupGetChild(hboxcurrent,DELETEPOS);                                // getting appropriate containers from hbox

    #ifdef ADRESSESSCHECK
    printf("open_cb: hboxcurrent address = %p\n", hboxcurrent );
    printf("open_cb: length address = %p\n", delete);
    #endif 

          item_delete_cb(delete);
        }
        
        ClearStack(&st);                                                              // reinitialize stack
        InitializeStack(&st);
      }
  // ************************************************
      while (fscanf(file,"%s", &marker) != EOF)                                                    // read row after the row, ends when meets \0                              
      {
        if (marker == ';')                                                                 // ; - comment; let's skip it
          {
            read_file(skipline,file);                                                      
            continue;
          }
        else if (marker == '*')                                                         // * - db entry
        {
          /*OPENING MAIN DIALOG*/
          item_createnew_cb(self);                                                      // creates new entry
          
          vboxmain = IupGetDialogChild(self, "VBOX");                                   // getting main dialog Vbox
          vboxelcount = IupGetChildCount(vboxmain);                                     // amount of elements in vbox
          latesthboxnr = vboxelcount - DBHBOXOFFSETEND; 
          hboxcurrent = IupGetChild(vboxmain, latesthboxnr-1);                          // getting appropriate hbox from vbox
          
          title = IupGetChild(hboxcurrent,TITLEPOS);                                    // getting appropriate containers from hbox
          length = IupGetChild(hboxcurrent,LENGTHPOS); 
          time = IupGetChild(hboxcurrent,TIMEPOS);

  #ifdef ADRESSESSCHECK
  printf("open_cb: vboxmain = %p, hboxcurrent = %p, title = %p, length = %p, time= %p\n", vboxmain, hboxcurrent, title, length, time); 
  printf("open_cb: vboxelcount = %u, latesthboxnr = %u\n", vboxelcount, latesthboxnr - 1);
  #endif

          read_file(row,file);                                                          // reading row of data containing db entry

  #ifdef OPENFILESTR
  printf("open_cb: open str = %s\n", row);
  #endif
          // here we go with dividing obtained row of data into appropriate containers
          j = 0;
          k = 0; 
          l = 0;
          count = 0;

          for (unsigned int i = 0; i < strlen(row); i++)
          { 
              if (row[i] == ';')                                            // reading char after char
                count++;
              else if (row[i] == ' ')
                continue; 
              else if (count == 0)
                titlestr[j++] = row[i];
              else if (count == 1)
                distancestr[k++] = row[i];
              else if (count == 2)
                timestr[l++] = row[i];
          }

          titlestr[j] = '\0';
          distancestr[k] = '\0';
          timestr[l] = '\0';

  #ifdef OPENFILESTR
  printf("open_cb: titlestr = %s, distancestr = %s, timestr = %s\n", titlestr, distancestr, timestr);
  #endif

          IupSetAttribute(title, "VALUE",titlestr);
          IupSetAttribute(length, "VALUE", distancestr);
          // IupSetAttribute(time, "TITLE", timestr);
          IupSetStrf(time, "TITLE","%s", timestr);

          /*OPENING "MORE RESULTS" TAB*/
          moreres = IupGetChild(hboxcurrent, MORERESPOS);                        // steps in order to get to the proper vbox
          moreresults_cb(moreres);                                                        // creating the dialog with more results
          mrdlg = IupGetAttributeHandle(moreres,"MRDLG");                        // getting into more results dialog vbox
          mrvbox = IupGetDialogChild(mrdlg, "VBOXMORE");
    
          while (fscanf(file,"%s", &marker) && marker == '#')                // # - row of data to more results tab 
          {
            moreres_createnew_cb(moreres);                                  // adding a new row in the more results dialog mrdlg
            countrows = IupGetChildCount(mrvbox);
            latesthboxnrmr = countrows - 1;                                 // getting appropriate containers 
            hboxrow = IupGetChild(mrvbox, latesthboxnrmr);
            datemr = IupGetChild(hboxrow ,DATEMRPOS);
            timemr = IupGetChild(hboxrow ,TIMEMRPOS); 
            timeperkmmr = IupGetChild(hboxrow ,TIMEPERKMMRPOS);
            speedmr = IupGetChild(hboxrow ,SPEEDMRPOS);   

  #ifdef ADRESSESSCHECK
  printf("open_cb: countrows = %u\n",countrows);
  printf("open_cb: hboxcurrent address = %p, moreres address = %p\n",hboxcurrent,moreres);
  printf("open_cb: mrdlg address = %p, mrvbox address = %p, hboxrow address = %p\n",mrdlg, mrvbox, hboxrow);
  printf("open_cb: datemr address = %p, timemr address = %p, timeperkmmr adress = %p, speedmr address = %p\n", datemr, timemr, timeperkmmr, speedmr);
  #endif

            read_file(row,file);                                            // reading row of data contianing more results entry

            // here we go with dividing obtained row of data into appropriate containers
            j = 0;
            k = 0; 
            l = 0;
            m = 0;
            count = 0;

            for (unsigned int i = 0; i < strlen(row); i++)
            { 
                if (row[i] == ';')                                            // reading char after char
                  count++;
                else if (row[i] == ' ')
                  continue; 
                else if (count == 0)
                  datemrstr[j++] = row[i];
                else if (count == 1)
                  timemrstr[k++] = row[i];
                else if (count == 2)
                  timeperkmstr[l++] = row[i];
                else if (count == 3)
                  speedstr[m++] = row[i];
            }

            datemrstr[j] = '\0';
            timemrstr[k] = '\0';
            timeperkmstr[l] = '\0';
            speedstr[m] = '\0';

    #ifdef OPENFILESTR
    printf("open_cb: datemrstr = %s, timemrstr = %s, timeperkmstr = %s, speedstr = %s\n", datemrstr, timemrstr, timeperkmstr, speedstr);
    #endif

            IupSetStrf(datemr, "TITLE", "%s", datemrstr);                     // placing obtained data into appropriate containers
            IupSetStrf(timemr, "TITLE", "%s", timemrstr);  
            IupSetStrf(timeperkmmr, "TITLE", "%s", timeperkmstr);  
            IupSetStrf(speedmr, "TITLE", "%s", speedstr);  
          }
        }
        else
        {
          printf("open_cb: Problem with marker. Marker = %s\n", marker);
          IupMessagef("Error", "Problem with marker in file %s", filename);
          return false;
        }
      
      }

      IupConfigSetVariableStr(config, "MainDialog", "Filename", filename);
      IupConfigSave(config);
    }

    IupSetAttribute(dlgmain,"DIRTYOPEN","NO");
    IupDestroy(filedlg);
    return IUP_DEFAULT;
  }
}

int exit_cb(Ihandle *self)
{
  // function calls other necessary functions before closing and then closes the program

  Ihandle * vboxmain = IupGetDialogChild(self, "VBOX");            // getting vbox from main dialog
  unsigned int vboxelcount = IupGetChildCount(vboxmain); 

  if (vboxelcount != VBOXELCOUNTINI)                               // do the checks only when: -there are entries in db; -there are changes in db
  {
    if (!label_check(self))
      return IUP_IGNORE;
    printf("tu jestem\n");
    if (!save_file(self))
      return IUP_IGNORE;
    printf("tu jestem2\n");
  }
  printf("tu jestem3\n");
  Ihandle* config = (Ihandle*)IupGetAttribute(self, "CONFIG");    // saves config
  IupConfigSave(config);
  IupDestroy(config);
  return IUP_CLOSE;
}

/*TOOLBAR OPTIONS*/
int item_new_action_cb(Ihandle *self)
{
  // function creates new spreadsheet -  deletes all current data, clear windows, reinicialize stack

  Ihandle *vboxmain, *hboxcurrent, *delete;

  vboxmain = IupGetDialogChild(self, "VBOX");                   // getting vbox from main dialog
  unsigned int vboxelcount = IupGetChildCount(vboxmain); 
  unsigned int firsthboxnr = DBHBOXOFFSETSTART;
  unsigned int latesthboxnr = vboxelcount - DBHBOXOFFSETEND;    // calculating current position of new entry in database

  if (vboxelcount != VBOXELCOUNTINI)                            // do the checks only when: -there are entries in db; -there are changes in db
  { 
  if (!label_check(self))
  return IUP_IGNORE;
  
  if (!save_file(self))
    return IUP_IGNORE;
  }

  Ihandle* distancea = IupGetDialogChild(self, "DISTANCE");     // getting containers from main dialog
  Ihandle* timea = IupGetDialogChild(self, "TIME");
  Ihandle* timeperkma = IupGetDialogChild(self, "TIMEPERKM");
  Ihandle* speeda = IupGetDialogChild(self, "SPEED");

  IupSetAttribute(distancea,"VALUE","");                        // set containers from main dialog
  IupSetAttribute(timea,"VALUE","");
  IupSetAttribute(timeperkma,"VALUE","");
  IupSetAttribute(speeda,"VALUE","");

#ifdef ADRESSESSCHECK
printf("item_new_action_cb dlg adress - %p, vbox adress - %p\n", self, vboxmain);
#endif 

  for (unsigned int i = firsthboxnr; i < latesthboxnr; i++)  // there is a need to delete all data in db - delete row after the row (one hbox after the other one)
  {
    hboxcurrent = IupGetChild(vboxmain, firsthboxnr);         // getting appropriate hbox from vbox
    delete = IupGetChild(hboxcurrent,DELETEPOS);              // getting appropriate containers from hbox

#ifdef ADRESSESSCHECK
printf("item_new_action_cb hboxcurrent address = %p\n", hboxcurrent );
printf("item_new_action_cb length address = %p\n", delete);
#endif 

    item_delete_cb(delete);
  }
  ClearStack(&st);                                            // reinitialize stack
  InitializeStack(&st);

  Ihandle* config = (Ihandle*)IupGetAttribute(self, "CONFIG");  // set config to empty
  IupConfigSetVariableStr(config, "MainDialog", "Filename", NULL);
  return IUP_DEFAULT;
}

int item_open_action_cb(Ihandle *self)
{
  // function does checks if necessary, and finally opens new file 

  Ihandle *dlgmain = IupGetDialog(self);
  IupSetAttribute(dlgmain,"DIRTYOPEN","YES");                      // setting parameter
  Ihandle * vboxmain = IupGetDialogChild(self, "VBOX");            // getting vbox from main dialog
  unsigned int vboxelcount = IupGetChildCount(vboxmain); 

  if (vboxelcount != VBOXELCOUNTINI)                               // do the checks only when: -there are entries in db; -there are changes in db
  {
    if (!label_check(self))
    return IUP_IGNORE;
    
    if (!save_file(self))
      return IUP_IGNORE;
  }

  open_cb(self,0);
  return IUP_DEFAULT;
}

int item_saveas_action_cb(Ihandle *self)
{
  // function does checks if necessary, and finally save a new file 

  Ihandle *dlgmain = IupGetDialog(self);
  IupSetAttribute(dlgmain,"DIRTYSAVE","YES");                      // setting parameter
  Ihandle * vboxmain = IupGetDialogChild(self, "VBOX");            // getting vbox from main dialog
  unsigned int vboxelcount = IupGetChildCount(vboxmain);

  if (vboxelcount != VBOXELCOUNTINI)                               // do the checks only when: -there are entries in db;
    save_file(self);
    
  return IUP_DEFAULT;
}

/*CREATION OF NEW ENTRIES*/
int item_createnew_cb(Ihandle *self)
{
  // function creates new blank row in the database (creates all containers in one row)

  if (EmptyStack(&st))                                                          // early exit - stack is empty, limit of entries in db reached
  {
    IupMessage("Cannot add new position","Too many positions - cannot add new position\nPlease delete some positions");
    return IUP_DEFAULT;
  }

  Item n;           
  PopStack(&st, &n);                                                            // getting number from the stack

  generatestack(n);                                                             // generating new stack for more results

#ifdef STACKCONTENT 
puts("item_createnew_cb");  
  stackscontent();
#endif 

  Ihandle* dlgmain = IupGetDialog(self);
  Ihandle *vboxmain = IupGetDialogChild(self, "VBOX");
  Ihandle *vboxfill = IupGetDialogChild(self, "VBOXFILL");                      // two last hboxes from main vbox are shifting to the end
  Ihandle *hboxsignature = IupGetDialogChild(self, "HBOXSIGNATURE");
  Ihandle *FILLAREA(n), *POSITION(n), *TITLE(n), *LENGTH(n), *TIME(n), *MORERES(n), *MAP(n), *SELECTBTN(n), *DELETE(n), *HBOX(n);  

  FILLAREA(n) = IupFill();
  IupSetAttribute(FILLAREA(n), "SIZE", FILL);
  
  POSITION(n) = IupLabel("");
  IupSetAttribute(POSITION(n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(POSITION(n), "ALIGNMENT", RESULTLBLALIGN);
  IupSetStrf(POSITION(n), "NAME", "POSITION%u", n);
  IupSetStrf(POSITION(n), "TITLE", "%u", MAXSTACK - AmountofPosition(&st));

  TITLE(n) = IupText(NULL);
  IupSetAttribute(TITLE(n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(TITLE(n), "ALIGNMENT", RESULTLBLALIGN);
  IupSetStrf(TITLE(n), "NAME", "TITLE%u", n);
  IupSetAttribute(TITLE(n), "ACTIVE", "YES");
  IupSetAttribute(TITLE(n), "VALUE", "");
  IupSetAttribute(TITLE(n), "NC", TITLESIZE);               

  LENGTH(n) = IupText(NULL);
  IupSetAttribute(LENGTH(n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(LENGTH(n), "ALIGNMENT", RESULTLBLALIGN);
  IupSetStrf(LENGTH(n), "NAME", "LENGTH%u", n);
  IupSetAttribute(LENGTH(n), "MASK", IUP_MASK_FLOAT);
  IupSetAttribute(LENGTH(n), "ACTIVE", "YES");
  IupSetAttribute(LENGTH(n), "VALUE", "");
  IupSetCallback(LENGTH(n), "CARET_CB", (Icallback)checkdata_distance_cb);
  IupSetCallback(LENGTH(n), "KILLFOCUS_CB", (Icallback)distance_dcmplcs_cb);

  TIME(n) = IupLabel("");
  IupSetAttribute(TIME(n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(TIME(n), "ALIGNMENT", RESULTLBLALIGN);
  IupSetStrf(TIME(n), "NAME", "TIME%u", n);                                 
  IupSetAttribute(TIME(n), "TITLE", "0:0:0");

  MORERES(n) = IupButton(NULL, NULL);
  IupSetAttribute(MORERES(n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(MORERES(n), "ALIGNMENT", RESULTLBLALIGN);
  IupSetAttribute(MORERES(n), "IMAGE", "IUP_ArrowDown");
  IupSetAttribute(MORERES(n), "FLAT", "Yes");
  IupSetAttribute(MORERES(n), "CANFOCUS", "No");
  IupSetAttribute(MORERES(n), "ACTIVE", "Yes");
  IupSetStrf(MORERES(n), "NAME", "MORERES%u", n);
  IupSetCallback(MORERES(n), "ACTION", (Icallback)show_moreresultsdlg_cb);

  MAP(n) = IupButton(NULL, NULL);
  IupSetAttribute(MAP(n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(MAP(n), "ALIGNMENT", RESULTLBLALIGN);
  IupSetAttribute(MAP(n), "IMAGE", "IMGLOC");
  IupSetAttribute(MAP(n), "FLAT", "Yes");
  IupSetAttribute(MAP(n), "CANFOCUS", "No");
  IupSetAttribute(MAP(n), "ACTIVE", "No");
  IupSetStrf(MAP(n), "NAME", "MAP%u", n);
  // IupSetCallback(MAP(n), "ACTION", (Icallback)item_createnew_cb);
  IupSetAttribute(MAP(n), "TIP", "Feature will be added in the future");

  SELECTBTN(n) = IupButton(NULL, NULL);
  IupSetAttribute(SELECTBTN(n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(SELECTBTN(n), "ALIGNMENT", RESULTLBLALIGN);
  IupSetAttribute(SELECTBTN(n), "IMAGE", "IMGCHCK");
  IupSetAttribute(SELECTBTN(n), "FLAT", "Yes");
  IupSetAttribute(SELECTBTN(n), "CANFOCUS", "No");
  IupSetStrf(SELECTBTN(n), "NAME", "SELECTBTN%d", n);
  IupSetCallback(SELECTBTN(n), "ACTION", (Icallback)item_select_cb);
  IupSetAttribute(SELECTBTN(n), "TIP", "Select this trail");

  DELETE(n) = IupButton(NULL, NULL);
  IupSetAttribute(DELETE(n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(DELETE(n), "ALIGNMENT", RESULTLBLALIGN);
  IupSetAttribute(DELETE(n), "IMAGE", "IUP_EditErase");
  IupSetAttribute(DELETE(n), "FLAT", "Yes");
  IupSetAttribute(DELETE(n), "CANFOCUS", "No");
  IupSetStrf(DELETE(n), "NAME", "DELETE%u", n);
  IupSetCallback(DELETE(n), "ACTION", (Icallback)item_delete_cb);
  IupSetAttribute(DELETE(n), "TIP", "Delete selected trail");
  IupSetStrf(DELETE(n), "OWN_ID", "%u", n);                           // Own attribute equal to stack number used for delete_cb to push entry back into stack

  HBOX(n) = IupHbox(
    FILLAREA(n),
    POSITION(n),
    TITLE(n),
    LENGTH(n),
    TIME(n),
    MORERES(n),
    MAP(n),
    SELECTBTN(n),
    DELETE(n),
    NULL);

  IupSetStrf(HBOX(n), "NAME", "HBOX%u", n);
  IupSetAttribute(HBOX(n), "MARGIN", "10x0");
  IupSetAttribute(HBOX(n), "GAP", "0");
  IupSetAttribute(HBOX(n), "ALIGNMENT", "ACENTER");

#ifdef ADRESSESSCHECK
printf("item_createnew_cb: dlgmain adress - %p, vboxmain adress - %p\n", dlgmain, vboxmain);
printf("item_createnew_cb: fill %p, position %p, title %p, length %p, time %p, moreres %p, map %p, select %p, delete %p, hbox %p\n", FILLAREA(n), POSITION(n), TITLE(n), LENGTH(n),TIME(n),MORERES(n),MAP(n),SELECTBTN(n),DELETE(n),HBOX(n));
#endif

  IupAppend(vboxmain, HBOX(n));                                     // adding new hbox into the vbox
  IupAppend(vboxmain, vboxfill);                                    // reposition of two last hboxes
  IupAppend(vboxmain, hboxsignature);
  IupMap(HBOX(n));
  IupRefresh(vboxmain);

  return IUP_DEFAULT;
}

int create_newentry_cb(Ihandle *self)
{
  // function activates when user click ADD button in db toolbar - creates new dialog which allows for entering new entry
  
  Ihandle *labelnamene, *labeldistancene, *hbox1ne, *namene, *distancene, *hbox2ne, *btnok, *btncancel, *hbox3ne, *vboxne, *dlgne;    // ne means new entry

  /*HBOX1*/
  labelnamene = IupLabel("Name");
  IupSetAttribute(labelnamene, "SIZE", PACECALCSIZE);
  IupSetAttribute(labelnamene, "ALIGNMENT", PACECALCALIGN);

  labeldistancene = IupLabel("Distance\n[km]");
  IupSetAttribute(labeldistancene, "SIZE", PACECALCSIZE);
  IupSetAttribute(labeldistancene, "ALIGNMENT", PACECALCALIGN);

  hbox1ne = IupHbox(
    labelnamene,
    labeldistancene,
    NULL);

  IupSetAttribute(hbox1ne, "MARGIN", "10x0");
  IupSetAttribute(hbox1ne, "GAP", "0");
  IupSetAttribute(hbox1ne, "ALIGNMENT", "ACENTER");
  IupSetAttribute(hbox1ne, "NORMALIZESIZE", "HORIZONTAL");

  /*HBOX2*/
  namene = IupText(NULL);
  IupSetAttribute(namene, "SIZE", PACECALCSIZE);
  IupSetAttribute(namene, "ALIGNMENT", PACECALCALIGN);
  IupSetAttribute(namene, "NAME", "NAMENE");
  IupSetAttribute(namene, "VALUE", "");
  IupSetAttribute(namene, "NC", TITLESIZE);

  distancene = IupText(NULL);
  IupSetAttribute(distancene, "SIZE", PACECALCSIZE);
  IupSetAttribute(distancene, "ALIGNMENT", PACECALCALIGN);
  IupSetAttribute(distancene, "NAME", "DISTANCE");
  IupSetAttribute(distancene, "MASK", IUP_MASK_FLOAT);
  IupSetAttribute(distancene, "VALUE", "");
  IupSetCallback(distancene, "CARET_CB", (Icallback)checkdata_distance_cb);
  IupSetCallback(distancene, "KILLFOCUS_CB", (Icallback)distance_dcmplcs_cb);

  hbox2ne = IupHbox(
    namene,
    distancene,
    NULL);

  IupSetAttribute(hbox2ne, "MARGIN", "10x10");
  IupSetAttribute(hbox2ne, "GAP", "0");
  IupSetAttribute(hbox2ne, "ALIGNMENT", "ACENTER");
  IupSetAttribute(hbox2ne, "NORMALIZESIZE", "HORIZONTAL");

  /*HBOX3*/
  btnok = IupButton("OK", NULL);
  IupSetAttribute(btnok, "PADDING", "10x2");
  IupSetCallback(btnok, "ACTION", (Icallback)confirm_newentry_cb);

  btncancel = IupButton("Cancel", NULL);
  IupSetCallback(btncancel, "ACTION", (Icallback)close_newentry_cb);
  IupSetAttribute(btncancel, "PADDING", "10x2");

  hbox3ne = IupHbox(
    IupFill(),
    btnok,
    btncancel,
    NULL);

  IupSetAttribute(hbox3ne, "MARGIN", "10x10");
  IupSetAttribute(hbox3ne, "GAP", "5");
  IupSetAttribute(hbox3ne, "ALIGNMENT", "ACENTER");
  IupSetAttribute(hbox3ne, "NORMALIZESIZE", "HORIZONTAL");

  /*DIALOG*/
  vboxne = IupVbox(
    hbox1ne,
    hbox2ne,
    hbox3ne,
    NULL);

  IupSetAttribute(vboxne, "NAME", "VBOXNE");

  dlgne = IupDialog(vboxne);
  IupSetAttribute(dlgne, "TITLE", "Running pace calculator");
  IupSetAttribute(dlgne, "NAME", "DIALOGNE");
  IupSetAttributeHandle(dlgne, "DEFAULTENTER", btnok);
  IupSetAttributeHandle(dlgne, "DEFAULTESC", btncancel);
  IupSetAttributeHandle(dlgne, "PARENTDIALOG", IupGetDialog(self));

#ifdef ADRESSESSCHECK
// Ihandle* dlgmain = IupGetDialog(self);
// Ihandle * dlgszuk = IupGetAttributeHandle(dlgne,"PARENTDIALOG");
// printf("create_newentry_cb: dlg main - %p, dlgszuk - %p\n", dlgmain, dlgszuk);
// printf("create_newentry_cb: dlg adress - %p, vbox adress - %p, hbox - %p, distance - %p\n", dlgne, vboxne, hbox2ne, distancene);
#endif

  IupShowXY(dlgne, IUP_CENTERPARENT, IUP_CENTERPARENT);

  return IUP_DEFAULT;
}

int confirm_newentry_cb(Ihandle* self)
{
  // function activates when user click OK button in create_newentry_cb dialog - fills appropriate containers in main dialog

  Ihandle* dlgmain = IupGetAttributeHandle(IupGetDialog(self),"PARENTDIALOG");  // getting main dialog 
  Ihandle* vboxmain = IupGetDialogChild(dlgmain, "VBOX");         // getting vbox from main dialog
  Ihandle* namea = IupGetDialogChild(self, "NAMENE");             // getting containers from the upper dialog
  Ihandle* distancea = IupGetDialogChild(self, "DISTANCE"); 

  char *nm = IupGetAttribute(namea , "VALUE");                    // getting time from main dialog
  char *dist = IupGetAttribute(distancea, "VALUE");               // getting distance from main dialog
  
  if (*nm == '\0' || *dist == '\0' )
  {
     IupMessage("Invalid data","Please fill all cells until proceed futher");
     return IUP_DEFAULT;
  }
  
#ifdef ADRESSESSCHECK
// printf("confirm_newentry_cb: dlgmain - %p, vbox adress - %p\n", dlgmain, vboxmain);
#endif

  unsigned int vboxelcount = IupGetChildCount(vboxmain);          // amount of elements in vbox
  unsigned int latesthboxnr = vboxelcount - DBHBOXOFFSETEND;      // calculating current position of new entry in database

  if (nodouble_dbne_check(dist, nm, vboxmain, latesthboxnr))
    return IUP_DEFAULT;
  else
  {
    item_createnew_cb(dlgmain);                                   // adding new item into the database - only now
    Ihandle* hboxcurrent = IupGetChild(vboxmain,latesthboxnr);    // getting appropriate hbox from vbox
    Ihandle* length = IupGetChild(hboxcurrent,LENGTHPOS);         // getting appropriate containers from hbox
    Ihandle* title = IupGetChild(hboxcurrent,TITLEPOS); 
    
    IupSetAttribute(length, "VALUE", dist);                       // setting values in selected containers of main dialog
    IupSetAttribute(title, "VALUE", nm);
  }
  
  IupDestroy(IupGetDialog(self));                                 // destroy the create_newentry_cb dialog
  return IUP_DEFAULT;
}

int close_newentry_cb(Ihandle* btncancel)
{
  // function closes create_newentry_cb dialog without saving any data

  IupDestroy(IupGetDialog(btncancel));
  return IUP_DEFAULT;
}

int addtodb_cb(Ihandle *self)
{
  // QUICK ADD - function adds result into database - if the distance does exist in db - function adds time to container, if does not - function creates new entry
  
  calculate_running_cb(IupGetDialog(self));                         // first recalculate the pace - especially needed when user changes parameters in the meantime of adding new entries

  Ihandle* distancea = IupGetDialogChild(self, "DISTANCE");         // getting containers from main dialog
  Ihandle* timea = IupGetDialogChild(self, "TIME");               
  Ihandle* timeperkma = IupGetDialogChild(self, "TIMEPERKM");
  Ihandle* speeda = IupGetDialogChild(self, "SPEED");

  char *dist = IupGetAttribute(distancea, "VALUE");                 // getting distance from main dialog
  char *tm = IupGetAttribute(timea, "TITLE");                       // getting time from main dialog
  char *tpk = IupGetAttribute(timeperkma, "TITLE");                 // getting time per km from main dialog
  char *spd = IupGetAttribute(speeda, "TITLE");                     // getting speed from main dialog

  if (*dist == '\0' || *tm == '\0' || *tpk == '\0')                 // early exit - if one parameter is empty, then don't add
  {
     return IUP_DEFAULT;
  }
  
  Ihandle* vboxmain = IupGetDialogChild(self, "VBOX");              // getting vbox from main dialog
  unsigned int vboxelcount = IupGetChildCount(vboxmain);            // amount of elements in vbox
  unsigned int latesthboxnr = vboxelcount - DBHBOXOFFSETEND;        // calculating current position of new entry in database
  unsigned int lasthboxnr = vboxelcount - DBHBOXOFFSETEND - 1;      // calculating current position of last entry in database
  unsigned int hboxresult = nodouble_db_check(dist, vboxmain, lasthboxnr);  // searching db for doubles

  if (hboxresult)          // if entry with such distance is present in db, then ask for date
  {
    getddate(self);

    Ihandle *dlg = IupGetAttributeHandle(self, "DLGDLG");           // getting lower dialog in hierarchy
    Ihandle *date = IupGetDialogChild(dlg, "DATENEW");
    char *dateval = IupGetAttribute(date, "VALUE");

#ifdef ADRESSESSCHECK
printf("addtodb_cb: self address = %p, getdialog address = %p\n", self, IupGetDialog(self));
printf("addtodb_cb: dlg address = %p, date adress = %p\n", dlg, date);
printf("addtodb_cb: date value = %s\n", dateval);
#endif

    Ihandle* hboxcurrent = IupGetChild(vboxmain,hboxresult);        // getting appropriate hbox from vbox
    Ihandle* besttimedb = IupGetChild(hboxcurrent,TIMEPOS);
    Ihandle* moreresdb = IupGetChild(hboxcurrent,MORERESPOS);
   
    moreres_createnew_cb(moreresdb);                                // adding a new row in the more results tab

    Ihandle* mrdlg = IupGetAttributeHandle(moreresdb,"MRDLG");      // steps in order to get to the containers 
    Ihandle* mrhbox = IupGetDialogChild(mrdlg, "HBOXMORE");
    Ihandle* mrvbox = IupGetDialogChild(mrdlg, "VBOXMORE");
    unsigned int countrows = IupGetChildCount(mrvbox);
    unsigned int latesthboxnrmr = countrows - 1;
    Ihandle* hboxrow = IupGetChild(mrvbox, latesthboxnrmr);
    Ihandle* datemr = IupGetChild(hboxrow ,DATEMRPOS);
    Ihandle* timemr = IupGetChild(hboxrow ,TIMEMRPOS); 
    Ihandle* timeperkmmr = IupGetChild(hboxrow ,TIMEPERKMMRPOS);
    Ihandle* speedmr = IupGetChild(hboxrow ,SPEEDMRPOS);               

#ifdef ADRESSESSCHECK
printf("addtodb_cb: countrows = %u\n",countrows);
printf("addtodb_cb: hboxcurrent address = %p, moreresdb address = %p\n",hboxcurrent,moreresdb);
printf("addtodb_cb: mrdlg address = %p, mrhbox address = %p, mrvbox address = %p, hboxrow address = %p\n",mrdlg, mrhbox, mrvbox,hboxrow);
printf("addtodb_cb: datemr address = %p, timemr address = %p, timeperkmmr adress = %p, speedmr address = %p\n", datemr, timemr, timeperkmmr, speedmr);
#endif

    IupSetStrf(datemr, "TITLE", "%s", dateval);  
    IupSetStrf(timemr, "TITLE", "%s", tm);  
    IupSetStrf(timeperkmmr, "TITLE", "%s", tpk);  
    IupSetStrf(speedmr, "TITLE", "%s", spd);  
    // IupSetAttribute(timemr, "VALUE", tm);
    // IupSetAttribute(timeperkmmr, "VALUE", tpk);
    // IupSetAttribute(speedmr, "VALUE", spd);

    set_besttime(besttimedb,mrvbox);
  }                                                                 // if entry with such distance is not present in db, then ask for date and title, then create new entry
  else
  {
    getdateandtitle(self);

    Ihandle *dlg = IupGetAttributeHandle(self, "DLGDLG");           // getting lower dialog in hierarchy
    Ihandle* title  = IupGetDialogChild(dlg, "TITLENEW");           // getting appropriate contianers and resp. values
    Ihandle *date = IupGetDialogChild(dlg, "DATENEW");
    char *titleval = IupGetAttribute(title, "VALUE");
    char *dateval = IupGetAttribute(date, "VALUE");

#ifdef ADRESSESSCHECK
printf("addtodb_cb: self address = %p, getdialog address = %p\n", self, IupGetDialog(self));
printf("addtodb_cb: dlg address = %p, title address = %p, date adress = %p\n", dlg, title, date);
printf("addtodb_cb: title value = %s, date value = %s\n", titleval, dateval);
#endif

    item_createnew_cb(self);                                          // adding new item into the database - only now
    
    Ihandle* hboxcurrent = IupGetChild(vboxmain,latesthboxnr);        // getting appropriate hbox from vbox
    Ihandle* titledb = IupGetChild(hboxcurrent,TITLEPOS);             // getting appropriate containers from hbox
    Ihandle* lengthdb = IupGetChild(hboxcurrent,LENGTHPOS);
    Ihandle* besttimedb = IupGetChild(hboxcurrent,TIMEPOS);
    Ihandle* moreresdb = IupGetChild(hboxcurrent,MORERESPOS);

    moreresults_cb(moreresdb);                                        // it is necessary to create the dialog with more results here
    moreres_createnew_cb(moreresdb);                                  // adding a new row in the dialog above

    Ihandle* mrdlg = IupGetAttributeHandle(moreresdb,"MRDLG");        // steps in order to get to the containers 
    Ihandle* mrhbox = IupGetDialogChild(mrdlg, "HBOXMORE");
    Ihandle* mrvbox = IupGetDialogChild(mrdlg, "VBOXMORE");
    unsigned int countrows = IupGetChildCount(mrvbox);
    unsigned int latesthboxnrmr = countrows - 1;
    Ihandle* hboxrow = IupGetChild(mrvbox, latesthboxnrmr);
    Ihandle* datemr = IupGetChild(hboxrow ,DATEMRPOS);
    Ihandle* timemr = IupGetChild(hboxrow ,TIMEMRPOS); 
    Ihandle* timeperkmmr = IupGetChild(hboxrow ,TIMEPERKMMRPOS);
    Ihandle* speedmr = IupGetChild(hboxrow ,SPEEDMRPOS);               

#ifdef ADRESSESSCHECK
printf("addtodb_cb: countrows = %u\n",countrows);
printf("addtodb_cb: hboxcurrent address = %p, moreresdb address = %p\n",hboxcurrent,moreresdb);
printf("addtodb_cb: mrdlg address = %p, mrhbox address = %p, mrvbox address = %p, hboxrow address = %p\n",mrdlg, mrhbox, mrvbox,hboxrow);
printf("addtodb_cb: datemr address = %p, timemr address = %p, timeperkmmr adress = %p, speedmr address = %p\n", datemr, timemr, timeperkmmr, speedmr);
#endif

    IupSetAttribute(titledb, "VALUE", titleval);                           // setting values in selected containers
    IupSetAttribute(lengthdb, "VALUE", dist);  
    IupSetStrf(datemr, "TITLE", "%s", dateval);  
    IupSetStrf(timemr, "TITLE", "%s", tm);  
    IupSetStrf(timeperkmmr, "TITLE", "%s", tpk);  
    IupSetStrf(speedmr, "TITLE", "%s", spd);  
    // IupSetAttribute(datemr, "VALUE", dateval);
    // IupSetAttribute(timemr, "VALUE", tm);
    // IupSetAttribute(timeperkmmr, "VALUE", tpk);
    // IupSetAttribute(speedmr, "VALUE", spd);

    set_besttime(besttimedb,mrvbox);
  
  }
  return IUP_DEFAULT;
}

int getddate(Ihandle *self)
{ 
  // function creates new dialog which allows for adding date to existing entry

  Ihandle *labelDate, *adddate, *btn_ok;
  Ihandle *hbox1, *hbox2, *hbox3, *vbox, *dlg;

  /*HBOX1*/
  labelDate = IupLabel("Date\n[year/month/day]");
  IupSetAttribute(labelDate, "SIZE", MORERESSIZE);
  IupSetAttribute(labelDate, "ALIGNMENT", MORERESALIGN);

  hbox1 = IupHbox(
    labelDate,
    NULL);
  IupSetAttribute(hbox1, "MARGIN", "10x0");
  IupSetAttribute(hbox1, "GAP", "0");
  IupSetAttribute(hbox1, "ALIGNMENT", MORERESALIGN);

  /*HBOX2*/
  adddate = IupText(NULL);
  IupSetAttribute(adddate, "SIZE", MORERESSIZE);
  IupSetAttribute(adddate, "ALIGNMENT", MORERESALIGN);
  IupSetAttribute(adddate, "NAME", "DATENEW");
  IupSetAttribute(adddate, "ACTIVE", "YES");
  IupSetAttribute(adddate, "VALUE", "");       
  IupSetAttribute(adddate, "TIP", "Start typing and the calendar will pop up");
  IupSetCallback(adddate, "K_ANY", (Icallback)calendar_cb);

  hbox2 = IupHbox(
    adddate,
    NULL);
  IupSetAttribute(hbox2, "MARGIN", "10x0");
  IupSetAttribute(hbox2, "GAP", "0");
  IupSetAttribute(hbox2, "ALIGNMENT", MORERESALIGN);

  /*HBOX3*/
  btn_ok = IupButton("OK", NULL);
  IupSetAttribute(btn_ok, "SIZE", "30");
  IupSetAttribute(btn_ok, "NAME", "BUT_OK");
  IupSetAttribute(btn_ok, "VALUE", (char*) self);
  IupSetCallback(btn_ok, "ACTION", (Icallback)getback_addtodb_cb); 

  hbox3 = IupHbox(
    IupFill(),
    btn_ok,
    NULL);

  /*DIALOG*/
  vbox = IupVbox(
    hbox1,
    hbox2,
    hbox3,
    NULL);

  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "Add additional info");
  IupSetAttribute(dlg, "NAME", "DATEDLG");
  IupSetAttributeHandle(dlg, "DEFAULTENTER", btn_ok);
  
  IupSetAttributeHandle(dlg, "PARENTDIALOG", IupGetDialog(self));
  IupSetAttributeHandle(self, "DLGDLG", dlg);                           // handle necessary to get the title and date in addtodb

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

#ifdef ADRESSESSCHECK
printf("getdate: dialog = %p, parentdialog = %p\n", dlg, IupGetDialog(self));        
printf("getdate: adddate = %p\n", adddate);  
#endif

  return IUP_DEFAULT;
}

int getdateandtitle(Ihandle *self)
{ 
  // function creates new dialog which allows for adding date and title to new entry

  Ihandle *labelTitle, *labelDate, *addtitle, *adddate, *btn_ok;
  Ihandle *hbox1, *hbox2, *hbox3, *vbox, *dlg;

  /*HBOX1*/
  labelTitle = IupLabel("Title");
  IupSetAttribute(labelTitle, "SIZE", MORERESSIZE);
  IupSetAttribute(labelTitle, "ALIGNMENT", MORERESALIGN);

  labelDate = IupLabel("Date\n[year/month/day]");
  IupSetAttribute(labelDate, "SIZE", MORERESSIZE);
  IupSetAttribute(labelDate, "ALIGNMENT", MORERESALIGN);

  hbox1 = IupHbox(
    labelTitle,
    labelDate,
    NULL);
  IupSetAttribute(hbox1, "MARGIN", "10x0");
  IupSetAttribute(hbox1, "GAP", "0");
  IupSetAttribute(hbox1, "ALIGNMENT", MORERESALIGN);

  /*HBOX2*/
  addtitle = IupText(NULL);
  IupSetAttribute(addtitle, "SIZE", MORERESSIZE);
  IupSetAttribute(addtitle, "ALIGNMENT", MORERESALIGN);
  IupSetAttribute(addtitle, "NAME", "TITLENEW");
  IupSetAttribute(addtitle, "ACTIVE", "YES");
  IupSetAttribute(addtitle, "VALUE", "");
  IupSetAttribute(addtitle, "NC", TITLESIZE);      
  IupSetCallback(addtitle, "KILLFOCUS_CB", (Icallback)nodouble_label_check);


  adddate = IupText(NULL);
  IupSetAttribute(adddate, "SIZE", MORERESSIZE);
  IupSetAttribute(adddate, "ALIGNMENT", MORERESALIGN);
  IupSetAttribute(adddate, "NAME", "DATENEW");
  IupSetAttribute(adddate, "ACTIVE", "YES");
  IupSetAttribute(adddate, "VALUE", "");       
  IupSetAttribute(adddate, "TIP", "Start typing and the calendar will pop up");
  IupSetCallback(adddate, "K_ANY", (Icallback)calendar_cb);

  hbox2 = IupHbox(
    addtitle,
    adddate,
    NULL);
  IupSetAttribute(hbox2, "MARGIN", "10x0");
  IupSetAttribute(hbox2, "GAP", "0");
  IupSetAttribute(hbox2, "ALIGNMENT", MORERESALIGN);

  /*HBOX3*/
  btn_ok = IupButton("OK", NULL);
  IupSetAttribute(btn_ok, "SIZE", "30");
  IupSetAttribute(btn_ok, "NAME", "BUT_OK");
  IupSetAttribute(btn_ok, "VALUE", (char*) self);
  IupSetCallback(btn_ok, "ACTION", (Icallback)getback_addtodb_cb);          

  hbox3 = IupHbox(
    IupFill(),
    btn_ok,
    NULL);

  /*DIALOG*/
  vbox = IupVbox(
    hbox1,
    hbox2,
    hbox3,
    NULL);

  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "Add additional info");
  IupSetAttribute(dlg, "NAME", "DATETITLEDLG");
  IupSetAttributeHandle(dlg, "DEFAULTENTER", btn_ok);
  
  IupSetAttributeHandle(dlg, "PARENTDIALOG", IupGetDialog(self));
  IupSetAttributeHandle(self, "DLGDLG", dlg);                           // handle necessary to get the title and date in addtodb

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

#ifdef ADRESSESSCHECK
printf("getdateandtitle: dialog = %p, parentdialog = %p\n", dlg, IupGetDialog(self));        
printf("getdateandtitle: addtitle = %p, adddate = %p\n", addtitle, adddate);          
#endif
  
  return IUP_DEFAULT;
  }

int getback_addtodb_cb(Ihandle *self)
{
  // function checks the whether the user input does exist and then hide the dialog
  char *titleval;
  Ihandle *title = IupGetDialogChild(self, "TITLENEW");
  if (title != NULL)
    titleval = IupGetAttribute(IupGetDialogChild(self, "TITLENEW"), "VALUE");

  Ihandle *date = IupGetDialogChild(self,"DATENEW");
  char *dateval = IupGetAttribute(date, "VALUE");
  
  if (title!= NULL)
  {
    if (*titleval == '\0' || *dateval == '\0')
    {
      IupMessage("Empty cells", "Please make sure you fill all cells");
      return IUP_CONTINUE;
    }
  }
  else 
  {
    if (*dateval == '\0')
    {
      IupMessage("Empty cell", "Please make sure you fill the cell");
      return IUP_CONTINUE;
    }
  }
    
  IupHide(IupGetDialog(self));
  
if (title != NULL)
  printf("getback_addtodb_cb: title = %s, date = %s\n", titleval, dateval);
else
  printf("getback_addtodb_cb: date = %s\n", dateval);

  IupExitLoop();
  
  return IUP_DEFAULT;
}

int calendar_cb(Ihandle* self)
{   
  // function creates calendar which allows the user to pick up the date

  Ihandle *date, *btn_ok, *hbox1;
  Ihandle *dlg,*vbox;

  date = IupDatePick();
  IupSetAttribute(date,"NAME", "DATE");
  
  btn_ok = IupButton("OK",NULL);
  IupSetCallback(btn_ok, "ACTION", (Icallback)confirm_calendar_cb);
  IupSetAttribute(btn_ok, "PADDING", "20x2");

  hbox1 = IupHbox(
    IupFill(),
    btn_ok,
    NULL);

  IupSetAttribute(hbox1,"ALIGNMENT", "ACENTER");
  IupSetAttribute(hbox1,"GAP", "0");
  IupSetAttribute(hbox1,"MARGIN", "0x0");
  IupSetAttribute(hbox1,"NORMALIZESIZE", "HORIZONTAL");

  vbox = IupVbox(
    date,
    hbox1,
    NULL);
  
  IupSetAttribute(vbox,"ALIGNMENT", "ACENTER");
  IupSetAttribute(vbox,"GAP", "10");
  IupSetAttribute(vbox,"MARGIN", "10x10");

  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "Calendar");
  IupSetAttributeHandle(dlg, "DEFAULTENTER", btn_ok);
  IupSetAttributeHandle(dlg, "PARENTDIALOG", IupGetDialog(self));
  IupSetAttributeHandle(dlg, "CALLINGFNC", self);

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

#ifdef ADRESSESSCHECK
printf("calendar_cb: self - %p\n",self);
printf("calendar_cb: dlg - %p, vbox - %p, hbxo1 - %p, date - %p\n", dlg, vbox, hbox1, date);
#endif

  return IUP_DEFAULT;
}

int confirm_calendar_cb(Ihandle* self)
{
  // function puts the selected date in appopriate container

  Ihandle *maindatebox = IupGetAttributeHandle(IupGetDialog(self),"CALLINGFNC");
  Ihandle *datebox = IupGetDialogChild(self,"DATE");
  char * datev = IupGetAttribute(datebox, "VALUE");
  
#ifdef ADRESSESSCHECK 
printf("confirm_calendar_cb: self - %p, datebox - %p\n", maindatebox, datebox);
#endif

  IupSetStrf(maindatebox ,"VALUE","%s",datev);
  IupDestroy(IupGetDialog(self));

  return IUP_DEFAULT;
}

/*MANAGEMENT FUNCTIONS OF ENTRIES IN DATABASE*/
int item_select_cb(Ihandle *self)
{
  //function takes distance value from the db and place it into the pace calculator

  Ihandle* hbox = IupGetParent(self);                     
  Ihandle* length = IupGetChild(hbox,LENGTHPOS);                //getting distance container from the entry in database
  char *dist = IupGetAttribute(length, "VALUE");                // getting distance from main dialog
  Ihandle* distancea = IupGetDialogChild(self, "DISTANCE");     // getting containers from main dialog
  IupSetAttribute(distancea, "VALUE", dist);                     // setting values in selected containers
  
  return IUP_DEFAULT;
}

int item_delete_cb(Ihandle *self)
{
  //function gets appropriate containers from vbox and detach its hbox. Then renumbering of labels take place, and after all the deletion happens

  Ihandle* vboxmain = IupGetDialogChild(self, "VBOX");
  Ihandle* hbox = IupGetParent(self);
  Ihandle* fillarea = IupGetChild(hbox,FILLAREAPOS);
  Ihandle* position = IupGetChild(hbox,POSITIONPOS);
  Ihandle* title = IupGetChild(hbox,TITLEPOS);
  Ihandle* length = IupGetChild(hbox,LENGTHPOS);
  Ihandle* time = IupGetChild(hbox,TIMEPOS);
  Ihandle* moreres = IupGetChild(hbox,MORERESPOS);
  Ihandle* map = IupGetChild(hbox,MAPPOS);
  Ihandle* select = IupGetChild(hbox,SELECTPOS);
  Ihandle* delete = IupGetChild(hbox,DELETEPOS);
 
  Ihandle* moreresdlg = IupGetAttributeHandle(moreres, "MRDLG");              // getting more results appropriate container
  IupSetAttribute(moreresdlg,"DLTROW","1");                                   // own parameter which is used to inform moreres_delete_cb whether to show dialog or not (not - when user asks for delete the entire row from db, not the row from more res tab)
  
  if (moreresdlg)                                                             // checking wheter the dialog does exist or not
  {
    Ihandle* morereshbox = IupGetDialogChild(moreresdlg, "HBOXMORE");         // getting its childs
    Ihandle* moreresvbox = IupGetDialogChild(morereshbox, "VBOXMORE"); 
    unsigned int vboxelcount = IupGetChildCount(moreresvbox); 
    unsigned int firsthboxnr = MRHBOXOFFSETSTART;                             // first entry
    unsigned int lasthboxnr = vboxelcount - 1;                                // calculating the position of last entry
    Ihandle *hboxcurrent, *deletemr;

    for (unsigned int i = firsthboxnr; i <= lasthboxnr; i++)                  // deleting row after row; "i" is only a counter of a loop
    {
      hboxcurrent = IupGetChild(moreresvbox, firsthboxnr);                    // getting the appropriate hbox; always it's a first hbox 
      deletemr = IupGetChild(hboxcurrent,DELETEMRPOS); 
      
#ifdef ADRESSESSCHECK
printf("item_delete_cb: vboxelcount = %u, i = %u\n",vboxelcount,i);
printf("item_delete_cb: moreresdlg address = %p, morereshbox address = %p, moreresvbox address = %p, hboxcurrent address = %p, deletemr address = %p,\n",moreresdlg,morereshbox,moreresvbox,hboxcurrent,deletemr);
#endif

      moreres_delete_cb(deletemr);
    }
  }

  // releasing memory and resetting stack pointer 
  Item id = (Item) IupGetInt(delete, "OWN_ID");
  free(stacktab[id]);
  stacktab[id] = NULL;

#ifdef STACKCONTENT
puts("item_delete_cb:");
  stackscontent();
#endif
  // pushing entry to the top of the stack
  Item n = (Item) IupGetInt(self, "OWN_ID");
  PushStack(&st, n);

#ifdef ADRESSESSCHECK 
printf("item_delete_cb: fillarea %p, position %p, title %p, length %p, time %p, map %p, select %p, delete %p, hbox %p\n", fillarea, position, title, length, time, map, select, delete, hbox);
#endif 

  IupDetach(hbox);  // must be prior the renumbering

  if (!renumberpostitl(n, vboxmain))
  {
    IupMessage("Error in renumberpostitl",
      "Unknown error in renubmerpostitl. renumbering not done");
    return IUP_DEFAULT;
  }
    
  IupDestroy(fillarea);
  IupDestroy(position);
  IupDestroy(title);
  IupDestroy(length);
  IupDestroy(time);
  IupDestroy(moreres);
  IupDestroy(map);
  IupDestroy(select);
  IupDestroy(delete);
  IupDestroy(hbox);

  IupRefresh(vboxmain);

  return IUP_DEFAULT;
}

int moreresults_cb(Ihandle *self)
{
  // function creates dialog which contains historic results related to particular distance

  Ihandle *labelDistance, *containerDistance, *vbox1;
  Ihandle *labelDate, *labelTime, *labelTimeperkm, *labelSpeed, *labelDelete, *hbox1, *vbox2;
  Ihandle *hbox, *dlg, *separatorver;

                    /*VBOX1*/
  Ihandle* hboxparent = IupGetParent(self);                           // getting distance from parent
  Ihandle* length = IupGetChild(hboxparent,LENGTHPOS);
  char * dist = IupGetAttribute(length,"VALUE");

  labelDistance = IupLabel("Distance\n[km]");
  IupSetAttribute(labelDistance, "SIZE", MORERESSIZE);
  IupSetAttribute(labelDistance, "ALIGNMENT", MORERESALIGN);

  containerDistance = IupLabel("");
  IupSetAttribute(containerDistance, "SIZE", MORERESSIZE);
  IupSetAttribute(containerDistance, "ALIGNMENT", MORERESALIGN);
  IupSetStrf(containerDistance, "TITLE", "%s", dist);

  vbox1 = IupVbox(
      labelDistance,
      containerDistance,
      NULL);

                    /*VBOX2*/
  /*HBOX1*/
  labelDate = IupLabel("Date\n[year/month/day]");
  IupSetAttribute(labelDate, "SIZE", MORERESSIZE);
  IupSetAttribute(labelDate, "ALIGNMENT", MORERESALIGN);

  labelTime = IupLabel("Time\n[hrs:min:sec]");
  IupSetAttribute(labelTime, "SIZE", MORERESSIZE);
  IupSetAttribute(labelTime, "ALIGNMENT", MORERESALIGN);

  labelTimeperkm = IupLabel("Time per km\n[min:sec/km]");
  IupSetAttribute(labelTimeperkm , "SIZE", MORERESSIZE);
  IupSetAttribute(labelTimeperkm , "ALIGNMENT", MORERESALIGN);

  labelSpeed = IupLabel("Speed\n[km/h]");
  IupSetAttribute(labelSpeed, "SIZE", MORERESSIZE);
  IupSetAttribute(labelSpeed, "ALIGNMENT", MORERESALIGN);

  labelDelete = IupLabel("Delete");
  IupSetAttribute(labelDelete, "SIZE", MORERESSIZE);
  IupSetAttribute(labelDelete, "ALIGNMENT", MORERESALIGN);

  hbox1 = IupHbox(
    labelDate,
    labelTime,
    labelTimeperkm,
    labelSpeed,
    labelDelete,
    NULL);

  IupSetAttribute(hbox1, "MARGIN", "10x0");
  IupSetAttribute(hbox1, "GAP", "0");
  IupSetAttribute(hbox1, "ALIGNMENT", "ACENTER");

  vbox2 = IupVbox(
      hbox1,
      NULL);
  IupSetAttribute(vbox2, "NAME", "VBOXMORE");   

  /*DIALOG*/
  separatorver = IupLabel("");
  IupSetAttribute(separatorver, "SEPARATOR", "VERTICAL");
  IupSetAttribute(separatorver, "MARGIN", "10x10");
  IupSetAttribute(separatorver, "GAP", "10");

  hbox = IupHbox(
    vbox1,
    separatorver,
    IupScrollBox(vbox2),
    NULL);
  IupSetAttribute(hbox, "NAME", "HBOXMORE");

  dlg = IupDialog(hbox);
  IupSetAttribute(dlg, "TITLE", "More results");
  IupSetAttribute(dlg, "NAME", "MORERESULTS");
  IupSetCallback(dlg, "CLOSE_CB",(Icallback)moreresults_closedia_cb);
  IupSetAttribute(dlg,"DLTROW","0");                                        // own parameter which is used to inform moreres_delete_cb whether to show dialog or not (not - when user asks for delete the entire row from db, not the row from more res tab)
  IupSetAttributeHandle(dlg, "PARENTDIALOG", IupGetDialog(self));           // main dialog assigned to current dialog
  IupSetAttributeHandle(self, "MRDLG", dlg);                                // dialog assigned to calling function
  IupSetAttributeHandle(dlg, "MRCALLER", self);                             // calling function assigned to dialog

#ifdef ADRESSESSCHECK
printf("moreresults_cb: dlg adress = %p, hbox adress = %p, vbox address = %p\n", dlg, hbox, vbox2);
printf("moreresults_cb: dlg address = %p\n",IupGetAttributeHandle(self, "MRDLG"));
#endif

  return IUP_DEFAULT;
}

int moreres_createnew_cb(Ihandle *self)
{
  // function creates new blank row in the database "more results" tab(creates all containers in one row)

  Ihandle *DATEMR(id,n), *TIMEMR(id,n), *TIMEPERKMMR(id,n), *SPEEDMR(id,n), *DELETEMR(id,n), *HBOXMR(id,n); 
  Ihandle *dlgmain = IupGetDialog(self);
  Ihandle *vboxmain = IupGetDialogChild(self, "VBOX");

  Ihandle* dlgmore = IupGetAttributeHandle(self, "MRDLG");     // getting more results dlg 
  Ihandle* vboxmore = IupGetDialogChild(dlgmore, "VBOXMORE");     // getting appropriate vbox from dialog
  Ihandle* hboxmore = IupGetDialogChild(dlgmore, "HBOXMORE");     // getting  appropriate hbox from dialog

  Ihandle* hboxcurrent = IupGetParent(self);                       // getting parent of calling function in order to get to proper stack
  Ihandle* deletedb = IupGetChild(hboxcurrent,DELETEPOS);
  Item id = (Item) IupGetInt(deletedb, "OWN_ID");

  if (EmptyStack(stacktab[id]))                                                      // early exit - stack is empty, limit of entries in db reached
  {
    IupMessage("Cannot add new position","Too many positions - cannot add new position\nPlease delete some positions");
    return IUP_DEFAULT;
  }

  Item n;           
  PopStack(stacktab[id], &n);                                                            // getting number from the stack

  DATEMR(id,n) = IupLabel("");
  IupSetAttribute(DATEMR(id,n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(DATEMR(id,n), "ALIGNMENT", RESULTLBLALIGN);
  // IupSetStrf(DATEMR(id,n), "NAME", "DATEMR%u", n);
  IupSetAttribute(DATEMR(id,n), "TITLE", "");               
    
  TIMEMR(id,n) = IupLabel("");
  IupSetAttribute(TIMEMR(id,n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(TIMEMR(id,n), "ALIGNMENT", RESULTLBLALIGN);
  // IupSetStrf(TIMEMR(id,n), "NAME", "TIMEMR%u", n);                                 
  IupSetAttribute(TIMEMR(id,n), "TITLE", "");

  TIMEPERKMMR(id,n) = IupLabel("");
  IupSetAttribute(TIMEPERKMMR(id,n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(TIMEPERKMMR(id,n), "ALIGNMENT", RESULTLBLALIGN);
  // IupSetStrf(TIMEPERKMMR(id,n), "NAME", "TIMEPERKMMR%u", n);                                 
  IupSetAttribute(TIMEPERKMMR(id,n), "TITLE", "");

  SPEEDMR(id,n) = IupLabel("");
  IupSetAttribute(SPEEDMR(id,n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(SPEEDMR(id,n), "ALIGNMENT", RESULTLBLALIGN);
  // IupSetStrf(SPEEDMR(id,n), "NAME", "SPEEDMR%u", n);                                 
  IupSetAttribute(SPEEDMR(id,n), "TITLE", "");

  DELETEMR(id,n) = IupButton(NULL, NULL);
  IupSetAttribute(DELETEMR(id,n), "SIZE", RESULTLBLSIZE);
  IupSetAttribute(DELETEMR(id,n), "ALIGNMENT", RESULTLBLALIGN);
  IupSetAttribute(DELETEMR(id,n), "IMAGE", "IUP_EditErase");
  IupSetAttribute(DELETEMR(id,n), "FLAT", "Yes");
  IupSetAttribute(DELETEMR(id,n), "CANFOCUS", "No");
  // IupSetStrf(DELETEMR(id,n), "NAME", "DELETEMR%u", n);
  IupSetCallback(DELETEMR(id,n), "ACTION", (Icallback)moreres_delete_cb);
  IupSetAttribute(DELETEMR(id,n), "TIP", "Delete selected entry");
  IupSetStrf(DELETEMR(id,n), "DELETEMR OWN_ID", "%u", n);                           // Own attribute equal to stack number used for moreres_delete_cb to push entry back into stack
  // IupSetAttribute(DELETEMR(id,n),"DLTROW","0");                                     // own attribute 

  HBOXMR(id,n) = IupHbox(
    DATEMR(id,n),
    TIMEMR(id,n),
    TIMEPERKMMR(id,n),
    SPEEDMR(id,n),
    DELETEMR(id,n),
    NULL);

  IupSetStrf(HBOXMR(id,n), "NAME", "HBOXMR%u", n);
  IupSetAttribute(HBOXMR(id,n), "MARGIN", "10x0");
  IupSetAttribute(HBOXMR(id,n), "GAP", "0");
  IupSetAttribute(HBOXMR(id,n), "ALIGNMENT", "ACENTER");

#ifdef ADRESSESSCHECK
printf("moreres_createnew_cb: dlgmain adress - %p, vboxmain adress - %p\n", dlgmain, vboxmain);
printf("moreres_createnew_cb: hboxcurrent adress - %p, moreres address - %p, vboxmore address - %p, hboxmore address - %p, dlgmore address - %p\n", IupGetParent(self), self, vboxmore, hboxmore, dlgmore);
printf("moreres_createnew_cb: date %p, time %p, timeperkm %p, speed %p, delete %p, hbox %p\n",DATEMR(id,n),TIMEMR(id,n),TIMEPERKMMR(id,n),SPEEDMR(id,n),DELETEMR(id,n),HBOXMR(id,n));
#endif

  IupAppend(vboxmore, HBOXMR(id,n));                                     // adding new hbox into the vbox
  IupMap(HBOXMR(id,n));
  IupRefresh(vboxmore);
  IupRefresh(hboxmore);
  IupRefresh(dlgmore);

  return IUP_DEFAULT;
}

int show_moreresultsdlg_cb(Ihandle *self)
{
  // function shows the more results tab
  Ihandle *dlg = IupGetAttributeHandle(self, "MRDLG");  
  IupSetCallback(dlg, "CLOSE_CB",(Icallback)moreresults_closedia_cb);
  IupSetAttribute(dlg, "SIZE", "THIRDxQUARTER");
  IupSetAttribute(dlg, "EXPAND", "YES");

#ifdef ADRESSESSCHECK
printf("show_moreresultsdlg_cb: dlg adress = %p, self address = %p\n", dlg, self);
#endif

  IupShowXY(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
  return IUP_DEFAULT;
}

int moreres_delete_cb(Ihandle *self)
{
  // function gets appropriate containers from vbox and detach its hbox. Then renumbering of labels take place, and after all the deletion happens

  Ihandle* dlgmr = IupGetDialog(self);                            // getting appropriate containers from more results tab in order to delete
  Ihandle* vboxmr = IupGetParent(IupGetParent(self));
  Ihandle* hboxmr = IupGetParent(self);
  Ihandle* datemr = IupGetChild(hboxmr ,DATEMRPOS);
  Ihandle* timemr = IupGetChild(hboxmr ,TIMEMRPOS); 
  Ihandle* timeperkmmr = IupGetChild(hboxmr ,TIMEPERKMMRPOS);
  Ihandle* speedmr = IupGetChild(hboxmr ,SPEEDMRPOS); 
  Ihandle* deletemr = IupGetChild(hboxmr,DELETEMRPOS);              

  unsigned int flag = IupGetInt(dlgmr,"DLTROW");                  // getting a flag whether the delete is called by more results tab or db from main dialog      
  
  Ihandle *mrcaller = IupGetAttributeHandle(dlgmr, "MRCALLER");   // getting a calling function - in this case - more results button
  Ihandle* hboxcurrent = IupGetParent(IupGetAttributeHandle(self, "MRCALLER"));        // getting parent of calling function
  Ihandle* deletedb = IupGetChild(hboxcurrent,DELETEPOS);
  Item id = (Item) IupGetInt(deletedb, "OWN_ID");

  // pushing entry to the top of the stack
  Item n = (Item) IupGetInt(self, "DELETEMR OWN_ID");
  PushStack(stacktab[id], n);

#ifdef ADRESSESSCHECK 
printf("moreres_delete_cb: dlg adress = %p, mrcaller = %p\n", dlgmr, mrcaller);
printf("moreres_delete_cb: datemr %p, timemr %p, timeperkmmr %p, speedmr %p, deletemr %p, hboxmr %p, vboxmr %p\n",  datemr, timemr, timeperkmmr, speedmr, deletemr, hboxmr, vboxmr);
#endif 

  IupDetach(hboxmr);                                            // must be prior the renumbering
  IupDestroy(datemr);
  IupDestroy(timemr);
  IupDestroy(timeperkmmr);
  IupDestroy(speedmr);
  IupDestroy(deletemr);
  IupDestroy(hboxmr);

  IupRefresh(self);

  if (!flag)                                                   // refreshes the dialog depending on flag status  (0 - more res; 0 - main dialog db)
  {
    show_moreresultsdlg_cb(mrcaller);
    Ihandle *hboxdb = IupGetParent(mrcaller);                       // getting corresponding hbox
    Ihandle *besttimedb = IupGetChild(hboxdb,TIMEPOS);              // getting respective time container from db

#ifdef ADRESSESSCHECK 
printf("moreres_delete_cb: hboxdb adress = %p, besttimedb = %p\n", hboxdb, besttimedb);
#endif 

    set_besttime(besttimedb, vboxmr); 
  }
  return IUP_DEFAULT;
}

int moreresults_closedia_cb(Ihandle *self)
{
  // function hides the moreresults dialog
  IupHide(IupGetDialog(self));
  return IUP_DEFAULT;
}

/*PACE CALCULATIONS*/
int calculate_running_cb(Ihandle *self)
{
  // function used for calculating runnig parameters

  Ihandle* distancea = IupGetDialogChild(self, "DISTANCE");     // getting containers from main dialog
  Ihandle* timea = IupGetDialogChild(self, "TIME");
  Ihandle* timeperkma = IupGetDialogChild(self, "TIMEPERKM");
  Ihandle* speeda = IupGetDialogChild(self, "SPEED");

  double distance = IupGetDouble(distancea, "VALUE");           // getting distance from main dialog
  char *tab = IupGetAttribute(timea, "TITLE");                  // getting time from main dialog

  int hours, minutes, seconds;                                  // total time in format h:m:s separated into h,m,s
  double speed;                                                 // speed
  double tempo;                                                 // pace [sec/km]
  int tempomin;                                                 // pace [min]- minutes' part, for showing results
  double temposec;                                              // pace [sec] seconds' part, for showing results
  int timetot;                                                  // total time [sec]
  int speedfltpt;                                               // speed [km/h] - floating part, for showing results

/*************dividing total time into separate containers*************/
  int count = 0;
  char temph[TEMPSIZETIME], tempm[TEMPSIZETIME], temps[TEMPSIZETIME];
  int i = 0, j = 0, k = 0, l = 0;

  while (tab[i] != '\0')
  {
    if (tab[i] == ':')
      count++;
    else
    {
      if (count == 0)
        temph[j++] = tab[i];
      else if (count == 1)
        tempm[k++] = tab[i];
      else if (count == 2)
        temps[l++] = tab[i];
    }
    i++; 
  }

  temph[j] = '\0';
  tempm[k] = '\0';
  temps[l] = '\0';

  hours = atoi(temph);
  minutes = atoi(tempm);
  seconds = atoi(temps);

//*************running calculataions**************
  if (hours == 0 && minutes == 0 && seconds == 0)                   // early exit
  {
    return IUP_DEFAULT;
  }

  timetot = hours*HTOMIN*MINTOSEK + minutes*MINTOSEK + seconds;
  tempo = timetot / distance;                                       // in sec/km
  tempomin = (int) floor((tempo/ MINTOSEK));
  temposec =  (tempo / MINTOSEK - (double) tempomin) * MINTOSEK; 

  if (((int) round(temposec)) < 10)                                 // adding first 0 into seconds parameter smaller than 10
    IupSetStrf(timeperkma, "TITLE", "%d:0%d", tempomin, ((int) round(temposec)));     // setting calculated value into container in main dialog
  else
    IupSetStrf(timeperkma, "TITLE", "%d:%d", tempomin, ((int) round(temposec)));

  speed = 1/((double) tempomin*MINTOSEK + temposec) * HTOSEK;
  speedfltpt = (int) round(((speed - floor(speed))*100));

  if  (speedfltpt < 10)                                             // adding first 0 into float part of speed result whenr smaller than 10
    IupSetStrf(speeda, "TITLE", "%d:0%d", (int) floor(speed), speedfltpt);            // setting calculated value into container in main dialog
  else
    IupSetStrf(speeda, "TITLE", "%d:%d", (int) floor(speed), speedfltpt);

  return IUP_DEFAULT;
}

/*TIME INPUT DIALOG*/
int timeinput_cb(Ihandle *self, int c)
{
  // function creates dialog for time input

  Ihandle *labeltimedesch, *labeltimedescm, *labeltimedescs, *hours, *minutes, *seconds;
  Ihandle *btn, *hbox1, *hbox2, *hbox3, *vbox, *dlg;

  /*HBOX1*/
  labeltimedesch = IupLabel("hrs:");
  IupSetAttribute(labeltimedesch, "SIZE", TIMEINPSIZE);
  IupSetAttribute(labeltimedesch, "ALIGNMENT", TIMEINPALIGN);

  labeltimedescm = IupLabel("min:");
  IupSetAttribute(labeltimedescm, "SIZE", TIMEINPSIZE);
  IupSetAttribute(labeltimedescm, "ALIGNMENT", TIMEINPALIGN);

  labeltimedescs = IupLabel("sec:");
  IupSetAttribute(labeltimedescs, "SIZE", TIMEINPSIZE);
  IupSetAttribute(labeltimedescs, "ALIGNMENT", TIMEINPALIGN);

  hbox1 = IupHbox(
    labeltimedesch,
    labeltimedescm,
    labeltimedescs,
    NULL);
  IupSetAttribute(hbox1, "MARGIN", "10x0");
  IupSetAttribute(hbox1, "GAP", "0");
  IupSetAttribute(hbox1, "ALIGNMENT", TIMEINPALIGN);

  /*HBOX2*/
  hours = IupText(NULL);
  IupSetAttribute(hours, "SIZE", TIMEINPSIZE);
  IupSetAttribute(hours, "NAME", "HOURS");
  IupSetAttribute(hours, "MASK", IUP_MASK_UINT);
  IupSetAttribute(hours, "VALUE", "");
  IupSetCallback(hours, "CARET_CB", (Icallback)checkdata_time_cb);

  minutes = IupText(NULL);
  IupSetAttribute(minutes, "SIZE", TIMEINPSIZE);
  IupSetAttribute(minutes, "NAME", "MINUTES");
  IupSetAttribute(minutes, "MASK", IUP_MASK_UINT);
  IupSetAttribute(minutes, "VALUE", "");
  IupSetCallback(minutes, "CARET_CB", (Icallback)checkdata_time_cb);

  seconds = IupText(NULL);
  IupSetAttribute(seconds, "SIZE", TIMEINPSIZE);
  IupSetAttribute(seconds, "NAME", "SECONDS");
  IupSetAttribute(seconds, "MASK", IUP_MASK_UINT);
  IupSetAttribute(seconds, "VALUE", "");
  IupSetCallback(seconds, "CARET_CB", (Icallback)checkdata_time_cb);

  hbox2 = IupHbox(
    hours,
    minutes,
    seconds,
    NULL);
  IupSetAttribute(hbox2, "MARGIN", "10x0");
  IupSetAttribute(hbox2, "GAP", "0");
  IupSetAttribute(hbox2, "ALIGNMENT", "ACENTER");

  /*HBOX3*/
  btn = IupButton("OK", NULL);
  IupSetAttribute(btn, "SIZE", "30");
  IupSetCallback(btn, "ACTION", (Icallback)timeinput_buttonok_cb);

  hbox3 = IupHbox(
    IupFill(),
    btn,
    NULL
  );

  /*DIALOG*/
  vbox = IupVbox(
    hbox1,
    hbox2,
    hbox3,
    NULL);

  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "Enter time of the run");
  IupSetAttribute(dlg, "NAME", "TIMEDIALOG");
  IupSetAttribute(dlg, "SHOWNOFOCUS", "YES");
  IupSetAttributeHandle(dlg, "DEFAULTENTER", btn);

  /* parent for pre-defined dialogs in closed functions (IupMessage) */
  IupSetAttributeHandle(dlg, "PARENTDIALOG", IupGetDialog(self));
  IupSetAttributeHandle(dlg, "CALLINGFNC", self);

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

#ifdef ADRESSESSCHECK
printf("timeinput_cb: dialog = %p\n", dlg);
#endif
    
  return IUP_DEFAULT;
}

int timeinput_buttonok_cb(Ihandle *self)
{
  // function called when button "OK" from timeinput_cb is pressed; sets the value into main dialog time container

  Ihandle *uppdlg = IupGetDialog(self);                                             // getting upper dialog
  Ihandle *maindlg = IupGetAttributeHandle(uppdlg,"PARENTDIALOG");                  // getting main dialog
  Ihandle *timea = IupGetAttributeHandle(uppdlg, "CALLINGFNC");                     // getting calling function: time button from main()

  int hours = IupGetInt(IupGetDialogChild(self, "HOURS"), "VALUE");
  int minutes = IupGetInt(IupGetDialogChild(self, "MINUTES"), "VALUE");
  int seconds = IupGetInt(IupGetDialogChild(self, "SECONDS"), "VALUE");

  IupSetStrf(timea, "TITLE", "%d:%d:%d", hours, minutes, seconds);

#ifdef ADRESSESSCHECK
printf("timeinput_buttonok_cb: dialog = %p\n", uppdlg);
printf("timeinput_buttonok_cb: main dialog = %p\n", maindlg);
#endif

  IupDestroy(uppdlg);     // destroys upper dialog

  return IUP_DEFAULT;
}

/*CHECKS OF INPUT DATA*/
int checkdata_distance_cb(Ihandle *self)
{
  // function checks the distance against the limits

  double distance = IupGetDouble(self, "VALUE");

  if (!(distance > 0.00 && distance < LIMIT))
  {
    IupSetAttribute(self,"VALUE", "");
    if (distance != 0.00)                   // zero excluded to avoid doubling IUPMESSAGE after setting value to empty; possible change of behavior when CARET_CB will be changed to KILLFOCUS_CB
    {
      IupMessage("Incorrect data",
      "The data you entered is incorrect.\nThe distance you enter shall be > 0km & < 200km");
    }
  }

  return IUP_DEFAULT;
}

int distance_dcmplcs_cb(Ihandle *self)
{
  // function sets distance into appropriate format; called by KILLFOCUS_CB 

  double distance = IupGetDouble(self,"VALUE");

#ifdef DISTANCECHECK
printf("distance_dcmplcs_cb, distance = %lf\n", distance);
#endif

  if(distance == 0)
    IupSetAttribute(self,"VALUE", "");
  else
    IupSetStrf(self,"VALUE","%.3lf", distance);

  return IUP_DEFAULT;
}

int checkdata_time_cb(Ihandle *self)
{
  // function checks time against the limits

  Ihandle* hoursa = IupGetDialogChild(self, "HOURS");
  Ihandle* minutesa = IupGetDialogChild(self, "MINUTES");
  Ihandle* secondsa = IupGetDialogChild(self, "SECONDS");

  int hours = IupGetInt(hoursa, "VALUE");
  int minutes = IupGetInt(minutesa, "VALUE");
  int seconds = IupGetInt(secondsa, "VALUE");

  if (hours)
    if (!(hours >= 0 && hours < LIMITHOURS))
      {
        IupMessage("Incorrect data",
        "The time you entered is incorrect.\nLimit of hours - 24h");
        IupSetAttribute(hoursa,"VALUE", "");
      }

  if (minutes)
    if (!(minutes >= 0 && minutes < LIMITMINUTES))
        {
          IupMessage("Incorrect data",
          "The time you entered is incorrect.\nLimit of minutes - 60min");
          IupSetAttribute(minutesa,"VALUE", "");
        }

  if (seconds)
    if (!(seconds >= 0 && seconds < LIMITSECONDS))
        {
          IupMessage("Incorrect data",
          "The time you entered is incorrect.\nLimit sekund - 60sek");
          IupSetAttribute(secondsa,"VALUE", "");
        }

  return IUP_DEFAULT;
}

bool label_check(Ihandle *self)
{
  // function checks whether there are no labels in the db; called immediately before exiting program; returs false when labels are missing

  Ihandle *hboxcurrent, *title;
  char *titleval;
  Ihandle* vboxmain = IupGetDialogChild(self, "VBOX");        // getting vbox from main dialog
  unsigned int vboxelcount = IupGetChildCount(vboxmain);      // amount of elements in vbox
  unsigned int firsthboxnr = DBHBOXOFFSETSTART;
  unsigned int latesthboxnr = vboxelcount - DBHBOXOFFSETEND;  // calculating current position of new entry in database

#ifdef ADRESSESSCHECK
printf("label_check: dlg adress - %p, vbox adress - %p\n", self, vboxmain);
#endif 
  
  for (unsigned int i = firsthboxnr ; i < latesthboxnr; i++)  // checks row after row
  {
    hboxcurrent = IupGetChild(vboxmain, i);                   // getting appropriate hbox from vbox
    title = IupGetChild(hboxcurrent,TITLEPOS);                // getting appropriate containers from hbox
    titleval = IupGetAttribute(title, "VALUE");

    if (strcmp(titleval,"\0") == 0)
    {
      IupMessage("Data missing","Please fill all title cells before exit");
      return false;
    }
  }
  return true;
}

int nodouble_label_check(Ihandle *self) 
{
  // function called by getdateandtitle; checks whether the title of entry one try to add is already in db
  Ihandle *hboxcurrent, *title;
  char *titleval;

  Ihandle *titlebox = IupGetDialogChild(self,"TITLENEW");     // getting date value from upper dialog
  char * titlev = IupGetAttribute(titlebox, "VALUE");
  Ihandle *dlgmain = IupGetAttributeHandle(self, "PARENTDIALOG");
  Ihandle *vboxmain = IupGetDialogChild(dlgmain, "VBOX");        // getting vbox from main dialog
  unsigned int vboxelcount = IupGetChildCount(vboxmain);      // amount of elements in vbox
  unsigned int firsthboxnr = DBHBOXOFFSETSTART;
  unsigned int latesthboxnr = vboxelcount - DBHBOXOFFSETEND;  // calculating current position of new entry in database

#ifdef ADRESSESSCHECK
printf("nodouble_label_check: dlg main - %p\n", dlgmain);
printf("nodouble_label_check: dlg adress - %p, vbox adress - %p\n", self, vboxmain);
#endif 
  
  for (unsigned int i = firsthboxnr ; i < latesthboxnr; i++)  // checks row after row
  {
    hboxcurrent = IupGetChild(vboxmain, i);                   // getting appropriate hbox from vbox
    title = IupGetChild(hboxcurrent,TITLEPOS);                // getting appropriate containers from hbox
    titleval = IupGetAttribute(title, "VALUE");

    if (strcmp(titleval,titlev) == 0)
    {
      IupMessage("Double alert","The title does exist in database. Please use another title");
      IupSetAttribute(titlebox,"VALUE","");
      return IUP_DEFAULT;
    }
  }
  return IUP_DEFAULT;
}

unsigned int nodouble_db_check(char * dist, Ihandle *vboxmain, unsigned int lasthboxnr)
{
  // function called by addtodb_cb; checks whether the entry one try to add is already in db; returns true when there is a double

  Ihandle* hboxcurrent, *length, *time; 
  char *lengthval, *timeval;
  unsigned int firsthboxnr = DBHBOXOFFSETSTART;

  for (unsigned int i = firsthboxnr ; i <= lasthboxnr; i++)    // function compares new entry with all entries present in db
  {
    hboxcurrent = IupGetChild(vboxmain, i);                     // getting appropriate hbox from vbox
    length = IupGetChild(hboxcurrent,LENGTHPOS);                // getting appropriate containers from hbox
    // time = IupGetChild(hboxcurrent,TIMEPOS);

    lengthval = IupGetAttribute(length, "VALUE");               // getting appropriate values from containers
    // timeval = IupGetAttribute(time, "VALUE");

#ifdef DBLDBCHECK
// printf("nodouble_db_check: firstboxnr = %u, current i = %u, latesthboxnr = %u\n", firsthboxnr, i, latesthboxnr);
// printf("nodouble_db_check: lengthval = %s, timeval = %s\n", lengthval, timeval);
#endif
    if (strcmp(dist,lengthval) == 0) // comparison of values 
        return i;
  }

  return 0;
}

bool nodouble_dbne_check(char * dist, char *nm, Ihandle *vboxmain, unsigned int latesthboxnr)
{
  // function called by confirm_newentry_cb; checks whether the entry one try to add is already in db; returns true when there is a double

  Ihandle* hboxcurrent, *length, *title; 
  char *lengthval, *titleval;
  unsigned int firsthboxnr = DBHBOXOFFSETSTART;

  for (unsigned int i = firsthboxnr ; i < latesthboxnr; i++)  // function compares new entry with all entries present in db
  {
    hboxcurrent = IupGetChild(vboxmain, i);                   // getting appropriate hbox from vbox
    length = IupGetChild(hboxcurrent,LENGTHPOS);              // getting appropriate containers from hbox
    title = IupGetChild(hboxcurrent,TITLEPOS);

    lengthval = IupGetAttribute(length, "VALUE");             // getting appropriate values from containers
    titleval = IupGetAttribute(title, "VALUE");

#ifdef DBLDBCHECK
// printf("nodouble_dbne_check: firstboxnr = %u, current i= %u, latesthboxnr = %u\n", firsthboxnr, i, latesthboxnr);
// printf("nodouble_dbne_check: lengthval = %s, timeval = %s\n", lengthval, timeval);
#endif

    if (strcmp(dist,lengthval) == 0 || strcmp(nm,titleval) == 0)      // comparison of values 
    {
      if (strcmp(dist,lengthval) == 0 && strcmp(nm,titleval) == 0)
        IupMessage("Double warning","The entry with declared parameters does exist in the database\n- cannot be added");
      else if (strcmp(dist,lengthval) == 0)
        IupMessage("Double warning","The entry with declared distance does exist in the database\n- cannot be added");
      else
        IupMessage("Double warning","The entry with declared title does exist in the database\n- cannot be added");
      return true;
    }
       
  }

  return false;
}

/*ICONS*/

static Ihandle* create_image_addicon(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 31, 128, 31, 255, 31, 128, 31, 255, 31, 128, 31, 255, 31, 128, 31, 255, 31, 128, 31, 255, 31, 128, 31, 255, 31, 128, 31, 255, 31, 128, 31, 255, 31, 128, 31, 255, 31, 128, 31, 255, 31, 128, 31, 255, 31, 128, 31, 255, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 128, 31, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Ihandle* image_add = IupImageRGBA(16, 16, imgdata);
  //IupSetAttribute(image_add, "AUTOSCALE","1.5");
  IupSetAttribute(image_add, "RESIZE","24x24");
  IupSetHandle("IMGADD", image_add);
  return image_add;
}

static Ihandle* create_image_locationicon(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 79, 0, 0, 0, 159, 0, 0, 0, 191, 0, 0, 0, 223, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 223, 0, 0, 0, 191, 0, 0, 0, 159, 0, 0, 0, 79, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 159, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 159, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 143, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 143, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 207, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 208, 0, 0, 0, 128, 0, 0, 0, 64, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 64, 0, 0, 0, 128, 0, 0, 0, 208, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 207, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 239, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 192, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 192, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 239, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 239, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 240, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 239, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 223, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 240, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 223, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 240, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 176, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 176, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 208, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 208, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 111, 0, 0, 0, 111, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 144, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 112, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 112, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 240, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 144, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 223, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 223, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 191, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 191, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 176, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 223, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 223, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 176, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 240, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 208, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 239, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 223, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 208, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 207, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 191, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 240, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 159, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 159, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 208, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 208, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 208, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 208, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 208, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 208, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 208, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 96, 0, 0, 0, 96, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 208, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 208, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 208, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 208, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 192, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 0, 0, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Ihandle* image_location = IupImageRGBA(48, 48, imgdata);
  IupSetAttribute(image_location, "RESIZE","24x24");
  IupSetHandle("IMGLOC", image_location);
  return image_location;
}

static Ihandle* create_image_checkicon(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 53, 0, 0, 0, 95, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 0, 0, 240, 0, 0, 0, 254, 0, 0, 0, 108, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 228, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 227, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 227, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 227, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 227, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 231, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 231, 0, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 231, 0, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 0, 0, 239, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 232, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 0, 0, 0, 253, 0, 0, 0, 234, 0, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 240, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 234, 0, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 232, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 238, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 235, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 78, 0, 0, 0, 251, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 231, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 237, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 234, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 78, 0, 0, 0, 252, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 227, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 237, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 234, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 252, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 223, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 237, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 234, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 252, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 223, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 237, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 237, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 252, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 219, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 237, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 237, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 215, 0, 0, 0, 18, 0, 0, 0, 45, 0, 0, 0, 237, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 237, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 86, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 211, 0, 0, 0, 236, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 238, 0, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 253, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 237, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 91, 0, 0, 0, 253, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 240, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 91, 0, 0, 0, 253, 0, 0, 0, 240, 0, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 93, 0, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Ihandle* image_check = IupImageRGBA(48, 48, imgdata);
  IupSetAttribute(image_check, "RESIZE","24x24");
  IupSetHandle("IMGCHCK", image_check);
  return image_check;
}



