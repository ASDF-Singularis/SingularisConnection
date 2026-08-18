#include "Components/SingularisConnectionComponent.h"

#include <GameFramework/Actor.h>

#include "Objects/SingularisConnectionProvider.h"
#include "Types/SingularisConnectionType.h"

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

void USingularisConnectionComponent::ConnectComponent(AActor* Instigator, USceneComponent* TargetComponent)
{
	FSingularisConnectionContext Context{};
	Context.Instigator = Instigator;
	Context.Avatar = GetOwner();
	Context.ConnectionComponent = this;
	Context.TargetComponent = TargetComponent;
	ConnectInternal(Context);
}

void USingularisConnectionComponent::ConnectActor(AActor* Instigator, AActor* TargetActor)
{
	FSingularisConnectionContext Context{};
	Context.Instigator = Instigator;
	Context.Avatar = GetOwner();
	Context.ConnectionComponent = this;
	Context.TargetActor = TargetActor;
	ConnectInternal(Context);
}

void USingularisConnectionComponent::Disconnect() const
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
	{
		Owner->OnDestroyed.AddDynamic(
			this,
			&USingularisConnectionComponent::OnOwnerDestroyed
		);
	}
}

void USingularisConnectionComponent::UnbindOwnerDestroyed()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->OnDestroyed.RemoveDynamic(
			this,
			&USingularisConnectionComponent::OnOwnerDestroyed
		);
	}
}

void USingularisConnectionComponent::ConnectInternal(const FSingularisConnectionContext& Context) const
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

	// 3) 委托提供器执行连接
	ConnectionProvider->ExecuteConnect(Context);

	// 4) 连接成功后广播
	if (IsConnected())
		OnConnectionEstablishedEvent.Broadcast();
}
