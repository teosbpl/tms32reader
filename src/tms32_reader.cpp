//---------------------------------------------------------------------------
#include <TTms32Reader.h>
#include "unilib.h"
#include <fstream>
#include <string.h>

#pragma hdrstop
//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char* argv[])
{
  char buffer[2048];
  std::string     text_file;
  std::string     output_file;
  char cif[] = "if";
  char cof[] = "of";
  char call[] = "all";
  char cchannel[] = "channel";
  char cheaders[] = "headers";
  fprintf(stdout,"tms32_reader Copyright by Teodor Buchner, Warsaw University of Technology @ 2014\n");

//  if( parse(argc,argv,"ar", NULL) )    use_autoregression = true;
  if( parse(argc,argv,cif, buffer) )
      if(strlen(buffer))         text_file = buffer;
      else
      {        fprintf(stderr,"Input file not defined\n");        return 0;      }
  buffer[0] = 0;
  if( parse(argc,argv,cof, buffer) )//
      if(strlen(buffer))         output_file = buffer;
  buffer[0] = 0;
  TTms32Reader reader(text_file);
  reader.LoadHeaders();
  if( parse(argc,argv,cheaders, NULL) )
  {
    reader.DumpHeaders("DumpTms32Headers.txt");
  }
  reader.LoadSignals();
  if( parse(argc,argv,call, NULL) )
  {
    reader.DumpSignals();
  }
  if( parse(argc,argv,cchannel, buffer) )
  {//-channel 0:0:1000
  if(strlen(buffer))
  {               //-win 100:200
    char separator = ':';
    double values[3];   //-channel 0:4950000:5046000     1sec=1600 60 sec = 60*1600 = 96000
    if(3==scan_line_sep(buffer,values,3,separator))
    {
      unsigned int channel = (unsigned int) values[0];
      unsigned int smp_from = (unsigned int) values[1];
      unsigned int smp_to = (unsigned int) values[2];
//  filename = text_resp_file+"rr_by_resp_file.txt";
      if(output_file.empty()) output_file = "dump.txt";
      reader.DumpChannelFromTo(channel,smp_from,smp_to,output_file.c_str());
    }
  }
  }

  return 0;
}

