#include "faceController.h"

extern ID3DXEffect *g_pEffect;

Viseme::Viseme() {
    m_morphTarget = 0;
    m_blendAmount = 0.0f;
    m_time = 0.0f;
}

Viseme::Viseme(int target, float amount, float time) {
    m_morphTarget = target;
    m_blendAmount = amount;
    m_time = time;
}

FaceController::FaceController(D3DXVECTOR3 pos, Face *pFace) {
    m_pFace = pFace;

    m_emotionIndex = 0;
    m_speechIndices[0] = 0;
    m_speechIndices[1] = 0;

    m_blinkTime = -(1500 + rand()%4000) / 1000.0f;
    m_emotionTime = 0.0f;
    m_emotionBlend = 0.0f;

    m_morphWeights = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

    D3DXMatrixTranslation(&m_headMatrix, pos.x, pos.y, pos.z);

    //Init Eyes
    m_eyes[0].Init(D3DXVECTOR3(pos.x - 0.037f, pos.y + 0.04f, pos.z - 0.057f));
    m_eyes[1].Init(D3DXVECTOR3(pos.x + 0.037f, pos.y + 0.04f, pos.z - 0.057f));
}

void FaceController::Update(float deltaTime) {
    if (m_pFace == NULL)return;

    UpdateSpeech(deltaTime);

    //Update Morph Weights
    float time = GetTickCount() * 0.002f;

    //Blink Eye-lids
    m_blinkTime += deltaTime * 0.8f;
    if (m_blinkTime >= 0.15f) {
        m_blinkTime = -(1500 + rand()%4000) / 1000.0f;
    }

    float blink = 0.0f;
    if (m_blinkTime > 0.0f)blink = sin(m_blinkTime * D3DX_PI * 6.667f);

    //emotions
    m_emotionTime -= deltaTime;
    m_emotionBlend += (m_emotionTime > 0.0f ? deltaTime : -deltaTime);

    //Clamp Emotion Blend
    m_emotionBlend = min(1.0f, m_emotionBlend);
    m_emotionBlend = max(0.0f, m_emotionBlend);

    //Select New Emotion
    if (m_emotionTime <= 0.0f && m_emotionBlend <= 0.0f) {
        m_emotionBlend = 0.0f;
        m_emotionTime = (2000 + rand()%6000) / 1000.0f;
        m_emotionIndex = rand() % (int)m_pFace->m_emotionMeshes.size();
    }

    //Set Morph Weights
    m_morphWeights = D3DXVECTOR4(blink,
                                 m_emotionBlend,
                                 m_morphWeights.z,      //The speech weights are updated in UpdateSpeech()
                                 m_morphWeights.w);

    //Get look at point from cursor location
    POINT p;
    GetCursorPos(&p);

    float x = (p.x - 400) / 800.0f;
    float y = (p.y - 300) / 600.0f;

    //Tell both Eyes to look at this point
    m_eyes[0].LookAt(D3DXVECTOR3(x, y, -1.0f));
    m_eyes[1].LookAt(D3DXVECTOR3(x, y, -1.0f));
}

void FaceController::Render() {
    if (m_pFace != NULL) {
        m_pFace->Render(this);

        //Render Eyes
        D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
        g_pEffect->SetTechnique(hTech);
        g_pEffect->Begin(NULL, NULL);
        g_pEffect->BeginPass(0);

        m_eyes[0].Render(g_pEffect);
        m_eyes[1].Render(g_pEffect);

        g_pEffect->EndPass();
        g_pEffect->End();
    }
}

void FaceController::Speak(vector<Viseme> &visemes) {
    //Copy visemes to the m_visemes array
    m_visemes.clear();
    for (int i=0; i<(int)visemes.size(); i++)
        m_visemes.push_back(visemes[i]);

    //Reset playback variables
    m_visemeIndex = 1;
    m_speechTime = 0.0f;
}

void FaceController::UpdateSpeech(float deltaTime) {
    m_speechTime += deltaTime;

    if (m_visemeIndex > 0 && m_visemeIndex < (int)m_visemes.size()) {
        //Get visemes to blend between
        Viseme &v1 = m_visemes[m_visemeIndex - 1];
        Viseme &v2 = m_visemes[m_visemeIndex];

        //Set speech meshes
        m_speechIndices[0] = v1.m_morphTarget % (int)m_pFace->m_speechMeshes.size();
        m_speechIndices[1] = v2.m_morphTarget % (int)m_pFace->m_speechMeshes.size();

        //Set blend amounts
        float timeBetweenVisemes = v2.m_time - v1.m_time;
        float p = (m_speechTime - v1.m_time) / timeBetweenVisemes;
        m_morphWeights.z = (1.0f - p) * v1.m_blendAmount;
        m_morphWeights.w = p * v2.m_blendAmount;

        //update index
        if (m_speechTime >= v2.m_time)
            m_visemeIndex++;
    }
    else {
        m_morphWeights.z = 0.0f;
        m_morphWeights.w = 0.0f;
    }
}