#pragma once
#include "Player.h"
#include <vector>
#include <string>

// 포메이션 데이터 구조체.
struct Formation {
    int def;
    int mid;
    int fwd;
    const char* label;
};



// 포메이션 배열
extern const Formation FORMATIONS[10];
extern const int FORMATION_COUNT;


enum class Tactic {
    ATTACK,   // 공격적 전술: 공격 보정 +70, 수비 보정 +0
    BALANCE,  // 균형적 전술: 공격 보정 +60, 수비 보정 +10
    DEFENSE   // 수비적 전술: 공격 보정 +50, 수비 보정 +20
};
std::string tacticToString(Tactic t);

// Team 클래스
class Team {
public:
    // 팀 이름
    std::string name;

    // 선수 명단
    std::vector<Player> roster;

    // 현재 적용된 포메이션
    int formationIndex;

    // 현재 선택된 전술
    Tactic tactic;

    // 리그 승점
    double points;

    int wins;         // 승리
    int draws;        // 무승부
    int losses;       // 패배
    int goalsFor;     // 득점 
    int goalsAgainst; // 실점

    
    // 생성자
    Team(const std::string& name);
    
    // 포메이션 적용
    void applyFormation();
    
    // 포메이션을 변경하고 재배정
    void changeFormation(int idx);
    
    // 공격 수치
    double getAttackValue() const;
    
    // 수비 수치
    double getDefenseValue() const;
    
    // 득점 최댓값 계산.
    int calcMaxGoals(const Team& opponent) const;
    
    // 선수 명단, 포메이션, 전술 출력
    void printRoster() const;
    
    // 승점
    int displayPoints() const;
    
    // 순위표에서 한 줄 출력
    void printStandingRow(int rank) const;
};
