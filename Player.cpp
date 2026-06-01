#include "Player.h"   // Player 클래스, Role 열거형 선언
#include "Utils.h"    // randInt() 함수
#include "NamePool.h" // FIRST_NAMES, LAST_NAMES 배열
#include <iostream>   // std::cout
#include <iomanip>    // std::setw(), std::left (출력 정렬)


std::string roleToString(Role r) {
    switch (r) {
        case Role::DEFENDER:   return "수비수";
        case Role::MIDFIELDER: return "미드필더";
        case Role::FORWARD:    return "공격수";
    }
    return "?";  //예외처리
}



// Player 생성자
Player::Player(const std::string& n)
    : name(n), role(Role::MIDFIELDER)  // 이름
{
    attackStat = randInt(100);
    midStat = randInt(100);
    defStat = randInt(100);
}



// 선수 한 명의 정보를 출력
void Player::printInfo() const {
    std::cout << std::left << std::setw(22) << name
              << " | 공격:" << std::setw(4) << attackStat
              << " 미드:" << std::setw(4) << midStat
              << " 수비:" << std::setw(4) << defStat
              << " [" << roleToString(role) << "]\n";
}

void Player::printInfo_nopos() const {
    std::cout << std::left << std::setw(22) << name
              << " | 공격:" << std::setw(4) << attackStat
              << " 미드:" << std::setw(4) << midStat
              << " 수비:" << std::setw(4) << defStat;
}


// 이름 자동생성
std::string generateRandomName() {
    int fi = randInt((int)FIRST_NAMES.size() - 1);
    int li = randInt((int)LAST_NAMES.size() - 1);
    return FIRST_NAMES[fi] + " " + LAST_NAMES[li];
}

// 랜덤 이름을 생성한 뒤 그 이름으로 Player 객체를 만들어 반환
Player generateRandomPlayer() {
    return Player(generateRandomName());  // 랜덤 이름으로 Player 생성 후 반환
}
