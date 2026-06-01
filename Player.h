#pragma once
#include <string>


enum class Role {
    DEFENDER,
    MIDFIELDER,
    FORWARD
};
std::string roleToString(Role r);


// 선수 클래스
class Player {
public:
    // 선수 이름
    std::string name;

    // 공격 스탯
    int attackStat;

    // 미드 스탯
    int midStat;

    // 수비 스탯
    int defStat;
    
    // 현재 포메이션에서 이 선수의 역할
    Role role;
    
    // 생성자: 이름을 받아서 선수 초기화
    Player(const std::string& name);

	Player() = default;
    
    // 선수의 정보 출력
    void printInfo() const;
	void printInfo_nopos() const;
};


// 이름 랜덤 생겅
std::string generateRandomName();

// 선수 객체를 생성해서 반환
Player generateRandomPlayer();
