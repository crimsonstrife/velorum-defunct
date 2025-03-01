// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	C++ class header boilerplate exported from UnrealHeaderTool.
	This is automatically generated by the tools.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "ObjectBase.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UBluJsonObj;
class UObject;
class UBluEye;
#ifdef BLU_BluBlueprintFunctionLibrary_generated_h
#error "BluBlueprintFunctionLibrary.generated.h already included, missing '#pragma once' in BluBlueprintFunctionLibrary.h"
#endif
#define BLU_BluBlueprintFunctionLibrary_generated_h

#define projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execJSONToString) \
	{ \
		P_GET_OBJECT(UBluJsonObj,Z_Param_ObjectToParse); \
		P_FINISH; \
		*(FString*)Z_Param__Result=UBluBlueprintFunctionLibrary::JSONToString(Z_Param_ObjectToParse); \
	} \
 \
	DECLARE_FUNCTION(execParseJSON) \
	{ \
		P_GET_PROPERTY(UStrProperty,Z_Param_JSONString); \
		P_FINISH; \
		*(UBluJsonObj**)Z_Param__Result=UBluBlueprintFunctionLibrary::ParseJSON(Z_Param_JSONString); \
	} \
 \
	DECLARE_FUNCTION(execRunBluEventLoop) \
	{ \
		P_FINISH; \
		UBluBlueprintFunctionLibrary::RunBluEventLoop(); \
	} \
 \
	DECLARE_FUNCTION(execNewBluJSONObj) \
	{ \
		P_GET_OBJECT(UObject,Z_Param_WorldContextObject); \
		P_FINISH; \
		*(UBluJsonObj**)Z_Param__Result=UBluBlueprintFunctionLibrary::NewBluJSONObj(Z_Param_WorldContextObject); \
	} \
 \
	DECLARE_FUNCTION(execNewBluEye) \
	{ \
		P_GET_OBJECT(UObject,Z_Param_WorldContextObject); \
		P_FINISH; \
		*(UBluEye**)Z_Param__Result=UBluBlueprintFunctionLibrary::NewBluEye(Z_Param_WorldContextObject); \
	}


#define projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execJSONToString) \
	{ \
		P_GET_OBJECT(UBluJsonObj,Z_Param_ObjectToParse); \
		P_FINISH; \
		*(FString*)Z_Param__Result=UBluBlueprintFunctionLibrary::JSONToString(Z_Param_ObjectToParse); \
	} \
 \
	DECLARE_FUNCTION(execParseJSON) \
	{ \
		P_GET_PROPERTY(UStrProperty,Z_Param_JSONString); \
		P_FINISH; \
		*(UBluJsonObj**)Z_Param__Result=UBluBlueprintFunctionLibrary::ParseJSON(Z_Param_JSONString); \
	} \
 \
	DECLARE_FUNCTION(execRunBluEventLoop) \
	{ \
		P_FINISH; \
		UBluBlueprintFunctionLibrary::RunBluEventLoop(); \
	} \
 \
	DECLARE_FUNCTION(execNewBluJSONObj) \
	{ \
		P_GET_OBJECT(UObject,Z_Param_WorldContextObject); \
		P_FINISH; \
		*(UBluJsonObj**)Z_Param__Result=UBluBlueprintFunctionLibrary::NewBluJSONObj(Z_Param_WorldContextObject); \
	} \
 \
	DECLARE_FUNCTION(execNewBluEye) \
	{ \
		P_GET_OBJECT(UObject,Z_Param_WorldContextObject); \
		P_FINISH; \
		*(UBluEye**)Z_Param__Result=UBluBlueprintFunctionLibrary::NewBluEye(Z_Param_WorldContextObject); \
	}


#define projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_INCLASS_NO_PURE_DECLS \
	private: \
	static void StaticRegisterNativesUBluBlueprintFunctionLibrary(); \
	friend BLU_API class UClass* Z_Construct_UClass_UBluBlueprintFunctionLibrary(); \
	public: \
	DECLARE_CLASS(UBluBlueprintFunctionLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/Blu"), NO_API) \
	DECLARE_SERIALIZER(UBluBlueprintFunctionLibrary) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_INCLASS \
	private: \
	static void StaticRegisterNativesUBluBlueprintFunctionLibrary(); \
	friend BLU_API class UClass* Z_Construct_UClass_UBluBlueprintFunctionLibrary(); \
	public: \
	DECLARE_CLASS(UBluBlueprintFunctionLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/Blu"), NO_API) \
	DECLARE_SERIALIZER(UBluBlueprintFunctionLibrary) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UBluBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UBluBlueprintFunctionLibrary) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UBluBlueprintFunctionLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UBluBlueprintFunctionLibrary); \
private: \
	/** Private copy-constructor, should never be used */ \
	NO_API UBluBlueprintFunctionLibrary(const UBluBlueprintFunctionLibrary& InCopy); \
public:


#define projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UBluBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private copy-constructor, should never be used */ \
	NO_API UBluBlueprintFunctionLibrary(const UBluBlueprintFunctionLibrary& InCopy); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UBluBlueprintFunctionLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UBluBlueprintFunctionLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UBluBlueprintFunctionLibrary)


#define projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_5_PROLOG
#define projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_RPC_WRAPPERS \
	projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_INCLASS \
	projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_RPC_WRAPPERS_NO_PURE_DECLS \
	projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_INCLASS_NO_PURE_DECLS \
	projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h_9_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class BluBlueprintFunctionLibrary."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID projectvelorum_Plugins_BLUI_Source_Blu_Public_BluBlueprintFunctionLibrary_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
