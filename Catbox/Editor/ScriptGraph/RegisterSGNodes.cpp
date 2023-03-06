#include "stdafx.h"
#include "RegisterSGNodes.h"
#include "ScriptGraph/ScriptGraphSchema.h"

#include "Utility\SGNode_Print.h"
#include "Utility\SGNode_Compare.h"
#include "Utility\SGNode_GetOpposite.h"

#include "Input\SGNode_GetMousePos.h"
#include "Input\SGNode_MouseToWorld.h"
#include "Input\SGNode_GetKeyPress.h"

#include "GameObjects\SGNode_CreateGameObject.h"
#include "GameObjects\SGNode_GetPosition.h"
#include "GameObjects\SGNode_SetPosition.h"
#include "GameObjects\SGNode_SetScale.h"
#include "GameObjects\SGNode_GetScale.h"
#include "GameObjects\SGNode_OnCollisionEvents.h"
#include "GameObjects\SGNode_GetObjectIDByName.h"
//#include "GameObjects\SGNode_GetBlueprint.h"
#include "GameObjects\SGNode_GetVariables.h"
#include "GameObjects\SGNode_SetVariables.h"
#include "GameObjects/SGNode_SetActiveGameObject.h"

#include "Math\SGNode_MathOps.h"
#include "Math\SGNode_Vec3MathOps.h"
#include "Conversion/SGNode_Conversions.h"
#include "Conversion\SGNode_Splitters.h"
#include "Conversion\SGNode_Constructors.h"

#include "FlowControl\SGNode_Branch.h"

#include "FlowControl/SGNode_Delay.h"

#include "Audio/SGNode_PlayAudio.h"

void SGNodes::RegisterNodes()
{
	//events
	ScriptGraphSchema::RegisterNodeType<SGNode_OnCollisionEnter>();
	ScriptGraphSchema::RegisterNodeType<SGNode_OnCollisionStay>();
	ScriptGraphSchema::RegisterNodeType<SGNode_OnCollisionExit>();

	//conversion
	ScriptGraphSchema::RegisterNodeType<SGNode_FloatToString>();
	ScriptGraphSchema::RegisterNodeType<SGNode_Vector3fToString>();
	ScriptGraphSchema::RegisterNodeType<SGNode_Vec3fSplitter>();
	ScriptGraphSchema::RegisterNodeType<SGNode_Vec3fConstructor>();


	//utility
	ScriptGraphSchema::RegisterNodeType<SGNode_Print>();
	ScriptGraphSchema::RegisterNodeType<SGNode_Compare>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetOpposite>();

	//input
	ScriptGraphSchema::RegisterNodeType<SGNode_GetMousePos>();
	ScriptGraphSchema::RegisterNodeType<SGNode_MouseToWorld>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetKeyPress>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetKeyHeld>();

	//gameobjects
	ScriptGraphSchema::RegisterNodeType<SGNode_CreateGameObject>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetObjectIDByName>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetChildByName>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetPosition>();
	ScriptGraphSchema::RegisterNodeType<SGNode_SetPosition>();
	//ScriptGraphSchema::RegisterNodeType<SGNode_GetScale>();
	//ScriptGraphSchema::RegisterNodeType<SGNode_SetScale>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetBool>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetInt>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetFloat>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetString>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetVector3>();

	ScriptGraphSchema::RegisterNodeType<SGNode_SetBool>();
	ScriptGraphSchema::RegisterNodeType<SGNode_SetInt>();
	ScriptGraphSchema::RegisterNodeType<SGNode_SetFloat>();
	ScriptGraphSchema::RegisterNodeType<SGNode_SetString>();
	ScriptGraphSchema::RegisterNodeType<SGNode_SetVector3>();

	//ScriptGraphSchema::RegisterNodeType<SGNode_GetBlueprint>();
	ScriptGraphSchema::RegisterNodeType<SGNode_SetActiveGameObject>();
	ScriptGraphSchema::RegisterNodeType<SGNode_GetActiveGameObject>();

	//math
	ScriptGraphSchema::RegisterNodeType<SGNode_MathAdd>();
	ScriptGraphSchema::RegisterNodeType<SGNode_MathSubtract>();
	ScriptGraphSchema::RegisterNodeType <SGNode_MathMultiply>();
	ScriptGraphSchema::RegisterNodeType <SGNode_MathDivide>();
	ScriptGraphSchema::RegisterNodeType <SGNode_MathRemainder>();
	ScriptGraphSchema::RegisterNodeType <SGNode_MathAbs>();
	ScriptGraphSchema::RegisterNodeType <SGNode_MathAtan2>();
	ScriptGraphSchema::RegisterNodeType <SGNode_MathCos>();
	ScriptGraphSchema::RegisterNodeType <SGNode_MathSin>();
	ScriptGraphSchema::RegisterNodeType <SGNode_MathMax>();
	ScriptGraphSchema::RegisterNodeType <SGNode_MathMin>();
	//ScriptGraphSchema::RegisterNodeType <SGNode_FloatToString>();
	ScriptGraphSchema::RegisterNodeType <SGNode_MathDistance2D>();
	ScriptGraphSchema::RegisterNodeType <SGNode_MathLerpVec3>();
	ScriptGraphSchema::RegisterNodeType <SGNode_Vec3Add>();
	ScriptGraphSchema::RegisterNodeType <SGNode_Vec3Subtract>();
	ScriptGraphSchema::RegisterNodeType <SGNode_Vec3Multiply>();
	ScriptGraphSchema::RegisterNodeType <SGNode_Vec3Divide>();
	ScriptGraphSchema::RegisterNodeType <SGNode_Vec3ScalarMultiply>();


	ScriptGraphSchema::RegisterNodeType<SGNode_Delay>();

	//Flow control
	ScriptGraphSchema::RegisterNodeType<SGNode_Branch>();

	//audio
	ScriptGraphSchema::RegisterNodeType<SGNode_PlayAudio>();
}