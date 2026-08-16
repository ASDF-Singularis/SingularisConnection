#pragma once

#include "SingularisConnectionProvider.h"
#include "Types/SingularisConnectionType.h"
#include "SingularisConnectionAttachProvider.generated.h"

/**
 * 引力奇点附加连接提供器。
 *
 * 管理 UE 原生 Actor 层级附加关系。
 * Parented 模式：Owner 附加到目标，跟随移动。
 * Peered 模式：仅记录关联引用，不执行附加。
 */
UCLASS(Blueprintable, BlueprintType)
class SINGULARISCONNECTION_API USingularisConnectionAttachProvider : public USingularisConnectionProvider
{
	GENERATED_BODY()

public:
#pragma region Parameter

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "SingularisConnection|引力奇点附加连接提供器|参数",
		meta = (DisplayName = "父子关系")
	)
	ESingularisConnectionHierarchy Hierarchy = ESingularisConnectionHierarchy::Parented;

#pragma endregion

private:
#pragma region Internal Variable

	TWeakObjectPtr<AActor> AttachedToActor = nullptr;
	TWeakObjectPtr<USceneComponent> PeeredTarget = nullptr;

#pragma endregion

public:
#pragma region USingularisConnectionProvider Interface

	virtual void ExecuteConnect_Implementation(
		const FSingularisConnectionParams& Params) override;
	virtual void ExecuteDisconnect_Implementation() override;
	virtual bool IsConnected_Implementation() const override;

#pragma endregion
};
