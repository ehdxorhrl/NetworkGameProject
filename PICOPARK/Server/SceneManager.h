#pragma once

enum class SceneType
{
    None,
    Intro,
    Main,
    Stage1,
    Stage2,
    Stage3,
    Ending
};

class CScene;

class SceneManager
{
public:
    // �̱��� �ν��Ͻ��� ��ȯ�ϴ� �Լ�
    static SceneManager& GetInstance()
    {
        static SceneManager instance; // ���� ���� ������ ������ �ν��Ͻ� ����
        return instance;
    }

    // �̱��濡���� �ܺο��� ���� �ν��Ͻ��� ���� �� ������ ���� �����ڿ� ���� �����ڸ� ����
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

public:
    // �ֿ� ���
    void Init();
    void Update();
    void Clear();

public:
    // �� ��ȯ ���
    void ChangeScene();
    CScene* GetCurrentScene() const { return scene; } // ���� �� ��ȯ
private:
    // �⺻ �����ڸ� private���� ó���Ͽ� �ܺο��� ���� �Ұ�
    SceneManager() = default;
    ~SceneManager() = default;

    CScene* scene = nullptr;
    SceneType sceneType = SceneType::None;
};
