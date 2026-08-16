#include "Components/SingularisConnectionComponent.h"

#include "Objects/SingularisConnectionProvider.h"

USingularisConnectionComponent::USingularisConnectionComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void USingularisConnectionComponent::BeginPlay()
{
	Super::BeginPlay();
	BindOwnerDestroyed();
}

bool USingularisConnectionComponent::IsConnected() const
{
	return IsValid(ConnectionProvider) && ConnectionProvider->IsConnected();
}

void USingularisConnectionComponent::Connect(USceneComponent* Target)
{
	// 1) 卫语句：服务器权威 + 提供器有效
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority()) return;
	if (!IsValid(ConnectionProvider)) return;

	// 2) 断开旧连接（幂等：未连接时无副作用）
	if (IsConnected())
	{
		ConnectionProvider->ExecuteDisconnect();
		OnConnectionBrokenEvent.Broadcast();
	}

	// 3) 组装上下文，委托提供器执行连接
	ConnectionProvider->ExecuteConnect(FSingularisConnectionParams{Target});

	// 4) 连接成功后广播
	if (IsConnected())
		OnConnectionEstablishedEvent.Broadcast();
}

void USingularisConnectionComponent::Disconnect()
{
	// 1) 卫语句：服务器权威 + 提供器有效
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority()) return;
	if (!IsValid(ConnectionProvider)) return;

	// 2) 断开连接
	if (IsConnected())
	{
		ConnectionProvider->ExecuteDisconnect();
		OnConnectionBrokenEvent.Broadcast();
	}
}

void USingularisConnectionComponent::OnOwnerDestroyed(AActor* DestroyedActor)
{
	Disconnect();
}

void USingularisConnectionComponent::BindOwnerDestroyed()
{
	if (AActor* Owner = GetOwner())
		Owner->OnDestroyed.AddDynamic(this,
			&USingularisConnectionComponent::OnOwnerDestroyed);
}

void USingularisConnectionComponent::UnbindOwnerDestroyed()
{
	if (AActor* Owner = GetOwner())
		Owner->OnDestroyed.RemoveDynamic(this,
			&USingularisConnectionComponent::OnOwnerDestroyed);
}
