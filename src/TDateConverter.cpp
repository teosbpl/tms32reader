//---------------------------------------------------------------------------

//#include "precompiled.h"
//#pragma hdrstop

#include "TDateConverter.h"
#include <stdlib.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

int TDateConverter::ParseDate(std::vector <std::string>& strings,std::string date)//returns number of parts
{
  std::string::size_type pos = std::string::npos;
  std::string::size_type lastpos = 0;
  std::string specials = " -:.";
  if (date.empty()) return 0;
  strings.clear();
  do
  {
    pos = date.find_first_of(specials,lastpos);
    if(pos == std::string::npos)
      strings.push_back(std::string(date,lastpos,date.length()));
    else
      strings.push_back(std::string(date,lastpos,pos-lastpos));
    lastpos = pos+1;
  }while(pos != std::string::npos);
  return strings.size();
};

TDateTime TDateConverter::operator()(void)
{
     TDateTime tdt;
     if(ParseDate(strings,str) >= 3)
     {
       int d_order = (tMode == dmPolish) ? 0 :  2;
       int y_order = (tMode == dmPolish) ? 2 :  0;
       unsigned short  y = atoi(strings[y_order].c_str());
       unsigned short  m = atoi(strings[1].c_str());
       unsigned short  d = atoi(strings[d_order].c_str());
       tdt = TDateTime(y,m,d);
     }
     strings.clear();
     return tdt;
   }

std::string TDateConverter::MySQLString(TDateTime tdt)
{
  AnsiString date = tdt.FormatString("yyyy-mm-dd");
  return date.c_str();
}
