#pragma once
#include "CoreMinimal.h"

struct FPupLevel {
    const TCHAR* Name;
    int Moves, Score, Breed, Collect, Mud, Crates, Layers;
    static FPupLevel Get(int N) {
        static const FPupLevel Levels[] = {
            {TEXT("First tail wags"),22,1800,0,0,0,0,1},
            {TEXT("Corgi roll call"),25,0,0,12,0,0,1},
            {TEXT("Muddy paws"),26,0,0,0,10,0,1},
            {TEXT("A helping paw"),27,0,0,0,0,4,1},
            {TEXT("Meadow picnic"),28,2200,5,12,8,0,1},
            {TEXT("Poodle parade"),25,0,2,18,0,0,1},
            {TEXT("Puddle jumping"),27,0,0,0,16,0,1},
            {TEXT("Blossom rescue"),28,0,0,0,8,5,1},
            {TEXT("Beagle buddies"),29,0,1,18,12,0,1},
            {TEXT("Garden party"),30,3200,2,15,0,6,1},
            {TEXT("Seaside stroll"),26,0,3,20,0,0,1},
            {TEXT("Deep puddles"),30,0,0,0,14,0,2},
            {TEXT("Beach patrol"),29,0,5,16,0,7,1},
            {TEXT("Sandy paws"),31,0,0,0,16,6,2},
            {TEXT("Sunset splash"),32,3800,3,20,12,0,2},
            {TEXT("Moonlight pugs"),27,0,4,24,0,0,1},
            {TEXT("Starlight rescue"),31,0,0,0,0,9,1},
            {TEXT("Twilight trails"),33,0,4,20,18,0,2},
            {TEXT("Homeward bound"),34,0,0,18,12,8,2},
            {TEXT("The big pawty"),36,4500,2,20,16,8,2}
        };
        return Levels[FMath::Clamp(N,1,20)-1];
    }
    static const TCHAR* Zone(int N) {const TCHAR* Z[]={TEXT("Clover Meadow"),TEXT("Blossom Gardens"),TEXT("Sunset Shores"),TEXT("Starlight Park")};return Z[FMath::Clamp((N-1)/5,0,3)];}
    static const TCHAR* BreedName(int B) {const TCHAR* Names[]={TEXT("corgis"),TEXT("beagles"),TEXT("poodles"),TEXT("huskies"),TEXT("pugs"),TEXT("retrievers")};return Names[FMath::Clamp(B,0,5)];}
};
