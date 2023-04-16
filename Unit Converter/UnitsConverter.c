#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <iup.h>

/*TEMPERATURE LIMITS*/
#define KLIMBOT 0
#define KLIMTOP 1000
#define CLIMBOT -273.15
#define CLIMTOP 726.85
#define FLIMBOT -459.67
#define FLIMTOP 1340.33

/*LENGTHS DEFINITIONS*/
#define MTOCM 100.00
#define MTOMM 1000.00
#define CMTOMM 10.00
#define INTOMM 25.4
#define FTTOMM 304.8
#define INTOCM 2.54
#define FTTOCM 30.48
#define INTOM 0.0254
#define FTTOM 0.3048
#define INTOFT 1.00/12.00

/*PRESSURE DEFINITIONS*/
#define GRAVITY 9.80665
#define GTOKG 0.001
#define TONNETOKG 1000.00

/*GENERAL DEFINITIONS*/
#define DBLFLPTFORMATSIZE 20 //double-precision floating-point format size assumed as 1 int + dot + 16 after dot + zero sign + one extra
#define DROPDOWNSIZE 60


double celciustofahrenheit(double value);
double fahrenheittocelcius(double value);
double celciustokelvin(double value);
double kelvintocelcius(double value);
double fahrenheittokelvin(double value);
double kelvintofahrenheit(double value);
bool tmpr_value_limits(double value, char * unitin);
int calculate_tmpr_cb(Ihandle *self);
int calculate_pressure_cb(Ihandle *self);
int calculate_pressurerv_cb(Ihandle *self);
int calculate_area_cb(Ihandle *self);
void show_results(Ihandle *out, int notationtype, double result, unsigned int accuracy);

int main(int argc, char **argv)
{ 
  IupSetGlobal("UTF8MODE", "Yes");

  //general declarations
  Ihandle *vbox, *dlg, *descrtmpr, *descrftopr, *descrarea, *hboxlabtmpr, *hboxlabftopr, *hboxlabarea;
  Ihandle *separatorhor1, *separatorhor2, *signature, *hboxsignature;
  //temperature declarations
  Ihandle *labelsigntmpr, *list_dropdowntmprin,*list_dropdowntmprout,  *btntmpr, *tmprin, *tmprout, *hboxtmpr;    
  //force to pressure declarations  
  Ihandle *labelsignpr, *list_dropdownprinforce, *list_dropdownprinarea, *list_dropdownprout, *btnforcetopressure, *prin, *prout, *hboxpr, *scfipr, *spinpr;
  Ihandle *labelsignprrv, *list_dropdownprin, *list_dropdownproutforce, *list_dropdownproutarea, *btnforcetopressurerv, *prinrv, *proutrv, *hboxprrv, *scfiprrv, *spinprrv;
  //area declarations  
  Ihandle *labelsignarea, *list_dropdownareain, *list_dropdownareaout, *btnarea, *areain, *areaout, *hboxarea, *scfiarea, *spinarea;

  IupOpen(&argc, &argv);

  labelsigntmpr = IupLabel("=");
  IupSetAttribute(labelsigntmpr, "SIZE", "10");
  IupSetAttribute(labelsigntmpr, "ALIGNMENT", "ACENTER");


  /************************TEMPERATURE *************************/
  list_dropdowntmprin = IupList(NULL);
  IupSetAttributes (list_dropdowntmprin, "1=\"Celcius\", 2=\"Fahrenheit\", 3=\"Kelvin\","
                                   "DROPDOWN=YES, VISIBLEITEMS=4, VALUE = 1");
  IupSetAttribute(list_dropdowntmprin, "NAME", "TMPRUNITIN");
  IupSetAttribute(list_dropdowntmprin, "SIZE", "DROPDOWNSIZE");

  list_dropdowntmprout = IupList(NULL);
  IupSetAttributes (list_dropdowntmprout, "1=\"Celcius\", 2=\"Fahrenheit\", 3=\"Kelvin\","
                                   "DROPDOWN=YES, VISIBLEITEMS=4, VALUE = 2");
  IupSetAttribute(list_dropdowntmprout, "NAME", "TMPRUNITOUT");
  IupSetAttribute(list_dropdowntmprout, "SIZE", "DROPDOWNSIZE");
  
  // printf("list_dropdownin = %p, list_dropdownout = %p\n", list_dropdownin, list_dropdownout);
  // printf("list_dropdownin = %s, list_dropdownout = %s\n", IupGetAttribute(list_dropdownin,"VALUE"), IupGetAttribute(list_dropdownout,"VALUE"));
  
  btntmpr = IupButton("Calc", NULL);
  IupSetAttribute(btntmpr, "SIZE", "30");

  tmprin = IupText(NULL);
  IupSetAttribute(tmprin, "SIZE", "50");
  IupSetAttribute(tmprin, "NAME", "TMPRIN");
  IupSetAttribute(tmprin, "MASK", IUP_MASK_FLOAT);
  IupSetAttribute(tmprin, "VALUE", "");
  tmprout = IupText(NULL);
  IupSetAttribute(tmprout, "SIZE", "100");
  IupSetAttribute(tmprout, "NAME", "TMPROUT");
  IupSetAttribute(tmprout, "MASK", IUP_MASK_FLOAT);
  IupSetAttribute(tmprout, "ACTIVE", "NO");
  IupSetAttribute(tmprout, "VALUE", "");
  
  hboxtmpr = IupHbox(
    tmprin,
    list_dropdowntmprin,
    labelsigntmpr,
    btntmpr,
    tmprout,
    list_dropdowntmprout,
    NULL);
  IupSetAttribute(hboxtmpr, "MARGIN", "10x10");
  IupSetAttribute(hboxtmpr, "GAP", "10");
  IupSetAttribute(hboxtmpr, "ALIGNMENT", "ACENTER");




 /************************PRESSURE *************************/
  labelsignpr= IupLabel("=");
  IupSetAttribute(labelsignpr, "SIZE", "10");
  IupSetAttribute(labelsignpr, "ALIGNMENT", "ACENTER");

  list_dropdownprinforce = IupList(NULL);
  IupSetAttributes (list_dropdownprinforce, "1=\"N\", 2=\"kN\", 3=\"MN\", 4=\"GN\", 5=\"gram\", 6=\"kilogram\", 7=\"tonne\","
                                   "DROPDOWN=YES, VISIBLEITEMS=8, VALUE = 1");
  IupSetAttribute(list_dropdownprinforce, "NAME", "PRUNITIN");

  list_dropdownprinarea = IupList(NULL);
  IupSetAttributes (list_dropdownprinarea, "1=\"mm²\", 2=\"cm²\", 3=\"m²\","
                                   "DROPDOWN=YES, VISIBLEITEMS=4, VALUE = 1");
  IupSetAttribute(list_dropdownprinarea, "NAME", "PRAREAUNITIN");

  list_dropdownprout = IupList(NULL);
  IupSetAttributes (list_dropdownprout, "1=\"Pa\", 2=\"kPa\", 3=\"MPa\", 4=\"GPa\","
                                   "DROPDOWN=YES, VISIBLEITEMS=5, VALUE = 1");
  IupSetAttribute(list_dropdownprout, "NAME", "PRUNITOUT");
  
  btnforcetopressure = IupButton("Calc", NULL);
  IupSetAttribute(btnforcetopressure, "SIZE", "30");

  prin = IupText(NULL);
  IupSetAttribute(prin, "SIZE", "50");
  IupSetAttribute(prin, "NAME", "PRIN");
  IupSetAttribute(prin, "MASK", IUP_MASK_FLOAT);

  prout = IupText(NULL);
  IupSetAttribute(prout, "SIZE", "100");
  IupSetAttribute(prout, "NAME", "PROUT");
  IupSetAttribute(prout, "MASK", IUP_MASK_FLOAT);
  IupSetAttribute(prout, "ACTIVE", "NO");


  scfipr = IupToggle("Scientific notation", NULL);
  IupSetAttribute(scfipr, "NAME", "SCFIPR");
  IupSetCallback(scfipr, "VALUECHANGED_CB", (Icallback)calculate_pressure_cb);

  spinpr = IupText(NULL);
  IupSetAttributes(spinpr, "SPIN=Yes, SPINMIN=0, SPINMAX=16, RASTERSIZE=48x, NAME=SPINPR, VALUE=3");
  IupSetAttribute(spinpr, "TIP", "Number of decimal places");
  IupSetCallback(spinpr, "VALUECHANGED_CB", (Icallback)calculate_pressure_cb);

  hboxpr = IupHbox(
    prin,
    list_dropdownprinforce,
    list_dropdownprinarea,
    labelsignpr,
    btnforcetopressure,
    prout,
    spinpr,
    list_dropdownprout,
    scfipr,
    NULL);
  IupSetAttribute(hboxpr, "MARGIN", "10x10");
  IupSetAttribute(hboxpr, "GAP", "10");
  IupSetAttribute(hboxpr, "ALIGNMENT", "ACENTER");


/***************PRESSURE - REVERSED*************/

  labelsignprrv= IupLabel("=");
  IupSetAttribute(labelsignprrv, "SIZE", "10");
  IupSetAttribute(labelsignprrv, "ALIGNMENT", "ACENTER");

  list_dropdownprin = IupList(NULL);
  IupSetAttributes (list_dropdownprin, "1=\"Pa\", 2=\"kPa\", 3=\"MPa\", 4=\"GPa\","
                                   "DROPDOWN=YES, VISIBLEITEMS=5, VALUE = 1");
  IupSetAttribute(list_dropdownprin, "NAME", "PRUNITINRV");
  
  list_dropdownproutforce = IupList(NULL);
  IupSetAttributes (list_dropdownproutforce, "1=\"N\", 2=\"kN\", 3=\"MN\", 4=\"GN\", 5=\"gram\", 6=\"kilogram\", 7=\"tonne\","
                                   "DROPDOWN=YES, VISIBLEITEMS=8, VALUE = 1");
  IupSetAttribute(list_dropdownproutforce, "NAME", "PRUNITOUTFORCE");

  list_dropdownproutarea = IupList(NULL);
  IupSetAttributes (list_dropdownproutarea, "1=\"mm²\", 2=\"cm²\", 3=\"m²\","
                                   "DROPDOWN=YES, VISIBLEITEMS=4, VALUE = 1");
  IupSetAttribute(list_dropdownproutarea, "NAME", "PRUNITOUTAREA");

  btnforcetopressurerv = IupButton("Calc", NULL);
  IupSetAttribute(btnforcetopressurerv, "SIZE", "30");

  prinrv = IupText(NULL);
  IupSetAttribute(prinrv, "SIZE", "50");
  IupSetAttribute(prinrv, "NAME", "PRINRV");
  IupSetAttribute(prinrv, "MASK", IUP_MASK_FLOAT);

  proutrv = IupText(NULL);
  IupSetAttribute(proutrv, "SIZE", "100");
  IupSetAttribute(proutrv, "NAME", "PROUTRV");
  IupSetAttribute(proutrv, "MASK", IUP_MASK_FLOAT);
  IupSetAttribute(proutrv, "ACTIVE", "NO");

  scfiprrv = IupToggle("Scientific notation", NULL);
  IupSetAttribute(scfiprrv, "NAME", "SCFIPRRV");
  IupSetCallback(scfiprrv, "VALUECHANGED_CB", (Icallback)calculate_pressurerv_cb);

  spinprrv = IupText(NULL);
  IupSetAttributes(spinprrv, "SPIN=Yes, SPINMIN=0, SPINMAX=16, RASTERSIZE=48x, NAME=SPINPRRV, VALUE=3");
  IupSetAttribute(spinprrv, "TIP", "Number of decimal places");
  IupSetCallback(spinprrv, "VALUECHANGED_CB", (Icallback)calculate_pressurerv_cb);

  hboxprrv = IupHbox(
    prinrv,
    list_dropdownprin,
    labelsignprrv,
    btnforcetopressurerv,
    proutrv,
    spinprrv,
    list_dropdownproutforce,
    list_dropdownproutarea,
    scfiprrv,
    NULL);
  IupSetAttribute(hboxprrv, "MARGIN", "10x10");
  IupSetAttribute(hboxprrv, "GAP", "10");
  IupSetAttribute(hboxprrv, "ALIGNMENT", "ACENTER");

  /************************AREA *************************/
  labelsignarea= IupLabel("=");
  IupSetAttribute(labelsignarea, "SIZE", "10");
  IupSetAttribute(labelsignarea, "ALIGNMENT", "ACENTER");

  list_dropdownareain = IupList(NULL);
  IupSetAttributes (list_dropdownareain, "1=\"mm²\", 2=\"cm²\", 3=\"m²\", 4=\"in²\", 5=\"ft²\","
                                   "DROPDOWN=YES, VISIBLEITEMS=6, VALUE = 1");
  IupSetAttribute(list_dropdownareain, "NAME", "AREAUNITIN");

  list_dropdownareaout = IupList(NULL);
  IupSetAttributes (list_dropdownareaout, "1=\"mm²\", 2=\"cm²\", 3=\"m²\", 4=\"in²\", 5=\"ft²\","
                                   "DROPDOWN=YES, VISIBLEITEMS=6, VALUE = 2");
  IupSetAttribute(list_dropdownareaout, "NAME", "AREAUNITOUT");
  
  btnarea = IupButton("Calc", NULL);
  IupSetAttribute(btnarea, "SIZE", "30");

  areain = IupText(NULL);
  IupSetAttribute(areain, "SIZE", "50");
  IupSetAttribute(areain, "NAME", "AREAIN");
  IupSetAttribute(areain, "MASK", IUP_MASK_FLOAT);

  areaout = IupText(NULL);
  IupSetAttribute(areaout, "SIZE", "100");
  IupSetAttribute(areaout, "NAME", "AREAOUT");
  IupSetAttribute(areaout, "MASK", IUP_MASK_FLOAT);
  IupSetAttribute(areaout, "ACTIVE", "NO");

  scfiarea = IupToggle("Scientific notation", NULL);
  IupSetAttribute(scfiarea, "NAME", "SCFIAREA");
  IupSetCallback(scfiarea, "VALUECHANGED_CB", (Icallback)calculate_area_cb);

  spinarea = IupText(NULL);
  IupSetAttributes(spinarea, "SPIN=Yes, SPINMIN=0, SPINMAX=16, RASTERSIZE=48x, NAME=SPINAREA, VALUE=3");
  IupSetAttribute(spinarea, "TIP", "Number of decimal places");
  IupSetCallback(spinarea, "VALUECHANGED_CB", (Icallback)calculate_area_cb);

  hboxarea = IupHbox(
    areain,
    list_dropdownareain,
    labelsignarea,
    btnarea,
    areaout,
    spinarea,
    list_dropdownareaout,
    scfiarea,
    NULL);
  IupSetAttribute(hboxarea, "MARGIN", "10x10");
  IupSetAttribute(hboxarea, "GAP", "10");
  IupSetAttribute(hboxarea, "ALIGNMENT", "ACENTER");


/****************DIALOG********************/

  descrtmpr = IupLabel("Temperature");
  IupSetAttribute(descrtmpr, "SIZE", "100");
  IupSetAttribute(descrtmpr, "ALIGNMENT", "ACENTER");
  IupSetAttribute(descrtmpr, "FONTSTYLE", "Bold, Italic");
  descrftopr = IupLabel("Force per unit to pressure");
  IupSetAttribute(descrftopr, "SIZE", "100");
  IupSetAttribute(descrftopr, "ALIGNMENT", "ACENTER");
  IupSetAttribute(descrftopr, "FONTSTYLE", "Bold, Italic");
  descrarea = IupLabel("Area");
  IupSetAttribute(descrarea, "SIZE", "100");
  IupSetAttribute(descrarea, "ALIGNMENT", "ACENTER");
  IupSetAttribute(descrarea, "FONTSTYLE", "Bold, Italic");

  hboxlabtmpr = IupHbox(
    IupFill(),
    descrtmpr,
    IupFill(),
    NULL);
  IupSetAttribute(hboxlabtmpr, "MARGIN", "0x10");
  IupSetAttribute(hboxlabtmpr, "ALIGNMENT", "ACENTER");
  
  hboxlabftopr = IupHbox(
    IupFill(),
    descrftopr,
    IupFill(),
    NULL);
  IupSetAttribute(hboxlabftopr, "MARGIN", "0x10");
  IupSetAttribute(hboxlabftopr, "ALIGNMENT", "ACENTER");

  hboxlabarea = IupHbox(
    IupFill(),
    descrarea,
    IupFill(),
    NULL);
  IupSetAttribute(hboxlabarea, "MARGIN", "0x10");
  IupSetAttribute(hboxlabarea, "ALIGNMENT", "ACENTER");

  separatorhor1 = IupLabel("");
  IupSetAttribute(separatorhor1, "SEPARATOR", "HORIZONTAL");
  separatorhor2 = IupLabel("");
  IupSetAttribute(separatorhor2, "SEPARATOR", "HORIZONTAL");

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

  vbox = IupVbox(
    hboxlabtmpr,
    hboxtmpr, 
    separatorhor1,
    hboxlabftopr,
    hboxpr,
    hboxprrv,
    separatorhor2,
    hboxlabarea,
    hboxarea,
    hboxsignature,
    NULL);

  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "Unit converter");
  // IupSetAttributeHandle(dlg, "DEFAULTENTER", btntmpr);
  //IupSetAttribute(dlg, "SIZE", "QUARTERxTHIRD");

 
  /* Registers callbacks */
  IupSetCallback(btntmpr, "ACTION", (Icallback)calculate_tmpr_cb);
  IupSetCallback(btnforcetopressure, "ACTION", (Icallback)calculate_pressure_cb);
  IupSetCallback(btnarea, "ACTION", (Icallback)calculate_area_cb);
  IupSetCallback(btnforcetopressurerv, "ACTION", (Icallback)calculate_pressurerv_cb);

  /* parent for pre-defined dialogs in closed functions (IupMessage) */
  IupSetAttributeHandle(NULL, "PARENTDIALOG", dlg);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  IupClose();
  return EXIT_SUCCESS;
}

/****************************************FUNCTIONS******************************************************************************************/


/***********************AREA**************************/
int calculate_area_cb(Ihandle *self)
{
  Ihandle* in = IupGetDialogChild(self, "AREAIN");
  Ihandle* out = IupGetDialogChild(self, "AREAOUT");
  Ihandle* notation = IupGetDialogChild(self, "SCFIAREA");
  int notationtype = IupGetInt(notation, "VALUE");

  Ihandle* accuracy = IupGetDialogChild(self, "SPINAREA");
  int accur = IupGetInt(accuracy, "VALUE");

  //double value = IupGetDouble(in, "VALUE");
  char * valuestr = IupGetAttribute(in, "VALUE");   // in order to stop calcs when input is blank(empty)
  
  /*early exit*/
  
  if (strcmp(valuestr,"") == 0)
  {
    return IUP_DEFAULT;
  }
  
  double value = atof(valuestr);
  double result = 0;
  
  char * unitin = IupGetAttribute(IupGetDialogChild(self, "AREAUNITIN"),"VALUE");
  char * unitout = IupGetAttribute(IupGetDialogChild(self, "AREAUNITOUT"),"VALUE");

/*check against the limits*/  
if (value <= 0)     
  {
    IupMessage("Error","Given value shall be greater than 0\n"); 
    IupSetStrf(in, "VALUE", "");
    IupSetStrf(out, "VALUE", "");
    return IUP_DEFAULT;
  }

  /*early exit*/
  
  if (strcmp(unitin,unitout) == 0)
  {
    IupSetStrf(out, "VALUE", "%.2lf", value);
    return IUP_DEFAULT;
  }

  /*legend*/
  /*1 - mmÂ², 2 - cmÂ², 3 - mÂ², 4 - inÂ², 5 - ftÂ² */ 
  
  if (strcmp(unitin,"1") == 0)
  {
    switch(atoi(unitout))
    {
      case 2: result = value * 1.00/(CMTOMM * CMTOMM); break;
      case 3: result = value * 1.00/(MTOMM * MTOMM); break; 
      case 4: result = value * 1.00/(INTOMM * INTOMM); break;
      case 5: result = value * 1.00/(FTTOMM * FTTOMM); break;
      default: IupMessage("Error","Unknown error"); return IUP_DEFAULT;
    }
  
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"2") == 0)
  {
    switch(atoi(unitout))
    {
      case 1: result = value * (CMTOMM * CMTOMM); break;
      case 3: result = value * 1.00/(MTOCM * MTOCM); break; 
      case 4: result = value * 1.00/(INTOCM * INTOCM); break;
      case 5: result = value * 1.00/(FTTOCM * FTTOCM); break;
      default: IupMessage("Error","Unknown error"); return IUP_DEFAULT;
    }

    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"3") == 0)
  {
    switch(atoi(unitout))
    {
      case 1: result = value * (MTOMM * MTOMM); break;
      case 2: result = value * (MTOCM * MTOCM); break; 
      case 4: result = value * 1.00/(INTOM * INTOM); break;
      case 5: result = value * 1.00/(FTTOM * FTTOM); break;
      default: IupMessage("Error","Unknown error"); return IUP_DEFAULT;
    }

    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"4") == 0)
  {
    switch(atoi(unitout))
    {
      case 1: result = value * (INTOMM * INTOMM); break;
      case 2: result = value * (INTOCM * INTOCM); break; 
      case 3: result = value * (INTOM * INTOM); break;
      case 5: result = value * (INTOFT * INTOFT); break;
      default: IupMessage("Error","Unknown error"); return IUP_DEFAULT;
    }
    
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"5") == 0)
  {
    switch(atoi(unitout))
    {
      case 1: result = value * (FTTOMM * FTTOMM); break;
      case 2: result = value * (FTTOCM * FTTOCM); break; 
      case 3: result = value * (FTTOM * FTTOM); break;
      case 4: result = value * 1.00/(INTOFT * INTOFT); break;
      default: IupMessage("Error","Unknown error"); return IUP_DEFAULT;
    }

    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }  
  else
  {
    IupMessage("Error","Unknown error");
    return IUP_DEFAULT;
  }

return IUP_DEFAULT;
}

/***********************PRESSURE**************************/
int calculate_pressure_cb(Ihandle *self)
{
  Ihandle* in = IupGetDialogChild(self, "PRIN");
  Ihandle* out = IupGetDialogChild(self, "PROUT");
  Ihandle* notation = IupGetDialogChild(self, "SCFIPR");
  int notationtype = IupGetInt(notation, "VALUE");

  Ihandle* accuracy = IupGetDialogChild(self, "SPINPR");
  int accur = IupGetInt(accuracy, "VALUE");

  //double value = IupGetDouble(in, "VALUE");
  char * valuestr = IupGetAttribute(in, "VALUE");   // in order to stop calcs when input is blank(empty)
  
  /*early exit*/
  
  if (strcmp(valuestr,"") == 0)
  {
    return IUP_DEFAULT;
  }
  
  double value = atof(valuestr);
  double result = 0;
  double resunit = 0;
  
  char * unitinforce = IupGetAttribute(IupGetDialogChild(self, "PRUNITIN"),"VALUE");
  char * unitinarea = IupGetAttribute(IupGetDialogChild(self, "PRAREAUNITIN"),"VALUE");
  char * unitout = IupGetAttribute(IupGetDialogChild(self, "PRUNITOUT"),"VALUE");

/*check against the limits*/  
if (value <= 0)     
  {
    IupMessage("Error","Given value shall be greater than 0\n"); 
    IupSetStrf(in, "VALUE", "");
    IupSetStrf(out, "VALUE", "");
    return IUP_DEFAULT;
  }

  /*legend*/
  /*unitin: 1 - N, 2 - kN, 3 - MN, 4 - GN, 5 - gram, 6 - kilogram, 7 - tonne */ 
  /*unitinarea: 1 - mmÂ², 2 - cmÂ², 3 - mÂ² */
  /*unitout: 1 - Pa, 2 - kPa, 3 - MPa, 4 - GPa */

  if (strcmp(unitinforce,"1") == 0)
  {
    switch(atoi(unitinarea))
    {
      case 1: resunit = value; break;
      case 2: resunit = value * 1.00/(CMTOMM * CMTOMM); break; 
      case 3: resunit = value * 1.00/(MTOMM * MTOMM); break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    switch(atoi(unitout))
    {
      case 1: result = resunit * 1000000.00; break;
      case 2: result = resunit * 1000.00; break; 
      case 3: result = resunit * 1.00; break;
      case 4: result = resunit * 0.001; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }

    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitinforce,"2") == 0)
  {
    switch(atoi(unitinarea))
    {
      case 1: resunit = value; break;
      case 2: resunit = value * 1.00/(CMTOMM * CMTOMM); break; 
      case 3: resunit = value * 1.00/(MTOMM * MTOMM); break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    switch(atoi(unitout))
    {
      case 1: result = resunit * 1000000.00 * 1000.00; break;
      case 2: result = resunit * 1000.00 * 1000.00; break; 
      case 3: result = resunit * 1.00 * 1000.00; break;
      case 4: result = resunit * 0.001 * 1000.00; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitinforce,"3") == 0)
  {
    switch(atoi(unitinarea))
    {
      case 1: resunit = value; break;
      case 2: resunit = value * 1.00/(CMTOMM * CMTOMM); break; 
      case 3: resunit = value * 1.00/(MTOMM * MTOMM); break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    switch(atoi(unitout))
    {
      case 1: result = resunit * 1000000.00 * 1000000.00; break;
      case 2: result = resunit * 1000.00 * 1000000.00; break; 
      case 3: result = resunit * 1.00 * 1000000.00; break;
      case 4: result = resunit * 0.001 * 1000000.00; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitinforce,"4") == 0)
  {
    switch(atoi(unitinarea))
    {
      case 1: resunit = value; break;
      case 2: resunit = value * 1.00/(CMTOMM * CMTOMM); break; 
      case 3: resunit = value * 1.00/(MTOMM * MTOMM); break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    switch(atoi(unitout))
    {
      case 1: result = resunit * 1000000.00 * 1000000000.00; break;
      case 2: result = resunit * 1000.00 * 1000000000.00; break; 
      case 3: result = resunit * 1.00 * 1000000000.00; break;
      case 4: result = resunit * 0.001 * 1000000000.00; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitinforce,"5") == 0)
  {
    switch(atoi(unitinarea))
    {
      case 1: resunit = value; break;
      case 2: resunit = value * 1.00/(CMTOMM * CMTOMM); break; 
      case 3: resunit = value * 1.00/(MTOMM * MTOMM); break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    switch(atoi(unitout))
    {
      case 1: result = resunit * GRAVITY * GTOKG * 1000000.00 ; break;
      case 2: result = resunit * GRAVITY * GTOKG * 1000.00 ; break; 
      case 3: result = resunit * GRAVITY * GTOKG * 1.00 ; break;
      case 4: result = resunit * GRAVITY * GTOKG * 0.001 ; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitinforce,"6") == 0)
  {
    switch(atoi(unitinarea))
    {
      case 1: resunit = value; break;
      case 2: resunit = value * 1.00/(CMTOMM * CMTOMM); break; 
      case 3: resunit = value * 1.00/(MTOMM * MTOMM); break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    switch(atoi(unitout))
    {
      case 1: result = resunit * GRAVITY * 1000000.00 ; break;
      case 2: result = resunit * GRAVITY * 1000.00 ; break; 
      case 3: result = resunit * GRAVITY * 1.00 ; break;
      case 4: result = resunit * GRAVITY * 0.001 ; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitinforce,"7") == 0)
  {
    switch(atoi(unitinarea))
    {
      case 1: resunit = value; break;
      case 2: resunit = value * 1.00/(CMTOMM * CMTOMM); break; 
      case 3: resunit = value * 1.00/(MTOMM * MTOMM); break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    switch(atoi(unitout))
    {
      case 1: result = resunit * GRAVITY * TONNETOKG * 1000000.00 ; break;
      case 2: result = resunit * GRAVITY * TONNETOKG * 1000.00 ; break; 
      case 3: result = resunit * GRAVITY * TONNETOKG * 1.00 ; break;
      case 4: result = resunit * GRAVITY * TONNETOKG * 0.001 ; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else
  {
    IupMessage("Error","Unknown error");
    return IUP_DEFAULT;
  }

return IUP_DEFAULT;
}

/***********************PRESSURE REVERSED**************************/
int calculate_pressurerv_cb(Ihandle *self)
{
  Ihandle* in = IupGetDialogChild(self, "PRINRV");
  Ihandle* out = IupGetDialogChild(self, "PROUTRV");
  Ihandle* notation = IupGetDialogChild(self, "SCFIPRRV");
  int notationtype = IupGetInt(notation, "VALUE");

  Ihandle* accuracy = IupGetDialogChild(self, "SPINPRRV");
  int accur = IupGetInt(accuracy, "VALUE");

  //double value = IupGetDouble(in, "VALUE");
  char * valuestr = IupGetAttribute(in, "VALUE");   // in order to stop calcs when input is blank(empty)
  
  /*early exit*/
  
  if (strcmp(valuestr,"") == 0)
  {
    return IUP_DEFAULT;
  }
  
  double value = atof(valuestr);
  double result = 0;
  double resunit = 0;
  
  char * unitin = IupGetAttribute(IupGetDialogChild(self, "PRUNITINRV"),"VALUE");
  char * unitoutforce = IupGetAttribute(IupGetDialogChild(self, "PRUNITOUTFORCE"),"VALUE");
  char * unitoutarea = IupGetAttribute(IupGetDialogChild(self, "PRUNITOUTAREA"),"VALUE");

/*check against the limits*/  
if (value <= 0)     
  {
    IupMessage("Error","Given value shall be greater than 0\n"); 
    IupSetStrf(in, "VALUE", "");
    IupSetStrf(out, "VALUE", "");
    return IUP_DEFAULT;
  }

  /*legend*/
  /*unitin: 1 - Pa, 2 - kPa, 3 - MPa, 4 - GPa */
  /*unitoutforce: 1 - N, 2 - kN, 3 - MN, 4 - GN, 5 - gram, 6 - kilogram, 7 - tonne */ 
  /*unitoutarea: 1 - mmÂ², 2 - cmÂ², 3 - mÂ² */

  if (strcmp(unitin,"1") == 0)
  {
    switch(atoi(unitoutarea))
    {
      case 1: resunit = value * 1.00/(MTOMM * MTOMM) ; break;
      case 2: resunit = value * 1.00/(MTOCM * MTOCM); break; 
      case 3: resunit = value; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    switch(atoi(unitoutforce))
    {
      case 1: result = resunit; break;
      case 2: result = resunit / 1000.00; break; 
      case 3: result = resunit / 1000000.00; break;
      case 4: result = resunit / 1000000000.00; break;
      case 5: result = resunit / (GRAVITY * GTOKG); break;
      case 6: result = resunit / GRAVITY; break;
      case 7: result = resunit / (GRAVITY * TONNETOKG); break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"2") == 0)
  {
    switch(atoi(unitoutarea))
    {
      case 1: resunit = value * 1.00/(MTOMM * MTOMM) ; break;
      case 2: resunit = value * 1.00/(MTOCM * MTOCM); break; 
      case 3: resunit = value; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    switch(atoi(unitoutforce))
    {
      case 1: result = resunit * 1000.00; break;
      case 2: result = resunit * 1.00; break; 
      case 3: result = resunit / 1000.00; break;
      case 4: result = resunit / 1000000.00; break;
      case 5: result = resunit / (GRAVITY * GTOKG) * 1000.00; break;
      case 6: result = resunit / GRAVITY * 1000.00; break;
      case 7: result = resunit / (GRAVITY * TONNETOKG) * 1000.00; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"3") == 0)
  {
    switch(atoi(unitoutarea))
    {
      case 1: resunit = value * 1.00/(MTOMM * MTOMM) ; break;
      case 2: resunit = value * 1.00/(MTOCM * MTOCM); break; 
      case 3: resunit = value; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    switch(atoi(unitoutforce))
    {
      case 1: result = resunit * 1000000.00; break;
      case 2: result = resunit * 1000.00; break; 
      case 3: result = resunit * 1.00; break;
      case 4: result = resunit / 1000.00; break;
      case 5: result = resunit / (GRAVITY * GTOKG) * 1000000.00; break;
      case 6: result = resunit / GRAVITY * 1000000.00; break;
      case 7: result = resunit / (GRAVITY * TONNETOKG) * 1000000.00; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"4") == 0)
  {
    switch(atoi(unitoutarea))
    {
      case 1: resunit = value * 1.00/(MTOMM * MTOMM) ; break;
      case 2: resunit = value * 1.00/(MTOCM * MTOCM); break; 
      case 3: resunit = value; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    switch(atoi(unitoutforce))
    {
      case 1: result = resunit * 1000000000.00; break;
      case 2: result = resunit * 1000000.00; break; 
      case 3: result = resunit * 1000.00; break;
      case 4: result = resunit * 1.00; break;
      case 5: result = resunit / (GRAVITY * GTOKG) * 1000000000.00; break;
      case 6: result = resunit / GRAVITY * 1000000000.00; break;
      case 7: result = resunit / (GRAVITY * TONNETOKG) * 1000000000.00; break;
      default: IupMessage("Error","Unknown error in pressure calcs"); return IUP_DEFAULT;
    }
    
    show_results(out, notationtype, result, accur);
    return IUP_DEFAULT;
  }
  else
  {
    IupMessage("Error","Unknown error");
    return IUP_DEFAULT;
  }

return IUP_DEFAULT;
}


/***********************TEMPERATURE*************************/
int calculate_tmpr_cb(Ihandle *self)
{
  Ihandle* in = IupGetDialogChild(self, "TMPRIN");
  Ihandle* out = IupGetDialogChild(self, "TMPROUT");

  //double value = IupGetDouble(in, "VALUE");
  char * valuestr = IupGetAttribute(in, "VALUE");   // in order to stop calcs when input is blank(empty)
  
  /*early exit*/
  
  if (strcmp(valuestr,"") == 0)
  {
    return IUP_DEFAULT;
  }
  
  double value = atof(valuestr);
  double result = 0;
  
  char * unitin = IupGetAttribute(IupGetDialogChild(self, "TMPRUNITIN"),"VALUE");
  char * unitout = IupGetAttribute(IupGetDialogChild(self, "TMPRUNITOUT"),"VALUE");
  
if (tmpr_value_limits(value, unitin))
  {
    IupMessage("Error","Given value is outside the limits.\n"
                       "Limits:\n"
                       "Kelvin            0 to 1000\n"
                       "Celcius          -273.15 to 726.85\n"
                       "Fahrenheit    -459.67 to 1340.33");
    // IupMessagef("Error","Given value is outside the limits.\n"
    //                    "Limits:\n"
    //                    "%s            %s\n"
    //                    "%s          %s\n"
    //                    "%s    %s",
    //                    "Kelvin","0 to 1000","Celcius","-273.15 to 726.85","Fahrenheit","-459.67 to 1340.33");
    IupSetStrf(in, "VALUE", "0");
    IupSetStrf(out, "VALUE", "");
    return IUP_DEFAULT;
  }

  /*early exit*/
  
  if (strcmp(unitin,unitout) == 0)
  {
    IupSetStrf(out, "VALUE", "%.2lf", value);
    return IUP_DEFAULT;
  }

  /*legend*/
  /*1 - Celcius, 2 - Fahrenheit, 3 - Kelvin*/

  if (strcmp(unitin,"1") == 0 && strcmp(unitout,"2") == 0)  
  {
    result = celciustofahrenheit(value);

    IupSetStrf(out, "VALUE", "%.2lf", result);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"2") == 0 && strcmp(unitout,"1") == 0)  
  {
    result = fahrenheittocelcius(value);

    IupSetStrf(out, "VALUE", "%.2lf", result);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"1") == 0 && strcmp(unitout,"3") == 0)  
  {
    result = celciustokelvin(value);

    IupSetStrf(out, "VALUE", "%.2lf", result);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"3") == 0 && strcmp(unitout,"1") == 0)  
  {
    result = kelvintocelcius(value);

    IupSetStrf(out, "VALUE", "%.2lf", result);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"2") == 0 && strcmp(unitout,"3") == 0)  
  {
    result = fahrenheittokelvin(value);

    IupSetStrf(out, "VALUE", "%.2lf", result);
    return IUP_DEFAULT;
  }
  else if (strcmp(unitin,"3") == 0 && strcmp(unitout,"2") == 0)  
  { 
    result = kelvintofahrenheit(value);

    IupSetStrf(out, "VALUE", "%.2lf", result);
    return IUP_DEFAULT;
  }
  else
  {
    IupMessage("Error","Unknown error");
    return IUP_DEFAULT;
  }

  return IUP_DEFAULT;
}

bool tmpr_value_limits(double value, char * unitin)
{
   /*legend*/
  /*1 - Celcius, 2 - Fahrenheit, 3 - Kelvin*/

  if (strcmp(unitin,"1") == 0)  
  {
    if (value < CLIMBOT || value > CLIMTOP)
      return true;
    else
      return false;
  }
  else if (strcmp(unitin,"2") == 0)  
  {
    if (value < FLIMBOT || value > FLIMTOP)
      return true;
    else
      return false;
  }
  else if (strcmp(unitin,"3") == 0)  
  {
    if (value < KLIMBOT || value > KLIMTOP)
      return true;
    else
      return false;
  }
  else
  {
    IupMessage("Error","Unknown error in value_limits()");
    return IUP_DEFAULT;
  }
}

double celciustofahrenheit(double value)
{
  double result = value * (9./5.) + 32;
  return result;
}

double fahrenheittocelcius(double value)
{
  double result = (value - 32) * (5./9.);
  return result;
}

double celciustokelvin(double value)
{
  double result = value + 273.15;
  return result;
}

double kelvintocelcius(double value)
{
  double result = value - 273.15;
  return result;
}


double fahrenheittokelvin(double value)
{
  double result = (value + 459.67) * (5./9.);
  return result;
}

double kelvintofahrenheit(double value)
{
  double result = (9./5.)*value - 459.67;
  return result;
}

void show_results(Ihandle *out, int notationtype, double result, unsigned int accuracy)
{
  double n =  result;
  unsigned int countbefore = 0;

  while (n != 0)
  {
    n = floor(n / 10);
    countbefore++;
  }

  // parameters for showing number in groups by 3
  unsigned int modulobefore = countbefore % 3;
  unsigned int divbefore = countbefore / 3; 
  unsigned int resaccuracy = accuracy;  // result accuracy - taken from user - remember to change it

  char temp[DBLFLPTFORMATSIZE];   // temporary string
  sprintf(temp,"%.*lf", resaccuracy ,result);
  temp[DBLFLPTFORMATSIZE] = '\0';

  char resultstr[DBLFLPTFORMATSIZE];    // result string

  unsigned int i = 0, j = 0;           // string counters
  unsigned int counter = 0;   // digit counter
  bool fwflag = false;          // first whitespace flag - inform whether the first whitespace was insterted (case when modulobefore returns 1)
  bool separ = false;         // inform whether the dot was already met
 
  while (temp[i] != '\0')
  {
      if (temp[i] != '.' && separ == false)
      {
          if (countbefore <= 3)       //case when there are less than 3 digits before dot - no need for adding whitespace
              resultstr[j++] = temp[i];
          else if (countbefore > 3)   //case when there are more than 3 digits before dot - need for adding whitespace
          {
              if (modulobefore)       //number of digits is not a multiple of 3
              {
                  resultstr[j++] = temp[i];
                  counter++;
                  if (counter == modulobefore && fwflag == false)    //adding first whitespace
                  {
                    resultstr[j++] = ' ';
                    fwflag = true; 
                    counter = 0; //counter reset
                  }
                  else if (fwflag == true)        //adding another whitespaces in segments of three
                  {
                      if (counter % 3 == 0 && divbefore > 1)  //divbefore > 1 means that no extra spacing is added straight before the dot
                      {
                          resultstr[j++] = ' ';
                          divbefore--;
                      }
                  }
              }
              else        //number of digits is a multiple of 3
              {
                  resultstr[j++] = temp[i];
                  counter++;
                  if (counter % 3 == 0 && divbefore > 1)    //divbefore > 1 means that no extra spacing is added straight before the dot
                  {
                      resultstr[j++] = ' ';
                      divbefore--;
                  }
              }

          }
      }
      else if (temp[i] == '.')      //decimal separator (dot) is met - counter reset
      {
          resultstr[j++] = temp[i];  
          counter = 0; // counter reset
          separ = true;
      }
      else                        // adding whitespaces after the dot 
      {
          if (counter <= resaccuracy)
          {
              resultstr[j++] = temp[i];
              counter++;
              if (counter % 3 == 0)
              {
                  resultstr[j++] = ' ';
              }
          }
      }

      i++;
  }
  
  resultstr[j] = '\0';
  //printf("resultstr = %s\n", resultstr);
  // for(int i = 0; i < strlen(resultstr); i++)
  //   printf("%u, %c\n", i, resultstr[i]);
  
  // j = strlen(resultstr) - 1; //reset the counter
  // counter = 0;
  
  // while ((resultstr[j] == '0' || resultstr[j] == ' ' || resultstr[j] == '.' )  && counter < strlen(resultstr))
  // {
  //     counter++;
  //     j--;
  //     if (resultstr[j+1] == '.')
  //       break;
      
  // }
  
  // unsigned int decpl = strlen(resultstr) - countbefore - 1;
  // printf("counter = %u, decimal places = %u\n", counter,decpl);
  // resultstr[j+1] = '\0';
  // printf("resultstr = %s\n", resultstr);

  if (notationtype)
    IupSetStrf(out, "VALUE", "%.4e", result);
  else
    IupSetStrf(out, "VALUE", "%s", resultstr);
  
}

  