#include <fstream>

#include "Settings.h"
#include "Average.h"

class App{
  const Settings settings;
  std::ifstream data_file;
  std::ofstream average_file;
  Average* average_trace;
  int event_number=1;
  int single_trace_number_index=0;
public:
  App();
  void openDataFile();
};