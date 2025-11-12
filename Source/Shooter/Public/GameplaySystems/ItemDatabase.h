#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Structs/ItemData.h"
#include "Engine/DataTable.h"
#include "ItemDatabase.generated.h"

UCLASS()
class SHOOTER_API UItemDatabase : public UObject
{
	GENERATED_BODY()
	
public:
	static UItemDatabase* Get();

	void Initialize(UDataTable* InTable);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Item Database")
	FItemData GetItemByID(FName ItemID) const;

private:
	static UItemDatabase* Instance;

	UPROPERTY()
	UDataTable* ItemTable;
};
