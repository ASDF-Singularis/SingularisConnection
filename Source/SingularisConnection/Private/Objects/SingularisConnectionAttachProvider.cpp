#include "Objects/SingularisConnectionAttachProvider.h"

#include <Components/SceneComponent.h>
#include <GameFramework/Actor.h>

void USingularisConnectionAttachProvider::ExecuteConnect_Implementation(
	const FSingularisConnectionContext& Context
)
{
	// 1) 解析目标 SceneComponent：Component 模式直接消费，Actor 模式取根组件
	USceneComponent* TargetSceneComp = Context.TargetComponent;
	if (!IsValid(TargetSceneComp))
		TargetSceneComp = IsValid(Context.TargetActor) ? Context.TargetActor->GetRootComponent() : nullptr;
	if (!IsValid(TargetSceneComp)) return;

	// 2) 幂等性：已附加到同一目标时静默返回
	if (AttachedToComponent.Get() == TargetSceneComp) return;

	// 3) 取 Avatar 根组件并执行附加
	if (!IsValid(Context.Avatar)) return;
	USceneComponent* AvatarRoot = Context.Avatar->GetRootComponent();
	if (!IsValid(AvatarRoot)) return;

	const FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, true);
	AvatarRoot->AttachToComponent(TargetSceneComp, Rules);
	AttachedToComponent = TargetSceneComp;
	AttachedAvatarRoot = AvatarRoot;
}

void USingularisConnectionAttachProvider::ExecuteDisconnect_Implementation()
{
	if (!AttachedToComponent.IsValid() || !AttachedAvatarRoot.IsValid()) return;

	const FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
	AttachedAvatarRoot->DetachFromComponent(Rules);

	AttachedToComponent.Reset();
	AttachedAvatarRoot.Reset();
}

bool USingularisConnectionAttachProvider::IsConnected_Implementation() const
{
	return AttachedToComponent.IsValid();
}
