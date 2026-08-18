#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>

#include "Types/SingularisConnectionType.h"
#include "SingularisConnectionProvider.generated.h"

/**
 * 引力奇点连接提供器（抽象基类）。
 *
 * 作为 USingularisConnectionComponent 的 UObject 子对象挂载。
 * 负责管理具体连接模式的关系——Attach 层级、物理集群、物理约束等，
 * 各模式的专属关系数据由子类自行持有与管理。
 *
 * 抽象层仅约定连接 / 断开 / 状态查询契约，参数与实现无关。
 * 提供器对 ConnectionComponent 零感知——由组件在 Connect/Disconnect
 * 中主动调用提供器 SPI。
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, CollapseCategories)
class SINGULARISCONNECTION_API USingularisConnectionProvider : public UObject
{
	GENERATED_BODY()

public:
#pragma region UObject Interface

	virtual UWorld* GetWorld() const override;

#pragma endregion

#pragma region SPI

	/**
	 * 执行连接逻辑。
	 *
	 * 由 ConnectionComponent::Connect 调用。
	 * 子类在此方法中建立并管理该模式的关系。
	 * 默认实现为空。
	 *
	 * @param Context 连接上下文，承载 Avatar / Target / Instigator 等公共上下文。
	 */
	UFUNCTION(
		BlueprintNativeEvent,
		BlueprintCallable,
		Category = "SingularisConnection|引力奇点连接提供器|SPI",
		meta = (DisplayName = "执行连接")
	)
	void ExecuteConnect(const FSingularisConnectionContext& Context);

	/**
	 * 执行断开逻辑。
	 *
	 * 由 ConnectionComponent::Disconnect 调用。
	 * 子类在此方法中释放该模式的关系资源。
	 * 默认实现为空。
	 */
	UFUNCTION(
		BlueprintNativeEvent,
		BlueprintCallable,
		Category = "SingularisConnection|引力奇点连接提供器|SPI",
		meta = (DisplayName = "执行断开")
	)
	void ExecuteDisconnect();

	/**
	 * 查询是否已连接。
	 *
	 * 关系状态由子类管理，组件通过此接口委托查询。
	 * 默认返回 false。
	 */
	UFUNCTION(
		BlueprintNativeEvent,
		BlueprintPure,
		Category = "SingularisConnection|引力奇点连接提供器|SPI",
		meta = (DisplayName = "已连接")
	)
	bool IsConnected() const;

#pragma endregion
};
