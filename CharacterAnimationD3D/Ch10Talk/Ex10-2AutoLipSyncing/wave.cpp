#include "wave.h"

extern ofstream g_debug;

WaveFile::WaveFile() {
    m_numSamples = 0;
    m_sampleRate = 0;
    m_bitsPerSample = 0;
    m_numChannels = 0;
    m_pData = NULL;
}

WaveFile::~WaveFile() {
    if (m_pData != NULL)
        delete [] m_pData;
}

void WaveFile::Load(string filename) {
    ifstream in(filename.c_str(), ios::binary);

    //RIFF
    char ID[4];
    in.read(ID, 4);
    if (ID[0] != 'R' || ID[1] != 'I' || ID[2] != 'F' || ID[3] != 'F') {      //4 first bytes should say 'RIFF'
        g_debug << "File: " << filename.c_str() << " is not a valid media file\n";
        in.close();
        return;
    }

    //RIFF chunk Size
    long fileSize = 0;
    in.read((char*)&fileSize, sizeof(long));

    //The actual file size is 8 bytes larger
    fileSize += 8;

    //Wave ID
    in.read(ID, 4);
    if (ID[0] != 'W' || ID[1] != 'A' || ID[2] != 'V' || ID[3] != 'E') {      //ID should be 'WAVE'
        g_debug << "File: " << filename.c_str() << " is not a valid Wave file\n";
        in.close();
        return;
    }

    //Format Chunk ID
    in.read(ID, 4);
    if (ID[0] != 'f' || ID[1] != 'm' || ID[2] != 't' || ID[3] != ' ') {      //ID should be 'fmt '
        g_debug << "File: " << filename.c_str() << ", no format chunk found\n";
        in.close();
        return;
    }

    //Format Chunk Size
    long formatSize = 0;
    in.read((char*)&formatSize, sizeof(long));


    //Audio Format
    short audioFormat = 0;
    in.read((char*)&audioFormat, sizeof(short));
    if (audioFormat != 1) {
        g_debug << "File: " << filename.c_str() << " is unsupported, contains compressed data\n";
        in.close();
        return;
    }

    //Num Channels
    in.read((char*)&m_numChannels, sizeof(short));

    //Sample Rate
    in.read((char*)&m_sampleRate, sizeof(long));

    //Byte Rate
    long byteRate = 0;
    in.read((char*)&byteRate, sizeof(long));

    //Block Align
    short blockAlign = 0;
    in.read((char*)&blockAlign, sizeof(short));

    //Bits Per Sample
    in.read((char*)&m_bitsPerSample, sizeof(short));
    if (m_bitsPerSample != 16) {
        g_debug << "File: " << filename.c_str() << " is not a 16-Bit wave file\n";
        in.close();
        return;
    }

    //Data Chunk ID
    in.read(ID, 4);
    if (ID[0] != 'd' || ID[1] != 'a' || ID[2] != 't' || ID[3] != 'a') {      //ID should be 'data'
        g_debug << "File: " << filename.c_str() << ", no data chunk found\n";
        in.close();
        return;
    }

    //Size
    long dataSize;
    in.read((char*)&dataSize, sizeof(long));
    m_numSamples = dataSize / 2;

    //Data
    m_pData = new short[m_numSamples];
    in.read((char*)m_pData, dataSize);

    in.close();
}

short WaveFile::GetMaxAmplitude() {
    if (m_pData == NULL)
        return 0;

    //Find sample with highest amplitude
    short maxAmp = 0;

    for (int i=0; i<m_numSamples; i++) {
        if (abs(m_pData[i]) > maxAmp)
            maxAmp = abs(m_pData[i]);
    }

    return maxAmp;
}

short WaveFile::GetAverageAmplitude(float startTime, float endTime) {
    if (m_pData == NULL)
        return 0;

    //Calculate start & end sample
    int startSample = (int)(m_sampleRate * startTime) * m_numChannels;
    int endSample = (int)(m_sampleRate * endTime) * m_numChannels;

    if (startSample >= endSample)
        return 0;

    //Calculate the average amplitude between start and end sample
    float c = 1.0f / (float)(endSample - startSample);
    float avg = 0.0f;
    for (int i=startSample; i<endSample && i<m_numSamples; i++) {
        avg += abs(m_pData[i]) * c;
    }

    avg = min(avg, (float)(SHRT_MAX - 1));
    avg = max(avg, (float)(SHRT_MIN + 1));

    return (short)avg;
}

float WaveFile::GetLength() {
    return (m_numSamples / m_numChannels) / (float)m_sampleRate;
}