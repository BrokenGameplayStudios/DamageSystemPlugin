# DamageSystemPlugin

A custom, replicated damage system plugin for Unreal Engine 5.7. This provides a component-based health management system with support for damage, healing, and events via delegates. It uses an interface-driven approach for applying damage/healing, without relying on Unreal's built-in ApplyDamage or similar functions.

## Installation
- Clone or download the repo: git clone https://github.com/BrokenGameplayStudios/DamageSystemPlugin.git
- Copy the `DamageSystemPlugin` folder into your UE project's `/Plugins/` directory (create the folder if it doesn't exist).
- Right-click your project's `.uproject` file in File Explorer and select Generate Visual Studio project files to update the solution.
- Open the `.sln` in Visual Studio and build the project (use Development Editor configuration).
- Launch the UE Editor, go to `Edit > Plugins`, search for "DamageSystemPlugin", enable it, and restart the Editor.

For pre-built versions (if available), check the Releases page and unzip directly into `/Plugins/` (no build needed for matching platforms like Win64).

## Usage

This plugin implements a custom damage system using the `IDamageableInterface` (in `DamageableInterface.h`) and `UDamageSystemComponent` (in `DamageSystemComponent.h/cpp`). Actors like characters can implement the interface to route damage/healing to the component.

## Key Concepts
- Component: `UDamageSystemComponent` handles replicated health state (CurrentHealth, MaxHealth), applies changes on the server, and broadcasts delegates (e.g., `OnDamageTaken`, `OnHealReceived`, `OnDeath`).
- Interface: Use `TakeDamage(const FDamageInfo& DamageInfo)` to apply damage (returns bool if applied) or `Heal(float HealAmount, AActor* Healer)` for healing. These route to the component's `HandleIncomingDamage`/`HandleIncomingHealing`.
- FDamageInfo: Custom struct (in `DamageSystemTypes.h`) for damage details (Amount, Causer, Type, etc.).
- Delegates: Bind to these in C++ or Blueprints for reactions (e.g., VFX on damage taken).

## Examples
Look in the source files for bare-bones implementations:
- **DamageableCharacterBase.h/cpp**: A base character class that implements `IDamageableInterface` and routes calls to the component. It also binds delegates in `BeginPlay()` (e.g., `RespondToDamageTaken` for custom logic like ragdoll on death). Use this as a starting point for your characters.
- **DamageSystemComponent.h/cpp**: Core logic for handling damage/healing, replication, and delegates. See `HandleIncomingDamage` for how damage is processed (checks if avoidable, applies, broadcasts events, multicasts death).
- **DamageableInterface.h**: The interface blueprint—implement in your actors to make them damageable.

To apply damage in your own systems (e.g., a weapon class):
- Get the target actor from a trace/overlap (e.g., in a melee attack function on server).
- Check if it implements the interface: if `(HitActor->Implements<UDamageableInterface>())`.
- Create `FDamageInfo` (set Amount, Causer = this, etc.).
- Call `IDamageableInterface::Execute_TakeDamage(HitActor, YourDamageInfo)`.

For healing (e.g., AOE sphere trace): Similar process, but call `IDamageableInterface::Execute_Heal(HitActor, HealAmount, this)` on valid targets.
Test in multiplayer: Use Editor's Play > Number of Players > 2 to verify replication.

