#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>

#include "SingularisConnectionComponent.generated.h"

class USceneComponent;
class USingularisConnectionProvider;

#pragma region 委托签名

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConnectionEstablishedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConnectionBrokenSignature);

#pragma endregion

/**
 * 引力奇点连接组件。
 *
 * 连接行为的载体与编排入口。组件本身不管理关系——
 * 具体连接模式的关系由 ConnectionProvider 子类自行持有与管理，
 * 组件仅负责服务器权威校验、状态编排与事件广播。
 * 组件与组件之间正交，不存在配对关系。
 */
UCLASS(
	Blueprintable,
	BlueprintType,
	ClassGroup = ("Singularis"),
	meta = (BlueprintSpawnableComponent, DisplayName = "引力奇点连接组件")
)
class SINGULARISCONNECTION_API USingularisConnectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
#pragma region Instantiation

	UPROPERTY(
		Instanced,
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "SingularisConnection|引力奇点连接|Instantiation",
		meta = (DisplayName = "连接提供器")
	)
	TObjectPtr<USingularisConnectionProvider> ConnectionProvider = nullptr;

#pragma endregion

#pragma region 事件分发器

	UPROPERTY(
		BlueprintAssignable,
		Category = "SingularisConnection|引力奇点连接|事件分发器",
		meta = (DisplayName = "连接建立时")
	)
	FOnConnectionEstablishedSignature OnConnectionEstablishedEvent{};

	UPROPERTY(
		BlueprintAssignable,
		Category = "SingularisConnection|引力奇点连接|事件分发器",
		meta = (DisplayName = "连接断开时")
	)
	FOnConnectionBrokenSignature OnConnectionBrokenEvent{};

#pragma endregion

#pragma region Constructors

	USingularisConnectionComponent();

#pragma endregion

#pragma region ActorComponent Interface

	virtual void BeginPlay() override;

#pragma endregion

#pragma region State

	UFUNCTION(
		BlueprintPure,
		Category = "SingularisConnection|引力奇点连接|State",
		meta = (DisplayName = "已连接")
	)
	bool IsConnected() const;

#pragma endregion

#pragma region API

	/**
	 * 连接到目标组件。
	 *
	 * 服务器权威。断开旧连接后委托 ConnectionProvider 建立新连接，
	 * 连接成功后广播 OnConnectionEstablishedEvent。
	 *
	 * @param Target 连接的目标组件。
	 */
	UFUNCTION(
		BlueprintCallable,
		BlueprintAuthorityOnly,
		Category = "SingularisConnection|引力奇点连接|API",
		meta = (DisplayName = "连接")
	)
	void Connect(USceneComponent* Target);

	UFUNCTION(
		BlueprintCallable,
		BlueprintAuthorityOnly,
		Category = "SingularisConnection|引力奇点连接|API",
		meta = (DisplayName = "断开连接")
	)
	void Disconnect();

#pragma endregion

protected:
#pragma region Shared Function

	UFUNCTION()
	virtual void OnOwnerDestroyed(AActor* DestroyedActor);

#pragma endregion

private:
#pragma region Internal Function

	void BindOwnerDestroyed();
	void UnbindOwnerDestroyed();

#pragma endregion
};
