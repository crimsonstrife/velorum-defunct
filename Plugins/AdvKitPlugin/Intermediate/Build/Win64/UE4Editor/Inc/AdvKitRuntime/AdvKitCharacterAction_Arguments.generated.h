// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	C++ class header boilerplate exported from UnrealHeaderTool.
	This is automatically generated by the tools.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "ObjectBase.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UObject;
class UAdvKitCharacterAction_Arguments;
#ifdef ADVKITRUNTIME_AdvKitCharacterAction_Arguments_generated_h
#error "AdvKitCharacterAction_Arguments.generated.h already included, missing '#pragma once' in AdvKitCharacterAction_Arguments.h"
#endif
#define ADVKITRUNTIME_AdvKitCharacterAction_Arguments_generated_h

#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execCreateArguments) \
	{ \
		P_GET_OBJECT(UObject,Z_Param_WorldContextObject); \
		P_GET_OBJECT(UClass,Z_Param_ArgumentClass); \
		P_FINISH; \
		*(UAdvKitCharacterAction_Arguments**)Z_Param__Result=UAdvKitCharacterAction_Arguments::CreateArguments(Z_Param_WorldContextObject,Z_Param_ArgumentClass); \
	}


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execCreateArguments) \
	{ \
		P_GET_OBJECT(UObject,Z_Param_WorldContextObject); \
		P_GET_OBJECT(UClass,Z_Param_ArgumentClass); \
		P_FINISH; \
		*(UAdvKitCharacterAction_Arguments**)Z_Param__Result=UAdvKitCharacterAction_Arguments::CreateArguments(Z_Param_WorldContextObject,Z_Param_ArgumentClass); \
	}


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_INCLASS_NO_PURE_DECLS \
	private: \
	static void StaticRegisterNativesUAdvKitCharacterAction_Arguments(); \
	friend ADVKITRUNTIME_API class UClass* Z_Construct_UClass_UAdvKitCharacterAction_Arguments(); \
	public: \
	DECLARE_CLASS(UAdvKitCharacterAction_Arguments, UObject, COMPILED_IN_FLAGS(CLASS_Abstract), 0, TEXT("/Script/AdvKitRuntime"), NO_API) \
	DECLARE_SERIALIZER(UAdvKitCharacterAction_Arguments) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_INCLASS \
	private: \
	static void StaticRegisterNativesUAdvKitCharacterAction_Arguments(); \
	friend ADVKITRUNTIME_API class UClass* Z_Construct_UClass_UAdvKitCharacterAction_Arguments(); \
	public: \
	DECLARE_CLASS(UAdvKitCharacterAction_Arguments, UObject, COMPILED_IN_FLAGS(CLASS_Abstract), 0, TEXT("/Script/AdvKitRuntime"), NO_API) \
	DECLARE_SERIALIZER(UAdvKitCharacterAction_Arguments) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAdvKitCharacterAction_Arguments(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAdvKitCharacterAction_Arguments) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAdvKitCharacterAction_Arguments); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAdvKitCharacterAction_Arguments); \
private: \
	/** Private copy-constructor, should never be used */ \
	NO_API UAdvKitCharacterAction_Arguments(const UAdvKitCharacterAction_Arguments& InCopy); \
public:


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAdvKitCharacterAction_Arguments(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private copy-constructor, should never be used */ \
	NO_API UAdvKitCharacterAction_Arguments(const UAdvKitCharacterAction_Arguments& InCopy); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAdvKitCharacterAction_Arguments); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAdvKitCharacterAction_Arguments); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAdvKitCharacterAction_Arguments)


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_12_PROLOG
#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_RPC_WRAPPERS \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_INCLASS \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_RPC_WRAPPERS_NO_PURE_DECLS \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_INCLASS_NO_PURE_DECLS \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Actions_AdvKitCharacterAction_Arguments_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
