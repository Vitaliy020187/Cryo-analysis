#include <fstream>
#include <iostream>
#include <cmath>
#include <string>

#include "Average.h"

Average::Average(const Settings* settings){
  this->settings=settings;
  sample_array = new double[settings->act_samples_per_rec*settings->num_channels];
  for (int channel=0; channel<settings->num_channels; channel++){
    mean[channel]=new double[settings->act_samples_per_rec];
    std_dev_of_mean[channel]=new double[settings->act_samples_per_rec];
    for (int i=0; i<settings->act_samples_per_rec; i++) {mean[channel][i]=0; std_dev_of_mean[channel][i]=0;}
  }
}
//====================================================================================================================
void Average::readDataFile(std::ifstream& data_file){
  if (settings->data_file_name.substr(settings->data_file_name.length()-8, settings->data_file_name.length())==".raw_bin") readBinaryDataFile(data_file);
  if (settings->data_file_name.substr(settings->data_file_name.length()-8, settings->data_file_name.length())==".raw_asc") readAsciiDataFile(data_file);
}
//---------------------------------------------------------------------------------------------------------------------
inline void Average::readBinaryDataFile(std::ifstream& data_file){
  std::uint32_t time_swapped_bytes;
  std::int16_t  amplitude_swapped_bytes;
  std::string line;

  for (int channel=0; channel<settings->num_channels; channel++){
  data_file.read (reinterpret_cast<char*>(&time_swapped_bytes), sizeof(uint32_t));
    for (int i=0; i<settings->act_samples_per_rec; i++){
      data_file.read (reinterpret_cast<char*>(&amplitude_swapped_bytes), sizeof(int16_t));
      sample_array[i+channel*settings->act_samples_per_rec]=__builtin_bswap16 (amplitude_swapped_bytes);
      if (sample_array[i+channel*settings->act_samples_per_rec]>INT16_MAX){
        sample_array[i+channel*settings->act_samples_per_rec]=sample_array[i+channel*settings->act_samples_per_rec]-(UINT16_MAX+1);
      }
    }
  }
  time=__builtin_bswap32 (time_swapped_bytes)*1000;
}
//---------------------------------------------------------------------------------------------------------------------
inline void Average::readAsciiDataFile(std::ifstream& data_file){
  for(int channel=0; channel<settings->num_channels; channel++){
    data_file >> time;
    time=time*1000;
    for (int i=0; i<settings->act_samples_per_rec; i++) data_file>>sample_array[i+channel*settings->act_samples_per_rec];
  }
}
//=====================================================================================================================
void Average::addToAverage(){
  double* baseline = new double(settings->num_channels);
  for (int channel=0; channel<settings->num_channels; channel++){
    calculateBaseline(channel, baseline);
    for (int i=0; i<settings->act_samples_per_rec; i++){
      mean[channel][i] += baseline[channel]-sample_array[i+channel*settings->act_samples_per_rec];
      std_dev_of_mean [channel][i] += pow(baseline[channel]-sample_array[i+channel*settings->act_samples_per_rec], 2);
    }
  }
}
//--------------------------------------------------------------------------------------------------------------------
inline void Average::calculateBaseline(int channel, double*& baseline){
  for (int i=0; i<int(settings->act_samples_per_rec*settings->pretrigger_fraction*settings->baseline_calc_fraction); i++){
    baseline[channel]+=sample_array[i+settings->act_samples_per_rec*channel];
  }
  baseline[channel]=baseline[channel]/int(settings->act_samples_per_rec*settings->pretrigger_fraction*settings->baseline_calc_fraction);
}
//=====================================================================================================================
void Average::calculate(int event_number){
  for (int channel=0; channel<settings->num_channels; channel++){
    for (int i=0; i<settings->act_samples_per_rec; i++){
      mean[channel][i] /= event_number;
      std_dev_of_mean[channel][i]=sqrt((std_dev_of_mean[channel][i]-event_number*mean[channel][i]*mean[channel][i])/(event_number-1));
      std_dev_of_mean[channel][i]/=sqrt(event_number);
    }
  }
}
//=====================================================================================================================
void Average::writeAverageToFile(std::ofstream& average_file){
  for (int i=0; i<settings->act_samples_per_rec; i++){
    average_file<<mean[0][i]<<'\t'<<std_dev_of_mean[0][i];
    if (settings->num_channels ==2){
      average_file<<'\t'<<mean[1][i]<<'\t'<<std_dev_of_mean[1][i];
    }
    average_file<<std::endl;
  }
}
//=================================================================================================================================================
void Average::writeSingleTrace(int single_trace_number_index){
  double* baseline = new double(settings->num_channels);
  for (int channel=0; channel<settings->num_channels; channel++){
    std::ofstream single_trace_file (settings->data_file_name.substr(0, settings->data_file_name.length()-8)+"_trace_"+std::to_string(channel)+"_"+std::to_string(settings->single_trace_number_list[single_trace_number_index])+".txt");
    calculateBaseline(channel, baseline);
    for (int i=0; i<settings->act_samples_per_rec; i++){
      single_trace_file<<baseline[channel]-sample_array[i+channel*settings->act_samples_per_rec]<<std::endl;
    }
    single_trace_file.close();
  }
}