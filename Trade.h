#pragma once
#include "Team.h"
#include "Match.h"

// 경기 결과에 따라 트레이드 횟수 반환
int calcTradeCount(MatchResult result);

// 트레이드 진행
void doTrade(Team& playerTeam);

int runTrades(Team& playerTeam, int tradeCount);
