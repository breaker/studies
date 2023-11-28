// Particles.h
//

#pragma once

#include <SGL/Depends.h>
#include <SGL/Defs.h>

#include <SGL/Utils.h>
#include <SGL/Geometry.h>
#include <SGL/Camera.h>
#include <SGL/Render.h>

SGL_NS_BEGIN

struct Particle {
    D3DXVECTOR3 position;
    D3DCOLOR    color;
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

struct Attribute {
    Attribute() {
        lifeTime = 0.0f;
        age = 0.0f;
        isAlive = TRUE;
    }

    D3DXVECTOR3 position;
    D3DXVECTOR3 velocity;
    D3DXVECTOR3 acceleration;
    float       lifeTime;   // how long the particle lives for before dying
    float       age;        // current age of the particle
    D3DXCOLOR   color;      // current color of the particle
    D3DXCOLOR   colorFade;  // how the color fades with respect to time
    BOOL        isAlive;
};

class SGL_API PSystem : private Uncopyable {
public:
    PSystem() : m_D3DDev(NULL) {}
    virtual ~PSystem() {}

    virtual BOOL Init(IDirect3DDevice9* dev, const _TCHAR* texFName);
    virtual void Reset();

    // sometimes we don't want to free the memory of a dead particle,
    // but rather respawn it instead.
    virtual void ResetParticle(__out Attribute* attr) = 0;
    virtual void AddParticle();

    virtual void Update(float timeDelta) = 0;

    virtual void PreRender();
    virtual void Render();
    virtual void PostRender();

    BOOL IsEmpty();
    BOOL IsDead();

protected:
    virtual void RemoveDeadParticles();

protected:
    IDirect3DDevice9*       m_D3DDev;
    D3DXVECTOR3             m_Origin;
    BoundingBox             m_BoundingBox;
    float                   m_EmitRate;     // rate new particles are added to system
    float                   m_Size;         // size of particles
    std::list<Attribute>    m_Particles;
    int                     m_MaxParticles; // max allowed particles system can have
    CComPtr<IDirect3DTexture9>      m_Texture;
    CComPtr<IDirect3DVertexBuffer9> m_VertexBuf;

    // Following three data elements used for rendering the p-system efficiently
    DWORD m_VertexBufSize;      // size of vb
    DWORD m_VertexBufOffset;    // offset in vb to lock
    DWORD m_VertexBufBatchSize; // number of vertices to lock starting at _vbOffset
};

class SGL_API Snow : public PSystem {
public:
    Snow(const BoundingBox& boundingBox, int numParticles);
    void ResetParticle(__out Attribute* attr);
    void Update(float timeDelta);
};

class SGL_API Firework : public PSystem {
public:
    Firework(const D3DXVECTOR3& origin, int numParticles);
    void ResetParticle(__out Attribute* attr);
    void Update(float timeDelta);
    void PreRender();
    void PostRender();
};

class SGL_API ParticleGun : public PSystem {
public:
    ParticleGun(Camera* camera);
    void ResetParticle(__out Attribute* attr);
    void Update(float timeDelta);
private:
    Camera* m_Camera;
};

SGL_NS_END
