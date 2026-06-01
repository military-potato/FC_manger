

#include "Game.h"     // Game 클래스 선언
#include "Trade.h"    // runTrades() (트레이드 진행)
#include "Playoff.h"  // runPlayoff() (플레이오프 진행)
#include "Utils.h"    // printLine(), waitEnter()
#include <iostream>   // std::cout, std::cin
#include <string>     // std::string, std::getline()
#include <algorithm>  // (현재 직접 사용하지 않음)
#include <cctype>     // tolower() (영어 소문자 변환에 사용)



static std::string toLower(std::string s) {
    for (auto& c : s)                       // 문자열의 각 문자를 순회
        c = (char)tolower((unsigned char)c); // ASCII 문자만 소문자로 변환
    return s;
}

int tradeCount = 0;

// Game 기본 생성자
Game::Game()
    : lastMatchResult(MatchResult::DRAW), hasPlayedMatch(false) {}

// 게임 시작
void Game::start() {
    printLine();
    std::cout << "  Rising Eleven\n";
    std::cout << "눈떠보니 과거로 돌아간 당신. 선수의 능력이 보이는 능력을 얻게 되는데...\n다시 주어진 기회, 리그 우승을 노려보세요!\n";
    printLine();

    // 팀 이름 입력
    std::cout << "팀 이름을 입력하세요: ";
    std::getline(std::cin, playerTeamName);

    // 아무것도 입력하지 않으면 기본 이름
    if (playerTeamName.empty()) playerTeamName = "Player";

    // 리그 초기화: 플레이어 팀 + AI 팀 19개 생성, 경기 일정 생성
    league.init(playerTeamName);

    std::cout << "\n[" << playerTeamName << "] 창단 완료!\n";
    std::cout << "선수 10명이 자동으로 배정되었습니다.\n";
    waitEnter();
}

// 게임 메인 루프
void Game::run() {
    start();
    
    while (true) {
        showMainMenu();
        
        std::cout << "명령어 입력: ";
        std::string cmd;
        if (!std::getline(std::cin, cmd)) break;
        
        std::string cmdLower = toLower(cmd);
        
        if (cmd == "1") {
            // 리그가 이미 끝났으면 안내 메시지만 출력
            if (league.isFinished()) {
                std::cout << "모든 리그 경기가 종료되었습니다.\n";
            } else {
                doSchedule();  // 다음 경기 진행
            }
        } else if (cmd == "2") {
            doRoster();        // 선수 명단/포메이션 확인 및 변경
        } else if (cmd == "3") {
            doTrade();         // 선수 트레이드
        } else if (cmd == "4") {
            league.printStandings();  // 현재 순위표 출력
            waitEnter();
        } else if (cmd == "5") {
            std::cout << "\n게임을 종료합니다. 진행 상황은 저장되지 않습니다.\n";
            break;             // 게임 종료
        } else {
            // 예외처리
            std::cout << "알 수 없는 명령어입니다.\n";
        }

        // 리그가 끝났는지 확인
        if (league.isFinished()) {
            std::cout << "\n리그전이 모두 종료되었습니다!\n";
            league.printStandings();  // 최종 리그 순위표
            waitEnter();

            std::cout << "플레이오프를 시작합니다.\n";
            waitEnter();

            // 플레이오프 진행 (우승 팀 이름 반환)
            std::string champion = runPlayoff(league);

            waitEnter();
            
            std::cout << "\n새 시즌을 시작하시겠습니까?\n";
            std::cout << "(1) 현재 팀으로 새 시즌  (2) 게임 종료\n입력: ";
            std::string ans;
            std::getline(std::cin, ans);

            if (ans == "1") {
                // 새 시즌: 선수 명단/포메이션/전술은 유지하고 성적만 초기화
				tradeCount = 0;
                // 현재 플레이어 팀 데이터 백업
                std::vector<Player> savedRoster     = league.teams[0].roster;
                int                 savedFormation  = league.teams[0].formationIndex;  
                Tactic              savedTactic      = league.teams[0].tactic;          

                // 리그 전체 초기화 (AI 팀 새로 생성, 일정 새로 생성)
                league.init(playerTeamName);

                // 팀 데이터 복원
                league.teams[0].roster         = savedRoster;
                league.teams[0].formationIndex = savedFormation;
                league.teams[0].tactic         = savedTactic;
                league.teams[0].applyFormation();

                hasPlayedMatch = false;

                std::cout << "\n새 시즌이 시작되었습니다!\n";
                waitEnter();
            } else {
                // 새 시즌을 선택하지 않으면 게임 종료
                std::cout << "\n게임을 종료합니다.\n";
                break;
            }
        }
    }
}



// 메인 메뉴 출력
void Game::showMainMenu() {
    printLine();
    
    int played = league.playedMatches();  // 지금까지 진행한 경기 수
    std::cout << "【" << playerTeamName << "】"
              << "  리그 " << played << "/" << league.totalMatches() << "경기"
              << "  |  순위: " << league.playerRank() << "위"   
              << "  |  승점: " << league.teams[0].displayPoints() << "\n";

    // 다음 상대 표시
    if (!league.isFinished()) {
        std::string opp = league.nextOpponentName();  // 다음 경기 상대 이름
        if (!opp.empty())
            std::cout << "다음 상대: " << opp << "\n";
    }

    printLine();
    
    std::cout << " (1) 일정     - 다음 경기 진행\n";
    std::cout << " (2) 로스터   - 선수 확인 및 포메이션 변경\n";
    std::cout << " (3) 트레이드 - 선수 트레이드\n";
    std::cout << " (4) 순위     - 현재 순위표\n";
    std::cout << " (5) 종료     - 게임 종료\n";
    printLine();
}

// 3가지 전술을 출력하고 플레이어가 번호를 입력해 선택하게 한다.

Tactic Game::chooseTactic() {
    printLine();
    std::cout << "전술을 선택하세요:\n";
    std::cout << "  1. 공격적 전술\n";
    std::cout << "  2. 균형적 전술\n";
    std::cout << "  3. 수비적 전술\n";
    std::cout << "입력 (1~3): ";

    int t = -1;

    // 예외처리
    while (t < 1 || t > 3) {
        std::string s;
        std::getline(std::cin, s);
        try { t = std::stoi(s); } catch (...) { t = -1; }
        if (t < 1 || t > 3)
            std::cout << "1~3 중에 입력하세요: ";
    }
    
    switch (t) {
        case 1: return Tactic::ATTACK;
        case 2: return Tactic::BALANCE;
        default: return Tactic::DEFENSE;
    }
}

// 다음 경기 진행
void Game::doSchedule() {
    int fixtureIdx = league.nextPlayerFixtureIdx();
    if (fixtureIdx < 0) {
        std::cout << "더 이상 진행할 경기가 없습니다.\n";
        return; 
    }

    // league 객체 내부의 경기 일정(fixture)과 상대 팀 인덱스를 가져옵니다.
    const auto& f = league.fixtures[fixtureIdx];
    bool playerIsHome = (f.homeIdx == 0);
    int oppIdx = playerIsHome ? f.awayIdx : f.homeIdx;
    
    // 상대 팀 객체 직접 참조
    const Team& oppTeam = league.teams[oppIdx];
    
    printLine();
    std::cout << "[ 매치 프리뷰 ]\n";
    std::cout << "-- 다음 상대 --\n";
    oppTeam.printRoster();

    // 전술 선택
    Tactic tactic = chooseTactic();

    std::cout << "\n[" << playerTeamName << "] "
              << tacticToString(tactic) << " 선택!\n";
    waitEnter();

    // 플레이어 경기 진행
    lastMatchResult = league.playPlayerMatch(tactic);
    hasPlayedMatch  = true;

    // 경기 결과 출력
    std::cout << "\n결과: ";
    if (lastMatchResult == MatchResult::WIN)       std::cout << "승리!\n";
    else if (lastMatchResult == MatchResult::DRAW) std::cout << "무승부\n";
    else                                           std::cout << "패배\n";

    if (lastMatchResult == MatchResult::LOSS) {
        tradeCount += 2;
    } else { 
        tradeCount += 1; 
    }
    waitEnter();
    
    league.printStandings(); 
    waitEnter();
}



// 선수 명단을 출력하고 포메이션 변경
void Game::doRoster() {
    league.teams[0].printRoster();  // 플레이어 팀 로스터 출력
    
    std::cout << "\n포메이션을 변경하시겠습니까? ( 1 입력 시 진행 ): ";
    std::string ans;
    std::getline(std::cin, ans);

    if (ans == "1")
        changeFormationMenu();  // 포메이션 변경 진입
    else
        waitEnter();
}

// 포메이션 변경
void Game::changeFormationMenu() {
    printLine();
    std::cout << "포메이션 선택:\n";

    // 포메이션 목록 출력
    for (int i = 0; i < (int)FORMATION_COUNT; ++i)
        std::cout << (i + 1) << ". " << FORMATIONS[i].label << "\n";

    std::cout << "번호 입력 (1~10): ";

    int pick = -1;
    
    while (pick < 1 || pick > 10) {
        std::string s;
        std::getline(std::cin, s);
        try { pick = std::stoi(s); } catch (...) { pick = -1; }
        if (pick < 1 || pick > 10)
            std::cout << "1~10 중에 입력하세요: ";
    }

    // 포메이션 인덱스 변경
    league.teams[0].formationIndex = pick - 1;

    // 원래 로스터 복제
    std::vector<Player> tempRoster = league.teams[0].roster;
    
    // 새로운 벡터
    std::vector<Player> newRoster(10);
    // 중복 입력 방지
    bool chosen[10] = { false, };

    std::cout << "\n[새 포메이션 선수 재배치]\n";
    std::cout << "새 포메이션의 1번부터 10번 자리에 배치할 원래 선수의 번호(1~10)를 순서대로 입력하세요.\n";
    std::cout << "(앞 번호부터 순서대로 수비수 -> 미드필더 -> 공격수 포지션이 배정됩니다.)\n\n";

    for (int i = 0; i < 10; ++i) {
        int playerNum = -1;
        while (true) {
            std::cout << "새 " << (i + 1) << "번 자리에 넣을 원래 선수 번호: ";
            std::string s;
            std::getline(std::cin, s);
            try { playerNum = std::stoi(s); } catch (...) { playerNum = -1; }
            
            if (playerNum < 1 || playerNum > 10) {
                std::cout << "1에서 10 사이의 숫자만 입력 가능합니다.\n";
                continue;
            }
            // 중복 검사
            if (chosen[playerNum - 1]) {
                std::cout << "이미 배치된 선수입니다. 다른 번호를 입력하세요.\n";
                continue;
            }
            break;
        }

        // 새로운 로스터 배열에 복사
        chosen[playerNum - 1] = true;
        newRoster[i] = tempRoster[playerNum - 1];
    }

    // 재배치된 로스터를 팀에 적용
    league.teams[0].roster = newRoster;
    league.teams[0].applyFormation();

    std::cout << "\n포메이션 변경 및 선수 재배치가 완료되었습니다: " << FORMATIONS[pick - 1].label << "\n";
    league.teams[0].printRoster();  // 변경 완료된 최종 로스터 출력
    waitEnter();
}



// 트레이드
void Game::doTrade() {
    if (!hasPlayedMatch) {
        std::cout << "트레이드는 첫 경기 이후부터 가능합니다.\n";
        waitEnter();
        return;
    }
	else if (tradeCount == 0) {
		std::cout << "남은 트레이드 횟수가 없습니다.\n";
		waitEnter();
		return;
	}
	else
    tradeCount = runTrades(league.teams[0], tradeCount);
    waitEnter();
}
