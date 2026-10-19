// ----------------------------------------------------------------------- //
//
// MODULE  : AIGoalSniper.cpp
//
// PURPOSE : AIGoalSniper implementation
//
// CREATED : 4/10/02
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "AIGoalSniper.h"
#include "AIGoalMgr.h"
#include "AIGoalButeMgr.h"
#include "AI.h"
#include "AINodeMgr.h"
#include "AIHumanStateSniper.h"
#include "AITarget.h"
#include "AIVolume.h"
#include "CharacterMgr.h"
#include "PlayerObj.h"

DEFINE_AI_FACTORY_CLASS_SPECIFIC(Goal, CAIGoalSniper, kGoal_Sniper);

#define VIEW_NODE_UPDATE_RATE	30.f

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalSniper::Con/destructor
//
//	PURPOSE:	Factory Con/destructor
//
// ----------------------------------------------------------------------- //

CAIGoalSniper::CAIGoalSniper()
{
	m_bRequireBareHands = LTFALSE;
	m_bAllowDialogue = LTFALSE;
	m_bTurnOffLights = LTFALSE;
	m_bTurnOnLights = LTFALSE;

	m_fNextViewNodeUpdate = 0.f;
	m_pLastTargetInfoVol = LTNULL;

	m_bSeekPlayer = LTFALSE;
	m_bNoRadius = LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalSniper::Save / Load
//
//	PURPOSE:	Save / Load
//
// ----------------------------------------------------------------------- //

void CAIGoalSniper::Save(ILTMessage_Write *pMsg)
{
	super::Save(pMsg);

	SAVE_TIME( m_fNextViewNodeUpdate );
	SAVE_COBJECT( m_pLastTargetInfoVol );
	SAVE_HOBJECT( m_hSeekTarget );
	SAVE_BOOL( m_bSeekPlayer );
	SAVE_BOOL( m_bNoRadius );
}

void CAIGoalSniper::Load(ILTMessage_Read *pMsg)
{
	super::Load(pMsg);

	LOAD_TIME( m_fNextViewNodeUpdate );
	LOAD_COBJECT( m_pLastTargetInfoVol, AIVolume );
	LOAD_HOBJECT( m_hSeekTarget );
	LOAD_BOOL( m_bSeekPlayer );
	LOAD_BOOL( m_bNoRadius );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalSniper::ActivateGoal
//
//	PURPOSE:	Activate goal.
//
// ----------------------------------------------------------------------- //

void CAIGoalSniper::ActivateGoal()
{
	super::ActivateGoal();

	// Ignore all senses other than SeeEnemy.

	m_pAI->SetCurSenseFlags( kSense_SeeEnemy | kSense_SeeEnemyLean );

	m_pAI->SetAwareness( kAware_Alert );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalSniper::GetUseObjectState
//
//	PURPOSE:	Get UseObject-derived state to set.
//
// ----------------------------------------------------------------------- //

EnumAIStateType CAIGoalSniper::GetUseObjectState()
{
	return kState_HumanSniper; 
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalSniper::UpdateGoal
//
//	PURPOSE:	Update goal.
//
// ----------------------------------------------------------------------- //

void CAIGoalSniper::UpdateGoal()
{
	// Intentionally do not call super.

	CAIState* pState = m_pAI->GetState();

	switch(pState->GetStateType())
	{
		case kState_HumanSniper:
			HandleStateSniper();
			break;

		// Unexpected State.
		default: AIASSERT( 0, m_pAI->m_hObject, "CAIGoalSniper::UpdateGoal: Unexpected State.");
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalSniper::HandleStateSniper
//
//	PURPOSE:	Determine what to do when in state Sniper.
//
// ----------------------------------------------------------------------- //

void CAIGoalSniper::HandleStateSniper()
{
	switch( m_pAI->GetState()->GetStateStatus() )
	{
		case kSStat_Initialized:
			break;

		case kSStat_Moving:
			break;

		case kSStat_Attacking:

			// Look for targets.

			if( !m_pAI->HasTarget() )
			{
				CAISenseRecorderAbstract* pSenseRecorder = m_pAI->GetSenseRecorder();
				if( pSenseRecorder && pSenseRecorder->HasAnyStimulation( kSense_SeeEnemy ) )
				{
					m_pAI->Target( pSenseRecorder->GetStimulusSource( kSense_SeeEnemy ) );
				}
			}

			HandleTargetPos();
			break;

		case kSStat_StateComplete:
			CompleteUseObject();
			break;

		// Unexpected StateStatus.
		default: AIASSERT( 0, m_pAI->m_hObject, "CAIGoalSniper::HandleStateSniper: Unexpected State Status.");
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalSniper::HandleTargetPos
//
//	PURPOSE:	React to target's position.
//
// ----------------------------------------------------------------------- //

AINode* CAIGoalSniper::HandleTargetPos()
{
	// RETAIL: Object 0x10035370: a sniper told to seek the player looks
	// for him before he is a target.

	if( !m_pAI->HasTarget() && !m_hSeekTarget )
	{
		return LTNULL;
	}

	// Do not change view nodes repeatedly every time player moves.
	// Wait a little while, or AI will run back and forth without attacking.
	// Ignore delay and change position if can't see target.

	LTFLOAT fCurTime = g_pLTServer->GetTime();
	if( m_pAI->HasTarget() && m_pAI->GetTarget()->IsVisiblePartially() && ( fCurTime < m_fNextViewNodeUpdate ) )
	{
		return LTNULL;
	}

	HOBJECT hTarget = m_pAI->HasTarget() ? m_pAI->GetTarget()->GetObject() : (HOBJECT)m_hSeekTarget;
	CCharacter* pCharacter = (CCharacter*)g_pLTServer->HandleToObject( hTarget );
	if( !pCharacter )
	{
		return LTNULL;
	}

	// Find target's current PlayerInfoVolume volume.

	AIInformationVolume *pInfoVolume = pCharacter->GetCurrentInformationVolume();
	if( ( !pInfoVolume ) ||
		( pInfoVolume->GetVolumeType() != AIInformationVolume::eTypePlayerInfo ) ||
		( (AIVolume*)pInfoVolume == m_pLastTargetInfoVol ) )
	{
		return LTNULL;
	}

	// Record changes in info volumes.

	m_pLastTargetInfoVol = (AIVolume*)pInfoVolume;

	// Find a guard node owned by this AI.

	AINode* pGuardNode = g_pAINodeMgr->FindOwnedNode( kNode_Guard, m_pAI->m_hObject );


	// Find sniper attractor node corresponding to this playerinfo volume.

	AIVolumePlayerInfo* pPlayerInfoVolume = (AIVolumePlayerInfo*)pInfoVolume;
	AIGBM_GoalTemplate* pTemplate = g_pAIGoalButeMgr->GetTemplate( GetGoalType() );

	AINode* pSniperNode = LTNULL;
	for(uint32 iAttractor=0; iAttractor < pTemplate->cAttractors; ++iAttractor)
	{
		// Find sniper nodes owned by the guard node, or owned by no one.

		if( pGuardNode )
		{
			pSniperNode = pPlayerInfoVolume->FindOwnedViewNode( m_pAI, pTemplate->aAttractors[iAttractor], pGuardNode->m_hObject, m_hSeekTarget );
		}
		else {
			pSniperNode = pPlayerInfoVolume->FindViewNode( m_pAI, pTemplate->aAttractors[iAttractor], m_pAI->GetPosition(), LTTRUE, m_hSeekTarget );
		}

		if( pSniperNode )
		{
			break;
		}
	}

	// Set new sniper node.

	if( pSniperNode && ( pSniperNode->GetType() == kNode_UseObject ) )
	{
		SetSniperNode( (AINodeUseObject*)pSniperNode );
		m_fNextViewNodeUpdate = fCurTime + VIEW_NODE_UPDATE_RATE;
	}

	return pSniperNode;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalSniper::FindNearestAttractorNode
//
//	PURPOSE:	Find attractor node, considering guarded nodes.
//
// ----------------------------------------------------------------------- //

AINode* CAIGoalSniper::FindNearestAttractorNode()
{
	// RETAIL: Object 0x10035550: remember who to look for.

	if( m_pAI->HasTarget() && ( m_hSeekTarget != m_pAI->GetTarget()->GetObject() ))
	{
		m_hSeekTarget = m_pAI->GetTarget()->GetObject();
	}

	AINode* pNode;

	// Find a guard node owned by this AI.

	AINode* pGuardNode = g_pAINodeMgr->FindOwnedNode( kNode_Guard, m_pAI->m_hObject );
	if( pGuardNode )
	{
		// Only find attractor nodes owned by the owner node.

		pNode = FindGoalAttractors( LTTRUE, pGuardNode->m_hObject );
	}
	else
	{
		// Find an unowned node.

		pNode = FindGoalAttractors( LTFALSE, LTNULL );
	}

	// Without regard to the nodes' radius, any view of the target will do.

	if( !pNode && m_bNoRadius )
	{
		pNode = HandleTargetPos();
	}

	return pNode;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalSniper::SetSniperNode
//
//	PURPOSE:	Set new sniper node.
//
// ----------------------------------------------------------------------- //

void CAIGoalSniper::SetSniperNode(AINodeUseObject* pNewNode)
{
	if( m_pAI->GetState()->GetStateType() == kState_HumanSniper )
	{
		// Unlock old node.

		AINodeUseObject* pNodeUseObject = (AINodeUseObject*)g_pLTServer->HandleToObject(m_hNodeUseObject);
		AIASSERT( pNodeUseObject && pNodeUseObject->IsLocked(), m_pAI->m_hObject, "CAIGoalSniper::DeactivateGoal: Node is NULL or not locked" );
		if( pNodeUseObject )
		{
			pNodeUseObject->Unlock( m_pAI->m_hObject );
		}

		// Set and lock new node.
	
		if( pNewNode )
		{
			CAIHumanStateSniper* pStateSniper = (CAIHumanStateSniper*)(m_pAI->GetState());
			pStateSniper->SetNode( pNewNode );
			pNewNode->Lock( m_pAI->m_hObject );
		}
	}

	m_hNodeUseObject = pNewNode ? pNewNode->m_hObject : LTNULL;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAIGoalSniper::HandleNameValuePair
//
//	PURPOSE:	Handles getting a name/value pair.
//
// ----------------------------------------------------------------------- //

LTBOOL CAIGoalSniper::HandleNameValuePair(const char *szName, const char *szValue)
{
	ASSERT(szName && szValue);

	if ( !_stricmp(szName, "NODE") )
	{
		AINode* pNewNode = g_pAINodeMgr->GetNode(szValue);		
		if( pNewNode && ( pNewNode->GetType() == kNode_UseObject ) )
		{
			AINodeUseObject* pNodeUseObject = (AINodeUseObject*)pNewNode;

			// Ensure that node is a sniper node.

			if( !pNodeUseObject->GetSmartObjectCommand( kNode_Sniper ) )
			{
				AIASSERT1( 0, m_pAI->m_hObject, "CAIGoalSniper::HandleNameValuePair: Setting a node that is not a Sniper node: %s", szValue );
				return LTTRUE;
			}

			// If Goal was already active, reset the state's node.

			if( m_pGoalMgr->IsCurGoal( this ) )
			{
				SetSniperNode( (AINodeUseObject*)pNewNode );
			}

			// Set the node and activate the goal.

			m_hNodeUseObject = pNewNode->m_hObject;
			SetCurToBaseImportance();
		}
		else {
			AIASSERT1( 0, m_pAI->m_hObject, "CAIGoalSniper::HandleNameValuePair: Setting a sniper node that is not an AINodeUseObject: %s", szValue );
		}

		return LTTRUE;
	}

	// RETAIL: Object 0x10035185

	if ( !_stricmp(szName, "SEEKPLAYER") )
	{
		if( IsTrueChar( szValue[0] ) )
		{
			m_bSeekPlayer = LTTRUE;
			AITRACE( AIShowGoals, ( m_pAI->m_hObject, "CAIGoalSniper: SEEKPLAYER=1" ) );

			CPlayerObj *pPlayer = g_pCharacterMgr->FindPlayer();
			if ( pPlayer )
			{
				m_hSeekTarget = pPlayer->m_hObject;
			}
		}

		return LTTRUE;
	}

	if ( !_stricmp(szName, "NORADIUS") )
	{
		if( IsTrueChar( szValue[0] ) )
		{
			m_bNoRadius = LTTRUE;
			AITRACE( AIShowGoals, ( m_pAI->m_hObject, "CAIGoalSniper: NORADIUS=1" ) );
		}

		return LTTRUE;
	}

	return LTFALSE;
}
