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
    // 싱글톤 인스턴스를 반환하는 함수
    static SceneManager& GetInstance()
    {
        static SceneManager instance; // 정적 지역 변수로 유일한 인스턴스 생성
        return instance;
    }

    // 싱글톤에서는 외부에서 직접 인스턴스를 만들 수 없도록 복사 생성자와 대입 연산자를 삭제
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

public:
    // 주요 기능
    void Init();
    void Update();
    void Clear();

public:
    // 씬 전환 기능
    void ChangeScene();
    CScene* GetCurrentScene() const { return scene; } // 현재 씬 반환
private:
    // 기본 생성자를 private으로 처리하여 외부에서 생성 불가
    SceneManager() = default;
    ~SceneManager() = default;

    CScene* scene = nullptr;
    SceneType sceneType = SceneType::None;
};
