# UE5_GameplayAbilitySystem_Aura

> Online repo for Gameplay Ability System


# 第1节：项目创建

1. 	在多人游戏中，复制的本质是当服务器上的实体发生变化时，服务器上发生的更改会复制或发送到它连接的所有客户端上。
    > 多人网络这块不熟悉的建议把官方文档上的这一模块看完：👉[Networking and Multiplayer](https://dev.epicgames.com/documentation/en-us/unreal-engine/networking-and-multiplayer-in-unreal-engine)

>

2. 	(1) 对于C++中的指针，我们可以使用 check 代替 if 判断是否有效，如果这是调试模式这会触发断点；
	(2) 对于转换我们也可以使用 CastChecked 代替 Cast，会比 Cast 多个断言；
    (3) Cast 如果转换失败会返回 nullptr；

>

3. 	创建动画蓝图时，可以选择创建 Specific Skeleton 和 Template，如果想做成通用的可以试试创建一个 Template，创建一个动画蓝图模板，继承该动画蓝图会同样继承 Event Graph 和 Anim Graph；

>

4. 	(1) 可以在 Character 和 Controller 中绑定和实现移动逻辑，不过比较推荐在 Controller 中写，
        这样让 Controller 分担一些逻辑更方便维护，而不是所有逻辑全写在 Character 里面；
	(2) 然后一些功能相关的逻辑推荐写到组件里面，这样方便复用以及维护；

>

5. 	使用接口的好处
	例如：
    在这个 Top-Down 游戏中，我们希望玩家可以使用鼠标点击敌人让敌人产生高亮轮廓，
    UPlayerController 可以获取光标下面的 AActor，但我们不想将逻辑写在 UPlayerController 中，
    我们可以创建一个高亮显示用的接口，让敌人实现高亮显示的函数，不同的敌人可以以不同的方式实现高亮函数，
    这样 UPlayerController 就不用管怎么实现了，交给实现了接口的 AActor 即可；
    > 我们还可以将这个接口添加到桶或门上，并以不同的方式突出显示这些对象

>

6. 	将成员变量保存到接口的正确做法是使用 TScriptInterface<> 而不是直接使用 IxxxInterface*
	参考文档：👉[Unreal Interfaces](https://dev.epicgames.com/documentation/en-us/unreal-engine/interfaces-in-unreal-engine)
	> 看这个模块Safely Store Object and Interface Pointers

	使用这个的好处：
	(1) 安全
	(2) 方便，TScriptInterface 内置了构造函数和操作符重载，比如可以省去 Cast 转换用来判断是否实现了接口：
    ```cpp
    TScriptInterface<IEnemyInterface> LastActor;
    TScriptInterface<IEnemyInterface> ThisActor;
    LastActor = ThisActor;
    ThisActor = CursorHit.GetActor();
    ```

>

7.  使用自定义深度注意有没有设置 ProjectSettings/Rendering->CustomDepth-StencilPass 为 Enabled with Stencil，默认 Enabled 是禁用 Stencil 的；
    不熟悉高亮材质制作思路的可以了解下 Content/Assets/Materials/PP_Highlight

# 第2节：介绍GAS系统（Gameplay Ability System）

官方文档：👉[Gameplay Ability System](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine)

这里列出目录，如果不太熟悉，建议先熟悉下官方文档介绍的 GAS

- **Ability System Component And Attributes**
  > Using the Ability System Component with Gameplay Attributes and Attribute Sets
- **Gameplay Ability**
  > Overview of the Gameplay Ability class.
- **Gameplay Attributes and Attribute Sets**
- **Gameplay Ability System Overview**
  > The Gameplay Ability System is a framework for building abilities and interactions that Actors can own and trigger. This system is designed mainly for RPGs, action-adventure games, MOBAs, and other types of games where characters have abilities that need to coordinate mechanics, visual effects, animations, sounds, and data-driven elements, although it can be adapted to a wide variety of projects. The Gameplay Ability System also supports replication for multiplayer games, and can save developers a lot of time scaling up their designs to support multiplayer.
  With this system, you can create abilities as simple as a single attack, or as complex as a spell that triggers many status effects depending on data from the user and the targets. This page provides an overview of the Ability System and how its components work together.
  > 什么是游戏能力？
  > 游戏能力是演员可以拥有并重复触发的游戏内动作。常见的例子包括法术、特殊攻击或物品触发的效果。
  > 虚幻引擎的游戏能力系统的设计涉及三个主要考虑因素:
  > - **追踪能力的拥有者**
  > - **跟踪能力状态**：
        1. 当能力被激活时；
        2. 当前正在进行该能力的执行时；
        3. 当能力完全完成并且不再有效时；
  > - **协调能力的执行** (Coordinating an Ability's Execution)：
        一种能力必须能够在执行过程中以特定的时间与多个不同的系统进行交互。这些交互可以是您在 Blueprint 中可以执行的任何操作，包括：
        - 激活动画蒙太奇；
        - 暂时控制角色的移动；
        - 触发视觉效果；
        - 执行重叠或碰撞事件；
        - 取消正在进行中的其他能力；
        - 等等...
        根据能力的工作方式，它可以在处于活动状态时在许多不同的时间点执行这些交互中的任何一个，包括在动画中间，并且某些效果可能需要在能力本身完成后持续存在。
  
  > 在多人游戏中，能力系统组件还负责将信息复制到客户端、将玩家操作传达到服务器以及验证客户端是否有权更改能力系统组件的状态。能力系统组件的父 Actor 必须由本地控制的玩家拥有才能进行远程激活，这意味着您只能对您控制的 Actor 执行能力。
  
  > - 您可以通过四种主要方法激活游戏能力：
      1. 您可以使用游戏能力句柄通过蓝图或 C++ 代码显式激活能力。这是在授予能力时由能力系统组件提供的；
      2. 使用游戏事件。这会通过匹配的能力触发器触发所有能力。如果您需要抽象输入和决策机制，则此方法更可取，因为它提供了最大程度的灵活性；
      3. 使用带有匹配标签的游戏​​效果。这会通过匹配的能力触发器触发所有能力。这是触发游戏效果能力的首选方法。一个典型的用例是睡眠减益，它会触发播放禁用动画并抑制其他游戏操作的能力；
      4. 使用输入代码。这些被添加到能力系统组件中，当被调用时它们将触发所有匹配的能力。其功能与游戏事件类似；
  
  > 当您激活游戏能力时，系统会将该能力识别为正在进行中。然后，它会触发附加到 Activate 事件的任何代码，遍历每个函数和游戏任务，直到您调用EndAbility函数来表示该能力已完成执行。如果需要进行任何额外的清理，您可以将更多代码附加到OnRemove事件。您还可以取消在执行过程中停止它的能力。 GameplayAbility 中的大多数函数在AbilitySystemComponent 中都有对应的函数，因此您可以为每个GameplayAbility 或每个AbilitySystemComponent 类选择不同的功能。
  
  > 游戏能力使用游戏标签来限制执行。所有能力都有一个在激活时添加到其所属 Actor 上的标签列表，以及阻止激活或自动取消该能力的标签列表。虽然您可以使用自己的代码手动取消、阻止或允许能力的执行，但这提供了一种系统一致的方法。

  > - **Attribute Sets and Attributes**
  > - **Handling Gameplay Effects**
      **游戏提示是负责运行视觉和声音效果的 Actor 和 UObject**，是在多人游戏中复制装饰反馈的首选方法。**游戏提示不使用可靠的复制**，因此，游戏提示应仅用于装饰性反馈。对于需要复制到所有客户端的游戏相关反馈，您应该依靠能力任务来处理复制。播放蒙太奇能力任务就是一个很好的例子。
  > - 网络游戏中的大部分能力都需要在服务器上运行并复制到客户端，因此能力激活通常会存在滞后。这在大多数快节奏的多人游戏中是不可取的。为了掩盖这种延迟，您可以在本地激活一项能力，然后告诉服务器您已激活它，以便它可以赶上。

  > 更多资料请阅读：👉[Gameplay Ability System Overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/understanding-the-unreal-engine-gameplay-ability-system)
- **Gameplay Effects**
  > 游戏能力系统使用游戏效果来更改游戏能力所针对的 Actor 的属性。游戏效果由可应用于 Actor 属性的函数库组成。这些可以是即时效果，例如施加伤害，也可以是持久效果，例如随着时间的推移对角色造成伤害的毒药。
  游戏效果是资产，因此在运行时是不可变的。
  存在一些例外情况，例如游戏效果在运行时创建，但数据一旦创建和配置就不会被修改。
  **游戏效果生命周期**
  游戏效果的持续时间可以设置为Instant 、 Infinite或Has Duration 。
  > - 游戏效果组件 (Gameplay Effect Components)
- **Ability Tasks**

![](./Res/ReadMe_Res/1_GameplayAbilitySystem.png)

![](./Res/ReadMe_Res/2_GameplayAbilitySystem.png)

可以将这些保留在 Pawn 或者 PlayerState 上

1. 如果是敌人 AI，这些敌人不需要 PlayerState，因为它们很简单，可以选择添加在 Pawn 上；
2. 如果是玩家，考虑到 Pawn 会销毁重生，如果你没有在 SaveGame 中保存这些数据，那么添加在 PlayerState 上是一个好的选择，还有一些原因，比如你不想让你的玩家类变得混乱，或者你不想只作用于特定的某一个角色，想更通用一点；

**在这个项目中，我们的敌人角色将直接拥有他们的能力、系统组件和属性集，但是对于我们的玩家控制的角色，我们将把我们的能力系统组件和属性；**

![](./Res/ReadMe_Res/3_GameplayAbilitySystem.png)

![](./Res/ReadMe_Res/4_GameplayAbilitySystem.png)

使用前需要启用插件

![](./Res/ReadMe_Res/5_EnablePugin.png)

![](./Res/ReadMe_Res/6_AbilitySystemComponent.png)

![](./Res/ReadMe_Res/7_AttributeSet.png)

![](./Res/ReadMe_Res/8_Add_Modules.png)

## GAS In Multiplayer

![](./Res/ReadMe_Res/9_GAS_In_Multiplayer.png)

![](./Res/ReadMe_Res/10_GAS_In_Multiplayer.png)

## Remarks

1. 在 PlayerState 构造函数中设置，服务器尝试更新客户端的频率（每秒更新多少次），我们可以设置得更快一些，比如可以设置 100.0f 左右：
   ```cpp
   NetUpdateFrequency = 100.0f;
   ```

2. 为 AbilitySystemComponent 设置复制模式
   
   ```cpp
   // Example
   UAbilitySystemComponent::SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
   ```

   ![](./Res/ReadMe_Res/11_ReplicationMode.png)

3. 初始化 ASC 的 Owner Actor 和 Avator Actor
   
   ![](./Res/ReadMe_Res/12_InitAbilityActorInfo.png)

   ![](./Res/ReadMe_Res/13_InitAbilityActorInfo.png)

4. 对于混合复制（Mixed）模式：OwnerActor 的 Owner 必须是 Controller。对于 pawn，这是在 posssedby() 中自动设置的；
   PlayerState 的所有者被自动设置为 Controller；
   因此，如果你的 OwnerActor 不是 PlayerState，并且你使用混合复制模式，你必须在 OwnerActor 上调用 SetOwner() 来将其所有者设置为 Controller；
   
   > 默认情况下。PlayerState 的所有者会自动设置为 Controller，因此我们实际上不需要执行任何操作。

