#include "Playoff.h"  // runPlayoff() 함수 선언
#include "Match.h"    // playoffTiebreak() (승부차기)
#include "Utils.h"    // randInt(), printLine(), waitEnter()
#include <iostream>   // std::cout
#include <algorithm>  // (현재 직접 사용하지 않음)


// 플레이오프진행
std::string runPlayoff(League& league) {
    printLine();
    std::cout << "*** 플레이오프 시작 ***\n";
    printLine();
	
    auto sorted = league.getSortedTeamIndices();

	// 5위까지 플렝이오프
    int survivorLeagueIdx = sorted[5];
	
    for (int step = 4; step >= 0; --step) {
        int challengerLeagueIdx = sorted[step];
    	
        Team& survivor   = league.teams[survivorLeagueIdx];
        Team& challenger = league.teams[challengerLeagueIdx];

        // 경기 정보 출력
        switch(step) {
			case 4: std::cout << "\n[와일드카드] " << survivor.name;
					break;
			case 3: std::cout << "\n[준플레이오프] " << survivor.name;
					break;
			case 2: std::cout << "\n[플레이오프] " << survivor.name;
					break;
			case 1: std::cout << "\n[준결승] " << survivor.name;
					break;
			case 0: std::cout << "\n[챔피언 결정전] " << survivor.name;
		}
			
		
            std::cout << " vs " << challenger.name << "\n";

        // 경기 진행
        int sMax   = survivor.calcMaxGoals(challenger);
        int cMax   = challenger.calcMaxGoals(survivor);
        int sGoals = randInt(sMax);
        int cGoals = randInt(cMax);

        // 스코어 출력
        std::cout << survivor.name << " " << sGoals
                  << " : " << cGoals << " " << challenger.name << "\n";

        // 승패 판정
        if (sGoals > cGoals) {
            std::cout << survivor.name << " 승리!\n";
        } else if (sGoals < cGoals) {
            std::cout << challenger.name << " 승리!\n";
            survivorLeagueIdx = challengerLeagueIdx;  // survivor 교체
        } else {
            // 득점이 같으면 승부차기 진행
            bool survivorWins = playoffTiebreak(survivor.name, challenger.name);
            if (!survivorWins)
                survivorLeagueIdx = challengerLeagueIdx;
        }
        waitEnter();
    }

    // 우승
    std::string champion = league.teams[survivorLeagueIdx].name;

    printLine();
    std::cout << "시즌 최종 우승: " << champion << "\n";
    printLine();

    return champion;  // 우승 팀 이름 반환
}
