#include "Objects/SingularisConnectionProvider.h"

UWorld* USingularisConnectionProvider::GetWorld() const
{
	if (HasAnyFlags(RF_ClassDefaultObject)) return nullptr;
	if (const UObject* Outer = GetOuter()) return Outer->GetWorld();
	return Super::GetWorld();
}

void USingularisConnectionProvider::ExecuteConnect_Implementation(
	const FSingularisConnectionParams& Params) {}

void USingularisConnectionProvider::ExecuteDisconnect_Implementation() {}

bool USingularisConnectionProvider::IsConnected_Implementation() const
{
	return false;
}
