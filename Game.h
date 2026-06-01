

#pragma once
#include "League.h"
#include <string>


class Game {
public:
    // 리그 전체 데이터 (팀 20개, 경기 일정, 순위 등)
    League league;

    // 플레이어 팀의 이름 (게임 시작 시 입력받음)
    std::string playerTeamName;

    // 직전 경기 결과 (트레이드 횟수 결정에 사용)
    // 초기값: DRAW (게임 시작 전이므로 임의로 DRAW로 설정)
    MatchResult lastMatchResult;

    // 첫 경기를 진행했는지 여부
    // false이면 트레이드 메뉴 진입 시 "첫 경기 후에만 가능" 안내를 출력
    bool hasPlayedMatch;



    // 기본 생성자
    Game();
    
    // 게임 시작 처리를 한다.
    void start();
    
    // 게임의 메인 루프를 실행한다.
    void run();


private:
    // 메인 메뉴를 콘솔에 출력한다.
    void showMainMenu();
    
    // 일정
    void doSchedule();
    
    // 로스터
    void doRoster();
    
    // 트레이드
    void doTrade();
    
    // 포메이션 변경
    void changeFormationMenu();
    
    // 경기 전 전술 선택
    Tactic chooseTactic();
};