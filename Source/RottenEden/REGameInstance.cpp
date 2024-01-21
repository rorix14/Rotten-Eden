// Fill out your copyright notice in the Description page of Project Settings.


#include "REGameInstance.h"
#include <OnlineSubsystem.h>
#include <OnlineSessionSettings.h>
#include "GameFramework/GameModeBase.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"

const static FName GServer_Name_Settings_Key = TEXT("ServerName");
const static FString GHTTP_Base_URL = TEXT("http://15.188.205.6:8080");

void UREGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (OnlineSubSystem)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Subsytem name %s"), *OnlineSubSystem->GetSubsystemName().ToString());
		SessionInterface = OnlineSubSystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
				this, &UREGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(
				this, &UREGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UREGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UREGameInstance::OnJoinSessionComplete);
		}
	}
	// else
	// UE_LOG(LogTemp, Warning, TEXT("No Online subsystem"));

	if (GEngine)
		GEngine->OnNetworkFailure().AddUObject(this, &UREGameInstance::OnNetWorkFailure);

	Http = &FHttpModule::Get();
}

void UREGameInstance::Host(const FString& ServerName)
{
	DesiredServerName = ServerName;
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (ExistingSession)
		{
			SessionInterface->DestroySession(NAME_GameSession);
		}
		else
		{
			CreateSession();
			if (SessionSearch.IsValid())
			{
				SessionSearch.Reset();
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Host"));
}

void UREGameInstance::Join(const uint32& Index) const
{
	if (!SessionInterface.IsValid() || !SessionSearch.IsValid()) return;

	SessionInterface->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[Index]);
}

void UREGameInstance::StartSession() const
{
	if (SessionInterface.IsValid())
		SessionInterface->StartSession(NAME_GameSession);
}

void UREGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionInterface.IsValid() && SessionSearch.IsValid())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Starting find Session"));
		SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName().IsEqual(NULL_SUBSYSTEM);
		SessionSearch->MaxSearchResults = 200;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UREGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!bWasSuccessful) return;

	// if (UEngine* Engine = GetEngine())
	// 	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(TEXT("/Game/_Main/Levels/Lobby?listen"));
		//UE_LOG(LogTemp, Warning, TEXT("Created session"));
	}
}

void UREGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
		CreateSession();
}

void UREGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful && SessionSearch.IsValid())
	{
		TArray<FServerData> ServerNames;
		for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
		{
			FServerData Data;
			Data.HostName = Result.Session.OwningUserName;
			Data.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - Result.Session.NumOpenPublicConnections;
			FString ServerName;
			if (Result.Session.SessionSettings.Get(GServer_Name_Settings_Key, ServerName))
				Data.Name = ServerName;
			else
				Data.Name = "Could not find name.";

			ServerNames.Add(Data);
			//UE_LOG(LogTemp, Warning, TEXT("Session ID: %s"), *Result.GetSessionIdStr());
		}

		OnGetSessionsList.Broadcast(ServerNames);
		//UE_LOG(LogTemp, Warning, TEXT("Finished find Sessions"));
	}
}

void UREGameInstance::CreateSession() const
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName().IsEqual(NULL_SUBSYSTEM);
		SessionSettings.NumPublicConnections = 4;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(GServer_Name_Settings_Key, DesiredServerName,
		                    EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
	}
}

void UREGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString ConnectInfo;
	if (!SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Could not get connect string"));
		return;
	}

	// if (UEngine* Engine = GetEngine())
	// 	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *ConnectInfo));

	if (APlayerController* PlayerController = GetFirstLocalPlayerController())
		PlayerController->ClientTravel(ConnectInfo, TRAVEL_Absolute);
}

void UREGameInstance::OnNetWorkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
                                       const FString& ErrorString)
{
	// Do any additional functionality here for when there is a when there is a net work error, like sending a message
	// to the user warning him of what happened 
	if (World)
	{
		if (World->IsServer())
		{
			if (AGameModeBase* GameMode = World->GetAuthGameMode())
				GameMode->ReturnToMainMenuHost();
		}
		else
		{
			if (APlayerController* PC = GetFirstLocalPlayerController())
				PC->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("Back to main menu")));
		}
	}
}

// http services functions
void UREGameInstance::GetRequest(FString&& SubRoot, TFunction<void(FString&& JsonContent)> OnReceivedCallBack)
{
	TSharedRef<IHttpRequest> Request = RequestWithRoot(SubRoot, TEXT("GET"));
	Request->OnProcessRequestComplete().BindUObject(this, &UREGameInstance::OnReceiveResponse, OnReceivedCallBack);
	Request->ProcessRequest();
}

void UREGameInstance::PostRequest(FString&& SubRoot, const FString& BodyJson,
                                  TFunction<void(FString&& JsonContent)> OnReceivedCallBack)
{
	TSharedRef<IHttpRequest> Request = RequestWithRoot(SubRoot, TEXT("POST"));
	Request->SetContentAsString(BodyJson);
	Request->OnProcessRequestComplete().BindUObject(this, &UREGameInstance::OnReceiveResponse, OnReceivedCallBack);
	Request->ProcessRequest();
}

void UREGameInstance::OnReceiveResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful,
                                        TFunction<void(FString&& JsonContent)> OnReceivedCallBack)
{
	if(!ResponseIsValid(Response, WasSuccessful)) return;

	//UE_LOG(LogTemp, Warning, TEXT("Responce: %s"), *Response->GetContentAsString());
	OnReceivedCallBack(Response->GetContentAsString());
}

bool UREGameInstance::ResponseIsValid(const FHttpResponsePtr& Response, const bool& WasSuccessful)
{
	return EHttpResponseCodes::IsOk(Response->GetResponseCode()) && WasSuccessful && Response.IsValid();
}

TSharedRef<IHttpRequest> UREGameInstance::RequestWithRoot(const FString& SubRoot, FString&& Verb) const
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(GHTTP_Base_URL + SubRoot);
	Request->SetHeader("User-Agent", "Unreal-Agent");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Accepts", "application/json");
	Request->SetVerb(Verb);
	return Request;
}
