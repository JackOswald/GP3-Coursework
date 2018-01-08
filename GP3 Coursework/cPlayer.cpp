#include "cPlayer.h"

cPlayer::cPlayer() : cModel()
{

}

void cPlayer::attachInputMgr(cInputMgr* inputMgr)
{
	m_InputMgr = inputMgr;
}

void cPlayer::update(float elapsedTime)
{
	if (m_InputMgr->isKeyDown(VK_RIGHT))
	{
		rotationAngle += 25.0f;
	}
	if (m_InputMgr->isKeyDown(VK_LEFT))
	{
		rotationAngle -= 25.0f;
	}

	if (m_InputMgr->isKeyDown(VK_SPACE))
	{
		glm::vec3 mdlLaserDirection;
		mdlLaserDirection.x = -(float)glm::sin(glm::radians(this->getRotation()));
		mdlLaserDirection.y = 0.0f;
		mdlLaserDirection.z = (float)glm::cos(glm::radians(this->getRotation()));
		mdlLaserDirection *= -1;

		// Add new bullet sprite to the vector array
		bullets.push_back(new cBullet);
		int numLasers = bullets.size() - 1;
		bullets[numLasers]->setDirection(mdlLaserDirection);
		bullets[numLasers]->setRotation(0.0f);
		bullets[numLasers]->setScale(glm::vec3(1, 1, 1));
		bullets[numLasers]->setSpeed(5.0f);
		bullets[numLasers]->setPosition(this->getPosition() + mdlLaserDirection);
		bullets[numLasers]->setIsActive(true);
		bullets[numLasers]->update(elapsedTime);
		// play the firing sound
		m_SoundMgr->getSnd("Shot")->playAudio(AL_TRUE);
	}

	/*
	==============================================================
	| Check for collisions
	==============================================================
	*/
	for (vector<cBullet*>::iterator bulletIterartor = bullets.begin(); bulletIterartor != bullets.end(); ++bulletIterartor)
	{
		(*bulletIterartor)->update(elapsedTime);
		for (vector<cEnemy*>::iterator enemyIterator = enemies.begin(); enemyIterator != enemies.end(); ++enemyIterator)
		{
			if ((*enemyIterator)->SphereSphereCollision((*bulletIterartor)->getPosition(), (*bulletIterartor)->getMdlRadius()))
			{
				// if a collision set the bullet and spaceship to false
				(*enemyIterator)->setIsActive(false);
				(*bulletIterartor)->setIsActive(false);
				// play the explosion sound.
				m_SoundMgr->getSnd("Explosion")->playAudio(AL_TRUE);
			}
		}
	}

	for (vector<cBullet*>::iterator bulletIterartor = bullets.begin(); bulletIterartor != bullets.end(); ++bulletIterartor)
	{
		(*bulletIterartor)->update(elapsedTime);
		for (vector<cEnemy*>::iterator bossIterator = bossEnemies.begin(); bossIterator != bossEnemies.end(); ++bossIterator)
		{
			if ((*bossIterator)->SphereSphereCollision((*bulletIterartor)->getPosition(), (*bulletIterartor)->getMdlRadius()))
			{
				// if a collision set the bullet and spaceship to false
				(*bossIterator)->setIsActive(false);
				(*bulletIterartor)->setIsActive(false);
				// play the explosion sound.
				m_SoundMgr->getSnd("Explosion")->playAudio(AL_TRUE);
			}
		}
	}

	vector<cBullet*>::iterator bulletIterartor = bullets.begin();
	while (bulletIterartor != bullets.end())
	{
		if ((*bulletIterartor)->isActive() == false)
		{
			bulletIterartor = bullets.erase(bulletIterartor);
		}
		else
		{
			++bulletIterartor;
		}
	}

	vector<cEnemy*>::iterator enemyIterartor = enemies.begin();
	while (enemyIterartor != enemies.end())
	{
		if ((*enemyIterartor)->isActive() == false)
		{
			enemyIterartor = enemies.erase(enemyIterartor);
		}
		else
		{
			++enemyIterartor;
		}
	}


	vector<cEnemy*>::iterator bossIterartor = bossEnemies.begin();
	while (bossIterartor != bossEnemies.end())
	{
		if ((*bossIterartor)->isActive() == false)
		{
			bossIterartor = bossEnemies.erase(bossIterartor);
		}
		else
		{
			++bossIterartor;
		}
	}

	// Find out what direction we should be thrusting, using rotation.
	glm::vec3 mdlVelocityAdd;
	mdlVelocityAdd.x = -(float)glm::sin(glm::radians(m_mdlRotation));  // Remember to adjust for radians
	mdlVelocityAdd.y = 0.0f;
	mdlVelocityAdd.z = -(float)glm::cos(glm::radians(m_mdlRotation));

	m_mdlRotation -= rotationAngle;

	mdlVelocityAdd *= translationZ;
	m_mdlDirection += mdlVelocityAdd;

	m_mdlPosition += m_mdlDirection * m_mdlSpeed *elapsedTime;
	m_mdlDirection *= 0.95f;

	rotationAngle = 0;
	translationZ = 0;
}

cPlayer::~cPlayer()
{

}