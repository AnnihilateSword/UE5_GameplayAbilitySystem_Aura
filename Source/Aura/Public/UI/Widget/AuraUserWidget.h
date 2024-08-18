// Copyright AnnihilateSword

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	/** ÿ������Ϊ������ Widget ���� WidgetController ʱ������Ҳ����ô˺��� */
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
