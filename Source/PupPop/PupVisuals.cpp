#include "PupGame.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

APupPiece::APupPiece() {
    RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    PrimaryActorTick.bCanEverTick=false;
}
UStaticMeshComponent* APupPiece::Part(FVector P,FVector S,FLinearColor C,const TCHAR* Shape,FRotator R) {
    auto* M=NewObject<UStaticMeshComponent>(this);
    M->SetupAttachment(RootComponent);
    M->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,*FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"),Shape,Shape)));
    M->SetRelativeLocation(P); M->SetRelativeScale3D(S); M->SetRelativeRotation(R);
    M->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    M->SetCastShadow(true);
    auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Materials/M_Toy.M_Toy"));
    auto* Mat=UMaterialInstanceDynamic::Create(Base,this);
    Mat->SetVectorParameterValue(TEXT("Color"),C);
    M->SetMaterial(0,Mat); M->RegisterComponent(); Parts.Add(M); return M;
}
void APupPiece::Build(int Breed,int Special) {
    static const FLinearColor Coats[]={FLinearColor(.92,.37,.065),FLinearColor(.31,.10,.045),FLinearColor(.83,.30,.48),FLinearColor(.24,.39,.54),FLinearColor(.69,.48,.25),FLinearColor(.87,.77,.55)};
    const FLinearColor Coat=Coats[Breed], Cream(.98,.89,.70), Dark(.021,.015,.027), Pink(.98,.25,.39);
    FLinearColor Muzzle=Breed==4 ? FLinearColor(.105,.06,.045) : Cream;
    Part({0,4,-17},{.44,.38,.44},Coat);
    Part({0,-15,-17},{.28,.14,.30},Muzzle);
    Part({0,0,12},{.64,.48,.58},Coat);
    // Stubby legs, little toes, and a colorful collar.
    Part({-17,-17,-34},{.23,.29,.18},Cream); Part({17,-17,-34},{.23,.29,.18},Cream);
    Part({0,1,-15},{.46,.40,.075},FLinearColor(.10,.77,.60));
    Part({0,-22,-16},{.08,.04,.11},FLinearColor(1,.67,.10));
    Tail=Part({31,7,-20},{.16,.19,.39},Coat,TEXT("Sphere"),FRotator(0,0,-35));
    if(Breed==0 || Breed==3) {
        for(int Sign : {-1,1}) {
            Ears.Add(Part({Sign*23.f,1,39},{.26,.24,.39},Coat,TEXT("Cone"),FRotator(0,0,Sign*-13.f)));
            Part({Sign*23.f,-10,38},{.115,.04,.22},Pink,TEXT("Cone"),FRotator(0,0,Sign*-13.f));
        }
        Part({0,-21,19},{.16,.10,.42},Cream);
    } else {
        for(int Sign : {-1,1}) {
            Ears.Add(Part({Sign*30.f,2,12},{.23,.24,Breed==2 ? .48 : .55},Breed==5 ? FLinearColor(.61,.38,.16) : Coat,TEXT("Sphere"),FRotator(0,0,Sign*14.f)));
        }
    }
    if(Breed==1) {
        Part({-13,-19,20},{.25,.10,.29},FLinearColor(.12,.055,.025));
        Part({0,-21,21},{.12,.10,.43},Cream);
    }
    if(Breed==2) {
        for(int I=0;I<7;++I) {
            float A=I*PI/6;
            Part({FMath::Cos(A)*25,-2,29+FMath::Sin(A)*10},{.23,.30,.23},FLinearColor(.98,.47,.62));
        }
    }
    if(Breed==3) for(int Sign:{-1,1}) Part({Sign*14.f,-20,19},{.28,.12,.27},Cream);
    // Soft two-part muzzle, glossy eyes, button nose, and tiny tongue.
    Part({-9,-24,1},{.28,.25,.23},Muzzle); Part({9,-24,1},{.28,.25,.23},Muzzle);
    for(int Sign:{-1,1}) {
        Eyes.Add(Part({Sign*14.f,-24,19},{.11,.075,.14},Dark));
        Part({Sign*14.f-1.5f,-27.8f,21.5f},{.034,.023,.041},FLinearColor::White);
        Part({Sign*22.f,-21,5},{.095,.035,.058},FLinearColor(1,.39,.38));
    }
    Part({0,-38,7},{.13,.095,.09},Dark);
    Part({0,-30,-7},{.095,.07,.13},Pink);
    MarkSpecial(Special);
}
void APupPiece::MarkSpecial(int Value) {
    if(!Value) return;
    Badge=Part({0,-3,53},{.18,.15,.18},Value==1 ? FLinearColor(1,.75,.12) : FLinearColor(.18,1,.88),TEXT("Sphere"));
    for(int Sign:{-1,1}) Part({Sign*11.f,-3,53},{.09,.09,.09},FLinearColor(1,.85,.27));
}

void APupPiece::Animate(float T,float Joy) {
    if(Tail) Tail->SetRelativeRotation(FRotator(0,FMath::Sin(T*(9+Joy*7)+Phase)*(25+Joy*22),-35));
    const float Blink=FMath::Fmod(T+Phase,4.7f)<.12f ? .025f : .14f;
    for(auto* Eye:Eyes)Eye->SetRelativeScale3D(FVector(.11,.075,Blink));
    for(int I=0;I<Ears.Num();++I) {
        float Sign=I==0?-1:1;
        Ears[I]->SetRelativeRotation(FRotator(FMath::Sin(T*4+Phase)*3*Joy,0,Sign*12+FMath::Sin(T*6+Phase)*6*Joy));
    }
    if(Badge){Badge->SetRelativeLocation(FVector(FMath::Sin(T*2)*3,-3,54+FMath::Sin(T*4)*3));Badge->SetRelativeScale3D(FVector(.18+.025*FMath::Sin(T*6)));}
}
