#pragma once

#include "SingularisConnectionProvider.h"
#include "SingularisConnectionClusterUnionProvider.generated.h"

class UClusterUnionComponent;
class UPrimitiveComponent;

/**
 * 引力奇点集群联合连接提供器。
 *
 * 管理 Chaos 物理集群联合关系：将承载 ConnectionComponent 的
 * Actor 的根 PrimitiveComponent 加入目标 Actor 上的集群组件。
 * 仅在服务器执行（Connect 为服务器权威）。
 */
UCLASS(Blueprintable, BlueprintType)
class SINGULARISCONNECTION_API USingularisConnectionClusterUnionProvider : public USingularisConnectionProvider
{
	GENERATED_BODY()

public:
#pragma region Parameter

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "SingularisConnection|引力奇点集群联合连接提供器|参数",
		meta = (DisplayName = "集群标识")
	)
	FName ClusterId = NAME_None;

#pragma endregion

private:
#pragma region Internal Variable

	TWeakObjectPtr<UClusterUnionComponent> ClusterUnionComponent = nullptr;
	TWeakObjectPtr<UPrimitiveComponent> ClusterMemberComponent = nullptr;

#pragma endregion

public:
#pragma region USingularisConnectionProvider Interface

	virtual void ExecuteConnect_Implementation(
		const FSingularisConnectionContext& Context
	) override;
	virtual void ExecuteDisconnect_Implementation() override;
	virtual bool IsConnected_Implementation() const override;

#pragma endregion

private:
#pragma region Internal Function

	UClusterUnionComponent* ResolveTargetCluster(AActor* TargetOwner) const;
	UPrimitiveComponent* ResolveSourcePrimitive(AActor* SourceOwner) const;

#pragma endregion
};
