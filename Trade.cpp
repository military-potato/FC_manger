#include "Trade.h"
#include "Utils.h"
#include <iostream>
#include <string>
#include <algorithm>

void doTrade(Team& playerTeam) {
    printLine();
    std::cout << "【선수 트레이드】\n\n";
    
    Player newPlayer = generateRandomPlayer();

    // 현재 로스터 출력
    printLine();
    std::cout << "현재 로스터:\n";
    for (int i = 0; i < (int)playerTeam.roster.size(); ++i) {
        std::cout << (i + 1) << ". ";
        playerTeam.roster[i].printInfo();
    }

    // 방출할 선수 번호 입력
    std::cout << "\n방출할 선수 번호를 입력하세요 (1~10): ";
    int release = -1;

    while (release < 1 || release > 10) {
        std::string input;
        std::getline(std::cin, input);
        try { release = std::stoi(input); } catch (...) { release = -1; }
        if (release < 1 || release > 10)
            std::cout << "1~10 사이의 숫자를 입력하세요: ";
    }

    // 방출할 선수 이름 저장
    Player releasedPlayer = playerTeam.roster[release - 1];

    // 방출 선수 자리에 새 선수 덮어쓰기
    playerTeam.roster[release - 1] = newPlayer;

    // 포메이션에 맞게 역할 배정
    playerTeam.applyFormation();
    
    std::cout << "\n";
    releasedPlayer.printInfo_nopos();
    std::cout<< "방출\n";
    newPlayer.printInfo_nopos();
    std::cout << "영입 완료!\n";
    printLine();
}


int runTrades(Team& playerTeam, int count) {
    //트레이드 횟수
    std::cout << "\n트레이드 가능 횟수: " << count << "회\n";


    std::cout << "트레이드를 진행하시겠습니까?\n(주의! 방츨한 선수는 되돌릴 수 없으며 영입한 선수의 능력치는 랜덤으로 정해집니다.)\n( 1 입력 시 진행 ): ";
    std::string ans;
    std::getline(std::cin, ans);
    
    if (ans == "1")
    {
		
        doTrade(playerTeam);
		return count-1;
    }
	return count;
}

