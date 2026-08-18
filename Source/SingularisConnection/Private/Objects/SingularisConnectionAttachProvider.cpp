#include "Objects/SingularisConnectionAttachProvider.h"

#include <GameFramework/Actor.h>

#include "Components/SingularisConnectionComponent.h"

void USingularisConnectionAttachProvider::ExecuteConnect_Implementation(
	const FSingularisConnectionParams& Params
)
{
	// 1) 卫语句
	if (!IsValid(Params.Target) || !IsValid(Params.Target->GetOwner())) return;

	AActor* TargetOwner = Params.Target->GetOwner();

	// 2) 幂等性：已附加到同一目标时静默返回
	if (AttachedToActor.Get() == TargetOwner) return;

	// 3) 执行 Actor 附加
	const USingularisConnectionComponent* OwnerComp =
		Cast<USingularisConnectionComponent>(GetOuter());
	if (!IsValid(OwnerComp) || !IsValid(OwnerComp->GetOwner())) return;

	const FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, true);
	OwnerComp->GetOwner()->AttachToActor(TargetOwner, Rules);
	AttachedToActor = TargetOwner;
}

void USingularisConnectionAttachProvider::ExecuteDisconnect_Implementation()
{
	if (!AttachedToActor.IsValid()) return;

	const USingularisConnectionComponent* OwnerComp =
		Cast<USingularisConnectionComponent>(GetOuter());
	if (IsValid(OwnerComp) && IsValid(OwnerComp->GetOwner()))
	{
		const FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
		OwnerComp->GetOwner()->DetachFromActor(Rules);
	}
	AttachedToActor.Reset();
}

bool USingularisConnectionAttachProvider::IsConnected_Implementation() const
{
	return AttachedToActor.IsValid();
}
