#pragma once

#include "SingularisConnectionProvider.h"
#include "Types/SingularisConnectionType.h"
#include "SingularisConnectionAttachProvider.generated.h"

/**
 * 引力奇点附加连接提供器。
 *
 * 通过 UE 原生 Actor 层级附加实现连接：Owner 附加到目标 Actor，跟随移动；断开时分离。
 */
UCLASS(Blueprintable, BlueprintType)
class SINGULARISCONNECTION_API USingularisConnectionAttachProvider : public USingularisConnectionProvider
{
	GENERATED_BODY()

#pragma region Internal Variable

	TWeakObjectPtr<AActor> AttachedToActor = nullptr;

#pragma endregion

public:
#pragma region USingularisConnectionProvider Interface

	virtual void ExecuteConnect_Implementation(
		const FSingularisConnectionParams& Params
	) override;
	virtual void ExecuteDisconnect_Implementation() override;
	virtual bool IsConnected_Implementation() const override;

#pragma endregion
};
