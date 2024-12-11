#pragma once
#include "stdafx.h"
#include "CPlayer.h"

class ObjectManager {
public:
    // 싱글톤 인스턴스를 반환하는 메서드
    static ObjectManager& GetInstance() {
        static ObjectManager instance;
        return instance;
    }

    // 소멸자
    ~ObjectManager() = default;

    // 객체 추가, 제거, 초기화 메서드
    void Add(CObject* object);
    void Remove(CObject* object);
    void Clear();

    // 모든 객체 반환
    const std::vector<CObject*>& GetObjects() const { return objects; }

    // 템플릿 메서드: 객체 생성 및 추가
    template <typename T>
    T* CreateObject()
    {
        T* newObject = new T();
        if (newObject) {
            objects.push_back(newObject);
        }
        return newObject;
    }

    CPlayer* GetPlayerByID(uint32_t playerID) const {
        for (auto* obj : objects) {
            if (auto* player = dynamic_cast<CPlayer*>(obj)) {
                if (player->GetID() == playerID) {
                    return player;
                }
            }
        }
        return nullptr;
    }

    CPlayer* AddPlayer(PlayerType playerType) {
        static uint32_t currentID = 0; // 고유 ID 생성기

        CPlayer* newPlayer = new CPlayer(playerType);
        newPlayer->SetID(currentID++); // 고유 ID 설정
        Add(newPlayer); // ObjectManager의 Add 메서드를 호출
        return newPlayer;
    }

    // 싱글톤이므로 복사 및 이동 금지
    ObjectManager(const ObjectManager&) = delete;
    ObjectManager& operator=(const ObjectManager&) = delete;

private:
    // private 생성자: 외부에서 인스턴스 생성 불가
    ObjectManager() = default;

    // 관리할 객체의 벡터
    std::vector<CObject*> objects;
};

