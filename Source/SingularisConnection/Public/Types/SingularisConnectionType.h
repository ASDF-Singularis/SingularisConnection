#pragma once

#include <CoreMinimal.h>

#include "SingularisConnectionType.generated.h"

class AActor;
class USingularisConnectionComponent;
class USceneComponent;

/**
 * 引力奇点连接上下文。
 *
 * 在 ConnectComponent / ConnectActor 中由组件组装，传递至各 Provider 的 ExecuteConnect。
 * 字段布局对齐 FSingularisGeneralAbilityContext：Instigator / Avatar / Target / Component。
 */
USTRUCT(BlueprintType)
struct SINGULARISCONNECTION_API FSingularisConnectionContext
{
	GENERATED_BODY()

	/** 发起连接的 Actor，可空（如玩家放置装饰时由 Pawn 触发） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "发起者"))
	TObjectPtr<AActor> Instigator = nullptr;

	/** 承载连接组件的 Actor（执行主体，对应 GeneralAbility 的 Avatar） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "执行主体"))
	TObjectPtr<AActor> Avatar = nullptr;

	/** 连接目标组件（ConnectComponent 入口选填，供 Component 模式 Provider 直接消费） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "目标组件"))
	TObjectPtr<USceneComponent> TargetComponent = nullptr;

	/** 连接目标 Actor（ConnectActor 入口选填） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "目标 Actor"))
	TObjectPtr<AActor> TargetActor = nullptr;

	/** 执行连接的组件引用（对应 GeneralAbility 的 AbilityComponent） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "连接组件"))
	TObjectPtr<USingularisConnectionComponent> ConnectionComponent = nullptr;
};
