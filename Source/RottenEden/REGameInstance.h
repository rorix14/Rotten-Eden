// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Engine/GameInstance.h"
#include <Interfaces/OnlineSessionInterface.h>
#include "REGameInstance.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostName;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetSessionsListSigniture, const TArray<FServerData>&, ServerNames);

UCLASS()
class ROTTENEDEN_API UREGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FOnGetSessionsListSigniture OnGetSessionsList;

	virtual void Init() override;

	void Host(const FString& ServerName);

	void Join(const uint32& Index) const;

	void StartSession() const;

	void RefreshServerList();

	// http requests
	void GetRequest(FString&& SubRoot, TFunction<void(FString&& JsonContent)> OnReceivedCallBack);

	void PostRequest(FString&& SubRoot, const FString& BodyJson,
                     TFunction<void(FString&& JsonContent)> OnReceivedCallBack);

private:
	TSubclassOf<class UUserWidget> MainMenuClass;

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	void OnFindSessionComplete(bool bWasSuccessful);

	void CreateSession() const;

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnNetWorkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	                      const FString& ErrorString);

	FString DesiredServerName;

	// http services
	class FHttpModule* Http;

	void OnReceiveResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful,
                       TFunction<void(FString&& JsonContent)> OnReceivedCallBack);

	bool ResponseIsValid(const FHttpResponsePtr& Response, const bool& WasSuccessful);

	TSharedRef<IHttpRequest> RequestWithRoot(const FString& SubRoot, FString&& Verb) const;
	//void(UREGameOverMenu::*)(FString&&)
};
