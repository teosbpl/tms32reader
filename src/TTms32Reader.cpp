//---------------------------------------------------------------------------

#ifndef edf_readerH
#define edf_readerH
//---------------------------------------------------------------------------

//#include <vcl.h>
//#pragma hdrstop

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "TTms32Reader.h"
//#include "TDateConverter.h"
#include "extalg.h"
#include "unilib.h"
#include "TException.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
  const unsigned int TTms32Reader::HEADER_SIZE = 217;
  const unsigned int TTms32Reader::SIGNAL_HEADER_SIZE_BLOCK = 136;
  const unsigned int TTms32Reader::SAMPLE_DATA_BLOCK_HEADER_SIZE = 86;

void TTms32Reader::TTms32Signal::PushBack(float value)
{
  TFloatSignal::PushBack(ADCToPhysical(value));
}

void TTms32Reader::TTms32Header::DumpHeader(std::ostream& ostr)
{
  ostr << "================= Tms32/Poly5 Header =====================" << std::endl;
//  ostr << "local_patient_identification:     " << local_patient_identification << std::endl;
//  ostr << "local_recording_identification:   " << local_recording_identification << std::endl;
//  ostr << "number_of_bytes_in_header_record: " << number_of_bytes_in_header_record << std::endl;
//  ostr << "start_date:                       " << start_date.DateString().c_str() << std::endl;
//  ostr << "start_time:                       " << start_time.TimeString().c_str() << std::endl;
//  ostr << "number_of_data_records:           " << number_of_data_records << std::endl;
//  ostr << "duration_of_a_data_record_sec:    " << duration_of_a_data_record_sec << std::endl;
//  ostr << "number_of_signals_in_data_record: " << number_of_signals_in_data_record << std::endl;
}

void TTms32Reader::TTms32Signal::DumpHeader(std::ostream& ostr)
{
      ostr << "signal_name:  " << SignalHeader.signal_name  << std::endl;//SN
      ostr << "signal_index: " << SignalHeader.unit_name    << std::endl;//
      ostr << "unit_low:     " << SignalHeader.unit_low     << std::endl;//UL in portiLab 0.0
      ostr << "unit_high:    " << SignalHeader.unit_high    << std::endl;//UH bin PortiLab: Sample value in Units equal to 1000 ADC bits
      ostr << "adc_low:      " << SignalHeader.adc_low      << std::endl;//AL In PortiLab 0.0
      ostr << "adc_high:     " << SignalHeader.adc_high     << std::endl;//AH In PortiLab 1000.0
      ostr << "signal_index: " << SignalHeader.signal_index << std::endl;//SI signal index
}

bool TTms32Reader::ReadHeaderFixed()
{
  FILE*  ifile = fopen(filename.c_str(),"rb");
  if(!ifile) return false;
//  unsigned int          _HEADER_SIZE = sizeof(TTms32Header);

  if(1 != fread((void*)&header,
                HEADER_SIZE,
                1,
                ifile))
                {
                  fclose(ifile);
                  return false;
                }
  if(ifile) fclose(ifile);
//TTms32HeaderChars to TTms32Header
  //initialize empy signals
  for_each(signals.begin(),signals.end(),FDeleteObject());
  while(signals.size() < header.number_of_signals/2)
    signals.push_back(new TTms32Signal());
  return true;
}

bool TTms32Reader::ReadSignalHeaders()
{
  FILE*  ifile = fopen(filename.c_str(),"rb");
  if(!ifile) return false;

  if(fseek(ifile,HEADER_SIZE,SEEK_SET))
    return false;
  if(signals.empty() || header.number_of_signals == 0) return false;//fixed header not loaded?
  for(TTms32Signals::iterator item = signals.begin(); item != signals.end();++item)
  {
    TTms32Signal* signal = *item;
//read twice for each channel
    if(1!=fread(&(signal->SignalHeader),
                SIGNAL_HEADER_SIZE_BLOCK,
                1,ifile))
                {
                  fclose(ifile);
                  return false;
                }
    if(1!=fread(&(signal->SignalHeader),
                SIGNAL_HEADER_SIZE_BLOCK,
                1,ifile))
                {
                  fclose(ifile);
                  return false;
                }
  }
  if(ifile) fclose(ifile);
  return true;
}

void TTms32Reader::DumpHeaders(std::string filename)
{
  std::ostringstream stream_buffer;
  int sizz2 = signals.size();
  header.DumpHeader(stream_buffer);
  for(TTms32Signals::iterator item = signals.begin(); item != signals.end();++item)
  {
    unsigned int number = distance(signals.begin(),item);
    stream_buffer << "============ Signal " << number << " ===========" << std::endl;
    (*item)->DumpHeader(stream_buffer);
  }
  FILE* fout = fopen(filename.c_str(),"wt");
  if(!stream_buffer.str().empty())
  fputs(stream_buffer.str().c_str(),fout);
  fclose(fout);
}
  TFloatSignal& TTms32Reader::LoadSignal(unsigned int channel)
  {
//    if(signals == NULL) throw TException("Channels empty");
  selectedSignal = (int)channel;
   std::string msg1 = "Channels empty";
   if(signals.empty()) throw TException(msg1);
  if(channel>=signals.size()) throw TException("Channel number out of range");
  ReadSignals();
  selectedSignal = -1;
  return *(signals[channel]);
  }
bool TTms32Reader::ReadSignals()
{
  FILE*  ifile = fopen(filename.c_str(),"rb");
  if(!ifile) return false;
  unsigned int number_of_bytes_in_header_record = HEADER_SIZE + SIGNAL_HEADER_SIZE_BLOCK*header.number_of_signals;
  if(fseek(ifile,number_of_bytes_in_header_record,SEEK_SET))
    return false;
  TTms32BlockHeader block_header;
  int abc = sizeof(TTms32BlockHeader);
  bool bUnexpectedEndOfSignal = false;
//  std::ofstream dump("dump_signal.txt");
  #pragma pack(push)
  #pragma pack(1)

  union float_and_two_shorts
  {
    float     f;
    short    lo;
    short    hi;
  };
  #pragma pack(pop)
  int abcd = sizeof(float_and_two_shorts);
  unsigned long buffer_size = header.size_of_sample_data_in_block_in_bytes /sizeof(float_and_two_shorts);
  float_and_two_shorts* buffer = new float_and_two_shorts[buffer_size];
  int buffer_size_in_shorts = 2* buffer_size;
  int number_of_samples_of_a_channel_in_block = buffer_size/4;//real channels
  int expected_channel_length = number_of_samples_of_a_channel_in_block * header.number_of_sample_data_blocks;
  for(TTms32Signals::iterator item = signals.begin(); item != signals.end();++item)
  {
    if(selectedSignal >= 0)
    {
      if(std::distance(signals.begin(),item) == selectedSignal)
      {
         (*item)->ResetWithCapacity(expected_channel_length);
      }
    }
    else
    {
        (*item)->ResetWithCapacity(expected_channel_length);
    }
    (*item)->SetUnitsConversion();
    (*item)->SetSamplingFrequency(header.sampling_frequency);
  }
//    repeat number_of_sample_data_blocks times
  for(signed long block_counter = 0; block_counter < header.number_of_sample_data_blocks; ++block_counter )
  {
    int base_offset = number_of_samples_of_a_channel_in_block *  block_counter;
  //read 86 bytes header
    if(1!=fread(&block_header, SAMPLE_DATA_BLOCK_HEADER_SIZE, 1,ifile))
    {
       bUnexpectedEndOfSignal = true;
       break;
    }

    if(buffer_size_in_shorts!=fread(buffer, sizeof(short), buffer_size_in_shorts,ifile))
    {
       bUnexpectedEndOfSignal = true;
       break;
    }
    //here process data
    int column_counter = 0;
    TTms32Signals::iterator begin_item = signals.begin();
    TTms32Signals::iterator       item = signals.begin();
    TTms32Signals::iterator   end_item = signals.end();
    for(int float_counter=0; float_counter <  buffer_size; ++float_counter)
    {
       float ff = buffer[  float_counter ].f;
       if(item == end_item)
       {
          item = begin_item;
          column_counter = 0;
       }
//       if(_NumberOfSamples >= _Capacity) throw TException("out of range");
//       storage[_NumberOfSamples++] = value;
       if(selectedSignal >= 0)
       {
         if(column_counter == selectedSignal)
         {
          (*item)->PushBack(ff);
         }
       }
       else
       {
          (*item)->PushBack(ff);
       }
       ++item;
       ++column_counter;
    }
//    (*item)->SetNumberOfSamples()
  }
//       dump << ff << '\t';
//       if(++column_counter==4)
//       {
//         column_counter = 0;
//         dump << std::endl;
//       }


//    header.size_of_sample_data_in_block_in_bytes;
  //read size_of_sample_data_in_block_in_bytes

  delete[] buffer;
  if(ifile) fclose(ifile);

  return true;
}

void TTms32Reader::DumpSignals()
{
  for(TTms32Signals::iterator item = signals.begin(); item != signals.end();++item)
  {
    std::string filename = "dump_";
    //filename += AnsiString(distance(signals.begin(),item)).c_str();
    char str[16];
    sprintf(str,  "%ld", distance(signals.begin(),item) );
    filename += str;
    filename += ".txt";
    TTms32Signal* signal = (*item);
    signal->Dump(filename.c_str());
  }
}

bool TTms32Reader::DumpChannelFromTo(unsigned int channel,unsigned int smp_from,unsigned int smp_to,const char* filename)
{
    if(channel>=signals.size()) throw TException("Out of range");
    if(!filename) throw TException("Null filename");
    TFloatSignal& s = *(signals[channel]);
    s.DumpFromTo(filename,smp_from,smp_to);
    return true;
}

TTms32Reader::TTms32Reader(std::string _filename):
filename(_filename),
selectedSignal(-1)
{}

TTms32Reader::~TTms32Reader()
{
  for_each(signals.begin(),signals.end(),FDeleteObject());
}

void TTms32Reader::LoadSignals()
{
      ReadSignals();
}

void TTms32Reader::LoadHeaders()
{
  ReadHeaderFixed();
  ReadSignalHeaders();
}

TFloatSignal& TTms32Reader::GetChannel(unsigned int channel)
{
//    if(signals == NULL) throw TException("Channels empty");
   std::string msg1 = "Channels empty";
   if(signals.empty()) throw TException(msg1);
  if(channel>=signals.size()) throw TException("Channel number out of range");
  return *(signals[channel]);
}

TDateTime TTms32Reader::RecordingStart()
{
return 0.0;//TDateTime(header.start_of_measurement[0],header.start_of_measurement[1],header.start_of_measurement[2]) +
//       TDateTime(header.start_of_measurement[4],header.start_of_measurement[5],header.start_of_measurement[6],0);
//fastcall TDateTime(unsigned short year, unsigned short month, unsigned short day);
//__fastcall TDateTime(unsigned short hour, unsigned short min, unsigned short sec, unsigned short msec);


//start_of_measurement
}
#endif
