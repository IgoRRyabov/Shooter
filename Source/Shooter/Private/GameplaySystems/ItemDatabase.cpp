#include "GameplaySystems/ItemDatabase.h"

UItemDatabase* UItemDatabase::Instance = nullptr;

UItemDatabase* UItemDatabase::Get()
{
	if (!Instance)
	{
		Instance = NewObject<UItemDatabase>();
		Instance->AddToRoot();
	}
	return Instance;
}

void UItemDatabase::Initialize(UDataTable* InTable)
{
	ItemTable = InTable;
}

FItemData UItemDatabase::GetItemByID(FName ItemID) const
{
	FItemData Result;

	if (!ItemTable) return Result;

	FString Context;
	const FItemData* Row = ItemTable->FindRow<FItemData>(ItemID, Context);
	if (Row)
	{
		Result = *Row; // копируем значение
	}

	return Result;
}