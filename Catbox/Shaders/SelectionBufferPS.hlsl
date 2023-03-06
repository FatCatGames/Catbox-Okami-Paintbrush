#include "Struct/ShaderStructs.hlsli"
#include "PBRFunctions.hlsli"

struct ObjectIDOutput
{
	int id : SV_TARGET0;
};

ObjectIDOutput main(VertexToPixel input)
{
	ObjectIDOutput result;
	result.id = OB_id;
	return result;
}