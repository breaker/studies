//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef WAVE_H
#define WAVE_H

#include <vector>
#include <fstream>

using namespace std;

class WaveFile {
public:
    WaveFile();
    ~WaveFile();
    void Load(string filename);
    short GetMaxAmplitude();
    short GetAverageAmplitude(float startTime, float endTime);
    float GetLength();

public:
    long m_numSamples;
    long m_sampleRate;
    short m_bitsPerSample;
    short m_numChannels;
    short *m_pData;
};

#endif

