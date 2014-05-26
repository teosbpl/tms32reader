//---------------------------------------------------------------------------

#include "TFloatSignal.h"
#include <stdio.h>
#include "unilib.h"
#include <stdlib.h>
#include <string.h>
//#include <mem.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)
TFloatSignal::TFloatSignal(const TFloatSignal& arg):
storage(NULL),
storageOwner(true)
{
  *this = arg;
}

TFloatSignal::TFloatSignal(const TFloatSignal& arg,bool _storageOwner):
storage(NULL)
{
   storageOwner = _storageOwner;
  *this = arg;
}
const TFloatSignal& TFloatSignal::Downsample(const TFloatSignal& arg,int factor)
{
  this->ResetWithCapacity(arg._Capacity/factor);
  this->_NumberOfSamples = arg._NumberOfSamples/factor;
  int ii=0;
  for(int i = 0; i < this->_NumberOfSamples; ++i,ii+=factor)
    this->storage[i] = arg.storage[ii];
  this->SetSamplingFrequency(arg.sampling_frequency/factor);
  return *this;
}
const TFloatSignal& TFloatSignal::operator=(const TFloatSignal& arg)
{
  if(storageOwner)
  {
    this->ResetWithCapacity(arg._Capacity);
    memcpy(this->storage,arg.storage,_Capacity*sizeof(float));
  }
  else
  {
     this->storage = arg.storage;
  }
  this->_NumberOfSamples = arg._NumberOfSamples;
  this->SetSamplingFrequency(arg.sampling_frequency);
  return *this;
}

void TFloatSignal::ResetWithCapacity(int _Capacity)
{
  if(!storageOwner) throw TException("TFloatSignal::ResetWithCapacity Cannot reset foreign data");
  this->_Capacity = _Capacity;
  _NumberOfSamples = 0;
  if(storage)  delete storage;
  storage = new storage_element[_Capacity];
}

void TFloatSignal::PushBack(storage_element value)
{
  if(_NumberOfSamples >= _Capacity) throw TException("out of range");
  storage[_NumberOfSamples++] = value;
}

void TFloatSignal::Dump(const char* filename)
{
  if(!filename) return;
  FILE* fout = fopen(filename,"wt");
  unsigned int nums = NumberOfSamples();
  for(unsigned int i = 0; i < nums;++i)
    fprintf(fout,"%lf\n",storage[i]);
  fclose(fout);
}

void TFloatSignal::Square()
{
  int nums = NumberOfSamples();
  for(int i = 0; i < nums;++i)
    storage[i] = storage[i] * storage[i];
}

void TFloatSignal::Cube()
{
  int nums = NumberOfSamples();
  for(int i = 0; i < nums;++i)
    storage[i] = storage[i] * storage[i] * storage[i];
}
void TFloatSignal::Nullify()
{
  //int nums = NumberOfSamples();
  memset(this->storage, 0,_Capacity*sizeof(storage_element));
  //for(int i = 0; i < nums;++i)    SetValue(i,0.0);
}

unsigned int TFloatSignal::ReadFromDump(const char* filename)
{
  if(!filename) return 0;
  FILE* fin = fopen(filename,"rt");
  if(!fin) return 0;
  unsigned int lines_count = count_lines(fin);
  ResetWithCapacity(lines_count);

  for(unsigned int i = 0; i < lines_count;++i)
  {
    double value;
    fscanf(fin,"%lf\n",&value);
    PushBack(value);
  }
  fclose(fin);
  return NumberOfSamples();
}

void TFloatSignal::DumpFromTo(const char* filename,unsigned int smp_from,unsigned int smp_to)
{
  if(!filename) return;
  FILE* fout = fopen(filename,"wt");
  unsigned int nums = NumberOfSamples();
  if(smp_to > nums) smp_to = nums;
  for(unsigned int i = smp_from; i < smp_to;++i)
    fprintf(fout,"%lf\n",storage[i]);
  fclose(fout);
}

