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
    template <typename T>
    T* CreateObject()
    {
        T* newObject = new T();
        if (newObject) {
            objects.push_back(newObject);
            std::cout << "Object added: " << typeid(T).name() << std::endl;
        }
        else {
            std::cerr << "Error: Object creation failed!\n";
        }
        return newObject;
    }

    CPlayer* GetPlayer() const
    {
        for (auto* obj : objects) {
            if (auto* player = dynamic_cast<CPlayer*>(obj)) {
                return player;
            }
        }
        std::cerr << "Error: No player object found in ObjectManager!\n";
        return nullptr;
    }

    CPlayer* AddPlayer(PlayerType playerType) {
        static uint32_t currentID = 0; // ���� ID ������

        CPlayer* newPlayer = new CPlayer(playerType);
        newPlayer->SetID(currentID++); // ���� ID ����
        Add(newPlayer); // ObjectManager�� Add �޼��带 ȣ��
        return newPlayer;
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
