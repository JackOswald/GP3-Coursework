#ifndef _CPLAYER_H
#define _CPLAYER_H

#include "GameConstants.h"
#include "spaceGame.h"
#include "cModel.h"
#include "cInputMgr.h"

class cPlayer : public cModel
{
private:
	cInputMgr* m_InputMgr;
public:
	cPlayer();
	void attachInputMgr(cInputMgr* inputMgr);
	virtual void update(float elapsedTime);

	~cPlayer();
};
#endif