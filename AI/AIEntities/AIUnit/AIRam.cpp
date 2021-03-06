#include "AIRam.h"

AIRam::AIRam()
{

}

AIRam::AIRam(bool bIsAttacking, int type, int race, Ogre::Vector2 position, Ogre::Vector2 orientation)
{

    m_bIsAttacking = bIsAttacking;
    m_iType = type;
    m_iRace = race;
    m_vPos = position;
    m_vPrevPos = m_vPos;
    m_vOri = orientation.normalisedCopy();
    m_iVision = 100;
    m_dTotalHealth = 2000;
    m_dActualHealth = 2000;
    m_iAttackDistance = 20;
    m_MaxForce = 0.05;
    m_MaxSpeed = 0.5;
    m_dMass = 3.0;
    m_iCollisionType = circle1;
    m_iAnimation = WALK;
    setAttack(2);

    MessageSystem::getSingletonPtr()->DispatchMessageToGraphFactory(this->ID(), factoryGraph, SMSG_AICreateGraphUnit, position, m_vOri, type, race, Vector2(this->getActualHealth(), this->getTotalHealth()));

    m_pStateMachine = new AIStateSystem<AIRam>(this);
    m_pStateMachine->SetCurrentState(SeekToFlagRam::Instance());
}

void AIRam::Update(double timeElapsed)
{
    m_dTimeSinceLastFrame = timeElapsed;

    m_vSteeringForce.ZERO;
    m_pStateMachine->Update();

    translateEntity(timeElapsed);
    separateEntity(timeElapsed);

    MessageSystem::getSingletonPtr()->DispatchMessageGraphUnit(this->ID(), this->ID(), SMSG_AISystemToGraphSystem, this->getPosition(), 
        this->getOrientation(), timeElapsed, this->m_iType, race(), getAnimation(), this->getActualHealth());
}

bool AIRam::HandleMessage(const Telegram& msg)
{
  return m_pStateMachine->HandleMessage(msg);
}

Ogre::Vector2 AIRam::seekFlag()
{
    m_iAnimation = WALK;
    if(this->IsAttacking())
    {
        m_vSteeringForce = getSteering()->Seek(EntityCityManager::getSingletonPtr()->GetType(BARRACK)[0]->getPosition());
        this->setObjetive((AIUnit*)EntityCityManager::getSingletonPtr()->GetType(BARRACK)[0]);
    }else{
        m_vSteeringForce = getSteering()->Seek(EntityCityManager::getSingletonPtr()->GetType(HALL)[0]->getPosition());
        this->setObjetive((AIUnit*)EntityCityManager::getSingletonPtr()->GetType(HALL)[0]);
    }

    return m_vSteeringForce;
}

Ogre::Vector2 AIRam::charge()
{
    m_dMass = 0.1f;
    m_MaxSpeed = 1.0;
    m_vSteeringForce = getSteering()->Seek(m_pObjetive->getPosition());
    m_iAnimation = CHARGE;
    return m_vSteeringForce;
}
