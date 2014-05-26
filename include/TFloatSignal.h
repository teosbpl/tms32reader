//---------------------------------------------------------------------------

#ifndef TFloatSignalH
#define TFloatSignalH
#include <math.h>
#include <stdlib.h>
#include "TException.h"
//---------------------------------------------------------------------------

class TFloatSignal
{
  friend class TSignalWindow;
  typedef float storage_element;
  protected:
    storage_element*              storage;
    bool           storageOwner;
    unsigned int    _NumberOfSamples;
    unsigned int           _Capacity;
    float                     TQuant;
    int           sampling_frequency;
  public:
    void ResetWithCapacity(int _Capacity);
    TFloatSignal():storage(NULL),storageOwner(true){};
    TFloatSignal(const TFloatSignal& arg);
    TFloatSignal(const TFloatSignal& arg,bool _storageOwner);
    ~TFloatSignal(){ if(storage && storageOwner) delete[] storage; };
    const TFloatSignal& operator=(const TFloatSignal& arg);
    const TFloatSignal& Downsample(const TFloatSignal& arg,int factor);
    virtual void PushBack(storage_element value);
    unsigned int NumberOfSamples(){ return _NumberOfSamples; };
    unsigned int Capacity(){ return _Capacity; };
    void SetNumberOfSamples(unsigned int arg){ _NumberOfSamples = arg; };
    int GetSamplingFrequency(){ return sampling_frequency;};
    void SetSamplingFrequency(int _sampling_frequency)
    {
       sampling_frequency = _sampling_frequency;
       TQuant = 1000.0 / (double)sampling_frequency;
    };
    inline storage_element operator[](unsigned int index)
    {
      if(index > _NumberOfSamples)
      {
//      AnsiString msg
       throw TException("Out of range");
       }
      return storage[index];
    };
    inline float TimePosition(unsigned int index)
    {
      return TQuant*index;
    }
    inline float GetTQuant(){ return TQuant;};
    inline void SetValue(unsigned int index,storage_element value)
    {
      if(index > _NumberOfSamples) throw TException("Out of range");
      storage[index] = value;
    };
    unsigned int ReadFromDump(const char* filename);
    virtual void Dump(const char* filename);
    void DumpFromTo(const char* filename,unsigned int smp_from,unsigned int smp_to);
    void Square();
    void Cube();
    void Nullify();
    storage_element* GetStorage(){ return storage;};

 // STL-style interface
    typedef storage_element* iterator;
    iterator begin(void) { return storage; }
    iterator end(void) { return storage + _NumberOfSamples; }
    typedef const storage_element* const_iterator;
    const_iterator const_begin(void) const { return storage; }//&storage[0]
    const_iterator const_end(void) const { return storage + _NumberOfSamples; }
    unsigned int distance(iterator pos1,iterator pos2)
    {
      signed int diff = pos2 - pos1;
      return abs(diff);
    }
    unsigned int distance(const_iterator pos1,const_iterator pos2)
    {
      signed int diff = pos2 - pos1;
      return abs(diff);
    }

};

class TSignalWindow: public TFloatSignal
{
public:
    TSignalWindow(TFloatSignal& signal,unsigned int offset,unsigned int length)
    {
      storage       = signal.storage + offset;
      _Capacity          = length;
      _NumberOfSamples   = length;
      TQuant             = signal.TQuant;
      sampling_frequency = signal.sampling_frequency;
    }
    ~TSignalWindow()
    {
      storage=NULL;
    };//do not delete the signal. the next desctructor called will see null and will do nothing
    double operator*(float* vector)
    {
      if(!vector) throw TException("Null argument");
      double result = 0.0;
      for(unsigned int i=0;i<_NumberOfSamples;++i)
        result += storage[i] * vector[i];
      return result;
    };
};

class TNoOwnStorageFloatSignal: public TFloatSignal
{
  public:
    TNoOwnStorageFloatSignal(const TFloatSignal& arg):
    TFloatSignal(arg,false){}
};
#endif
