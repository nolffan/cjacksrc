// ----------------------------------------------------------------------- //
//
// MODULE  : AIGoalAttackMelee.cpp
//
// PURPOSE : AIGoalAttackMelee implementation
//
// CREATED : 10/09/01
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "AIGoalAttackMelee.h"
#include "AIGoalMgr.h"
#include "AnimatorPlayer.h"
#include "AI.h"
#include "AIVolumeMgr.h"
#include "AIVolume.h"
#include "AISenseRecorderAbstract.h"

DEFINE_AI_FACTORY_CLASS_SPECIFIC(Goal, CAIGoalAttackMelee, kGoal_AttackMelee);


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalAttackMelee::Con/destructor
//
//	PURPOSE:	Factory Con/destructor
//
// ----------------------------------------------------------------------- //

CAIGoalAttackMelee::CAIGoalAttackMelee()
{
	m_eWeaponType = kAIWeap_Melee;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalAttackMelee::Save / Load
//
//	PURPOSE:	Save / Load
//
// ----------------------------------------------------------------------- //

void CAIGoalAttackMelee::Save(ILTMessage_Write *pMsg)
{
	super::Save(pMsg);
}

void CAIGoalAttackMelee::Load(ILTMessage_Read *pMsg)
{
	super::Load(pMsg);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalAttackMelee::ActivateGoal
//
//	PURPOSE:	Activate goal.
//
// ----------------------------------------------------------------------- //

void CAIGoalAttackMelee::ActivateGoal()
{
	super::ActivateGoal();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalAttackMelee::HandleGoalSenseTrigger
//
//	PURPOSE:	RETAIL: Object 0x1001eef0.  Only go for a melee attack when
//				there is a straight walk to the stimulus.
//
// ----------------------------------------------------------------------- //

LTBOOL CAIGoalAttackMelee::HandleGoalSenseTrigger(AISenseRecord* pSenseRecord)
{
	uint32 dwExcludeVolumes =	AIVolume::kVolumeType_Ladder |
								AIVolume::kVolumeType_Stairs |
								AIVolume::kVolumeType_Ledge |
								AIVolume::kVolumeType_JumpUp |
								AIVolume::kVolumeType_JumpOver |
								AIVolume::kVolumeType_Teleport |
								AIVolume::kVolumeType_AmbientLife;

	// RETAIL: Object 0x1001eef9: one condition, true or false.
	if( super::HandleGoalSenseTrigger( pSenseRecord ) &&
		( m_pGoalMgr->IsCurGoal( this ) ||
		  g_pAIVolumeMgr->StraightPathExists( m_pAI, m_pAI->GetPosition(), pSenseRecord->vLastStimulusPos, m_pAI->GetVerticalThreshold() * 2.f, dwExcludeVolumes, m_pAI->GetLastVolume() ) ) )
	{
		return LTTRUE;
	}

	return LTFALSE;
}
