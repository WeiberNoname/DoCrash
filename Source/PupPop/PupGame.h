#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "PupRules.h"
#include "PupCampaign.h"
#include "PupGame.generated.h"
class UCameraComponent;
class UStaticMeshComponent;
class UAudioComponent;
class UInstancedStaticMeshComponent;

UCLASS()
class APupPiece : public AActor {
    GENERATED_BODY()
public:
    APupPiece();
    void Build(int Breed,int Special=0);
    UStaticMeshComponent* Part(FVector P,FVector S,FLinearColor C,const TCHAR* Shape=TEXT("Sphere"),FRotator R=FRotator::ZeroRotator);
    void MarkSpecial(int Value);
    void Animate(float Time,float Joy);
    UPROPERTY() TArray<UStaticMeshComponent*> Parts;
    UPROPERTY() TArray<UStaticMeshComponent*> Eyes;
    UPROPERTY() TArray<UStaticMeshComponent*> Ears;
    UPROPERTY() UStaticMeshComponent* Tail=nullptr;
    UPROPERTY() UStaticMeshComponent* Badge=nullptr;
    FVector Target;
    float Phase=0;
};
struct FPupSpark {
    FVector P,V; FRotator Rotation,Spin; float Life=0,MaxLife=1,Size=1; int Pool=0;
};
UCLASS()
class APupEffects : public AActor {
    GENERATED_BODY()
public:
    APupEffects();
    void Init(); void Burst(FVector P,int Count,bool Confetti=false); void Advance(float Dt);
    UPROPERTY() TArray<UInstancedStaticMeshComponent*> Pools;
    TArray<FPupSpark> Sparks;
};
struct FPupFloat { FVector P; FString Text; FLinearColor Color; float Life=1.2f; };
UCLASS()
class APupController : public APlayerController {
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    void Press();void Release();void Hint();void Restart();void PauseGame();void Confirm();void Mute();void Map();void MusicToggle();
    void Left();void Right();void Up();void Down();void Pick();
    int DownCell=-1,Cursor=24;
};
UCLASS()
class APupGameMode : public AGameModeBase {
    GENERATED_BODY()
public:
    APupGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float Delta) override;
    FPupBoard Board;
    FPupLevel Spec=FPupLevel::Get(1);
    UPROPERTY() TArray<APupPiece*> Puppies;
    UPROPERTY() TArray<APupPiece*> Popping;
    UPROPERTY() TArray<APupPiece*> DirtVisuals;
    UPROPERTY() TArray<APupPiece*> CrateVisuals;
    UPROPERTY() APupPiece* Decor=nullptr;
    UPROPERTY() APupPiece* Mascot=nullptr;
    UPROPERTY() APupEffects* Effects=nullptr;
    UPROPERTY() UCameraComponent* Camera=nullptr;
    UPROPERTY() UAudioComponent* Music=nullptr;
    UPROPERTY() UAudioComponent* Ambience=nullptr;
    int Score=0,Moves=22,Level=1,Goal=1800,Best=0,Selected=-1,HintA=-1,HintB=-1;
    int Chain=0,SwapA=-1,SwapB=-1,State=0,LastGain=0,Collected=0,Rescued=0,Unlocked=1,MapSelection=1;
    TArray<int32> Stars;
    TArray<FPupFloat> Floating;
    float Timer=0,Age=0,NoticeTime=0,Idle=0,Joy=0,WinTime=0,Shake=0;
    bool bPaused=false,bWon=false,bLost=false,bMuted=false,bMap=true,bMusic=true;
    FString Notice;
    FVector Position(int Index) const;
    int HitCell() const;
    static FVector2D MapNode(int N);
    void Select(int Index);void TrySwap(int A,int B);
    void Resolve();void Collapse();void Settle();
    void NewLevel(bool Next=false);void StartLevel(int Number);void ShowHint();void Sound(const TCHAR* Name,float Pitch=1,float Volume=.45f);
    void SaveBest();void HandleClick();void ClickAt(float X,float Y);void ToggleMap();void ConfirmAction();
    void BuildScene();void BuildObstacles();void HitObstacles(const TSet<int32>& Hit);void TickPresentation(float Dt);
    bool ObjectivesDone() const; FString GoalText() const; void FinishLevel();
    void TickVerification(float Dt);
    bool IsVerifying() const {return bVerify;}
private:
    bool bVerify=false;int VerifyStage=0,VerifyMoves=0;float VerifyTime=0,StageTime=0;
};
UCLASS()
class APupHUD : public AHUD {
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
    float Scale=1,OX=0,OY=0;
    void Box(float X,float Y,float W,float H,FLinearColor C);
    void Text(const FString& T,float X,float Y,float Size,FLinearColor C);
    void Circle(float X,float Y,float R,FLinearColor C);
    void Panel(float X,float Y,float W,float H,FLinearColor C,float Radius=20);
    void Button(const FString& T,float X,float Y,float W,FLinearColor C);
    void Star(float X,float Y,float R,FLinearColor C);
    void DrawMap(APupGameMode* G);void DrawGoals(APupGameMode* G);void DrawOverlay(APupGameMode* G);
};
