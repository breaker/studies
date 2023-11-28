#include "animation.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXLine *pLine;

Animation::Animation() {
    m_time = 0.0f;

    //Create new Animation set
    D3DXCreateKeyframedAnimationSet("AnimationSet1", 500, D3DXPLAY_PINGPONG, 1, 0, NULL, &m_pAnimSet);

    //Create Keyframes
    D3DXKEY_VECTOR3 pos[3];
    pos[0].Time = 0.0f;
    pos[0].Value = D3DXVECTOR3(0.2f, 0.3f, 0.0f);
    pos[1].Time = 1000.0f;
    pos[1].Value = D3DXVECTOR3(0.8f, 0.5f, 0.0f);
    pos[2].Time = 2000.0f;
    pos[2].Value = D3DXVECTOR3(0.4f, 0.8f, 0.0f);

    D3DXKEY_VECTOR3 sca[2];
    sca[0].Time = 500.0f;
    sca[0].Value = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
    sca[1].Time = 1500.0f;
    sca[1].Value = D3DXVECTOR3(4.0f, 4.0f, 4.0f);

    //Register Keyframes
    m_pAnimSet->RegisterAnimationSRTKeys("Animation1", 2, 0, 3, sca, NULL, pos, 0);
}

void Animation::Update(float deltaTime) {
    //Update Animation time
    m_time += deltaTime;

    if (m_time > m_pAnimSet->GetPeriod()) {
        m_time -= (float) m_pAnimSet->GetPeriod();
    }
}

void Animation::Draw() {
    D3DVIEWPORT9 vp;
    g_pDevice->GetViewport(&vp);

    //Get current frame data
    D3DXVECTOR3 pos, sca;
    D3DXQUATERNION rot;
    m_pAnimSet->GetSRT(m_time, 0, &sca, &rot, &pos);

    //Set current size
    float size = sca.x * 20.0f;
    pLine->SetWidth(size);

    pLine->Begin();

    //Scale to fit screen coordinates
    pos.x *= vp.Width;
    pos.y *= vp.Height;

    D3DXVECTOR2 p[] = {D3DXVECTOR2(pos.x - size * 0.5f, pos.y),
                       D3DXVECTOR2(pos.x + size * 0.5f, pos.y)
                      };

    // 横着画的一条线
    pLine->Draw(p, 2, 0xffff0000);

    pLine->End();
}
