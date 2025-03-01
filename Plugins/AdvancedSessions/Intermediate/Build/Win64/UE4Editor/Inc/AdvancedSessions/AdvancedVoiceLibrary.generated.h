// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	C++ class header boilerplate exported from UnrealHeaderTool.
	This is automatically generated by the tools.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "ObjectBase.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FBPUniqueNetId;
#ifdef ADVANCEDSESSIONS_AdvancedVoiceLibrary_generated_h
#error "AdvancedVoiceLibrary.generated.h already included, missing '#pragma once' in AdvancedVoiceLibrary.h"
#endif
#define ADVANCEDSESSIONS_AdvancedVoiceLibrary_generated_h

#define projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execGetNumLocalTalkers) \
	{ \
		P_GET_PROPERTY_REF(UIntProperty,Z_Param_Out_NumLocalTalkers); \
		P_FINISH; \
		UAdvancedVoiceLibrary::GetNumLocalTalkers(Z_Param_Out_NumLocalTalkers); \
	} \
 \
	DECLARE_FUNCTION(execUnMuteRemoteTalker) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalUserNum); \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_GET_UBOOL(Z_Param_bIsSystemWide); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::UnMuteRemoteTalker(Z_Param_LocalUserNum,Z_Param_Out_UniqueNetId,Z_Param_bIsSystemWide); \
	} \
 \
	DECLARE_FUNCTION(execMuteRemoteTalker) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalUserNum); \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_GET_UBOOL(Z_Param_bIsSystemWide); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::MuteRemoteTalker(Z_Param_LocalUserNum,Z_Param_Out_UniqueNetId,Z_Param_bIsSystemWide); \
	} \
 \
	DECLARE_FUNCTION(execIsPlayerMuted) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalUserNumChecking); \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::IsPlayerMuted(Z_Param_LocalUserNumChecking,Z_Param_Out_UniqueNetId); \
	} \
 \
	DECLARE_FUNCTION(execIsRemotePlayerTalking) \
	{ \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::IsRemotePlayerTalking(Z_Param_Out_UniqueNetId); \
	} \
 \
	DECLARE_FUNCTION(execIsLocalPlayerTalking) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::IsLocalPlayerTalking(Z_Param_LocalPlayerNum); \
	} \
 \
	DECLARE_FUNCTION(execRemoveAllRemoteTalkers) \
	{ \
		P_FINISH; \
		UAdvancedVoiceLibrary::RemoveAllRemoteTalkers(); \
	} \
 \
	DECLARE_FUNCTION(execUnRegisterRemoteTalker) \
	{ \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::UnRegisterRemoteTalker(Z_Param_Out_UniqueNetId); \
	} \
 \
	DECLARE_FUNCTION(execRegisterRemoteTalker) \
	{ \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::RegisterRemoteTalker(Z_Param_Out_UniqueNetId); \
	} \
 \
	DECLARE_FUNCTION(execUnRegisterAllLocalTalkers) \
	{ \
		P_FINISH; \
		UAdvancedVoiceLibrary::UnRegisterAllLocalTalkers(); \
	} \
 \
	DECLARE_FUNCTION(execUnRegisterLocalTalker) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		UAdvancedVoiceLibrary::UnRegisterLocalTalker(Z_Param_LocalPlayerNum); \
	} \
 \
	DECLARE_FUNCTION(execRegisterAllLocalTalkers) \
	{ \
		P_FINISH; \
		UAdvancedVoiceLibrary::RegisterAllLocalTalkers(); \
	} \
 \
	DECLARE_FUNCTION(execRegisterLocalTalker) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::RegisterLocalTalker(Z_Param_LocalPlayerNum); \
	} \
 \
	DECLARE_FUNCTION(execStopNetworkedVoice) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		UAdvancedVoiceLibrary::StopNetworkedVoice(Z_Param_LocalPlayerNum); \
	} \
 \
	DECLARE_FUNCTION(execStartNetworkedVoice) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		UAdvancedVoiceLibrary::StartNetworkedVoice(Z_Param_LocalPlayerNum); \
	} \
 \
	DECLARE_FUNCTION(execIsHeadsetPresent) \
	{ \
		P_GET_UBOOL_REF(Z_Param_Out_bHasHeadset); \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		UAdvancedVoiceLibrary::IsHeadsetPresent(Z_Param_Out_bHasHeadset,Z_Param_LocalPlayerNum); \
	}


#define projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execGetNumLocalTalkers) \
	{ \
		P_GET_PROPERTY_REF(UIntProperty,Z_Param_Out_NumLocalTalkers); \
		P_FINISH; \
		UAdvancedVoiceLibrary::GetNumLocalTalkers(Z_Param_Out_NumLocalTalkers); \
	} \
 \
	DECLARE_FUNCTION(execUnMuteRemoteTalker) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalUserNum); \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_GET_UBOOL(Z_Param_bIsSystemWide); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::UnMuteRemoteTalker(Z_Param_LocalUserNum,Z_Param_Out_UniqueNetId,Z_Param_bIsSystemWide); \
	} \
 \
	DECLARE_FUNCTION(execMuteRemoteTalker) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalUserNum); \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_GET_UBOOL(Z_Param_bIsSystemWide); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::MuteRemoteTalker(Z_Param_LocalUserNum,Z_Param_Out_UniqueNetId,Z_Param_bIsSystemWide); \
	} \
 \
	DECLARE_FUNCTION(execIsPlayerMuted) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalUserNumChecking); \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::IsPlayerMuted(Z_Param_LocalUserNumChecking,Z_Param_Out_UniqueNetId); \
	} \
 \
	DECLARE_FUNCTION(execIsRemotePlayerTalking) \
	{ \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::IsRemotePlayerTalking(Z_Param_Out_UniqueNetId); \
	} \
 \
	DECLARE_FUNCTION(execIsLocalPlayerTalking) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::IsLocalPlayerTalking(Z_Param_LocalPlayerNum); \
	} \
 \
	DECLARE_FUNCTION(execRemoveAllRemoteTalkers) \
	{ \
		P_FINISH; \
		UAdvancedVoiceLibrary::RemoveAllRemoteTalkers(); \
	} \
 \
	DECLARE_FUNCTION(execUnRegisterRemoteTalker) \
	{ \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::UnRegisterRemoteTalker(Z_Param_Out_UniqueNetId); \
	} \
 \
	DECLARE_FUNCTION(execRegisterRemoteTalker) \
	{ \
		P_GET_STRUCT_REF(FBPUniqueNetId,Z_Param_Out_UniqueNetId); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::RegisterRemoteTalker(Z_Param_Out_UniqueNetId); \
	} \
 \
	DECLARE_FUNCTION(execUnRegisterAllLocalTalkers) \
	{ \
		P_FINISH; \
		UAdvancedVoiceLibrary::UnRegisterAllLocalTalkers(); \
	} \
 \
	DECLARE_FUNCTION(execUnRegisterLocalTalker) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		UAdvancedVoiceLibrary::UnRegisterLocalTalker(Z_Param_LocalPlayerNum); \
	} \
 \
	DECLARE_FUNCTION(execRegisterAllLocalTalkers) \
	{ \
		P_FINISH; \
		UAdvancedVoiceLibrary::RegisterAllLocalTalkers(); \
	} \
 \
	DECLARE_FUNCTION(execRegisterLocalTalker) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		*(bool*)Z_Param__Result=UAdvancedVoiceLibrary::RegisterLocalTalker(Z_Param_LocalPlayerNum); \
	} \
 \
	DECLARE_FUNCTION(execStopNetworkedVoice) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		UAdvancedVoiceLibrary::StopNetworkedVoice(Z_Param_LocalPlayerNum); \
	} \
 \
	DECLARE_FUNCTION(execStartNetworkedVoice) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		UAdvancedVoiceLibrary::StartNetworkedVoice(Z_Param_LocalPlayerNum); \
	} \
 \
	DECLARE_FUNCTION(execIsHeadsetPresent) \
	{ \
		P_GET_UBOOL_REF(Z_Param_Out_bHasHeadset); \
		P_GET_PROPERTY(UByteProperty,Z_Param_LocalPlayerNum); \
		P_FINISH; \
		UAdvancedVoiceLibrary::IsHeadsetPresent(Z_Param_Out_bHasHeadset,Z_Param_LocalPlayerNum); \
	}


#define projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_INCLASS_NO_PURE_DECLS \
	private: \
	static void StaticRegisterNativesUAdvancedVoiceLibrary(); \
	friend ADVANCEDSESSIONS_API class UClass* Z_Construct_UClass_UAdvancedVoiceLibrary(); \
	public: \
	DECLARE_CLASS(UAdvancedVoiceLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/AdvancedSessions"), NO_API) \
	DECLARE_SERIALIZER(UAdvancedVoiceLibrary) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_INCLASS \
	private: \
	static void StaticRegisterNativesUAdvancedVoiceLibrary(); \
	friend ADVANCEDSESSIONS_API class UClass* Z_Construct_UClass_UAdvancedVoiceLibrary(); \
	public: \
	DECLARE_CLASS(UAdvancedVoiceLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/AdvancedSessions"), NO_API) \
	DECLARE_SERIALIZER(UAdvancedVoiceLibrary) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAdvancedVoiceLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAdvancedVoiceLibrary) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAdvancedVoiceLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAdvancedVoiceLibrary); \
private: \
	/** Private copy-constructor, should never be used */ \
	NO_API UAdvancedVoiceLibrary(const UAdvancedVoiceLibrary& InCopy); \
public:


#define projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAdvancedVoiceLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private copy-constructor, should never be used */ \
	NO_API UAdvancedVoiceLibrary(const UAdvancedVoiceLibrary& InCopy); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAdvancedVoiceLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAdvancedVoiceLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAdvancedVoiceLibrary)


#define projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_25_PROLOG
#define projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_RPC_WRAPPERS \
	projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_INCLASS \
	projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_RPC_WRAPPERS_NO_PURE_DECLS \
	projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_INCLASS_NO_PURE_DECLS \
	projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h_28_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID projectvelorum_Plugins_AdvancedSessions_Source_AdvancedSessions_Classes_AdvancedVoiceLibrary_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
