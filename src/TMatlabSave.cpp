#include "TMatlabSave.h"
#include <matio.h>
using namespace std;
int TMatlabSave::DoSave(TTms32Reader::TTms32Signals& signals,const char* matFileName)
{
  mat_t * matfp;
  vector<matvar_t*> variables;
  matfp = Mat_CreateVer(matFileName,NULL,MAT_FT_DEFAULT);
  if( NULL == matfp )
  {
     fprintf(stderr,"Error creating MAT file %s",matFileName);
     return EXIT_FAILURE;
  }

  for(TTms32Reader::TTms32Signals::iterator item = signals.begin(); item != signals.end();++item)
  {
    TTms32Reader::TTms32Signal* signal = (*item);
    size_t ssize = signal->NumberOfSamples();
    float* storage = signal->GetStorage();
    string name = "ch"; //TODO: take from signal header.
    char str[16];
    size_t dims[2] = {ssize,1};
    sprintf(str,  "%ld", distance(signals.begin(),item) );
    name += str;
    matvar_t* matvar = Mat_VarCreate(name.c_str(),MAT_C_SINGLE,MAT_T_SINGLE,2,dims,storage,0);
    if (NULL == matvar)
    {
      fprintf(stderr,"Error creating variable for %s of size %ld\n",name.c_str(),ssize);
    }
    else
    {
       Mat_VarWrite(matfp,matvar,MAT_COMPRESSION_ZLIB);
    }
    variables.push_back(matvar);
  }
  Mat_Close(matfp);
  return EXIT_SUCCESS;
}
