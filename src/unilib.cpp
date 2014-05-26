#include <stdio.h>
#include "unilib.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <algorithm>

#ifndef DBL_MAX
  #define DBL_MAX 	1.79769313486231500e+308
#endif

//#include <sys\types.h>
//#include <sys\stat.h>
//#include <fcntl.h>
//#include <io.h>

typedef int ( *COMP_FN)(const void* ,const void* );
int comp_double_ascending(const void* ptr1,const void* ptr2);
int comp_double_descending(const void* ptr1,const void* ptr2);
static int buf_argc = 0;
static char* buf_argv[100];
static char* ptr = NULL;

char * strupr(char *s)
{
char *tmp = s;
while (*s)
*s = toupper(*s++);
return tmp;
}
/*
-outfileziutek.txt
char* key="Aa";
char optional_arg[20];
char result;
result = parse(argc,argv,key, optional_arg);
*/
void  strxch(char* str1, char *str2)
{
    char *aux = strdup(str1);
    strcpy(str1,str2);
    strcpy(str2,aux);
    free(aux);
}
void  add_to_name(char* filename, char *add)
  {
      char* ptr = strrchr(filename,'.');
      char ext[10];
      strcpy(ext,".txt");
      if (ptr)
        strncpy(ext,ptr,10);
      else
        ptr = filename+strlen(filename);
      if(*add != '_')
        *ptr++='_';
      strcpy(ptr,add);
      strcat(ptr,ext);
      return;
   }

int error(char* msg)
{
    if(msg)
    	fputs(msg,stderr);
    return 1;
}

long int filesize( FILE *fp )
  {
    long int save_pos, size_of_file;

    save_pos = ftell( fp );
    fseek( fp, 0L, SEEK_END );
    size_of_file = ftell( fp );
    fseek( fp, save_pos, SEEK_SET );
    return( size_of_file );
  }

uint get_file_size(const char* name){
  FILE* fp = fopen(name,"rb");
  long int fs = filesize( fp );
  fclose( fp );
  return fs;
  }

char* value2filename(char* filename,char* c_cvalue)
{
    char* ptr;
    strcpy(filename,c_cvalue);
    ptr = strchr(filename,'.');
    if(ptr)
      *ptr='_';
    strcat(filename,".txt");
    return filename;
}
/*
if(parse(argc,argv,"arg",buffer))
if( strlen(buffer))
  load_arg_file(buffer);
*/

char** load_arg_file(char* name,char* argv[])
{
    FILE* fp;
    uint size;
    int i;
    if(buf_argv) free_arg_file_buffer(); //if used for second time
    buf_argc = 0;
    size = get_file_size( name );
    if (!size) return NULL;

    ptr= (char*) calloc( size+1,sizeof(char));
    if(!buf_argv) return NULL;
    fp = fopen( name, "rt" );
    buf_argc = 2*count_lines( fp );
    if(!fp) return free_arg_file_buffer();
    size = fread( ptr, sizeof(char), size, fp);
    if(!size) return free_arg_file_buffer();
    i=buf_argc;
    while(size--)
      if(ptr[size]=='\n' || ptr[size]=='\t' || ptr[size]==' ')
      {
	buf_argv[--i]=ptr+size+1;
        ptr[size]=0;
      }
    buf_argv[--i]=ptr;
    buf_argv[0]=argv[0];
    fclose( fp );
    for(i=1;i<buf_argc;i++)
	if(buf_argv[i]) break;
    buf_argc = buf_argc - i;
    buf_argc = buf_argc + 2;
    memmove(buf_argv+1,buf_argv+i,buf_argc);
    return (char**)buf_argv;
}

char** free_arg_file_buffer(void)
{
    if(ptr)
      free (ptr);
    ptr=NULL;
//    if(buf_argv)
  //    free (buf_argv);
    //buf_argv=NULL;
    return NULL;
}
char parse(int argc,char* argv[],char* key, char* optional_arg)
{
    char result = 0;
//    char* ptr;
    int i;
    int keylen = strlen(key);
    if(ptr)//automatically switch to file-loaded buffer if present
    {
	argv = buf_argv;
	argc = buf_argc;
    }
    for(i=0;i<argc;i++) // last is file name
    {
	if(*argv[i]!='-') continue;
	if( strncmp( argv[i]+1,key,keylen ) ) continue;
	result = 1;
//        if( optional_arg && i+1<argc)
//	  if(*argv[i+1]!='-')//if no - : it may be the optional argument for the key
//	    strcpy(optional_arg,argv[i+1]);
//	  else result = 0;// arg required but nonexistent
        if( optional_arg)
	  if(i+1<argc)//if no - : it may be the optional argument for the key
	  {
	    strcpy(optional_arg,argv[i+1]);
	    //process optional_arg a little bit : xchange white signs
	    while(strstr( optional_arg,"\\t" ))
	    {
	        ptr = strstr( optional_arg,"\\t" );
		*ptr = '\t';
		ptr++;		//shift left by one char ( "\t" is two-character )
                memmove( ptr,ptr+1, strlen(ptr+1)+1);

	    }
	    while(strstr( optional_arg,"\\n" ))
	    {
	        ptr = strstr( optional_arg,"\\n" );
		*ptr = '\n';
		ptr++;		//shift left by one char ( "\t" is two-character )
                memmove( ptr,ptr+1, strlen(ptr+1)+1);
	    }

	  }
	  else result = 0;// arg required but nonexistent

	break;
    }
    return result;
}
int cannot_open(char* name,char *mode)
{
    return fprintf(stderr,"Cannot open file %s in %s mode\n",name,mode);
}

char check_if_exists(char* name)
{
      FILE* out = fopen(name,"r");
      char exists = 0;
      if(out)
      {
	exists = 1;
	fclose(out);
      }
      return exists;
}

void chg_extension(char* name,char* ext){
	char *ptr;
	ptr=strrchr(name,'.');
	if(ptr)
	  strcpy(++ptr,ext);
	else
	{
	  strcat(name,".");
	  strcat(name,ext);
	}
	return;
}

unsigned char* load_uchar_binary_signal(char *filename,uint* _max_index)
{
    FILE* in;
    unsigned char tmp;
    unsigned char* signal;
    uint signal_index;
    uint max_index;

    in = fopen(filename,"rb");
    if(!in) return NULL;
    max_index = filesize(in);
    if(max_index==0){ fclose(in); return NULL;}
    signal = (unsigned char*)calloc(max_index,sizeof(unsigned char));
    fread(signal,sizeof(unsigned char),max_index,in);
//   if(feof(in) ) break;
/*
    for(signal_index=0;signal_index<max_index;signal_index++)
    {
      tmp = (unsigned char)signal[signal_index];
      signal[signal_index]=tmp - 0x80;
    }
*/
    *_max_index = max_index;
    fclose(in);
    return signal;
}

unsigned short* load_ushort_binary_signal(char *filename,uint* _max_index)
{
    FILE* in;
    unsigned short* signal;
    uint signal_index;
    uint max_index;

    in = fopen(filename,"rb");
    if(!in) return NULL;
    max_index = filesize(in)/2;
    if(max_index==0){ fclose(in); return NULL;}
    signal = (unsigned short*)calloc(max_index,sizeof(unsigned short));
    fread(signal,sizeof(unsigned short),max_index,in);
//   if(feof(in) ) break;
    *_max_index = max_index;
    fclose(in);
    return signal;
}

char* load_signal(char *filename,uint* _max_index)
{
    FILE* in;
    char tmp;
    char* signal;
    uint signal_index;
    uint max_index;
    in = fopen(filename,"rt");
    if(!in) return NULL;
    max_index = count_lines(in);
    signal = (char*)calloc(max_index,sizeof(char));
    for(signal_index=0;signal_index<max_index;signal_index++)
    {
      fscanf(in,"%c\n",&tmp);
      signal[signal_index]=tmp;
      if(feof(in) ) break;
    }
    *_max_index = max_index;
    fclose(in);
    return signal;
}

int* load_int_signal(char *filename,uint* _max_index)
{
    FILE* in;
    int tmp;
    int* signal;
    uint signal_index;
    uint max_index;
    in = fopen(filename,"rt");
    if(!in) return NULL;
    max_index = count_lines( in );
    signal = (int*)calloc(max_index,sizeof(int));
    for(signal_index=0;signal_index<max_index;signal_index++)
    {
      fscanf(in,"%d\n",&tmp);
      signal[signal_index]=tmp;
      if(feof(in) ) break;
    }
    fclose(in);
    *_max_index = max_index;
    return signal;
}

uint count_lines(FILE* in)
{
    uint max_index = 0;
    char buffer[500];
    while( fgets(buffer, 500, in) )
      max_index++;
    rewind(in);
    return max_index;
}

double* load_double_signal(char *filename,uint* _max_index)
{
    FILE* in;
    double tmp;
    double* signal;
    uint signal_index;
    uint max_index;
    in = fopen(filename,"rt");
    if(!in) return NULL;
    max_index = count_lines( in );
    signal = (double*)calloc(max_index,sizeof(double));
    for(signal_index=0;signal_index<max_index;signal_index++)
    {
      fscanf(in,"%lf\n",&tmp);
      signal[signal_index]=tmp;
      if(feof(in) ) break;
    }
    fclose(in);
    *_max_index = max_index;
    return signal;
}

double* load_double_col_signal(char *filename,uint* _max_index,int col_nr)
{
    FILE* in;
    double value;
    double* signal;
    char buffer[2048];
    char* tmp;
    uint signal_index;
    int i;
    uint max_index;
    uint max_col;
    int result = 0;
    in = fopen(filename,"rt");
    if(!in) return NULL;
    max_index = count_lines( in );
    max_col = count_columns(in);
if(col_nr==0 || col_nr>max_col)
{
    fclose(in);
    return load_double_signal(filename,_max_index);
}
    signal = (double*)calloc(max_index,sizeof(double));
    for(signal_index=0;signal_index<max_index;signal_index++)
    {
      fgets(buffer,2048,in);
      i=col_nr-1;
      tmp = buffer;
      while(i-- > 0)
          tmp = strchr(++tmp,'\t');
     result = sscanf(tmp,"%lf",&value);
	if(!result) continue;
      signal[signal_index]=value;
      if(feof(in) ) break;
    }
    *_max_index = max_index;
    return signal;
}

char is_extremum(double* signal)
{
    double d1,d2;
//    double x1,x2,x3;
  //  x1 = *(signal-1);
  //  x2 = *signal;
  //  x3 = *(signal+1);
    d1 = *(signal) - *(signal-1);
    d2 = *(signal+1) - *signal;
    if(d1>0.0)
      if(d2<0.0) return 1;
      else return 0;
    else
      if(d2>0.0) return 1;
      else return 0;

    }

void get_stats(double* signal,uint max_index,STATS* stats)
{
    uint ii;
    double min = DBL_MAX, max = -DBL_MAX;
    double value,cumulant = 0.0,av;
    double lastvalue = DBL_MAX;
    double mindiff = DBL_MAX;
    double diff;

    for(ii=0;ii<max_index;ii++)
    {
	value = signal[ii];
	if(value>max) max = value;
	if(value<min) min = value;
	cumulant+=value;
        diff = fabs(lastvalue - value);
	if (diff < mindiff) mindiff = diff;
	lastvalue = value;
    }
    av = cumulant/(double)max_index;
    cumulant = 0.0;
    for(ii=0;ii<max_index;ii++)
    {
	value = signal[ii];
	value = (value-av) * (value-av);
	cumulant += value;
    }
    stats->min = min;
    stats->max = max;
    stats->av = av;
    stats->resolution = mindiff;
    stats->sd = sqrt(cumulant / (double)max_index);
    return;
}

uint* do_histogram(double *signal,uint max_index,uint num_of_bins,double min, double max,double *binsize)
{
    double daddress;
    uint* hist;
    uint ii;
    *binsize = ( max - min ) / (num_of_bins);
    hist = (uint*) calloc(num_of_bins + 1, sizeof( uint ) );
    //( x - base ) / binsize
    if( ! hist ) return NULL;

    for(ii=0;ii<max_index;ii++)
    {
      daddress = ( signal[ii] - min ) / *binsize;
      hist[(int)daddress]++;
    }

    return hist;
}

void do_hist_stats(FILE* in, FILE* out)
{
    char buffer[200];
    double value = 0;
    double probability;
    double cumulant = 0.0;
    double cumulant_probability = 0.0;
    double av;
    double sd;
    double min = DBL_MAX;
    double max = DBL_MAX;
    uint counter = 0;
    int result;

//av
//    fgets(buffer,200,in); //omit 1st line
    while (fgets(buffer,200,in))
    {
	result = sscanf(buffer,"%lf\t%lf\n",&value,&probability);
	if(result!=2) break;
	if(min==DBL_MAX && probability>0.0)
	   min = value;
	if(probability>0.0)
	   max = value;
	cumulant += value*probability;
	cumulant_probability += probability; //should be 1 if normalised
	counter++;
    }
    av = cumulant / cumulant_probability;
    cumulant = 0;
    rewind(in);
//    fgets(buffer,200,in); //omit 1st line
    while (fgets(buffer,200,in))
    {
	result = sscanf(buffer,"%lf\t%lf\n",&value,&probability);
	if(result!=2) break;
	value = (value-av) * (value-av);
	cumulant += value * probability;
    }
    sd = cumulant / cumulant_probability;
    sd = sqrt(sd);
    fprintf(out,"%d\t%g\t%g\t%g\t%g\n",counter,min,max,av,sd);
    return;
}

void do_hist_norm(FILE* in, FILE* out)
{
    char buffer[200];
    double value = 0;
    double probability;
    double probability2;
    double cumulant_probability = 0.0;
    int result;
    while (fgets(buffer,200,in))
    {
	result = sscanf(buffer,"%lf\t%lf\n",&value,&probability);
	if(result!=2) break;
	cumulant_probability += probability; //should be 1 if normalised
    }
    rewind(in);
    while (fgets(buffer,200,in))
    {
	result = sscanf(buffer,"%lf\t%lf\n",&value,&probability);
	if(result!=2) break;
	probability2 = probability/cumulant_probability;
        fprintf(out,"%g\t%g\t%g\n",value,probability,probability2);
    }
    return;
}

void do_uint_hist_stats(uint* hist,uint len,STATS* stats)
{
    uint i = 0;
    uint cumulant = 0;
    uint mass=0;
    uint min;
    uint max;
    uint peakvalue = 0;
    uint peakpos;
    double dav;
    double dvalue;
    double dcumulant = 0.0;
	/*minmax*/
    min = 0;
    while(!hist[min++]) if(min>=len) break;
    stats->min=(double)--min;
    max = len;
    while(!hist[--max]) if(max==0) break;
    stats->max=(double)max;
	/*av*/
    for(i = min; i<=max;i++)
    {
	cumulant	= cumulant + i*hist[i];
	mass 		= mass + hist[i];
	if (hist[i]>peakvalue)
	{
	    peakvalue = hist[i];
	    peakpos = i;
	}
    }
    stats->av = (double) cumulant / (double)mass;
    dav = (double) cumulant / (double)mass;
    stats->maxpos = (double)peakpos;
	/*sd*/
    for(i = min; i<=max;i++)
    {
	dvalue 		= ((double)i-dav);
	dvalue 		= dvalue * dvalue;
	dcumulant 	= dcumulant + (double)hist[i]*dvalue;
    }
    stats->sd = dcumulant / (double)mass;
    stats->sd = sqrt(stats->sd);

    return;
}

void do_col_stats(FILE* in, FILE* out,int ncol)
{
    char buffer[200];
    double value = 0;
    double av;
    double sd;
    double cumulant = 0.0;
    double min = DBL_MAX;
    double max = -DBL_MAX;
    double lastvalue = DBL_MAX;
    double mindiff = DBL_MAX;
    double diff;
    uint counter = 0;
    int result,i;
    char* tmp;
//av
//    fgets(buffer,200,in); //omit 1st line
    while (fgets(buffer,200,in))
    {
	i = ncol-1;
	tmp = buffer;
	while(i-- > 0)
          tmp = strchr(tmp,'\t');
 	result = sscanf(tmp,"%lf",&value);
	if(!result) break;
	cumulant += value;
	counter++;
	if (value > max) max = value;
	if (value < min) min = value;
        diff = fabs(lastvalue - value);
	if (diff > 0.0 && diff < mindiff) mindiff = diff;
	lastvalue = value;
    }
    av = cumulant / counter;
    cumulant = 0.0;
    rewind(in);
    fgets(buffer,200,in); //omit 1st line
    while (fgets(buffer,200,in))
    {
	result = sscanf(buffer,"%lf",&value);
	if(!result) break;
	value = (value-av) * (value-av);
	cumulant += value;
    }
    sd = cumulant / counter;
    sd = sqrt(sd);
    fprintf(out,"%d\t%g\t%g\t%g\t%g\t%g\n",counter,min,max,av,sd,mindiff);
    return;
}

uint split(char* text,char* separators, char** pieces, uint max_pieces)
{
  uint counter = 0;
  char * ptr = text;
  strcpy(pieces[counter++],strtok(text,separators));
  while ( (ptr=strtok(NULL, separators)) != NULL)
  {
    strcpy(pieces[counter++],ptr);
  }
  return counter;
}

uint scan_line_int(char* ptr,int* values,uint max_values)
{
  uint counter = 0;
  do{
    if( sscanf(ptr,"%d",values+counter) )
      ptr = strchr(ptr,'\t');
    else
      break;
    if(++counter>=max_values) break;
  }while(ptr++);// go from \t to first digit
  return counter;
}

uint scan_line_int_sep(char* ptr,int* values,uint max_values,char separator)
{
  uint counter = 0;
  do{
    if( sscanf(ptr,"%d",values+counter) )
      ptr = strchr(ptr,separator);
    else
      break;
    if(++counter>=max_values) break;
  }while(ptr++);// go from \t to first digit
  return counter;
}

uint scan_line(char* ptr,double* values,uint max_values)
{
  uint counter = 0;
  do{
    if( sscanf(ptr,"%lf",values+counter) )
      ptr = strchr(ptr,'\t');
    else
      break;
    if(++counter>=max_values) break;
  }while(ptr++);// go from \t to first digit
  return counter;
}

uint scan_line_sep(const char* ptr,double* values,uint max_values,char separator)
{
  uint counter = 0;
  do{
    if( sscanf(ptr,"%lf",values+counter) )
    {
      ptr = strchr(ptr,separator);
    }
    else
      break;
    if(++counter>=max_values) break;
    if(!ptr) break;
    while(*ptr == separator)
    {
      ++ptr;
      if(*ptr == '\n' || *ptr == '\0') break;
    }
  }while(ptr);// go from \t to first digit
  return counter;
}

uint count_columns(FILE* in)
{
  uint counter = 1;
  char buffer[2048];
  char *ptr;
  fgets(buffer,2048,in);
  if(!*buffer) return 0;
  ptr = buffer;
  do{
    if(ptr = strchr(++ptr,'\t')) counter++;
  }
  while( ptr );
  rewind(in);

  return counter;
}


double get_arg(double* y)
{
   return atan2(y[1],y[0]);/*arg*/
  /*
   else
     if(y[1] > 0.0)
       return asin(1.0);//PI/2 positive imaginary axis
     else
       return  3.0 * asin(1.0);// 3/2 PI negative imaginary axis
   */
}

void xy2mod_arg(double* mod,double* y)
/*
Zamiana liczby zespolonej X,Y na Mod,Arg
*/
{
   mod[0] = sqrt(y[0]*y[0]+y[1]*y[1]);
   mod[1] = atan2(y[1],y[0]);/*get_arg(y);*/
   return;
}

int comp_double_ascending(const void* ptr1,const void* ptr2)
/*
Fcja pomocnicza wymagana przez procedure qsort
*/
{
    double aux = *(double*)ptr1 - *(double*)ptr2;
    if (aux == 0.0) return 0;
    if (aux < 0.0) return -1;
    return 1;
    }

int comp_double_descending(const void* ptr1,const void* ptr2)
/*
Fcja pomocnicza wymagana przez procedure qsort
*/
{
    double aux = *(double*)ptr1 - *(double*)ptr2;
    if (aux == 0.0) return 0;
    if (aux < 0.0) return 1;
    return -1;
}

void sort(double* array,uint nelem, signed char order)
/*
Sortuje tablice typu double o dlugosci nelem rosnaco (dla order>0) lub malejaco (order<0)
*/
{
    if(order>=0)
      qsort(array,nelem,sizeof(double),comp_double_ascending);
    else
      qsort(array,nelem,sizeof(double),comp_double_descending);
    return;
}

int count_chars(char* buffer,char key)
{
    int count=0;
    if(!buffer) return 0;
    while(buffer = strchr(buffer,key))
    {
	if(!buffer) break;
        count++;
        buffer++;
    }
    return count;
}

double get_double_value(char* buffer)
/*
Pobiera ze stringu najblizsza wartosc double. Jezeli wartosc jest postaci 2PI, wykonuje mnozenie przez Pi
odroznia searatory: ,;
*/
{
    char* tmp1 = strstr( strupr(buffer),"PI");
    char* tmp2 = strpbrk(buffer, ",;" );
    if( !tmp1 )
      return  atof(buffer);
    if(!tmp2)
      return atof(buffer) * acos(-1.0);
    if(tmp1<tmp2)// string of format (1.0;1.0PI
      return atof(buffer) * acos(-1.0);//pi is from this variable
    return  atof(buffer);  //pi was not from this variable
}

uint get_arg_bracket(double* array, char* buffer, uint max_arg)
/*
Pobiera ze stringu serie argumentow typu double postaci (d1,d2,d3,d4,...,max_arg)
*/
{
	uint ii;
	char* separators="(,;";
        char* ptr = buffer;

	for(ii=0;ii<max_arg;ii++)
	{
	  ptr = strpbrk(ptr, separators );
	  if(!ptr) break;
	  array[ii] = get_double_value( ++ptr );
	}
	return ii;
}

void clean_stats(STATS* stats)
/*
Zeruje strukture STATS
*/
{
      stats->min = stats->max = stats->av = stats->sd = 0.0;
      stats->resolution = stats->maxpos = 0.0;
    return;
    }

char check_strictly_ascending_order(int* array, int range)
{
	int diff,i;
        for(i=1;i<range;i++)
        {
	    diff=array[i]-array[i-1];
	    if (diff<=0) return 0;
        }
return 1;
}

double norm_2pi(double variable)
{
    double PI2=acos(-1.0)+acos(-1.0);
    while(variable>PI2) variable-=PI2;
    while(variable<0.0) variable+=PI2;
    return variable;
}

double norm_plusminus_pi(double variable)
{
    double PI2=acos(-1.0)+acos(-1.0);
    double PI=acos(-1.0);
    double PIminus=-acos(-1.0);
    while(variable>PI) variable-=PI2;
    while(variable<PIminus) variable+=PI2;
    return variable;
}

void get_directory_path(char* path,const char* pathAndFileName)
{
  const char* lastSlash = strrchr(pathAndFileName, '\\');
  int len = (lastSlash == NULL) ? strlen(pathAndFileName) : (lastSlash - pathAndFileName) + 1;
  strncpy(path,pathAndFileName,len);
  path[len] = '\0';
}

void get_file_name(char* file_name,const char* pathAndFileName)
{
  const char* lastSlash = strrchr(pathAndFileName, '\\');
  if(lastSlash == NULL)
  {
    file_name[0] = '\0';
    return;
  }
  lastSlash++;
  strcpy(file_name,lastSlash);
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

