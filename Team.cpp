#include "Team.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
std::uniform_int_distribution<int> distribution(0, 9);


// Formation 구조체
const Formation FORMATIONS[10] = {
    {3, 5, 2, "3-5-2"},
    {3, 4, 3, "3-4-3"},
    {3, 6, 1, "3-6-1"},
    {4, 4, 2, "4-4-2"},
    {4, 3, 3, "4-3-3"},
    {4, 5, 1, "4-5-1"},
    {4, 2, 4, "4-2-4"},
    {5, 3, 2, "5-3-2"},
    {5, 4, 1, "5-4-1"},
    {5, 2, 3, "5-2-3"}
};

// 포메이션 종류 개수
const int FORMATION_COUNT = 10;

std::string tacticToString(Tactic t) {
    switch (t) {
        case Tactic::ATTACK:  return "공격적 전술";
        case Tactic::BALANCE: return "균형적 전술";
        case Tactic::DEFENSE: return "수비적 전술";
    }
    return "?";  // 예외 처리
}

// 전술에 따른 공격 보정치
static int attackBonus(Tactic t) {
    switch (t) {
        case Tactic::ATTACK:  return 70;
        case Tactic::BALANCE: return 60;
        case Tactic::DEFENSE: return 50;
    }
    return 60;
}

// 전술에 따른 수비 보정치

static int defenseBonus(Tactic t) {
    switch (t) {
        case Tactic::ATTACK:  return 0;
        case Tactic::BALANCE: return 10;
        case Tactic::DEFENSE: return 20;
    }
    return 10;
}

// Team 생성자
Team::Team(const std::string& n)
    : name(n), formationIndex(rand()%10), tactic(Tactic::BALANCE),
      points(0.0), wins(0), draws(0), losses(0), goalsFor(0), goalsAgainst(0)
{
    
    for (int i = 0; i < 10; ++i)
        roster.push_back(generateRandomPlayer());
    applyFormation();
}

// 포메이션 적용
void Team::applyFormation() {
    const Formation& f = FORMATIONS[formationIndex];
    int idx = 0;
    for (int i = 0; i < f.def; ++i) roster[idx++].role = Role::DEFENDER;
    for (int i = 0; i < f.mid; ++i) roster[idx++].role = Role::MIDFIELDER;
    for (int i = 0; i < f.fwd; ++i) roster[idx++].role = Role::FORWARD;
}

// 플레이어가 새 포메이션을 선택했을 때
void Team::changeFormation(int idx) {
    formationIndex = idx;  // 새로 선택한 포메이션 번호로 업데이트
    applyFormation();      // 바뀐 포메이션 기준으로 선수 역할 재배정
}

// 팀의 공격 수치 = 공격수 공격스탯 평균 + 미드필더 미드스탯 평균 * 20% + 공격 보정치
double Team::getAttackValue() const {
    double fwdAvg = 0;  // 공격수들의 공격스탯 합계
    double midAvg = 0;  // 미드필더들의 미드스탯 합계
    int fwdCnt = 0;     // 공격수 인원 수
    int midCnt = 0;     // 미드필더 인원 수
    
    for (int i = 0; i < (int)roster.size(); ++i) {
        if (roster[i].role == Role::FORWARD) {
            fwdAvg += roster[i].attackStat;
            fwdCnt++;
        }
        if (roster[i].role == Role::MIDFIELDER) {
            midAvg += roster[i].midStat;
            midCnt++;
        }
    }
    if (fwdCnt > 0) fwdAvg /= fwdCnt;  // 공격수 공격스탯 평균
    if (midCnt > 0) midAvg /= midCnt;  // 미드필더 미드스탯 평균
    
    return fwdAvg + midAvg * 0.2 + attackBonus(tactic);
}

// 팀의 수비 수치 = 수비수 수비스탯 평균 + 미드필더 미드스탯 평균 * 20% + 수비 보정치
double Team::getDefenseValue() const {
    double defAvg = 0;  // 수비수들의 수비스탯 합계
    double midAvg = 0;  // 미드필더들의 미드스탯 합계
    int defCnt = 0;     // 수비수 인원 수
    int midCnt = 0;     // 미드필더 인원 수

    for (int i = 0; i < (int)roster.size(); ++i) {
        if (roster[i].role == Role::DEFENDER) {
            defAvg += roster[i].defStat;
            defCnt++;
        }
        if (roster[i].role == Role::MIDFIELDER) {
            midAvg += roster[i].midStat;
            midCnt++;
        }
    }
    if (defCnt > 0) defAvg /= defCnt;  // 수비수 수비스탯 평균
    if (midCnt > 0) midAvg /= midCnt;  // 미드필더 미드스탯 평균

    return defAvg + midAvg * 0.2 + defenseBonus(tactic);
}



// 이 팀이 상대 팀과 경기할 때의 득점 최댓값을 계산
// 기획서 6.2.3 (3) 득점 최댓값 계산식:
//   (내 공격 수치 - 상대 수비 수치) / 20  ->  반올림
// 기획서 6.2.3 (4) 최소 보정:
//   결과가 0 이하이면 1로 고정 (약팀도 최소한의 득점 가능성 보장)
// 매개변수: opponent - 상대 팀 객체 (상대의 수비 수치를 가져오기 위해 필요)
// 반환값:   득점 최댓값 (최소 1)
int Team::calcMaxGoals(const Team& opponent) const {
    // 내 공격 수치에서 상대 수비 수치를 뺀 뒤 20으로 나눈다
    double raw = (getAttackValue() - opponent.getDefenseValue()) / 20.0;

    // 반올림
    int val = (int)(raw + 0.5);

    // 최소 보정: 계산 결과가 0 이하면 1로 고정
    // 아무리 약한 팀이라도 0~1 범위에서 득점 시도 가능
    if (val < 1) val = 1;
    
    return val;
}



// 현재 팀의 포메이션 정보 출력
void Team::printRoster() const {
    const Formation& f = FORMATIONS[formationIndex];

    printLine();

    // 팀 이름과 현재 포메이션/전술 표시
    std::cout << "팀명: " << name << "\n";
    std::cout << "포메이션: " << f.label
              << "   전술: " << tacticToString(tactic) << "\n";

    printLine();
    
    std::cout << "번호 | 이름                   | 공격  미드  수비  역할\n";

    printLine();

    // 선수 목록 출력
    for (int i = 0; i < (int)roster.size(); ++i) {
        std::cout << std::setw(2) << (i + 1) << "   ";
        roster[i].printInfo();
    }

    printLine();
}



// 승점을 정수로 변환해서 반환한다.
int Team::displayPoints() const {
    return (int)points;
}

// 순위표에서 팀 출력
void Team::printStandingRow(int rank) const {
    int gd = goalsFor - goalsAgainst;  // 득실차
    std::cout << std::setw(3) << rank
              << "  " << std::left << std::setw(22) << name
              << std::right
              << std::setw(4) << wins
              << std::setw(4) << draws
              << std::setw(4) << losses
              << std::setw(5) << gd
              << std::setw(5) << displayPoints() << "\n";
}
