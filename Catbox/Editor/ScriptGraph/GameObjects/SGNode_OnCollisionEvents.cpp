#include "stdafx.h"
#include "SGNode_OnCollisionEvents.h"

#define InitNode(T, title) void T::Init(){SetTitle(title);SetCategory("Events");CreateExecPin("Out", PinDirection::Output, true);CreateDataPin<int>("Collided object ID", PinDirection::Output);}

InitNode(SGNode_OnCollisionEnter, "On Collision Enter")
InitNode(SGNode_OnCollisionStay, "On Collision Stay")
InitNode(SGNode_OnCollisionExit, "On Collision Exit")

InitNode(SGNode_OnOverlapBegin, "On Overlap Begin")
InitNode(SGNode_OnOverlap, "On Overlap")
InitNode(SGNode_OnOverlapEnd, "On Overlap End")
