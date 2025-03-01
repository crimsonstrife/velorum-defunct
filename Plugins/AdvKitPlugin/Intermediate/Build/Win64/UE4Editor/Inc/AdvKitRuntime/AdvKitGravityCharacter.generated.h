// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	C++ class header boilerplate exported from UnrealHeaderTool.
	This is automatically generated by the tools.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "ObjectBase.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UAdvKitGravityCharacterMovementComponent;
struct FVector;
struct FFindFloorResult;
#ifdef ADVKITRUNTIME_AdvKitGravityCharacter_generated_h
#error "AdvKitGravityCharacter.generated.h already included, missing '#pragma once' in AdvKitGravityCharacter.h"
#endif
#define ADVKITRUNTIME_AdvKitGravityCharacter_generated_h

#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_RPC_WRAPPERS \
	virtual bool SetNewOrientationServer_Validate(FVector , FVector , bool ); \
	virtual void SetNewOrientationServer_Implementation(FVector GravityUpVector, FVector Origin, bool bForce); \
	virtual bool CanWallWalkOn_Implementation(FFindFloorResult NewFloor); \
 \
	DECLARE_FUNCTION(execGetAdvGravityMovementComponent) \
	{ \
		P_FINISH; \
		*(UAdvKitGravityCharacterMovementComponent**)Z_Param__Result=this->GetAdvGravityMovementComponent(); \
	} \
 \
	DECLARE_FUNCTION(execSetNewOrientationServer) \
	{ \
		P_GET_STRUCT(FVector,Z_Param_GravityUpVector); \
		P_GET_STRUCT(FVector,Z_Param_Origin); \
		P_GET_UBOOL(Z_Param_bForce); \
		P_FINISH; \
		if (!this->SetNewOrientationServer_Validate(Z_Param_GravityUpVector,Z_Param_Origin,Z_Param_bForce)) \
		{ \
			RPC_ValidateFailed(TEXT("SetNewOrientationServer_Validate")); \
			return; \
		} \
		this->SetNewOrientationServer_Implementation(Z_Param_GravityUpVector,Z_Param_Origin,Z_Param_bForce); \
	} \
 \
	DECLARE_FUNCTION(execSetNewOrientation) \
	{ \
		P_GET_STRUCT(FVector,Z_Param_GravityUpVector); \
		P_GET_STRUCT(FVector,Z_Param_Origin); \
		P_GET_UBOOL(Z_Param_bForce); \
		P_FINISH; \
		this->SetNewOrientation(Z_Param_GravityUpVector,Z_Param_Origin,Z_Param_bForce); \
	} \
 \
	DECLARE_FUNCTION(execCanWallWalkOn) \
	{ \
		P_GET_STRUCT(FFindFloorResult,Z_Param_NewFloor); \
		P_FINISH; \
		*(bool*)Z_Param__Result=this->CanWallWalkOn_Implementation(Z_Param_NewFloor); \
	}


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_RPC_WRAPPERS_NO_PURE_DECLS \
	virtual bool SetNewOrientationServer_Validate(FVector , FVector , bool ); \
 \
	DECLARE_FUNCTION(execGetAdvGravityMovementComponent) \
	{ \
		P_FINISH; \
		*(UAdvKitGravityCharacterMovementComponent**)Z_Param__Result=this->GetAdvGravityMovementComponent(); \
	} \
 \
	DECLARE_FUNCTION(execSetNewOrientationServer) \
	{ \
		P_GET_STRUCT(FVector,Z_Param_GravityUpVector); \
		P_GET_STRUCT(FVector,Z_Param_Origin); \
		P_GET_UBOOL(Z_Param_bForce); \
		P_FINISH; \
		if (!this->SetNewOrientationServer_Validate(Z_Param_GravityUpVector,Z_Param_Origin,Z_Param_bForce)) \
		{ \
			RPC_ValidateFailed(TEXT("SetNewOrientationServer_Validate")); \
			return; \
		} \
		this->SetNewOrientationServer_Implementation(Z_Param_GravityUpVector,Z_Param_Origin,Z_Param_bForce); \
	} \
 \
	DECLARE_FUNCTION(execSetNewOrientation) \
	{ \
		P_GET_STRUCT(FVector,Z_Param_GravityUpVector); \
		P_GET_STRUCT(FVector,Z_Param_Origin); \
		P_GET_UBOOL(Z_Param_bForce); \
		P_FINISH; \
		this->SetNewOrientation(Z_Param_GravityUpVector,Z_Param_Origin,Z_Param_bForce); \
	} \
 \
	DECLARE_FUNCTION(execCanWallWalkOn) \
	{ \
		P_GET_STRUCT(FFindFloorResult,Z_Param_NewFloor); \
		P_FINISH; \
		*(bool*)Z_Param__Result=this->CanWallWalkOn_Implementation(Z_Param_NewFloor); \
	}


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_EVENT_PARMS \
	struct AdvKitGravityCharacter_eventCanWallWalkOn_Parms \
	{ \
		FFindFloorResult NewFloor; \
		bool ReturnValue; \
 \
		/** Constructor, intializes return property only **/ \
		AdvKitGravityCharacter_eventCanWallWalkOn_Parms() \
			: ReturnValue(false) \
		{ \
		} \
	}; \
	struct AdvKitGravityCharacter_eventSetNewOrientationServer_Parms \
	{ \
		FVector GravityUpVector; \
		FVector Origin; \
		bool bForce; \
	};


extern ADVKITRUNTIME_API  FName ADVKITRUNTIME_CanWallWalkOn;
extern ADVKITRUNTIME_API  FName ADVKITRUNTIME_SetNewOrientationServer;
#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_CALLBACK_WRAPPERS
#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_INCLASS_NO_PURE_DECLS \
	private: \
	static void StaticRegisterNativesAAdvKitGravityCharacter(); \
	friend ADVKITRUNTIME_API class UClass* Z_Construct_UClass_AAdvKitGravityCharacter(); \
	public: \
	DECLARE_CLASS(AAdvKitGravityCharacter, AAdvKitCharacter, COMPILED_IN_FLAGS(CLASS_Abstract), 0, TEXT("/Script/AdvKitRuntime"), NO_API) \
	DECLARE_SERIALIZER(AAdvKitGravityCharacter) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC}; \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_INCLASS \
	private: \
	static void StaticRegisterNativesAAdvKitGravityCharacter(); \
	friend ADVKITRUNTIME_API class UClass* Z_Construct_UClass_AAdvKitGravityCharacter(); \
	public: \
	DECLARE_CLASS(AAdvKitGravityCharacter, AAdvKitCharacter, COMPILED_IN_FLAGS(CLASS_Abstract), 0, TEXT("/Script/AdvKitRuntime"), NO_API) \
	DECLARE_SERIALIZER(AAdvKitGravityCharacter) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC}; \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AAdvKitGravityCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AAdvKitGravityCharacter) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AAdvKitGravityCharacter); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AAdvKitGravityCharacter); \
private: \
	/** Private copy-constructor, should never be used */ \
	NO_API AAdvKitGravityCharacter(const AAdvKitGravityCharacter& InCopy); \
public:


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_ENHANCED_CONSTRUCTORS \
private: \
	/** Private copy-constructor, should never be used */ \
	NO_API AAdvKitGravityCharacter(const AAdvKitGravityCharacter& InCopy); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AAdvKitGravityCharacter); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AAdvKitGravityCharacter); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AAdvKitGravityCharacter)


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_13_PROLOG \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_EVENT_PARMS


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_RPC_WRAPPERS \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_CALLBACK_WRAPPERS \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_INCLASS \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_RPC_WRAPPERS_NO_PURE_DECLS \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_CALLBACK_WRAPPERS \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_INCLASS_NO_PURE_DECLS \
	projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h_16_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID projectvelorum_Plugins_AdvKitPlugin_Source_AdvKitRuntime_Classes_Player_AdvKitGravityCharacter_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
