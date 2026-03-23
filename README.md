# Damage System Plugin

A clean, standalone, fully replicated damage & health system for Unreal Engine 5.

Drop the component on any Actor/Character and go. Designed for easy extension and multiplayer.

## Features

* Current + Max Health (replicated)
* Flexible `FDamageInfo` struct (damage type, response, block/parry flags, location, causer, etc.)
* `IDamageableInterface` for any Actor
* Server-authoritative damage and healing
* Revival system (with optional transform + health scalar)
* Rich Blueprint-assignable delegates
* Ready-to-use `DamageableCharacterBase` example
* 100% multiplayer ready
* No dependencies

## Installation

1) Create folder `Plugins/DamageSystemPlugin` in your project
2) Copy the plugin files from this repo into it
3) Add the `.uplugin` file (or just enable the plugin in Editor)
4) Restart Editor

## Quick Start

1) Add **Damage System Component** to your Character/Actor
2) Call `InitializeHealth(YourMaxHealth)` (in BeginPlay)
3) Implement `IDamageableInterface` on your Actor (or inherit from `DamageableCharacterBase`)
4) To deal damage (on server):
   - Create an `FDamageInfo` struct
   - Call `IDamageableInterface::Execute_TakeDamage(TargetActor, DamageInfo)`
5) For healing: `IDamageableInterface::Execute_Heal(TargetActor, Amount, Healer)`
6) For revival: Call `Revive()` or `ReviveWithTransform()` on the component

## Delegates (bind in Character or Widget)

* **OnDamageTaken** → Hit reactions, VFX, sounds
* **OnDamageAvoided** → Block/parry feedback
* **OnHealReceived** → Healing particles
* **OnHealthChanged** → Update health bar
* **OnDeath** → Ragdoll, death logic
* **OnRevive** → Respawn VFX / logic

## About

A fully replicated, plug-and-play damage system designed for easy extension.
