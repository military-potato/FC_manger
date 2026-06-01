#include "Match.h"  // MatchResult, MatchScore, 함수 선언
#include "Utils.h"  // randInt() (랜덤 득점 생성에 사용)
#include <iostream> // std::cout (경기 결과 출력에 사용)


// 두 팀이 경기를 진행하고 결과를 반환
MatchScore playMatch(Team& home, Team& away, bool printDetail) {
    int homeMax = home.calcMaxGoals(away);  // 홈 팀의 득점 최댓값
    int awayMax = away.calcMaxGoals(home);  // 어웨이 팀의 득점 최댓값
    
    // 범위 안의 모든 정수가 동일한 확률로 뽑힌다.
    int homeGoals = randInt(homeMax);  // 홈 팀 득점
    int awayGoals = randInt(awayMax);  // 어웨이 팀 득점

    // 두 득점을 비교해서 홈 팀 기준 결과 판정
    MatchResult res;
    if (homeGoals > awayGoals)      res = MatchResult::WIN;
    else if (homeGoals < awayGoals) res = MatchResult::LOSS;
    else                            res = MatchResult::DRAW;

    // 플레이어 경기일 때만 결과를 콘솔에 출력
    // AI끼리 경기는 printDetail=false이므로 이 블록이 실행되지 않는다.
    if (printDetail) {
        std::cout << "\n[경기 결과]\n";
        std::cout << home.name << " " << homeGoals
                  << " : " << awayGoals << " " << away.name << "\n";
    }

    // 두 팀의 누적 성적 데이터 업데이트
    // 득실 기록: 홈/어웨이 각각 득점은 +, 실점은 상대 득점
    home.goalsFor     += homeGoals;  // 홈 팀 누적 득점에 이번 경기 득점 추가
    home.goalsAgainst += awayGoals;  // 홈 팀 누적 실점에 어웨이 득점 추가
    away.goalsFor     += awayGoals;  // 어웨이 팀 누적 득점
    away.goalsAgainst += homeGoals;  // 어웨이 팀 누적 실점
    
    if (res == MatchResult::WIN) {
        home.points += 3.001;  // 홈 팀 승리: 승점 3.001 추가 (승점 동률 시 다승팀이 상위)
        home.wins++;           // 홈 팀 승리 수 1 증가
        away.losses++;         // 어웨이 팀 패배 수 1 증가
    } else if (res == MatchResult::LOSS) {
        away.points += 3.001;  // 어웨이 팀 승리: 어웨이에 승점 3.001 추가
        away.wins++;           // 어웨이 팀 승리 수 1 증가
        home.losses++;         // 홈 팀 패배 수 1 증가
    } else {
        home.points += 1.0;  // 무승부: 양 팀 모두 승점 1 추가
        home.draws++;        // 홈 팀 무승부 수 1 증가
        away.points += 1.0;  // 어웨이 팀도 승점 1
        away.draws++;        // 어웨이 팀 무승부 수 1 증가
    }

    // 결과 구조체 만들어서 반환
    return {homeGoals, awayGoals, res};
}


// 플레이오프에서 무승부가 나왔을 때
bool playoffTiebreak(const std::string& teamA, const std::string& teamB) {
    int r = randInt(1);  // 0 또는 1을 랜덤으로 뽑음 (각 50% 확률)

    std::cout << "[승부차기] " << teamA << " vs " << teamB << " : ";

    if (r == 0) {
        std::cout << teamA << " 승리!\n";  // 0이 나오면 teamA 승
        return true;                       // true = teamA 승리
    } else {
        std::cout << teamB << " 승리!\n";  // 1이 나오면 teamB 승
        return false;                      // false = teamB 승리 (teamA 패배)
    }
}
