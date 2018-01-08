#include "cBullet.h"

cBullet::cBullet() : cModel()
{

}

void cBullet::update(float elapsedTime)
{
	// Move model based on speed and time
	cModel::m_mdlPosition += cModel::m_mdlDirection * cModel::m_mdlSpeed * elapsedTime;
	if (cModel::m_mdlPosition.x > PLAYFIELDX ||
		cModel::m_mdlPosition.x < -PLAYFIELDX ||
		cModel::m_mdlPosition.z > PLAYFIELDZ ||
		cModel::m_mdlPosition.z < -PLAYFIELDZ)
		cModel::m_IsActive = false;

}

cBullet::~cBullet()
{

}
