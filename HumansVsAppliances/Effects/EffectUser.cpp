// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectUser.h"
#include "../EnemiesPawn/EnemyPawn.h"
#include "../PlayerPawn/ArenaPlayerPawn.h"
#include "../IdleEffectUser/IdleEffectUser.h"
#include "Effect.h"


// Add default functionality here for any IEffectUser functions that are not pure virtual.

float IEffectUser::GetMaxEffectRadius(TArray<UEffect*>& effects)
{
    float res = 0.f;
    for (int i = 0; i < effects.Num(); i++)
        if (res < effects[i]->radius)
            res = effects[i]->radius;
    return res;
}

TArray<UEffect*> IEffectUser::GetAllEffectsOnDistantion(float distantion, TArray<UEffect*>& effects)
{
    TArray<UEffect*> arr;

    for (int i = 0; i < effects.Num(); i++)
        if (effects[i]->radius > distantion && effects[i]->radius > 0.1)
            arr.Add(effects[i]);

    return arr;
}


TArray<UEffect*> IEffectUser::GetAllEffectsOnPoint(TArray<UEffect*>& effects)
{
    TArray<UEffect*> arr;

    for (int i = 0; i < effects.Num(); i++)
        if (effects[i]->radius < 0.1)
            arr.Add(effects[i]);

    return arr;
}

TArray<UEffect*> IEffectUser::FilterEffects(TArray<UEffect*> effects, AActor* who)
{
    TArray<UEffect*> arr;

    for (int i = effects.Num() - 1; i >= 0; i--)
        if (effects[i]->effectAim == EEffectAim::ALL
            || who == nullptr
            || (effects[i]->effectAim == EEffectAim::PLAYER && Cast<AArenaPlayerPawn>(who))
            || (effects[i]->effectAim == EEffectAim::ENEMY && Cast<AEnemyPawn>(who))
            || (effects[i]->effectAim == EEffectAim::ENEMY && Cast<AIdleEffectUser>(who)))
            arr.Add(effects[i]);

    return arr;
}
