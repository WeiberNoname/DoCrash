#include "PupGame.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"

namespace PupColors {
    const FLinearColor Cream(.99,.95,.83), Mint(.53,.96,.77), Muted(.57,.76,.71), Ink(.025,.13,.13), Gold(1,.70,.23), Pink(1,.48,.58);
}
void APupHUD::Box(float X,float Y,float W,float H,FLinearColor C){DrawRect(C,OX+X*Scale,OY+Y*Scale,W*Scale,H*Scale);}
void APupHUD::Text(const FString& T,float X,float Y,float Size,FLinearColor C) {
    auto* Font=LoadObject<UFont>(nullptr,TEXT("/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField")); if(!Font) Font=GEngine->GetLargeFont(); float W,H; GetTextSize(TEXT("Ag"),W,H,Font,1);
    DrawText(T,C,OX+X*Scale,OY+Y*Scale,Font,Size/FMath::Max(1.f,H)*Scale,false);
}
void APupHUD::Circle(float X,float Y,float R,FLinearColor C) {
    TArray<FCanvasUVTri> Triangles;
    for(int I=0;I<32;++I) {
        float A=I*2*PI/32,B=(I+1)*2*PI/32;
        FCanvasUVTri T;
        T.V0_Pos={OX+X*Scale,OY+Y*Scale};
        T.V1_Pos={OX+(X+FMath::Cos(A)*R)*Scale,OY+(Y+FMath::Sin(A)*R)*Scale};
        T.V2_Pos={OX+(X+FMath::Cos(B)*R)*Scale,OY+(Y+FMath::Sin(B)*R)*Scale};
        T.V0_Color=T.V1_Color=T.V2_Color=C; Triangles.Add(T);
    }
    FCanvasTriangleItem Item(Triangles,GWhiteTexture); Item.BlendMode=SE_BLEND_Translucent; Canvas->DrawItem(Item);
}
void APupHUD::Panel(float X,float Y,float W,float H,FLinearColor C,float R) {
    Box(X+R,Y,W-2*R,H,C);Box(X,Y+R,W,H-2*R,C);
    Circle(X+R,Y+R,R,C);Circle(X+W-R,Y+R,R,C);Circle(X+R,Y+H-R,R,C);Circle(X+W-R,Y+H-R,R,C);
}
void APupHUD::Button(const FString& T,float X,float Y,float W,FLinearColor C) {
    Panel(X,Y,W,54,C,16); Text(T,X+22,Y+15,22,PupColors::Ink);
}
void APupHUD::DrawHUD() {
    Super::DrawHUD(); auto* G=Cast<APupGameMode>(GetWorld()->GetAuthGameMode());if(!G)return;
    using namespace PupColors;
    Scale=FMath::Min(Canvas->SizeX/1600.f,Canvas->SizeY/1000.f);OX=(Canvas->SizeX-1600*Scale)/2;OY=(Canvas->SizeY-1000*Scale)/2;
    // Paw-print brand mark.
    Circle(117,80,13,Mint); Circle(99,61,7,Mint); Circle(113,54,7,Mint); Circle(128,56,7,Mint);Circle(139,68,7,Mint);
    Text(TEXT("pup pop"),154,47,49,Cream);
    Text(TEXT("A LITTLE MATCH. A LOT OF HAPPY."),96,119,16,Muted);
    Panel(615,48,149,44,FLinearColor(.13,.30,.28),20);Text(FString::Printf(TEXT("PARK  %02d"),G->Level),640,59,20,Mint);
    Text(TEXT("Puppy playground"),790,54,28,Cream);
    Text(G->bMuted ? TEXT("SOUND OFF [M]") : TEXT("SOUND ON [M]"),1304,64,17,Muted);
    Box(96,161,354,1,FLinearColor(.24,.41,.37));
    Text(TEXT("LET'S PLAY"),96,197,18,Mint);
    Text(TEXT("Happy pups,"),93,233,46,Cream);Text(TEXT("happy you."),93,283,46,Cream);
    Panel(96,362,354,231,Cream,26);
    Text(TEXT("YOUR SCORE"),121,387,16,Ink);Text(FString::FromInt(G->Score),119,416,53,Ink);
    Text(FString::Printf(TEXT("GOAL  %s"),*FString::FromInt(G->Goal)),121,489,17,FLinearColor(.26,.40,.35));
    Panel(121,524,302,13,FLinearColor(.77,.83,.71),6);
    const float Progress=FMath::Clamp(float(G->Score)/G->Goal,0.f,1.f);
    if(Progress>0)Panel(121,524,FMath::Max(13.f,302*Progress),13,FLinearColor(.12,.55,.38),6);
    Text(FString::Printf(TEXT("%d MOVES LEFT"),G->Moves),121,559,20,Ink);
    Text(FString::Printf(TEXT("PERSONAL BEST   %d"),G->Best),116,619,17,Muted);
    Text(TEXT("Six breeds. Endless tail wags."),105,837,18,Cream);
    Text(TEXT("Click two neighbors, or drag to swap."),615,945,18,Muted);
    Text(TEXT("3 = match    4 = row burst    5 = cross burst"),96,940,16,Muted);
    Button(TEXT("Hint  [H]"),615,875,210,Cream);Button(TEXT("Pause  [Esc]"),850,875,210,Mint);Button(TEXT("Restart  [R]"),1085,875,285,Cream);
    if(G->NoticeTime>0 && !G->bWon && !G->bLost && !G->bPaused) {
        Panel(639,87,698,29,FLinearColor(.025,.13,.13,.94),14);Text(G->Notice,660,92,18,G->Chain>1 ? Gold : Cream);
    }
    auto* PC=GetOwningPlayerController();
    for(int I=0;I<49;++I) if(I==G->Selected || I==G->HintA || I==G->HintB) {
        FVector2D P;PC->ProjectWorldLocationToScreen(G->Position(I),P);
        float X=(P.X-OX)/Scale,Y=(P.Y-OY)/Scale;
        FLinearColor C=I==G->Selected ? Gold : Mint;
        float R=46;Box(X-R,Y-R,24,3,C);Box(X-R,Y-R,3,24,C);Box(X+R-24,Y-R,24,3,C);Box(X+R-3,Y-R,3,24,C);
        Box(X-R,Y+R-3,24,3,C);Box(X-R,Y+R-24,3,24,C);Box(X+R-24,Y+R-3,24,3,C);Box(X+R-3,Y+R-24,3,24,C);
    }
    if(G->bPaused || G->bWon || G->bLost) {
        Box(-OX/Scale,-OY/Scale,Canvas->SizeX/Scale,Canvas->SizeY/Scale,FLinearColor(.012,.045,.04,.86));
        Panel(495,295,610,410,Cream,32);
        Text(G->bPaused ? TEXT("TAKE A BREATHER") : G->bWon ? TEXT("TAILS ARE WAGGING!") : TEXT("ONE MORE WALK?"),550,336,20,FLinearColor(.17,.44,.33));
        Text(G->bPaused ? TEXT("Paws for a moment.") : G->bWon ? TEXT("Paw-some work!") : TEXT("So close, good pup."),549,386,43,Ink);
        Text(FString::Printf(TEXT("%d points  /  %d goal"),G->Score,G->Goal),551,458,26,Ink);
        Text(G->bPaused ? TEXT("Your puppies will be right here.") : G->bWon ? TEXT("The next park is waiting for you.") : TEXT("A fresh board. A fresh chance."),551,512,22,Ink);
        Button(G->bPaused ? TEXT("Keep playing  [Enter]") : G->bWon ? TEXT("Next park  [Enter]") : TEXT("Try again  [Enter]"),570,580,460,Mint);
        Text(TEXT("Alt + F4 to quit"),694,656,17,FLinearColor(.31,.44,.39));
    }
}


