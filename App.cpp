#include <fstream>
#include <iostream>

#include "App.h"

App::App(){
  openDataFile();
  average_file.open(settings.data_file_name.substr(0, settings.data_file_name.find_last_of(".")+1)+"average");
  average_trace = new Average(&settings);

  while (!data_file.eof()){
    average_trace->readDataFile(data_file);
    average_trace->addToAverage();
    if (settings.single_trace_number_list[single_trace_number_index]==event_number){
      average_trace->writeSingleTrace(single_trace_number_index);
      single_trace_number_index++;
    }
    event_number++;
  }
  event_number--;//the last event_number++ exceeds max event number, roll back
  average_trace->calculate(event_number);
  average_trace->writeAverageToFile(average_file);
  std::cout<<settings.data_file_name.substr(0, settings.data_file_name.find_last_of(".")+1)<<"average generated successfully"<<std::endl;
  if (settings.single_trace_number_list[0]>0 && settings.single_trace_number_list[0]<event_number){
    std::cout<<"Single trace files generated successfully"<<std::endl;
  }
}
//-----------------------------------------------------------------------------------------------------------------
void App::openDataFile(){
  try{
    if (settings.data_file_name.substr(settings.data_file_name.size()-8, settings.data_file_name.size())==".raw_bin")
      data_file.open (settings.data_file_name, std::ios::binary);
    if (settings.data_file_name.substr(settings.data_file_name.size()-8, settings.data_file_name.size())==".raw_asc")
      data_file.open (settings.data_file_name);
    if (!data_file.is_open()) {throw "raw data file is not found";}
  }
  catch (const char* msg){
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
  }
}