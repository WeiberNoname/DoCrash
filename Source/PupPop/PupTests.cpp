#include "PupRules.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPupRulesTest,"PupPop.Rules.MatchAndSpecials",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FPupRulesTest::RunTest(const FString& Parameters) {
    TestFalse(TEXT("Rows do not wrap"),FPupBoard::Adjacent(6,7));
    TestFalse(TEXT("Out of bounds rejected"),FPupBoard::Adjacent(-1,0));
    TestTrue(TEXT("Vertical neighbors"),FPupBoard::Adjacent(7,14));
    for(int Seed=0;Seed<100;++Seed) {
        FPupBoard B(Seed); B.Generate(); int A,C;
        TestTrue(TEXT("Fresh board has a legal move"),B.FindMove(A,C));
        TestTrue(TEXT("No free matches on fresh board"),B.Matches().IsEmpty());
        auto Original=B.Cells; TestTrue(TEXT("Reported hint is legal"),B.CanSwap(A,C));
        for(int I=0;I<49;++I)TestEqual(TEXT("Move search preserves cells"),B.Cells[I].Breed,Original[I].Breed);
        Swap(B.Cells[A],B.Cells[C]);TestFalse(TEXT("Legal swap creates match"),B.Matches().IsEmpty());
    }
    FPupBoard B; for(int I=0;I<49;++I)B.Cells[I]={-1,0};
    B.Cells[0]={2,0};B.Cells[1]={2,0};B.Cells[2]={2,0};
    TestEqual(TEXT("Horizontal triple"),B.Matches().Num(),1);
    B.Cells[1].Special=1;
    TestEqual(TEXT("Row special clears all seven"),B.Expanded(B.Matches()).Num(),7);
    B.Cells[1].Special=2;
    TestEqual(TEXT("Cross special clears thirteen"),B.Expanded(B.Matches()).Num(),13);
    B.Cells[8].Special=1;
    TestEqual(TEXT("Specials trigger other specials"),B.Expanded(B.Matches()).Num(),19);
    for(int I=0;I<49;++I)B.Cells[I]={-1,0};
    B.Cells[5]={1,0};B.Cells[6]={1,0};B.Cells[7]={1,0};
    TestTrue(TEXT("No row-wrap match"),B.Matches().IsEmpty());
    B.Cells[5]={-1,0};B.Cells[6]={-1,0};B.Cells[7]={3,0};B.Cells[14]={3,0};B.Cells[21]={3,0};
    TestEqual(TEXT("Vertical triple"),B.Matches().Num(),1);
    return true;
}
#endif
