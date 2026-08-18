#include "Objects/SingularisConnectionClusterUnionProvider.h"

#include <Components/PrimitiveComponent.h>
#include <GameFramework/Actor.h>
#include <PhysicsEngine/ClusterUnionComponent.h>

void USingularisConnectionClusterUnionProvider::ExecuteConnect_Implementation(
	const FSingularisConnectionContext& Context
)
{
	// 1) 卫语句
	if (!IsValid(Context.Avatar)) return;

	// 2) 解析目标集群与源物理组件
	// Component 模式：严格消费 TargetComponent，要求即集群组件本身；
	// TargetComponent 为空时才从 TargetActor 查询集群。
	UClusterUnionComponent* Cluster = nullptr;
	if (Context.TargetComponent)
		Cluster = Cast<UClusterUnionComponent>(Context.TargetComponent);
	else if (IsValid(Context.TargetActor))
		Cluster = ResolveTargetCluster(Context.TargetActor);
	UPrimitiveComponent* SourcePrim = ResolveSourcePrimitive(Context.Avatar);
	if (!IsValid(Cluster) || !IsValid(SourcePrim)) return;

	// 3) 幂等性：已连接同一集群时静默返回
	if (ClusterUnionComponent.Get() == Cluster && IsConnected_Implementation()) return;

	// 4) 已连接其他集群时先断开
	if (IsConnected_Implementation())
		ExecuteDisconnect_Implementation();

	// 5) 加入集群
	TArray<int32> BoneIds;
	BoneIds.Add(0);
	Cluster->AddComponentToCluster(SourcePrim, BoneIds);
	ClusterUnionComponent = Cluster;
	ClusterMemberComponent = SourcePrim;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[ClusterUnionProvider] %s joined cluster %s (ClusterId: %s)"),
		*GetNameSafe(SourcePrim),
		*GetNameSafe(Cluster),
		*ClusterId.ToString()
	);
}

void USingularisConnectionClusterUnionProvider::ExecuteDisconnect_Implementation()
{
	// 1) 移出集群
	if (ClusterUnionComponent.IsValid() && ClusterMemberComponent.IsValid())
		ClusterUnionComponent->RemoveComponentFromCluster(ClusterMemberComponent.Get());

	// 2) 清除状态
	ClusterUnionComponent.Reset();
	ClusterMemberComponent.Reset();
}

bool USingularisConnectionClusterUnionProvider::IsConnected_Implementation() const
{
	// 1) 卫语句
	if (!ClusterUnionComponent.IsValid() || !ClusterMemberComponent.IsValid())
		return false;

	// 2) 查询集群成员关系
	const TArray<UPrimitiveComponent*> Members =
		ClusterUnionComponent->GetPrimitiveComponents();
	return Members.Contains(ClusterMemberComponent.Get());
}

UClusterUnionComponent* USingularisConnectionClusterUnionProvider::ResolveTargetCluster(
	AActor* TargetOwner
) const
{
	return IsValid(TargetOwner)
		       ? TargetOwner->FindComponentByClass<UClusterUnionComponent>()
		       : nullptr;
}

UPrimitiveComponent* USingularisConnectionClusterUnionProvider::ResolveSourcePrimitive(
	AActor* SourceOwner
) const
{
	return IsValid(SourceOwner)
		       ? Cast<UPrimitiveComponent>(SourceOwner->GetRootComponent())
		       : nullptr;
}
