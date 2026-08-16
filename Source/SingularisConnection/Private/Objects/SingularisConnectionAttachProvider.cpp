#include "Objects/SingularisConnectionAttachProvider.h"

#include <GameFramework/Actor.h>

#include "Components/SingularisConnectionComponent.h"

void USingularisConnectionAttachProvider::ExecuteConnect_Implementation(
	const FSingularisConnectionParams& Params)
{
	// 1) 卫语句
	if (!IsValid(Params.Target) || !IsValid(Params.Target->GetOwner())) return;

	AActor* TargetOwner = Params.Target->GetOwner();

	// 2) 幂等性：已连接同一目标时静默返回
	if (Hierarchy == ESingularisConnectionHierarchy::Parented
		&& AttachedToActor.Get() == TargetOwner) return;
	if (Hierarchy == ESingularisConnectionHierarchy::Peered
		&& PeeredTarget.Get() == Params.Target) return;

	// 3) Parented：执行 Actor 附加
	if (Hierarchy == ESingularisConnectionHierarchy::Parented)
	{
		const USingularisConnectionComponent* OwnerComp =
			Cast<USingularisConnectionComponent>(GetOuter());
		if (!IsValid(OwnerComp) || !IsValid(OwnerComp->GetOwner())) return;

		const FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, true);
		OwnerComp->GetOwner()->AttachToActor(TargetOwner, Rules);
		AttachedToActor = TargetOwner;
		return;
	}

	// 4) Peered：仅记录关联
	PeeredTarget = Params.Target;
}

void USingularisConnectionAttachProvider::ExecuteDisconnect_Implementation()
{
	// 1) Parented：执行分离
	if (Hierarchy == ESingularisConnectionHierarchy::Parented
		&& AttachedToActor.IsValid())
	{
		const USingularisConnectionComponent* OwnerComp =
			Cast<USingularisConnectionComponent>(GetOuter());
		if (IsValid(OwnerComp) && IsValid(OwnerComp->GetOwner()))
		{
			const FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
			OwnerComp->GetOwner()->DetachFromActor(Rules);
		}
		AttachedToActor.Reset();
		return;
	}

	// 2) Peered：清除关联
	PeeredTarget.Reset();
}

bool USingularisConnectionAttachProvider::IsConnected_Implementation() const
{
	return Hierarchy == ESingularisConnectionHierarchy::Parented
		       ? AttachedToActor.IsValid()
		       : PeeredTarget.IsValid();
}
