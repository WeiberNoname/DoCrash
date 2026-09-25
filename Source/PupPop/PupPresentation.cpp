#include "PupGame.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Camera/CameraComponent.h"

APupEffects::APupEffects(){RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));}
void APupEffects::Init(){
    const FLinearColor Colors[]={FLinearColor(1,.64,.06),FLinearColor(1,.17,.36),FLinearColor(.1,.95,.72),FLinearColor(.2,.57,1),FLinearColor(.78,.3,1),FLinearColor(1,.94,.65)};
    for(int I=0;I<6;++I){
        auto* M=NewObject<UInstancedStaticMeshComponent>(this);M->SetupAttachment(RootComponent);
        M->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,I==5?TEXT("/Engine/BasicShapes/Sphere.Sphere"):TEXT("/Engine/BasicShapes/Cube.Cube")));
        M->SetCollisionEnabled(ECollisionEnabled::NoCollision);M->SetCastShadow(false);
        auto* Mat=UMaterialInstanceDynamic::Create(LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Materials/M_Toy.M_Toy")),this);
        Mat->SetVectorParameterValue(TEXT("Color"),Colors[I]);M->SetMaterial(0,Mat);M->RegisterComponent();Pools.Add(M);
    }
}
void APupEffects::Burst(FVector P,int Count,bool Confetti){
    for(int I=0;I<Count && Sparks.Num()<650;++I){
        FPupSpark S;S.P=P;S.P.Y=-110-FMath::FRand()*50;S.Pool=Confetti?FMath::RandRange(0,4):5;
        S.V=FVector(FMath::FRandRange(-180,180),FMath::FRandRange(-15,15),FMath::FRandRange(80,Confetti?430:210));
        S.Size=Confetti?FMath::FRandRange(.045,.095):FMath::FRandRange(.022,.055);S.Life=S.MaxLife=Confetti?FMath::FRandRange(1.8,3.6):.65f;
        S.Spin=FRotator(FMath::FRandRange(-240,240),FMath::FRandRange(-200,200),FMath::FRandRange(-270,270));Sparks.Add(S);
    }
}
void APupEffects::Advance(float Dt){
    for(auto* P:Pools)P->ClearInstances();
    for(int I=Sparks.Num()-1;I>=0;--I){
        auto& S=Sparks[I];S.Life-=Dt;if(S.Life<=0){Sparks.RemoveAtSwap(I);continue;}
        S.V.Z-=Dt*(S.Pool==5?260:160);S.P+=S.V*Dt;S.Rotation+=S.Spin*Dt;
        float Size=S.Size*FMath::Min(1.f,S.Life*3);
        Pools[S.Pool]->AddInstance(FTransform(S.Rotation,S.P,FVector(Size,S.Pool==5?Size:Size*.16,Size)),false);
    }
}
void APupGameMode::BuildScene(){
    if(Decor)Decor->Destroy();Decor=GetWorld()->SpawnActor<APupPiece>();
    int Zone=(Level-1)/5;
    const FLinearColor Back[]={FLinearColor(.018,.092,.084),FLinearColor(.10,.037,.085),FLinearColor(.018,.085,.15),FLinearColor(.025,.024,.09)};
    const FLinearColor Edge[]={FLinearColor(.38,.65,.40),FLinearColor(.75,.35,.46),FLinearColor(.34,.63,.75),FLinearColor(.46,.36,.72)};
    Decor->Part({0,170,0},{36,1,26},Back[Zone],TEXT("Cube"));
    Decor->Part({-190,77,10},{7.45,.5,7.45},Edge[Zone],TEXT("Cube"));
    Decor->Part({-190,48,10},{7.2,.25,7.2},Back[Zone]*2.1f,TEXT("Cube"));
    for(int I=0;I<49;++I){FVector P=Position(I);P.Y=29;Decor->Part(P,{.94,.23,.94},(I+I/7)%2?Edge[Zone]*.45f:Edge[Zone]*.35f,TEXT("Cube"));}
    // A toy garden with soft bushes and flowers around the mascot.
    for(int I=0;I<7;++I){
        float X=415+I*38;float Z=-305-FMath::Sin(I*1.7f)*15;
        Decor->Part({X,90,Z},{.55,.38,.45},Zone==3?FLinearColor(.16,.18,.42):FLinearColor(.12,.34,.21));
    }
    for(int I=0;I<9;++I){
        FVector P(400+I*30,35,-325-FMath::Sin(I*2.f)*30);
        FLinearColor Petal=Zone==2?FLinearColor(1,.59,.28):Zone==3?FLinearColor(.56,.40,.98):FLinearColor(1,.45,.64);
        if(Zone==0)Petal=FLinearColor(.96,.86,.48);
        for(int K=0;K<5;++K){float A=K*2*PI/5;Decor->Part(P+FVector(FMath::Cos(A)*9,0,FMath::Sin(A)*9),{.14,.065,.14},Petal);}
        Decor->Part(P+FVector(0,-5,0),{.105,.08,.105},FLinearColor(1,.75,.10));
    }
    // Raised polished corner studs make the board read as a real tabletop toy.
    for(int X:{-1,1})for(int Z:{-1,1})Decor->Part({-190+X*360.f,13,10+Z*360.f},{.11,.11,.11},FLinearColor(1,.80,.39));
}
void APupGameMode::BuildObstacles(){
    for(auto* P:DirtVisuals)if(P)P->Destroy();for(auto* P:CrateVisuals)if(P)P->Destroy();
    DirtVisuals.Init(nullptr,49);CrateVisuals.Init(nullptr,49);
    for(int I=0;I<49;++I){
        FVector P=Position(I);
        if(Board.Mud[I]){
            auto* D=GetWorld()->SpawnActor<APupPiece>();D->SetActorLocation(P);
            D->Part({0,11,0},{.91,.11,.91},Board.Mud[I]>1?FLinearColor(.15,.065,.022):FLinearColor(.31,.145,.046),TEXT("Cube"));
            for(int K=0;K<5;++K)D->Part({float((K*29)%68-34),-4,float((K*43)%74-37)},{.2,.06,.14},FLinearColor(.46,.24,.08));
            if(Board.Mud[I]>1)for(int K=0;K<3;++K)D->Part({-27.f+K*27,-10,-37},{.08,.04,.08},FLinearColor(.8,.51,.22));
            DirtVisuals[I]=D;
        }
        if(Board.Crates[I]){
            auto* C=GetWorld()->SpawnActor<APupPiece>();C->SetActorLocation(P);
            FLinearColor Wood(.49,.24,.095),Trim(.82,.51,.22);
            for(int Sign:{-1,1}){
                C->Part({Sign*40.f,-45,0},{.075,.12,.90},Wood,TEXT("Cube"));
                C->Part({0,-45,Sign*43.f},{.88,.12,.07},Trim,TEXT("Cube"));
                C->Part({Sign*28.f,-52,0},{.035,.06,.80},Trim,TEXT("Cube"));
            }
            C->Part({0,-57,-28},{.16,.08,.17},FLinearColor(1,.65,.075),TEXT("Cube"));
            C->Part({0,-63,-27},{.036,.025,.065},FLinearColor(.11,.055,.02));CrateVisuals[I]=C;
        }
    }
}
void APupGameMode::HitObstacles(const TSet<int32>& Hit){
    bool MudHit=false,Rescue=false;
    for(int I:Hit){
        if(Board.Mud[I]){--Board.Mud[I];MudHit=true;Effects->Burst(Position(I),7);}
        if(Board.Crates[I]){Board.Crates[I]=0;++Rescued;Rescue=true;Effects->Burst(Position(I),22,true);Floating.Add({Position(I),TEXT("RESCUED!"),FLinearColor(.58,1,.77),1.5f});}
    }
    if(MudHit||Rescue)BuildObstacles();
    if(MudHit)Sound(TEXT("Mud"),1,.35);
    if(Rescue){Sound(TEXT("Rescue"));Sound(TEXT("Bark"),1.1f,.3f);Joy=1.4f;}
}
void APupGameMode::TickPresentation(float Dt){
    if(Music)Music->SetVolumeMultiplier(bMuted||!bMusic?0:bPaused?.055f:.15f);
    if(Ambience)Ambience->SetVolumeMultiplier(bMuted?0:.4f);
    if(bPaused||bMap)return;
    Joy=FMath::Max(0.f,Joy-Dt);Shake=FMath::Max(0.f,Shake-Dt*2);
    Camera->SetWorldLocation(FVector(FMath::Sin(Age*73)*Shake*4,-2200,FMath::Cos(Age*65)*Shake*3));
    Effects->Advance(Dt);
    for(int I=Floating.Num()-1;I>=0;--I){Floating[I].Life-=Dt;Floating[I].P.Z+=Dt*28;if(Floating[I].Life<=0)Floating.RemoveAtSwap(I);}
    if(Mascot){Mascot->Animate(Age,bWon?2:Joy+.3f);FVector P=Mascot->Target;P.Z+=FMath::Abs(FMath::Sin(Age*(bWon?7:2)))*(bWon?20:3);Mascot->SetActorLocation(P);}
    if(bWon){
        float Before=WinTime;WinTime+=Dt;
        if(WinTime<5 && int(Before*3)!=int(WinTime*3))Effects->Burst({FMath::FRandRange(-490,150),-100,300},25,true);
    }
}
