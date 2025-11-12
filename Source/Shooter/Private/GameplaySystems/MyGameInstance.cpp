#include "GameplaySystems/MyGameInstance.h"

#include "GameplaySystems/ItemDatabase.h"

void UMyGameInstance::Init()
{
	Super::Init();
	if (ItemDatabaseTable)
	{
		UItemDatabase::Get()->Initialize(ItemDatabaseTable);
	}
}
