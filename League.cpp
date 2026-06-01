
#include "League.h"   // League 클래스, Fixture 구조체
#include "Match.h"    // playMatch() 함수
#include "Utils.h"    // randInt(), printLine()
#include "NamePool.h"// TEAM_NAMES 배열 (AI 팀 이름)
#include <iostream>   // std::cout
#include <iomanip>    // std::setw()
#include <algorithm>  // std::sort()
#include <vector>
#include <string>

// 기본 생성자
League::League()
    : currentFixtureIdx(0), leagueFinished(false) {}

void League::init(const std::string& playerTeamName) {
    teams.clear();
    fixtures.clear();
    currentFixtureIdx = 0;
    leagueFinished = false;

    // 플레이어 팀 생성 및 추가
    Team player(playerTeamName);
    teams.push_back(std::move(player));

    // AI 팀 생성 및 추가
    for (int i = 0; i < 19; ++i) {
        Team ai(TEAM_NAMES[i]);
        teams.push_back(std::move(ai));
    }

    // 38라운드 일정 생성
    int totalTeams = 20;
    int rounds = 38;

    std::vector<int> scheduleIndices(totalTeams);
    for (int i = 0; i < totalTeams; ++i) {
        scheduleIndices[i] = i;
    }

    for (int r = 0; r < rounds; ++r) {
        std::vector<Fixture> roundFixtures;

        for (int i = 0; i < totalTeams / 2; ++i) {
            int home = scheduleIndices[i];
            int away = scheduleIndices[totalTeams - 1 - i];

            Fixture f;
            if (r % 2 == 0) {
                f.homeIdx = home;
                f.awayIdx = away;
            } else {
                f.homeIdx = away;
                f.awayIdx = home;
            }
            f.played = false;

            roundFixtures.push_back(f);
        }

        // 플레이어 경기 맨 앞으로 스왑
        for (int i = 0; i < (int)roundFixtures.size(); ++i) {
            if (roundFixtures[i].homeIdx == 0 || roundFixtures[i].awayIdx == 0) {
                std::swap(roundFixtures[0], roundFixtures[i]);
                break;
            }
        }

        for (const auto& f : roundFixtures) {
            fixtures.push_back(f);
        }
        
        int last = scheduleIndices[totalTeams - 1];
        for (int i = totalTeams - 1; i > 1; --i) {
            scheduleIndices[i] = scheduleIndices[i - 1];
        }
        scheduleIndices[1] = last;
    }
}


int League::nextPlayerFixtureIdx() const {
    for (int i = currentFixtureIdx; i < (int)fixtures.size(); ++i) {
        if (!fixtures[i].played && (fixtures[i].homeIdx == 0 || fixtures[i].awayIdx == 0)) {
            return i;
        }
    }
    return -1;
}


std::string League::nextOpponentName() const {
    int idx = nextPlayerFixtureIdx();
    if (idx < 0) {
        return "없음 (리그 종료)";
    }

    const Fixture& f = fixtures[idx];
    int opponentIdx = (f.homeIdx == 0) ? f.awayIdx : f.homeIdx;
    
    return teams[opponentIdx].name;
}

void League::simulateAIMatches(int upToFixtureIdx) {
    for (int i = currentFixtureIdx; i < upToFixtureIdx && i < (int)fixtures.size(); ++i) {
        auto& f = fixtures[i];

        if (!f.played && f.homeIdx != 0 && f.awayIdx != 0) {
            playMatch(teams[f.homeIdx], teams[f.awayIdx], false);
            f.played = true;
        }
    }
}

MatchResult League::playPlayerMatch(Tactic tactic) {
    int idx = nextPlayerFixtureIdx();
    if (idx < 0) return MatchResult::DRAW;

    simulateAIMatches(idx);

    auto& f = fixtures[idx];
    bool playerIsHome = (f.homeIdx == 0);
    teams[0].tactic = tactic;

    MatchScore score;
    if (playerIsHome) {
        score = playMatch(teams[0], teams[f.awayIdx], true);
    } else {
        score = playMatch(teams[f.homeIdx], teams[0], true);
        if (score.resultForHome == MatchResult::WIN)
            score.resultForHome = MatchResult::LOSS;
        else if (score.resultForHome == MatchResult::LOSS)
            score.resultForHome = MatchResult::WIN;
    }

    f.played = true;
    currentFixtureIdx = idx + 1;

    int nextIdx = nextPlayerFixtureIdx();
    if (nextIdx >= 0) {
        simulateAIMatches(nextIdx);
    } else {
        simulateAIMatches((int)fixtures.size());
    }

    if (nextPlayerFixtureIdx() < 0) {
        leagueFinished = true;
    }

    return score.resultForHome;
}

std::vector<int> League::getSortedTeamIndices() const {
    std::vector<int> indices(teams.size());
    for (int i = 0; i < (int)teams.size(); ++i) {
        indices[i] = i;
    }

    std::sort(indices.begin(), indices.end(), [this](int a, int b) {
        const Team& teamA = teams[a];
        const Team& teamB = teams[b];

        if (teamA.points != teamB.points) {
            return teamA.points > teamB.points;
        }
        if (teamA.wins != teamB.wins) {
            return teamA.wins > teamB.wins;
        }
        int gdA = teamA.goalsFor - teamA.goalsAgainst;
        int gdB = teamB.goalsFor - teamB.goalsAgainst;
        return gdA > gdB;
    });

    return indices;
}

void League::printStandings() const {
    auto sorted = getSortedTeamIndices();
    
    std::cout << "\n=========================================================================\n";
    std::cout << " 순위 | 팀 이름                       | 승점 | 승 | 무 | 패 | 득 | 실 \n";
    std::cout << "-------------------------------------------------------------------------\n";

    for (int i = 0; i < (int)sorted.size(); ++i) {
        int idx = sorted[i];
        const Team& t = teams[idx];

        std::string marker = (idx == 0) ? ">" : " ";

        int visualWidth = 0;
        for (size_t k = 0; k < t.name.size(); ) {
            unsigned char c = static_cast<unsigned char>(t.name[k]);
            
            if (c < 0x80) { // ASCII (영어, 숫자, 공백) -> 1칸
                visualWidth += 1;
                k += 1;
            } else { // 멀티바이트 (한글) -> 2칸
                visualWidth += 2;
                
                // 인코딩 규격별 바이트 카운트 건너뛰기
                if ((c & 0xE0) == 0xC0) k += 2;
                else if ((c & 0xF0) == 0xE0) k += 3;
                else if ((c & 0xF8) == 0xF0) k += 4;
                else k += 1;
            }
        }
        
        int targetWidth = 26;
        int neededSpaces = targetWidth - visualWidth;
        if (neededSpaces < 0) neededSpaces = 0; 

        // 순위 및 확장된 팀 이름 출력
        std::cout << marker << " " << std::setw(2) << (i + 1) << "   | " << t.name;
        
        // 남은 공간만큼 안전하게 공백 출력
        for (int s = 0; s < neededSpaces; ++s) {
            std::cout << " ";
        }

        // 성적 지표 격자 출력
        std::cout << " | "
                  << std::setw(4) << std::right << static_cast<int>(t.points) << " | "
                  << std::setw(2) << t.wins << " | "
                  << std::setw(2) << t.draws << " | "
                  << std::setw(2) << t.losses << " | "
                  << std::setw(2) << t.goalsFor << " | "
                  << std::setw(2) << t.goalsAgainst << "\n";
    }

    std::cout << "=========================================================================\n";
}


bool League::isFinished() const {
    return leagueFinished;
}


int League::playedMatches() const {
    int cnt = 0;
    for (const auto& f : fixtures)
        if (f.played && (f.homeIdx == 0 || f.awayIdx == 0)) cnt++;
    return cnt;
}


int League::totalMatches() const {
    return 38;
}


int League::playerRank() const {
    auto sorted = getSortedTeamIndices();
    for (int i = 0; i < (int)sorted.size(); ++i) {
        if (sorted[i] == 0) return (i + 1);
    }
    return 1;
}