#ifndef __UNILIB_H__
#define __UNILIB_H__
//#ifdef __cplusplus
//extern "C" {
//#endif

#include <stdio.h>
#include <string>

using namespace std;
//#define unsigned int unsigned int
#define SORT_ASCENDING ((signed char)1)
#define SORT_DESCENDING ((signed char)-1)
#define get_pi() acos(-1.0)
#define get_2pi() (acos(-1.0)+acos(-1.0))

typedef struct tagSTATS
{
  double min;
  double max;
  double av;
  double sd;
  double resolution;
  double maxpos;
}STATS;
char * strupr(char *s);
void  strxch(char* str1, char *str2);
void  add_to_name(char* filename, char *add);
char parse(int argc,char* argv[],char* key, char* optional_arg);
char** load_arg_file(char* name,char* argv[]);
char** free_arg_file_buffer(void);
char* value2filename(char* filename,char* c_cvalue);
unsigned char* load_uchar_binary_signal(char *filename,unsigned int* _max_index);
unsigned short* load_ushort_binary_signal(char *filename,unsigned int* _max_index);
char* load_signal(char *filename,unsigned int* _max_index);
double* load_double_signal(char *filename,unsigned int* _max_index);
double* load_double_col_signal(char *filename,unsigned int* _max_index,int col_nr);
int* load_int_signal(char *filename,unsigned int* _max_index);
char is_extremum(double* signal);
char check_if_exists(char* name);
unsigned int* do_histogram(double *signal,unsigned int max_index,unsigned int num_of_bins,double min, double max,double *binsize);
int cannot_open(char* name,char *mode);
void do_hist_stats(FILE* in, FILE* out);
void do_hist_norm(FILE* in, FILE* out);
void do_col_stats(FILE* in, FILE* out,int ncol);

unsigned int count_lines(FILE* in);
unsigned int count_columns(FILE* in);
unsigned int split(char* text,char* separators, char** pieces, unsigned int max_pieces);
unsigned int scan_line(char* ptr,double* values,unsigned int max_values);
unsigned int scan_line_sep(const char* ptr,double* values,unsigned int max_values,char separator);
unsigned int scan_line_int(char* ptr,int* values,unsigned int max_values);
unsigned int scan_line_int_sep(char* ptr,int* values,unsigned int max_values,char separator);
void get_stats(double* signal,unsigned int max_index,STATS* stats);
double get_arg(double* y);
void xy2mod_arg(double* mod,double* y);
void sort(double* array,unsigned int nelem, signed char order);
int count_chars(char* buffer,char key);
double get_double_value(char* buffer);
unsigned int get_arg_bracket(double* array, char* buffer, unsigned int max_arg);
unsigned int get_file_size(const char* name);
long int filesize( FILE *fp );
void chg_extension(char* name,char* ext);
int error(char* msg);
void do_uint_hist_stats(unsigned int* hist,unsigned int len,STATS* stats);
void clean_stats(STATS* stats);
char check_strictly_ascending_order(int* array, int range);
double norm_2pi(double variable);
double norm_plusminus_pi(double variable);
void get_directory_path(char* path,const char* pathAndFileName);
void get_file_name(char* file_name,const char* pathAndFileName);

char* getCmdOption(char ** begin, char ** end, const string & option);
bool cmdOptionExists(char** begin, char** end, const string& option);

//#ifdef __cplusplus
//};
//#endif


#endif

