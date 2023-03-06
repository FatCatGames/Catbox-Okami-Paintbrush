#pragma once


enum class DataType
{
	Bool,
	Float,
	Int,
	String,

	//When adding new types do it BEFORE this line and DO NOT change their order
	Variable = 253,
	Exec = 254,
	Unknown = 255,
};