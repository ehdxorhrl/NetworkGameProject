#pragma once
#include "stdafx.h"
#include "CPlayer.h"

class ObjectManager {
public:
    // �̱��� �ν��Ͻ��� ��ȯ�ϴ� �޼���
    static ObjectManager& GetInstance() {
        static ObjectManager instance;
        return instance;
    }

    // �Ҹ���
    ~ObjectManager() = default;

    // ��ü �߰�, ����, �ʱ�ȭ �޼���
    void Add(CObject* object);
    void Remove(CObject* object);
    void Clear();

    // ��� ��ü ��ȯ
    const std::vector<CObject*>& GetObjects() const { return objects; }

    // ���ø� �޼���: ��ü ���� �� �߰�
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
                return player; // ù ��°�� �߰��� �÷��̾� ��ȯ
            }
        }
        return nullptr; // �÷��̾� ��ü�� ���� ���
    }

    // �̱����̹Ƿ� ���� �� �̵� ����
    ObjectManager(const ObjectManager&) = delete;
    ObjectManager& operator=(const ObjectManager&) = delete;

private:
    // private ������: �ܺο��� �ν��Ͻ� ���� �Ұ�
    ObjectManager() = default;

    // ������ ��ü�� ����
    std::vector<CObject*> objects;
};

