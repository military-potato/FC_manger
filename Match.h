#pragma once
#include "Team.h"
#include <string>


// 경기 결과를 나타낸다. 홈 팀 기준
enum class MatchResult {
    WIN,
    DRAW,
    LOSS
};


// 경기 결과 구조체.
struct MatchScore {
    int homeGoals;            // 홈 팀이 넣은 골 수
    int awayGoals;            // 어웨이 팀이 넣은 골 수
    MatchResult resultForHome; // 홈 팀 기준 경기 결과
};

// 홈 팀과 어웨이 팀이 경기를 진행하고 결과를 반환한다.
// 득점 최댓값을 계산하고, 그 범위에서 랜덤 득점을 뽑아 비교한다.
// 경기가 끝나면 두 팀의 승점/승무패/득실 기록을 자동으로 업데이트한다.
MatchScore playMatch(Team& home, Team& away, bool printDetail = false);



// 플레이오프에서 무승부가 나왔을 때 승부차기
bool playoffTiebreak(const std::string& teamA, const std::string& teamB);
