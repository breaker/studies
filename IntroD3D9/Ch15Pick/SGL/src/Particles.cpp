// Particles.cpp
//

#include "StdAfx.h"
#include <SGL/Particles.h>

SGL_NS_BEGIN

BOOL PSystem::Init(IDirect3DDevice9* dev, const _TCHAR* texFName) {
    // vertex buffer's size does not equal the number of particles in our system.  We
    // use the vertex buffer to draw a portion of our particles at a time.  The arbitrary
    // size we choose for the vertex buffer is specified by the _vbSize variable.

    m_D3DDev = dev; // save a ptr to the device
    IDirect3DVertexBuffer9* vb;
    HRESULT hr = dev->CreateVertexBuffer(
                     m_VertexBufSize * sizeof(Particle),
                     D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
                     Particle::FVF,
                     D3DPOOL_DEFAULT,   // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC
                     &vb,
                     NULL);
    SGL_FAILED_DO(hr, SGL_MRPT(_CRT_ERROR, "CreateVertexBuffer failed"); return FALSE);
    m_VertexBuf.Attach(vb);

    IDirect3DTexture9* tex;
    hr = D3DXCreateTextureFromFile(dev, texFName, &tex);
    SGL_FAILED_DO(hr, SGL_MRPT(_CRT_ERROR, "D3DXCreateTextureFromFile failed"); return FALSE);
    m_Texture.Attach(tex);
    return TRUE;
}

void PSystem::Reset() {
    for (std::list<Attribute>::iterator i = m_Particles.begin(); i != m_Particles.end(); ++i)
        ResetParticle(&(*i));
}

void PSystem::AddParticle() {
    Attribute attr;
    ResetParticle(&attr);
    m_Particles.push_back(attr);
}

void PSystem::PreRender() {
    m_D3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_D3DDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
    m_D3DDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
    m_D3DDev->SetRenderState(D3DRS_POINTSIZE, FtoDw(m_Size));
    m_D3DDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

    // control the size of the particle relative to distance
    m_D3DDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
    m_D3DDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
    m_D3DDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

    // use alpha from texture
    m_D3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_D3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    m_D3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_D3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void PSystem::PostRender() {
    m_D3DDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_D3DDev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
    m_D3DDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
    m_D3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void PSystem::Render() {
    // Remarks:  The render method works by filling a section of the vertex buffer with data,
    //           then we render that section.  While that section is rendering we lock a new
    //           section and begin to fill that section.  Once that sections filled we render it.
    //           This process continues until all the particles have been drawn.  The benifit
    //           of this method is that we keep the video card and the CPU busy.

    if (!m_Particles.empty()) {
        // set render states
        PreRender();

        m_D3DDev->SetTexture(0, m_Texture);
        m_D3DDev->SetFVF(Particle::FVF);
        m_D3DDev->SetStreamSource(0, m_VertexBuf, 0, sizeof(Particle));

        // render batches one by one

        // start at beginning if we're at the end of the vb
        if (m_VertexBufOffset >= m_VertexBufSize)
            m_VertexBufOffset = 0;

        Particle* v = NULL;
        m_VertexBuf->Lock(m_VertexBufOffset * sizeof(Particle), m_VertexBufBatchSize * sizeof(Particle), (void**) &v, m_VertexBufOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
        DWORD numParticlesInBatch = 0;

        // Until all particles have been rendered.
        for (std::list<Attribute>::iterator i = m_Particles.begin(); i != m_Particles.end(); ++i) {
            if (i->isAlive) {
                // Copy a batch of the living particles to the
                // next vertex buffer segment
                v->position = i->position;
                v->color = (D3DCOLOR)i->color;
                v++;    // next element;

                numParticlesInBatch++;  //increase batch counter

                // if this batch full?
                if (numParticlesInBatch == m_VertexBufBatchSize) {
                    // Draw the last batch of particles that was
                    // copied to the vertex buffer.
                    m_VertexBuf->Unlock();
                    m_D3DDev->DrawPrimitive(D3DPT_POINTLIST, m_VertexBufOffset, m_VertexBufBatchSize);

                    // While that batch is drawing, start filling the
                    // next batch with particles.

                    // move the offset to the start of the next batch
                    m_VertexBufOffset += m_VertexBufBatchSize;

                    // don't offset into memory thats outside the vb's range.
                    // If we're at the end, start at the beginning.
                    if (m_VertexBufOffset >= m_VertexBufSize)
                        m_VertexBufOffset = 0;

                    m_VertexBuf->Lock(m_VertexBufOffset * sizeof(Particle), m_VertexBufBatchSize * sizeof(Particle), (void**) &v, m_VertexBufOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
                    numParticlesInBatch = 0;    // reset for new batch
                }
            }
        }

        m_VertexBuf->Unlock();

        // its possible that the LAST batch being filled never
        // got rendered because the condition
        // (numParticlesInBatch == _vbBatchSize) would not have
        // been satisfied.  We draw the last partially filled batch now.

        if (numParticlesInBatch)
            m_D3DDev->DrawPrimitive(D3DPT_POINTLIST, m_VertexBufOffset, numParticlesInBatch);

        // next block
        m_VertexBufOffset += m_VertexBufBatchSize;

        // reset render states
        PostRender();
    }
}

BOOL PSystem::IsEmpty() {
    return m_Particles.empty();
}

BOOL PSystem::IsDead() {
    for (std::list<Attribute>::iterator i = m_Particles.begin(); i != m_Particles.end(); ++i) {
        // is there at least one living particle?  If yes,
        // the system is not dead.
        if (i->isAlive)
            return FALSE;
    }
    // no living particles found, the system must be dead.
    return TRUE;
}

void PSystem::RemoveDeadParticles() {
    std::list<Attribute>::iterator i = m_Particles.begin();
    while (i != m_Particles.end()) {
        // erase returns the next iterator, so no need to
        // incrememnt to the next one ourselves.
        if (!i->isAlive)
            i = m_Particles.erase(i);
        else
            ++i;    // next in list
    }
}

////////////////////////////////////////////////////////////////////////////////
// Snow System
////////////////////////////////////////////////////////////////////////////////

Snow::Snow(const BoundingBox& boundingBox, int numParticles) {
    m_BoundingBox           = boundingBox;
    m_Size                  = 0.25f;
    m_VertexBufSize         = 2048;
    m_VertexBufOffset       = 0;
    m_VertexBufBatchSize    = 512;
    for (int i = 0; i < numParticles; ++i)
        AddParticle();
}

void Snow::ResetParticle(__out Attribute* attr) {
    attr->isAlive  = TRUE;

    // get random x, z coordinate for the position of the snow flake.
    RandomVector(&attr->position, m_BoundingBox.min, m_BoundingBox.max);

    // no randomness for height (y-coordinate).  Snow flake
    // always starts at the top of bounding box.
    attr->position.y = m_BoundingBox.max.y;

    // snow flakes fall downwards and slightly to the left
    attr->velocity.x = RandomFloat(0.0f, 1.0f) * -3.0f;
    attr->velocity.y = RandomFloat(0.0f, 1.0f) * -10.0f;
    attr->velocity.z = 0.0f;

    // white snow flake
    attr->color = WHITE;
}

void Snow::Update(float timeDelta) {
    for (std::list<Attribute>::iterator i = m_Particles.begin(); i != m_Particles.end(); ++i) {
        i->position += i->velocity * timeDelta;

        // is the point outside bounds?
        // nope so kill it, but we want to recycle dead
        // particles, so respawn it instead.
        if (!m_BoundingBox.IsPointIn(i->position))
            ResetParticle(&(*i));
    }
}

////////////////////////////////////////////////////////////////////////////////
// Explosion System
////////////////////////////////////////////////////////////////////////////////

Firework::Firework(const D3DXVECTOR3& origin, int numParticles) {
    m_Origin                = origin;
    m_Size                  = 0.9f;
    m_VertexBufSize         = 2048;
    m_VertexBufOffset       = 0;
    m_VertexBufBatchSize    = 512;
    for (int i = 0; i < numParticles; ++i)
        AddParticle();
}

void Firework::ResetParticle(__out Attribute* attr) {
    attr->isAlive = TRUE;
    attr->position = m_Origin;

    D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
    D3DXVECTOR3 max = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

    RandomVector(&attr->velocity, min, max);

    // normalize to make spherical
    D3DXVec3Normalize(&attr->velocity, &attr->velocity);

    attr->velocity *= 100.0f;
    attr->color = D3DXCOLOR(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), 1.0f);
    attr->age = 0.0f;
    attr->lifeTime = 2.0f;  // lives for 2 seconds
}

void Firework::Update(float timeDelta) {
    for (std::list<Attribute>::iterator i = m_Particles.begin(); i != m_Particles.end(); ++i) {
        // only update living particles
        if (i->isAlive) {
            i->position += i->velocity * timeDelta;
            i->age += timeDelta;
            if (i->age > i->lifeTime)   // kill
                i->isAlive = FALSE;
        }
    }
}

void Firework::PreRender() {
    PSystem::PreRender();
    m_D3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    // read, but don't write particles to z-buffer
    m_D3DDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}

void Firework::PostRender() {
    PSystem::PostRender();
    m_D3DDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

////////////////////////////////////////////////////////////////////////////////
// Laser System
////////////////////////////////////////////////////////////////////////////////

ParticleGun::ParticleGun(Camera* camera) : m_Camera(camera) {
    m_Size                  = 0.8f;
    m_VertexBufSize         = 2048;
    m_VertexBufOffset       = 0;
    m_VertexBufBatchSize    = 512;
}

void ParticleGun::ResetParticle(__out Attribute* attr) {
    attr->isAlive  = TRUE;

    D3DXVECTOR3 cameraPos;
    m_Camera->GetPosition(&cameraPos);

    D3DXVECTOR3 cameraDir;
    m_Camera->GetLook(&cameraDir);

    // change to camera position
    attr->position = cameraPos;
    attr->position.y -= 1.0f;   // slightly below camera
    // so its like we're carrying a gun

    // travels in the direction the camera is looking
    attr->velocity = cameraDir * 100.0f;

    // green
    attr->color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
    attr->age = 0.0f;
    attr->lifeTime = 1.0f;  // lives for 1 seconds
}

void ParticleGun::Update(float timeDelta) {
    for (std::list<Attribute>::iterator i = m_Particles.begin(); i != m_Particles.end(); ++i) {
        i->position += i->velocity * timeDelta;
        i->age += timeDelta;

        if (i->age > i->lifeTime)   // kill
            i->isAlive = FALSE;
    }
    RemoveDeadParticles();
}

SGL_NS_END
