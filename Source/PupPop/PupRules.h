#pragma once
#include "CoreMinimal.h"

struct FPupCell {
    int32 Breed = -1;
    int32 Special = 0; // 1: row burst, 2: row and column burst
};
struct FPupMatch { TArray<int32> Cells; };
struct FPupBoard {
    static constexpr int32 Side = 7, Count = 49, Breeds = 6;
    TArray<FPupCell> Cells; TArray<int32> Mud, Crates;
    FRandomStream Random;
    FPupBoard(int32 Seed = 12345) : Random(Seed) { Cells.SetNum(Count); Mud.Init(0,Count); Crates.Init(0,Count); }
    static bool Adjacent(int32 A, int32 B) {
        return A>=0 && B>=0 && A<Count && B<Count && FMath::Abs(A/Side-B/Side)+FMath::Abs(A%Side-B%Side)==1;
    }
    TArray<FPupMatch> Matches() const {
        TArray<FPupMatch> Out;
        for(int Axis=0; Axis<2; ++Axis) for(int Line=0; Line<Side; ++Line) {
            int Start=0;
            while(Start<Side) {
                auto Index=[&](int P){return Axis ? P*Side+Line : Line*Side+P;};
                int End=Start+1, Breed=Cells[Index(Start)].Breed;
                while(End<Side && Cells[Index(End)].Breed==Breed) ++End;
                if(Breed>=0 && End-Start>=3) {
                    FPupMatch M; for(int P=Start;P<End;++P) M.Cells.Add(Index(P)); Out.Add(M);
                }
                Start=End;
            }
        }
        return Out;
    }
    bool CanSwap(int A,int B) {
        if(!Adjacent(A,B) || Crates[A] || Crates[B]) return false;
        Swap(Cells[A],Cells[B]); bool Valid=false; for(const auto& R:Matches()) if(R.Cells.Contains(A)||R.Cells.Contains(B)) Valid=true; Swap(Cells[A],Cells[B]); return Valid;
    }
    bool FindMove(int& A,int& B) {
        for(int I=0;I<Count;++I) for(int D : {1,Side}) if(CanSwap(I,I+D)) {A=I; B=I+D; return true;}
        A=B=-1; return false;
    }
    void Obstacles(int Level,int MudCount,int CrateCount,int Layers) {
        Mud.Init(0,Count); Crates.Init(0,Count);
        TArray<int32> Order; for(int I=0;I<Count;++I)Order.Add(I);
        FRandomStream Layout(9301+Level*117);
        for(int I=Count-1;I>0;--I) Swap(Order[I],Order[Layout.RandRange(0,I)]);
        for(int I=0;I<MudCount;++I)Mud[Order[I]]=Layers;
        for(int I=0;I<CrateCount;++I)Crates[Order[(I+MudCount)%Count]]=1;
    }
    int DirtLeft() const {int N=0;for(int M:Mud)if(M>0)++N;return N;}
    int CratesLeft() const {int N=0;for(int C:Crates)N+=C;return N;}
    void Generate() {
        int A,B;
        do {
            for(int I=0;I<Count;++I) {
                int BNew;
                do {BNew=Random.RandRange(0,Breeds-1);} while(
                    (I%Side>=2 && Cells[I-1].Breed==BNew && Cells[I-2].Breed==BNew) ||
                    (I>=Side*2 && Cells[I-Side].Breed==BNew && Cells[I-Side*2].Breed==BNew));
                Cells[I]={BNew,0};
            }
        } while(!FindMove(A,B));
    }
    TSet<int32> Expanded(const TArray<FPupMatch>& Runs) const {
        TSet<int32> Hit;
        TArray<int32> Queue;
        for(const auto& Run:Runs) for(int I:Run.Cells) if(!Hit.Contains(I)) {Hit.Add(I);Queue.Add(I);}
        for(int Q=0;Q<Queue.Num();++Q) {
            int I=Queue[Q];
            if(!Cells[I].Special) continue;
            for(int P=0;P<Side;++P) {
                int R=(I/Side)*Side+P;
                if(!Hit.Contains(R)) {Hit.Add(R);Queue.Add(R);}
                if(Cells[I].Special==2) {R=P*Side+I%Side; if(!Hit.Contains(R)){Hit.Add(R);Queue.Add(R);}}
            }
        }
        return Hit;
    }
};

