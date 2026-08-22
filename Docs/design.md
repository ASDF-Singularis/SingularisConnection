# SingularisConnection 设计文档

## 概述

SingularisConnection 是 Singularis 生态中的连接维护插件，管理两个 Actor 之间"连接建立后"的关系生命周期。它是通用连接系统三层架构中的连接层：查询层找到目标，锚点层执行停靠，连接层维护关系。

核心语义：**连接 = 建立并维护关系**，与定位无关。

---

## 设计哲学

### 组件不管理关系

不同的连接模式有不同的关系——Attach 是 Actor 层级关系，ClusterUnion 是物理集群关系，PhysicsConstraint 是约束关系。若组件管理关系，则每个新模式的加入都需要修改组件内部实现，扩展即修改，违反开闭原则。

### 关系归属提供器

关系由 Provider 子类自行持有与管理——`AttachProvider` 持有附加目标与对等引用，`ClusterUnionProvider` 持有集群成员关系。组件对关系零感知，只做编排：权威校验、断开旧连接、委托新连接、广播事件。

### 组件正交

组件与组件之间不存在配对关系。一个组件的 `ConnectComponent` / `ConnectActor` 接收的是目标组件或目标 Actor——目标可以是锚点、墙面、车架组件，任意东西。组件不知道自己"连到了谁的什么"上，那是 Provider 的事。

### 抽象契约与具体参数的分离

- **组件 API**：具体参数（`ConnectComponent(USceneComponent* Target)` / `ConnectActor(AActor* Target)`）——调用方面向显式直白，无需构造结构体
- **Provider SPI**：上下文模式（`ExecuteConnect(Context)`）——抽象契约需稳定，未来加公共参数不破坏子类签名

### 连接与定位互不相干

连接组件不关心位置是怎么算出来的，也不关心目标是怎么找到的——它只负责"已经对齐了，现在建立关系"。定位由锚点插件完成，查询由查询插件完成，三层零依赖。

---

## 架构

```
┌──────────────────────────────────────────────┐
│  USingularisConnectionComponent（编排入口）    │
│  ├── ConnectionProvider（Instanced）          │
│  ├── ConnectComponent(Target) / ConnectActor(Target) / Disconnect()  │
│  ├── IsConnected() → 委托 Provider            │
│  └── 事件广播                                 │
└──────────────┬───────────────────────────────┘
               │ 委托
               ▼
┌──────────────────────────────────────────────┐
│  USingularisConnectionProvider（抽象基类）     │
│  ├── ExecuteConnect(Context)                 │
│  ├── ExecuteDisconnect()                     │
│  └── IsConnected()                           │
└──────┬───────────────────────┬───────────────┘
       │                       │
       ▼                       ▼
┌─────────────────┐   ┌─────────────────┐
│ AttachProvider  │   │ ClusterUnion    │
│ 父子: AttachTo  │   │ Provider        │
│ 对等: 记录引用   │   │ 加入/移出集群    │
└─────────────────┘   └─────────────────┘
```

| 类 | 类型 | 职责 |
|----|------|------|
| `USingularisConnectionComponent` | ActorComponent | 编排入口：权威校验、委托、事件广播 |
| `USingularisConnectionProvider` | UObject 抽象基类 | SPI：连接 / 断开 / 状态查询 |
| `USingularisConnectionAttachProvider` | Provider 子类 | Actor 层级附加关系 |
| `USingularisConnectionClusterUnionProvider` | Provider 子类 | Chaos 物理集群关系 |

---

## 抽象契约

### 上下文 `FSingularisConnectionContext`

```cpp
struct FSingularisConnectionContext
{
    AActor* Instigator;             // 发起连接的 Actor，可空
    AActor* Avatar;                  // 承载连接组件的 Actor（执行主体）
    USceneComponent* TargetComponent; // ConnectComponent 入口选填，供 Component 模式 Provider 直接消费
    AActor* TargetActor;             // ConnectActor 入口选填
    USingularisConnectionComponent* ConnectionComponent; // 执行连接的组件引用
};
```

唯一的公共参数。Provider 子类的专属参数（层级模式、集群标识等）定义在子类自身的 UPROPERTY 上。

### SPI

```
USingularisConnectionProvider
├── ExecuteConnect(Context)   // 建立关系，默认空实现
├── ExecuteDisconnect()        // 释放关系，默认空实现
└── IsConnected() → bool       // 关系状态查询，默认 false
```

---

## 连接组件

```
USingularisConnectionComponent : UActorComponent
├── ConnectionProvider: USingularisConnectionProvider*（Instanced）
├── ConnectComponent(USceneComponent* Target)
├── ConnectActor(AActor* Target)
├── Disconnect()
├── IsConnected() → bool
├── OnConnectionEstablishedEvent()
└── OnConnectionBrokenEvent()
```

### 编排流程

`ConnectComponent` / `ConnectActor` 内部按固定顺序执行：

1. **权威校验** — 仅服务器可执行（`BlueprintAuthorityOnly`）
2. **断开旧连接** — 若已连接，委托 Provider 断开并广播 `OnConnectionBrokenEvent`（幂等：未连接时无副作用）
3. **组装上下文** — 填充 `FSingularisConnectionContext{Instigator, Avatar=GetOwner(), ConnectionComponent=this, TargetComponent 或 TargetActor}`，委托 Provider 建立新连接
4. **广播成功** — `IsConnected()` 为真时广播 `OnConnectionEstablishedEvent`

`Disconnect`：权威校验 → 委托 Provider 断开 → 广播断开事件。

组件生命周期：Owner 销毁时自动断开连接。

---

## 预置提供器

### AttachProvider

- **连接**：Owner 附加到目标 Actor（`AttachToActor`），跟随移动
- **断开**：`DetachFromActor` 分离 Owner
- **状态**：`AttachedToActor`（附加目标）是否有效

### ClusterUnionProvider

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `ClusterId` | None | 集群标识 |

- **连接**：解析目标 Actor 上的 `UClusterUnionComponent`，将本 Owner 的根 PrimitiveComponent 加入集群（`AddComponentToCluster`）
- **断开**：移出集群（`RemoveComponentFromCluster`）
- **状态**：查询集群成员关系（`GetPrimitiveComponents`）

网络分流：`Connect` 为服务器权威，组件无复制属性——物理集群操作仅在服务器执行。

---

## 扩展指南

### 自定义提供器

继承 `USingularisConnectionProvider`，覆写三个 SPI：

```
class UMyProvider : USingularisConnectionProvider
    override ExecuteConnect(Context)
        // 建立自定义关系
    override ExecuteDisconnect()
        // 释放自定义关系
    override IsConnected() → bool
        // 查询自定义关系状态
```

无需覆写组件逻辑——组件已处理权威校验、断开编排与事件广播。

---

## 数据流

### 载具装轮胎

```
1. 查询层在车架轮胎位锚点处发现匹配
2. 停靠层将轮胎对齐到锚点
3. 游戏层 Spawn 轮胎
4. 轮胎.ConnectionComponent.ConnectComponent(车架集群组件)
5. ClusterUnionProvider 解析车架集群组件
6. 轮胎根组件加入集群——物理联动成立
```

### 墙上挂装饰

```
1. 查询层自由追踪到墙面
2. 停靠层将装饰对齐到命中变换
3. 游戏层 Spawn 装饰
4. 装饰.ConnectionComponent.ConnectActor(墙面 Actor)
5. AttachProvider 附加到墙面 Actor——装饰跟随墙壁
```

---

## 网络模型

- **组件 API**：`Connect`/`Disconnect` 为 `BlueprintAuthorityOnly`，仅服务器执行
- **组件无复制属性**：关系状态在 Provider 内部，组件零复制
- **物理操作分流**：ClusterUnion 的集群操作随 `Connect` 仅在服务器执行，引擎负责集群数据复制

---

## 局限

### 无拓扑感知

连接只管理两两关系，不感知链、环、图等拓扑结构。围墙链与闭环拓扑需要上层拓扑管理组件维护——这是刻意的职责边界。

### 单连接点

每个连接组件一次维护一个关系。Actor 多连接点（围墙两端同时连接两侧）需要挂载多个连接组件。

---

## 展望

### 物理约束提供器

`PhysicsConstraintProvider` 骨架已就位：创建 `UPhysicsConstraintComponent` 约束两端，服务悬挂、铰链等物理模拟连接场景。

### 更多连接方式

Provider 策略模式为扩展留下通道：Rope（绳索）、Hinge（铰链）、Slider（滑轨）、FixedJoint（固定关节）均可作为新的 Provider 子类加入。

### 拓扑管理组件

在两两连接之上构建图管理组件：邻接查询、链路遍历、环检测、连通分量——围墙建造系统直接消费拓扑查询。
