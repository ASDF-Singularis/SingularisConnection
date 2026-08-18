#pragma once

#include <CoreMinimal.h>

#include "SingularisConnectionType.generated.h"

class USceneComponent;

/**
 * 连接参数。
 *
 * 抽象层公共参数，与具体连接实现逻辑无关。
 * 具体连接提供器的专属参数由其自身 UPROPERTY 定义，
 * 遵循开闭原则：抽象稳定，扩展开放。
 */
USTRUCT(BlueprintType)
struct SINGULARISCONNECTION_API FSingularisConnectionParams
{
	GENERATED_BODY()

	/** 连接目标组件 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "目标组件"))
	USceneComponent* Target = nullptr;
};
