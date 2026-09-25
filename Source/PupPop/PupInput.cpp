#include "PupGame.h"
#include "Engine/World.h"
#include "Components/InputComponent.h"
static APupGameMode* Game(APlayerController* PC){return Cast<APupGameMode>(PC->GetWorld()->GetAuthGameMode());}
void APupController::BeginPlay(){Super::BeginPlay();bShowMouseCursor=true;DefaultMouseCursor=EMouseCursor::Default;SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));}
void APupController::SetupInputComponent(){
    Super::SetupInputComponent();
    InputComponent->BindKey(EKeys::LeftMouseButton,IE_Pressed,this,&APupController::Press);
    InputComponent->BindKey(EKeys::LeftMouseButton,IE_Released,this,&APupController::Release);
    InputComponent->BindKey(EKeys::H,IE_Pressed,this,&APupController::Hint);
    InputComponent->BindKey(EKeys::R,IE_Pressed,this,&APupController::Restart);
    InputComponent->BindKey(EKeys::Escape,IE_Pressed,this,&APupController::PauseGame);
    InputComponent->BindKey(EKeys::Enter,IE_Pressed,this,&APupController::Confirm);
    InputComponent->BindKey(EKeys::M,IE_Pressed,this,&APupController::Mute);
    InputComponent->BindKey(EKeys::Left,IE_Pressed,this,&APupController::Left);
    InputComponent->BindKey(EKeys::Right,IE_Pressed,this,&APupController::Right);
    InputComponent->BindKey(EKeys::Up,IE_Pressed,this,&APupController::Up);
    InputComponent->BindKey(EKeys::Down,IE_Pressed,this,&APupController::Down);
    InputComponent->BindKey(EKeys::SpaceBar,IE_Pressed,this,&APupController::Pick);
}
void APupController::Press(){if(auto* G=Game(this)){DownCell=G->HitCell();if(G->bWon||G->bLost||G->bPaused)DownCell=-1;G->HandleClick();}}
void APupController::Release(){if(auto* G=Game(this)){int UpCell=G->HitCell();if(DownCell>=0 && UpCell!=DownCell)G->TrySwap(DownCell,UpCell);}DownCell=-1;}
void APupController::Hint(){if(auto* G=Game(this))G->ShowHint();}
void APupController::Restart(){if(auto* G=Game(this))if(!G->State)G->NewLevel();}
void APupController::PauseGame(){if(auto* G=Game(this))if(!G->bWon&&!G->bLost)G->bPaused=!G->bPaused;}
void APupController::Confirm(){if(auto* G=Game(this)){if(G->bPaused)G->bPaused=false;else if(G->bWon||G->bLost)G->NewLevel(G->bWon);else Pick();}}
void APupController::Mute(){if(auto* G=Game(this))G->bMuted=!G->bMuted;}
void APupController::Left(){Cursor=FMath::Max(0,Cursor-1);if(auto* G=Game(this))G->HintA=Cursor;}
void APupController::Right(){Cursor=FMath::Min(48,Cursor+1);if(auto* G=Game(this))G->HintA=Cursor;}
void APupController::Up(){Cursor=FMath::Max(0,Cursor-7);if(auto* G=Game(this))G->HintA=Cursor;}
void APupController::Down(){Cursor=FMath::Min(48,Cursor+7);if(auto* G=Game(this))G->HintA=Cursor;}
void APupController::Pick(){if(auto* G=Game(this))G->Select(Cursor);}
