//---------------------------------------------------------------------------

#ifndef TDateConverterH
#define TDateConverterH
//---------------------------------------------------------------------------
#include <string>
#include <vector>
//#include <vcl.h>
typedef double TDateTime;
class TDateConverter
{
  public:
    enum TDateMode{dmPolish,dmMysql};
    TDateConverter(std::string& _str,TDateMode _tmode = dmMysql):
    str(_str),tMode(_tmode){ };
    TDateTime operator()(void);
    static int ParseDate(std::vector <std::string>& strings,std::string date);//returns number of parts    
  private:
    TDateMode tMode;
    typedef std::vector <std::string> TStdStrings;
    TStdStrings strings;
    std::string& str;
    std::string MySQLString(TDateTime tdt);
};
#endif
 
