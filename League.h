
#pragma once
#include "Team.h"
#include "Match.h"   
#include <vector>    
#include <utility>   


// 경기 한 판의 일정 데이터를 담는 구조체.
struct Fixture {
    int homeIdx;  // 홈 팀의 teams 배열 인덱스 (0 = 플레이어 팀)
    int awayIdx;  // 어웨이 팀의 teams 배열 인덱스
    bool played;  // 이 경기가 이미 진행되었는지 여부 (true = 완료)
};


// 리그 전체의 상태(팀, 일정, 진행 상황)를 관리
class League {
public:
    // 리그에 참가하는 20개 팀
    std::vector<Team> teams;

    // 리그 전체 경기 일정 목록
    std::vector<Fixture> fixtures;

    // 다음에 처리할 경기의 배열 인덱스
    int currentFixtureIdx;

    // 리그의 모든 플레이어 경기(38경기)가 끝났는지 여부
    // true가 되면 플레이오프로 넘어간다.
    bool leagueFinished;

    
    // 기본 생성자
    League();
    
    // 리그를 초기화한다.
    void init(const std::string& playerTeamName);

    
    // 아직 진행하지 않은 플레이어 경기 중 가장 가까운 경기의 배열 인덱스를 반환한다.
    int nextPlayerFixtureIdx() const;
    
    // 다음 경기의 상대 팀 이름을 반환한다.
    std::string nextOpponentName() const;

    
    // AI 팀끼리의 경기를 자동으로 진행한다. 플레이어 경기 직전까지의 AI 경기를 처리할 때 사용한다.
    void simulateAIMatches(int upToFixtureIdx);

    
    // playPlayerMatch()
    // 플레이어의 다음 경기를 진행한다.
    MatchResult playPlayerMatch(Tactic tactic);



    // 현재 리그 순위표 출력
    void printStandings() const;



    // 승점 내림차순으로 정렬된 팀의 인덱스 배열을 반환
    std::vector<int> getSortedTeamIndices() const;
    
    // 리그가 완전히 끝났는지 반환한다.
    bool isFinished() const;
    
    // 플레이어가 지금까지 진행한 경기 수를 반환
    int playedMatches() const;



    // 리그에서 플레이어가 치러야 할 총 경기 수 반환
    int totalMatches() const;

    
    // 현재 순위표에서 플레이어 팀의 순위를 반환
    int playerRank() const;
};
