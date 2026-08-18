#pragma once

#include "SingularisConnectionProvider.h"
#include "SingularisConnectionPhysicsConstraintProvider.generated.h"

/**
 * 引力奇点物理约束连接提供器。
 *
 * 管理 UPhysicsConstraintComponent 约束关系。V7 中实现具体的约束创建/销毁逻辑。
 */
UCLASS(Blueprintable, BlueprintType)
class SINGULARISCONNECTION_API USingularisConnectionPhysicsConstraintProvider : public USingularisConnectionProvider
{
	GENERATED_BODY()

public:
#pragma region USingularisConnectionProvider Interface

	virtual void ExecuteConnect_Implementation(
		const FSingularisConnectionContext& Context
	) override;
	virtual void ExecuteDisconnect_Implementation() override;
	virtual bool IsConnected_Implementation() const override;

#pragma endregion
};
