#include "boid.h"
#include "flock.h"

extern IDirect3DDevice9 *g_pDevice;
extern ID3DXEffect* g_pEffect;
extern D3DXVECTOR4 g_lightPos;

Mesh* Boid::sm_pBoidMesh = NULL;
D3DXMATRIX g_shadowMatrix;


Boid::Boid(Flock *pFlock) {
    if (sm_pBoidMesh == NULL) {
        //Load boid mesh
        sm_pBoidMesh = new Mesh("resources/boid.x");

        //Set ground plane + light position
        D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);

        //Create the shadow matrix
        D3DXMatrixShadow(&g_shadowMatrix, &g_lightPos, &ground);
    }

    m_pFlock = pFlock;

    m_position = D3DXVECTOR3(((rand()%1000) / 1000.0f) * 20.0f - 10.0f,
                             ((rand()%1000) / 1000.0f) * 20.0f,
                             ((rand()%1000) / 1000.0f) * 20.0f - 10.0f);

    D3DXVec3Normalize(&m_velocity, &m_position);
}

Boid::~Boid() {

}

void Boid::Update(float deltaTime) {
    //Tweakable values
    const float NEIGHBORHOOD_SIZE = 2.5f;
    const float SEPARATION_LIMIT = 2.5f;
    const float SEPARATION_FORCE = 15.0f;
    const float BOID_SPEED = 3.0f;

    //Get Neighboring boids
    vector<Boid*> neighbors;
    m_pFlock->GetNeighbors(this, NEIGHBORHOOD_SIZE, neighbors);

    //Forces
    D3DXVECTOR3 acceleration(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 separationForce(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 alignmentForce(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 cohesionForce(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 toPointForce(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 floorForce(0.0f, 0.0f, 0.0f);

    if (!neighbors.empty()) {
        //Calculate neighborhood center
        D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
        for (int i=0; i<(int)neighbors.size(); i++) {
            center += neighbors[i]->m_position;
        }
        center /= (float)neighbors.size();


        //RULE 1: Separation
        for (int i=0; i<(int)neighbors.size(); i++) {
            D3DXVECTOR3 vToNeighbor = neighbors[i]->m_position - m_position;
            float distToNeightbor = D3DXVec3Length(&vToNeighbor);

            if (distToNeightbor < SEPARATION_LIMIT) {
                //Too close to neighbor
                float force = 1.0f - (distToNeightbor / SEPARATION_LIMIT);
                separationForce -= vToNeighbor * SEPARATION_FORCE * force;
            }
        }


        //RULE 2: Alignment
        for (int i=0; i<(int)neighbors.size(); i++) {
            alignmentForce += neighbors[i]->m_velocity;
        }
        alignmentForce /= (float)neighbors.size();


        //RULE 3: Cohesion
        float distToCenter = D3DXVec3Length(&(center - m_position)) + 0.01f;
        cohesionForce = (center - m_position) / distToCenter;
    }

    //RULE 4: Steer to point
    toPointForce = D3DXVECTOR3(0.0f, 15.0f, 0.0f) - m_position;
    D3DXVec3Normalize(&toPointForce, &toPointForce);
    toPointForce *= 0.5f;


    //RULE 5: Dont crash!
    if (m_position.y < 3.0f)
        floorForce.y += (3.0f - m_position.y) * 100.0f;


    //Sum up forces
    acceleration = separationForce +
                   alignmentForce +
                   cohesionForce +
                   toPointForce +
                   floorForce;

    //Update velocity & position
    D3DXVec3Normalize(&acceleration, &acceleration);
    m_velocity += acceleration * deltaTime * 3.0f;
    D3DXVec3Normalize(&m_velocity, &m_velocity);
    m_position += m_velocity * BOID_SPEED * deltaTime;

    //Cap Y position
    m_position.y = max(m_position.y, 1.0f);
}

void Boid::Render(bool shadow) {
    D3DXMATRIX world, pos, rot;

    //Position
    D3DXMatrixTranslation(&pos, m_position.x, m_position.y, m_position.z);

    //Orientation
    D3DXMATRIX rotY, rotXZ;
    float angleY = atan2(m_velocity.x, m_velocity.z);
    float angleXZ = atan2(m_velocity.y, cos(angleY) * m_velocity.z);

    D3DXVECTOR3 right;
    D3DXVec3Cross(&right, &m_velocity, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

    D3DXMatrixRotationY(&rotY, angleY);
    D3DXMatrixRotationAxis(&rotXZ, &right, angleXZ);

    rot = rotY * rotXZ;


    /*  D3DXVECTOR3 right, up;
        D3DXVec3Cross(&right, &m_velocity, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
        D3DXVec3Cross(&up, &m_velocity, &right);
        D3DXMatrixLookAtLH(&rot, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &m_velocity, &up);
        D3DXVECTOR3 tmpSca, tmpPos;
        D3DXQUATERNION tmpRot;
        D3DXMatrixDecompose(&tmpSca, &tmpRot, &tmpPos, &rot);
        D3DXQuaternionNormalize(&tmpRot, &tmpRot);
        D3DXMatrixRotationQuaternion(&rot, &tmpRot);
    */
    world = rot * pos;

    if (shadow) {
        world *= g_shadowMatrix;
    }

    //Render boid
    g_pEffect->SetMatrix("matW", &world);
    g_pEffect->CommitChanges();
    sm_pBoidMesh->Render();
}