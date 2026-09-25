#include "PupGame.h"
#include "Camera/CameraComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/CommandLine.h"
#include "HighResScreenshot.h"
#include "Sound/SoundBase.h"
#include "HAL/PlatformMisc.h"

APupGameMode::APupGameMode() {
    PrimaryActorTick.bCanEverTick=true;
    PlayerControllerClass=APupController::StaticClass(); HUDClass=APupHUD::StaticClass(); DefaultPawnClass=nullptr;
}
FVector APupGameMode::Position(int I) const {return FVector(-190-(I%7-3)*100,0,(3-I/7)*100+10);}
void APupGameMode::BeginPlay() {
    Super::BeginPlay();
    Board.Random.Initialize(FMath::Rand());
    bVerify=FParse::Param(FCommandLine::Get(),TEXT("PupVerify"));
    if(bVerify) Board.Random.Initialize(74921);
    FString Saved; if(FFileHelper::LoadFileToString(Saved,*(FPaths::ProjectSavedDir()/TEXT("BestScore.txt")))) Best=FCString::Atoi(*Saved);
    auto* View=GetWorld()->SpawnActor<AActor>();
    Camera=NewObject<UCameraComponent>(View); View->SetRootComponent(Camera); Camera->RegisterComponent();
    Camera->ProjectionMode=ECameraProjectionMode::Orthographic; Camera->OrthoWidth=1600; Camera->bConstrainAspectRatio=false; Camera->bOverrideAspectRatioAxisConstraint=true; Camera->AspectRatioAxisConstraint=AspectRatio_MaintainXFOV; Camera->bAutoCalculateOrthoPlanes=false; Camera->OrthoNearClipPlane=1; Camera->OrthoFarClipPlane=5000;
    Camera->SetWorldLocation(FVector(0,-2200,0)); Camera->SetWorldRotation(FRotator(0,90,0));
    GetWorld()->GetFirstPlayerController()->SetViewTarget(View);
    auto* Key=GetWorld()->SpawnActor<ADirectionalLight>(); Key->GetLightComponent()->SetMobility(EComponentMobility::Movable); Key->SetActorRotation(FRotator(-35,65,0));
    Key->GetLightComponent()->SetIntensity(3.0f); Key->GetLightComponent()->SetLightColor(FLinearColor(1,.86,.73));
    auto* Fill=GetWorld()->SpawnActor<ADirectionalLight>(); Fill->GetLightComponent()->SetMobility(EComponentMobility::Movable); Fill->SetActorRotation(FRotator(25,120,0));
    Fill->GetLightComponent()->SetIntensity(1.6f); Fill->GetLightComponent()->SetCastShadows(false);
    Fill->GetLightComponent()->SetLightColor(FLinearColor(.65,.83,1));
    auto* PP=GetWorld()->SpawnActor<APostProcessVolume>(); PP->bUnbound=true;
    PP->Settings.bOverride_AutoExposureMethod=true; PP->Settings.AutoExposureMethod=EAutoExposureMethod::AEM_Manual;
    PP->Settings.bOverride_AutoExposureApplyPhysicalCameraExposure=true; PP->Settings.AutoExposureApplyPhysicalCameraExposure=false; PP->Settings.bOverride_AutoExposureBias=true; PP->Settings.AutoExposureBias=0;
    PP->Settings.bOverride_VignetteIntensity=true; PP->Settings.VignetteIntensity=.2f;
    auto* Decor=GetWorld()->SpawnActor<APupPiece>();
    Decor->Part({0,150,0},{32,1,22},FLinearColor(.024,.105,.108),TEXT("Cube"));
    Decor->Part({-190,75,10},{7.38,.45,7.38},FLinearColor(.18,.38,.36),TEXT("Cube"));
    Decor->Part({-190,47,10},{7.18,.25,7.18},FLinearColor(.075,.23,.23),TEXT("Cube"));
    for(int I=0;I<49;++I) {
        FVector P=Position(I); P.Y=28;
        Decor->Part(P,{.94,.24,.94},(I+I/7)%2 ? FLinearColor(.14,.33,.31) : FLinearColor(.12,.29,.28),TEXT("Cube"));
    }
    // Oversized companion puppy beside the scorecard.
    auto* Mascot=GetWorld()->SpawnActor<APupPiece>(); Mascot->Build(0); Mascot->SetActorLocation({510,-35,-245}); Mascot->SetActorScale3D(FVector(2.0));
    NewLevel();
}
void APupGameMode::Sound(const TCHAR* Name,float Pitch) {
    if(bMuted) return;
    if(auto* S=LoadObject<USoundBase>(nullptr,*FString::Printf(TEXT("/Game/Audio/%s.%s"),Name,Name))) UGameplayStatics::PlaySound2D(this,S,.38f,Pitch);
}
void APupGameMode::NewLevel(bool Next) {
    for(auto* P:Puppies) if(IsValid(P)) P->Destroy(); for(auto* P:Popping) if(IsValid(P)) P->Destroy();
    Puppies.Empty(); Popping.Empty();
    if(Next) ++Level;
    Score=0; Moves=FMath::Max(18,25-Level); Goal=2600+(Level-1)*650; Selected=-1; State=0; Chain=0; HintA=HintB=-1;
    bWon=bLost=bPaused=false; Timer=0; Idle=0;
    Notice=Next ? TEXT("A fresh park. A new adventure!") : TEXT("Swap neighbors. Match 3 puppies."); NoticeTime=4;
    Board.Generate();
    for(int I=0;I<49;++I) {
        auto* P=GetWorld()->SpawnActor<APupPiece>(); P->Build(Board.Cells[I].Breed); P->Target=Position(I);
        P->SetActorLocation(P->Target); P->Phase=Board.Random.FRand()*6.28f; Puppies.Add(P);
    }
}
int APupGameMode::HitCell() const {
    auto* PC=GetWorld()->GetFirstPlayerController(); float X,Y; if(!PC->GetMousePosition(X,Y)) return -1;
    for(int I=0;I<49;++I) {
        FVector2D S, Edge; PC->ProjectWorldLocationToScreen(Position(I),S); PC->ProjectWorldLocationToScreen(Position(I)+FVector(52,0,52),Edge);
        if(FMath::Abs(X-S.X)<FMath::Abs(Edge.X-S.X) && FMath::Abs(Y-S.Y)<FMath::Abs(Edge.Y-S.Y)) return I;
    }
    return -1;
}
void APupGameMode::Select(int I) {
    if(State || bPaused || bWon || bLost || I<0) return;
    Idle=0; HintA=HintB=-1;
    if(Selected==I) {Selected=-1;return;}
    if(FPupBoard::Adjacent(Selected,I)) {TrySwap(Selected,I);return;}
    Selected=I; Sound(TEXT("Select"));
}
void APupGameMode::TrySwap(int A,int B) {
    if(State || bPaused || bWon || bLost || !FPupBoard::Adjacent(A,B)) return;
    SwapA=A; SwapB=B; Selected=-1; HintA=HintB=-1; Chain=0; Idle=0;
    Swap(Board.Cells[A],Board.Cells[B]); Swap(Puppies[A],Puppies[B]);
    Puppies[A]->Target=Position(A); Puppies[B]->Target=Position(B);
    State=1; Timer=.24f; Sound(TEXT("Swap"));
}
void APupGameMode::Resolve() {
    auto Runs=Board.Matches();
    if(Runs.IsEmpty()) {Settle();return;}
    ++Chain;
    auto Hit=Board.Expanded(Runs);
    TMap<int,int> Specials;
    for(const auto& R:Runs) if(R.Cells.Num()>=4) {
        int At=R.Cells.Contains(SwapB) ? SwapB : R.Cells[R.Cells.Num()/2];
        if(Board.Cells[At].Special==0) Specials.Add(At,R.Cells.Num()>=5 ? 2 : 1);
    }
    LastGain=Hit.Num()*60*Chain;
    Score+=LastGain; SaveBest();
    for(int I:Hit) {
        if(Specials.Contains(I)) {Board.Cells[I].Special=Specials[I]; Puppies[I]->MarkSpecial(Specials[I]);continue;}
        Board.Cells[I]={-1,0}; Popping.Add(Puppies[I]); Puppies[I]=nullptr;
    }
    Notice=Chain>1 ? FString::Printf(TEXT("PAW-SOME!  x%d cascade    +%d"),Chain,LastGain) : FString::Printf(TEXT("Good dogs!    +%d"),LastGain);
    if(!Specials.IsEmpty()) Notice=TEXT("SUPER PUP! Match its gold badge to burst.");
    NoticeTime=2.5f; Sound(TEXT("Pop"),FMath::Min(1.f+Chain*.12f,1.8f)); State=3; Timer=.30f;
}
void APupGameMode::Collapse() {
    for(auto* P:Popping) if(IsValid(P)) P->Destroy(); Popping.Empty();
    for(int C=0;C<7;++C) {
        int Dest=6;
        for(int R=6;R>=0;--R) if(Board.Cells[R*7+C].Breed>=0) {
            int From=R*7+C,To=Dest*7+C;
            if(From!=To) {Board.Cells[To]=Board.Cells[From];Puppies[To]=Puppies[From];Board.Cells[From]={-1,0};Puppies[From]=nullptr;}
            Puppies[To]->Target=Position(To); --Dest;
        }
        for(int R=Dest;R>=0;--R) {
            int I=R*7+C; Board.Cells[I]={Board.Random.RandRange(0,5),0};
            auto* P=GetWorld()->SpawnActor<APupPiece>(); P->Build(Board.Cells[I].Breed); P->Target=Position(I);
            P->SetActorLocation(P->Target+FVector(0,0,(Dest+1)*106)); P->Phase=Board.Random.FRand()*6.28f; Puppies[I]=P;
        }
    }
    State=4; Timer=.48f;
}
void APupGameMode::Settle() {
    State=0; SwapA=SwapB=-1;
    if(Score>=Goal) {bWon=true; Sound(TEXT("Win"));return;}
    if(Moves<=0) {bLost=true; Sound(TEXT("Swap"),.7f);return;}
    int A,B;
    if(!Board.FindMove(A,B)) {
        Board.Generate();
        for(int I=0;I<49;++I) {Puppies[I]->Destroy(); auto* P=GetWorld()->SpawnActor<APupPiece>();P->Build(Board.Cells[I].Breed);P->Target=Position(I);P->SetActorLocation(P->Target);Puppies[I]=P;}
        Notice=TEXT("A little reshuffle. Your moves are safe!"); NoticeTime=4;
    }
}
void APupGameMode::ShowHint() {
    if(State || bPaused || bWon || bLost) return;
    Board.FindMove(HintA,HintB); Idle=0; Notice=TEXT("Try swapping the two highlighted pups."); NoticeTime=4;
}
void APupGameMode::SaveBest() {
    if(Score<=Best || bVerify) return; Best=Score;
    FFileHelper::SaveStringToFile(FString::FromInt(Best),*(FPaths::ProjectSavedDir()/TEXT("BestScore.txt")));
}
void APupGameMode::HandleClick() {
    auto* PC=GetWorld()->GetFirstPlayerController(); int W,H; PC->GetViewportSize(W,H); float X,Y; PC->GetMousePosition(X,Y);
    float S=FMath::Min(W/1600.f,H/1000.f); X=(X-(W-1600*S)/2)/S; Y=(Y-(H-1000*S)/2)/S;
    if(bWon || bLost || bPaused) {
        if(X>=570 && X<=1030 && Y>=570 && Y<=635) {if(bPaused) bPaused=false; else NewLevel(bWon);} return;
    }
    if(Y>=875 && Y<=929) {
        if(X>=615 && X<=825) ShowHint();
        else if(X>=850 && X<=1060) {bPaused=true;}
        else if(X>=1085 && X<=1370 && !State) NewLevel();
        return;
    }
    if(X>1300 && Y<110) {bMuted=!bMuted;return;}
    Select(HitCell());
}
void APupGameMode::Tick(float Dt) {
    Super::Tick(Dt); Age+=Dt; GetWorld()->GetFirstPlayerController()->SetViewTarget(Camera->GetOwner());
    int W,H; GetWorld()->GetFirstPlayerController()->GetViewportSize(W,H);
    if(H>0) Camera->OrthoWidth=FMath::Max(1600.f,1000.f*W/H);
    if(!bPaused) {
        NoticeTime=FMath::Max(0.f,NoticeTime-Dt); Idle+=Dt;
        for(int I=0;I<Puppies.Num();++I) if(auto* P=Puppies[I]) {
            FVector T=P->Target;
            if(State==0) T.Z+=FMath::Sin(Age*2+P->Phase)*1.6f;
            P->SetActorLocation(FMath::VInterpTo(P->GetActorLocation(),T,Dt,17));
            float S=I==Selected ? 1.12f : 1.f; P->SetActorScale3D(FMath::VInterpTo(P->GetActorScale3D(),FVector(S),Dt,15));
            P->SetActorRotation(FRotator(0,0,State==0 ? FMath::Sin(Age*1.8f+P->Phase)*1.8f : 0));
        }
        for(auto* P:Popping) if(P) {P->AddActorLocalRotation(FRotator(0,0,Dt*260));P->SetActorScale3D(FVector(FMath::Max(.03f,Timer/.30f)));}
        if(State) {
            Timer-=Dt;
            if(Timer<=0) {
                if(State==1) {
                    if(Board.Matches().IsEmpty()) {
                        Swap(Board.Cells[SwapA],Board.Cells[SwapB]);Swap(Puppies[SwapA],Puppies[SwapB]);
                        Puppies[SwapA]->Target=Position(SwapA);Puppies[SwapB]->Target=Position(SwapB);
                        State=2;Timer=.24f;Notice=TEXT("Find a match of 3. No move lost!");NoticeTime=2;Sound(TEXT("Swap"),.65f);
                    } else {--Moves;Resolve();}
                } else if(State==2) Settle(); else if(State==3) Collapse(); else if(State==4) Resolve();
            }
        } else if(Idle>12 && !bWon && !bLost) ShowHint();
    }
    if(bVerify) {
        VerifyTime+=Dt;
        if(VerifyTime>75) {UE_LOG(LogTemp,Error,TEXT("PUP_VERIFY_TIMEOUT"));FPlatformMisc::RequestExitWithStatus(false,1);}
        if(VerifyStage==0 && Age>3) {
            FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("PupPop-Board.png"),false,false);
            check(Board.Matches().IsEmpty()); check(Puppies.Num()==49); VerifyStage=1;
        } else if(VerifyStage==1 && !State && Age>4) {
            int A=-1,B=-1;
            for(int I=0;I<48;++I) if(FPupBoard::Adjacent(I,I+1)&&!Board.CanSwap(I,I+1)){A=I;B=I+1;break;}
            check(A>=0); TrySwap(A,B); VerifyStage=2;
        } else if(VerifyStage==2 && !State) {check(Moves==24);check(Score==0);VerifyStage=3;}
        else if(VerifyStage==3 && !State) {
            check(Board.Matches().IsEmpty()); for(auto* P:Puppies) check(IsValid(P));
            if(bWon || bLost || VerifyMoves>=10) {
                check(Score>0); check(Moves==24-VerifyMoves);
                FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("PupPop-Played.png"),false,false);
                UE_LOG(LogTemp,Display,TEXT("PUP_VERIFY_OK moves=%d score=%d won=%d"),VerifyMoves,Score,bWon); VerifyStage=4; Timer=1;
            } else {int A,B;check(Board.FindMove(A,B));TrySwap(A,B);++VerifyMoves;}
        } else if(VerifyStage==4) {Timer-=Dt;if(Timer<0)FPlatformMisc::RequestExit(false);}
    }
}






