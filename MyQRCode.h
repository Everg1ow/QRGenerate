// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Texture2D.h"
#include "qrencode.h"
#include "MyQRCode.generated.h"

UCLASS()
class QRCODE_API AMyQRCode : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyQRCode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "My|MyActor")
		FString SetQECode(FString name = "Hello");

	
	UFUNCTION(BlueprintCallable, Category = "My|MyActor")
		static class UTexture2D* GetTexture2DFromDiskFile(const FString& FilePath);

	
	UFUNCTION(BlueprintCallable, Category = "My|MyActor")        
		static UTexture2D* LoadTexture2D(const FString& ImagePath, bool& IsValid, int32& OutWidth, int32& OutHeight);

};
