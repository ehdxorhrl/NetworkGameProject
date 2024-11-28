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
    template<typename T>
    T* CreateObject() {
        T* newObject = new T();
        newObject->Init();
        Add(newObject);
        return newObject;
    }

    CPlayer* GetPlayer() {
        for (auto* obj : objects) {
            if (CPlayer* player = dynamic_cast<CPlayer*>(obj)) {
                return player; // 첫 번째로 발견한 플레이어 반환
            }
        }
        return nullptr; // 플레이어 객체가 없는 경우
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

