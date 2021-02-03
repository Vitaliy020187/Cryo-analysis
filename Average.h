#pragma once

#include "Settings.h"

class Average{//average trace of all events with sandard deviation for each sample
	unsigned long int time;
    double *mean[2], *std_dev_of_mean[2];
    double *sample_array;
    const Settings* settings;

    inline void readBinaryDataFile(std::ifstream&);
    inline void readAsciiDataFile(std::ifstream&);
    inline void calculateBaseline(int, double*&);
  public:
    Average(const Settings*);
    void readDataFile(std::ifstream&);
    void addToAverage();
    void calculate(int);
    void writeAverageToFile(std::ofstream&);
    void writeSingleTrace(int);
};