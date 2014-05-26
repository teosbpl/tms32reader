//---------------------------------------------------------------------------

#ifndef TEdfReaderH
#define TEdfReaderH
//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <stdio.h>
#include <time.h>

#include "TFloatSignal.h"

typedef double TDateTime;

class TTms32Reader
{
  friend class TMatlabSave;
  static const unsigned int HEADER_SIZE;
  static const unsigned int SIGNAL_HEADER_SIZE_BLOCK;
  static const unsigned int SAMPLE_DATA_BLOCK_HEADER_SIZE;

  #pragma pack(push)
  #pragma pack(1)

  struct TTms32Header
  {
     char                 file_identifier[31];//                                  31    31
     short                     version_number;//                                  02    33  short
     char                measurement_name[81];//                                  81   114
     signed short          sampling_frequency; //SR                               02   116  signed short
     signed short                storage_rate; //SR                               02   118  signed short
     bool            storage_type_one_over_hz; //0                                01   119  bool
     signed short           number_of_signals;//NS                                02   121  signed short
     int32_t     number_of_sample_periods;//NP                                04   125  signed long
     char                        reserved1[4];//0                                 04   129
     short   start_of_measurement[7];//MS dos_time                                14   143  short
     int32_t number_of_sample_data_blocks;//NB                                04   147  signed long
     unsigned short sample_periods_per_sample_data_block;//PB. multiple of 16     02   149  unsigned short
     unsigned short size_of_sample_data_in_block_in_bytes;//SD                    02   151  unsigned short
     char           delta_compression_flag[2];//0                                 02   153  char
     char                       reserved2[64];//                                  64   217  char
     void DumpHeader(std::ostream& ostr);
  };//should be 217
  #pragma pack(pop)
//------------------------------------------------------------
  struct TTms32Signal: public TFloatSignal
  {
  #pragma pack(push)
  #pragma pack(1)
    struct TTms32SignalHeader
    {
      char            signal_name[41];//SN
      char                reserved[4];//in PortiLab 0
      char              unit_name[11];//
      float                  unit_low;//UL in portiLab 0.0
      float                 unit_high;//UH bin PortiLab: Sample value in Units equal to 1000 ADC bits
      float                   adc_low;//AL In PortiLab 0.0
      float                  adc_high;//AH In PortiLab 1000.0
      short              signal_index;//SI signal index
      short              cache_offset;//in PortiLab 0
      char              reserved2[60];
    };//should be 136
  #pragma pack(pop)
    TTms32SignalHeader SignalHeader;
    float PhysicalRangeToDigitalRange;
    TTms32Signal():TFloatSignal(),PhysicalRangeToDigitalRange(0.0){};
    void SetUnitsConversion()
    {
      PhysicalRangeToDigitalRange = (SignalHeader.unit_high - SignalHeader.unit_low)/(SignalHeader.adc_high - SignalHeader.adc_low);
    };
    inline float ADCToPhysical(float value)
    {

      value-=SignalHeader.adc_low;
      double dvalue = SignalHeader.unit_low + (double)value*PhysicalRangeToDigitalRange;
      return dvalue;
    };
    virtual void PushBack(float value);
    void DumpHeader(std::ostream& ostr);
  };
  #pragma pack(push)
  #pragma pack(1)

  struct TTms32BlockHeader
  {
    signed long period_index;//PI
    signed long reserved;//4 bytes
    short   time_of_first_sample_in_block[7];//BT dos_time
    char       reserved1[64];
  };//size should be 86
  #pragma pack(pop)
  TTms32Header             header;
//public:
  typedef std::vector<TTms32Signal*> TTms32Signals;
//private:
  TTms32Signals signals;
  std::string filename;
  bool ReadHeaderFixed();
  bool ReadSignalHeaders();
  bool ReadSignals();
  int selectedSignal;
public:
  TTms32Reader(std::string  _filename);
  void LoadSignals();
  TFloatSignal& LoadSignal(unsigned int channel);
  void LoadHeaders();
  void DumpSignals();
  void DumpHeaders(std::string filename);
  TFloatSignal& GetChannel(unsigned int channel);
  TTms32Header& GetHeader(){ return  header; }
  bool DumpChannelFromTo(unsigned int channel,unsigned int smp_from,unsigned int smp_to,const char* filename);
  double RecordingStart();
  ~TTms32Reader();
  public:
    TTms32Signals& getAllSignals(){ return signals;};
};
#endif
