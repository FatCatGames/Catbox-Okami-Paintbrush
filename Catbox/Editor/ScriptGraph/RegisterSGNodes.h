#pragma once
#include "MuninGraph.h"
#include "ScriptGraph/ScriptGraphTypes.h"
class SGNodes
{
public:
	static void RegisterNodes();


	template<typename D, typename T>
	struct ScriptGraphTypeHandler
	{
		static inline bool IsRegistered = ScriptGraphDataTypeRegistry::Register<D, T>();
	};

#define BeginDataTypeHandler(N, T, C, B) struct N##ScriptGraphType : public ScriptGraphType, public ScriptGraphTypeHandler<N##ScriptGraphType, T> { N##ScriptGraphType() : ScriptGraphType(typeid(T), sizeof(T), #N, C, B)  {  }
#define EndDataTypeHandler };


#undef BeginDataTypeHandler
#undef EndDataTypeHandler
};