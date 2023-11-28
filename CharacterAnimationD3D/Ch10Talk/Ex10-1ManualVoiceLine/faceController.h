//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _FACECONTROLLER_
#define _FACECONTROLLER_

#include <d3dx9.h>
#include <vector>
#include "face.h"

using namespace std;

class Viseme {
public:
    Viseme();
    Viseme(int target, float amount, float time);

public:
    int m_morphTarget;
    float m_blendAmount;
    float m_time;
};

class FaceController {
    friend class Face;
public:
    FaceController(D3DXVECTOR3 pos, Face *pFace);
    void Update(float deltaTime);
    void Render();

    void Speak(vector<Viseme> &visemes);
    void UpdateSpeech(float deltaTime);

public:

    //Reference to face class
    Face *m_pFace;

    //Morph variables
    int m_emotionIndex;
    int m_speechIndices[2];
    float m_blinkTime;
    float m_emotionTime, m_emotionBlend;
    D3DXVECTOR4 m_morphWeights;

    //Head position
    D3DXMATRIX m_headMatrix;

    //Eyes
    Eye m_eyes[2];

    //Speech viseme array
    vector<Viseme> m_visemes;
    int m_visemeIndex;
    float m_speechTime;
};

#endif