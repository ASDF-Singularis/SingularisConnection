#pragma once

#include "SingularisConnectionProvider.h"
#include "Types/SingularisConnectionType.h"
#include "SingularisConnectionAttachProvider.generated.h"

/**
 * 引力奇点附加连接提供器。
 *
 * 通过 UE 原生组件层级附加实现连接。
 * Component 模式：Owner 根组件附加到 TargetComponent，跟随移动。
 * Actor 模式：Owner 根组件附加到 TargetActor 的根组件。
 */
UCLASS(Blueprintable, BlueprintType)
class SINGULARISCONNECTION_API USingularisConnectionAttachProvider : public USingularisConnectionProvider
{
	GENERATED_BODY()

#pragma region Internal Variable

	TWeakObjectPtr<USceneComponent> AttachedToComponent = nullptr;
	TWeakObjectPtr<USceneComponent> AttachedAvatarRoot = nullptr;

#pragma endregion

public:
#pragma region USingularisConnectionProvider Interface

	virtual void ExecuteConnect_Implementation(
		const FSingularisConnectionContext& Context
	) override;
	virtual void ExecuteDisconnect_Implementation() override;
	virtual bool IsConnected_Implementation() const override;

#pragma endregion
};
