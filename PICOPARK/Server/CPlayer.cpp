#include "stdafx.h"
#include "CPlayer.h"
#include "Stage1.h"
#include "Stage2.h"
#include "Stage3.h"

CPlayer::CPlayer() {}

CPlayer::~CPlayer() {}


void CPlayer::Init() {
    size = 50;
    speed = 350;
    stageNum = 0;
}

void CPlayer::Update() {

    if (Pstate == PlayerState::intothedoor)
        return;

    float serverDeltaTime = TimeManager::GetInstance().GetDeltaTime(); // ���� �ð� ��� deltaTime

    float deltaTime;
    if (remainingInputTime > 0) {
        float inputDeltaTime = (float)remainingInputTime / 100000;
        if (inputDeltaTime < serverDeltaTime) {
            deltaTime = inputDeltaTime;
        } else {
            deltaTime = serverDeltaTime;
        }
        remainingInputTime -= static_cast<uint64_t>(deltaTime * 100000);
    } else {
        deltaTime = serverDeltaTime;
    }

    // �߷� ó��
    pos.y += jumpVelocity * deltaTime; // ���� �ӵ� ����
    jumpVelocity += gravity * deltaTime; // �߷� ����

    // �¿� �̵� �ӵ�
    float moveSpeed = 0.0f;

    if (input == KT::Left) {
        moveSpeed = -speed * deltaTime; // ���� �̵�
        if (Pstate != PlayerState::Jump) { // ���� ���°� �ƴ� ���� Move�� ��ȯ
            Pstate = PlayerState::Move;
            UpdateAnimation(serverDeltaTime);
        }
    } else if (input == KT::Right) {
        moveSpeed = speed * deltaTime; // ���� �̵�
        if (Pstate != PlayerState::Jump) { // ���� ���°� �ƴ� ���� Move�� ��ȯ
            Pstate = PlayerState::Move;
            UpdateAnimation(serverDeltaTime);
        }
    }

    // �¿� �浹 �˻�
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (SceneManager::GetInstance().GetSceneType()==SceneType::Stage1) {
                Stage1* stage = dynamic_cast<Stage1*>(SceneManager::GetInstance().GetCurrentScene());
                int(*currentMap)[8] = stage->GetMapData(stageNum);

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // ����� ���� ��� ��ŵ

                // ����� ��ǥ ���
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockTop = i * BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // y�� �浹 ���� Ȯ��
                if (!(blockTop < pos.y + size / 2 && pos.y - size / 2 < blockBottom)) continue;

                if (moveSpeed < 0.f) { // ���� �̵�
                    if (pos.x - size / 2 < blockRight) continue; // �����ʿ� �ִ� ��� ����

                    if (abs(blockRight - (pos.x - size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // ��Ȯ�� �´����� �̵� ����
                        break;
                    }

                    if (pos.x - size / 2 + moveSpeed < blockRight) {
                        float gap = blockRight - (pos.x - size / 2);
                        moveSpeed = gap; // �̵� ���� ����
                    }
                } else if (moveSpeed > 0.f) { // ���� �̵�
                    if (pos.x + size / 2 > blockLeft) continue; // ���ʿ� �ִ� ��� ����

                    if (abs(blockLeft - (pos.x + size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // ��Ȯ�� �´����� �̵� ����
                        break;
                    }

                    if (pos.x + size / 2 + moveSpeed > blockLeft) {
                        float gap = blockLeft - (pos.x + size / 2);
                        moveSpeed = gap; // �̵� ���� ����
                    }
                }
            } else if (SceneManager::GetInstance().GetSceneType() == SceneType::Stage2) {
                Stage2* stage = dynamic_cast<Stage2*>(SceneManager::GetInstance().GetCurrentScene());
                int(*currentMap)[8] = stage->GetMapData(stageNum);

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // ����� ���� ��� ��ŵ

                // ����� ��ǥ ���
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockTop = i * BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // y�� �浹 ���� Ȯ��
                if (!(blockTop < pos.y + size / 2 && pos.y - size / 2 < blockBottom)) continue;

                if (moveSpeed < 0.f) { // ���� �̵�
                    if (pos.x - size / 2 < blockRight) continue; // �����ʿ� �ִ� ��� ����

                    if (abs(blockRight - (pos.x - size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // ��Ȯ�� �´����� �̵� ����
                        break;
                    }

                    if (pos.x - size / 2 + moveSpeed < blockRight) {
                        float gap = blockRight - (pos.x - size / 2);
                        moveSpeed = gap; // �̵� ���� ����
                    }
                }
                else if (moveSpeed > 0.f) { // ���� �̵�
                    if (pos.x + size / 2 > blockLeft) continue; // ���ʿ� �ִ� ��� ����

                    if (abs(blockLeft - (pos.x + size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // ��Ȯ�� �´����� �̵� ����
                        break;
                    }

                    if (pos.x + size / 2 + moveSpeed > blockLeft) {
                        float gap = blockLeft - (pos.x + size / 2);
                        moveSpeed = gap; // �̵� ���� ����
                    }
                }
            }
            else if (SceneManager::GetInstance().GetSceneType() == SceneType::Stage3) {
                Stage3* stage = dynamic_cast<Stage3*>(SceneManager::GetInstance().GetCurrentScene());
                int(*currentMap)[8] = stage->GetMapData(stageNum);

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // ����� ���� ��� ��ŵ

                // ����� ��ǥ ���
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockTop = i * BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // y�� �浹 ���� Ȯ��
                if (!(blockTop < pos.y + size / 2 && pos.y - size / 2 < blockBottom)) continue;

                if (moveSpeed < 0.f) { // ���� �̵�
                    if (pos.x - size / 2 < blockRight) continue; // �����ʿ� �ִ� ��� ����

                    if (abs(blockRight - (pos.x - size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // ��Ȯ�� �´����� �̵� ����
                        break;
                    }

                    if (pos.x - size / 2 + moveSpeed < blockRight) {
                        float gap = blockRight - (pos.x - size / 2);
                        moveSpeed = gap; // �̵� ���� ����
                    }
                }
                else if (moveSpeed > 0.f) { // ���� �̵�
                    if (pos.x + size / 2 > blockLeft) continue; // ���ʿ� �ִ� ��� ����

                    if (abs(blockLeft - (pos.x + size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // ��Ȯ�� �´����� �̵� ����
                        break;
                    }

                    if (pos.x + size / 2 + moveSpeed > blockLeft) {
                        float gap = blockLeft - (pos.x + size / 2);
                        moveSpeed = gap; // �̵� ���� ����
                    }
                }
            }
        }
    }

    // �̵� ����
    pos.x += moveSpeed;

    // ���� ���� ó�� �� ��� ���� ����
    bool isLanding = false;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (SceneManager::GetInstance().GetSceneType() == SceneType::Stage1) {
                Stage1* stage = dynamic_cast<Stage1*>(SceneManager::GetInstance().GetCurrentScene());
                int(*currentMap)[8] = stage->GetMapData(stageNum);

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // ����� ���� ��� ��ŵ

                // ����� ��ǥ ���
                float blockTop = i * BLOCK_SIZE;
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // ���� ���� Ȯ��
                if (blockLeft < pos.x + size / 4 && pos.x - size / 4 < blockRight) {  // /4 ��, /2 �簢��
                    if (pos.y + size / 2 <= blockTop && pos.y + size / 2 + jumpVelocity * deltaTime >= blockTop) {
                        // ���� ó��
                        jumpVelocity = 0.0f;
                        pos.y = blockTop - size / 2; // ��� ���� ����
                        isLanding = true;
                        if (moveSpeed == 0.0f) { // �������� ���� ���� Idle
                            Pstate = PlayerState::Idle;
                        }
                        else {
                            Pstate = PlayerState::Move;
                        }
                        isJumping = false;
                        break;
                    }

                    if (pos.y - size / 2 >= blockBottom && pos.y - size / 2 + jumpVelocity * deltaTime < blockBottom) {
                        jumpVelocity = 0.0f; // �浹 �� �ӵ� �ʱ�ȭ
                        pos.y = blockBottom + size / 2; // ĳ���͸� ��� �Ʒ��� ����
                        isLanding = false; // ���� ���� ����
                        break;
                    }

                    if (pos.y - size / 2 < blockBottom && pos.y + size / 2 > blockTop) {
                        pos.y = blockTop - size / 2; // ��� ���� ����
                        jumpVelocity = 0.0f; // �߷� �ӵ� �ʱ�ȭ
                        isLanding = true;
                        isJumping = false;
                        if (moveSpeed == 0.0f) { // �������� ���� ���� Idle
                            Pstate = PlayerState::Idle;
                        }
                        else {
                            Pstate = PlayerState::Move;
                        }
                        break;
                    }
                }
            }else if (SceneManager::GetInstance().GetSceneType() == SceneType::Stage2) {
                Stage2* stage = dynamic_cast<Stage2*>(SceneManager::GetInstance().GetCurrentScene());
                int(*currentMap)[8] = stage->GetMapData(stageNum);

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // ����� ���� ��� ��ŵ

                // ����� ��ǥ ���
                float blockTop = i * BLOCK_SIZE;
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // ���� ���� Ȯ��
                if (blockLeft < pos.x + size / 4 && pos.x - size / 4 < blockRight) {  // /4 ��, /2 �簢��
                    if (pos.y + size / 2 <= blockTop && pos.y + size / 2 + jumpVelocity * deltaTime >= blockTop) {
                        // ���� ó��
                        jumpVelocity = 0.0f;
                        pos.y = blockTop - size / 2; // ��� ���� ����
                        isLanding = true;
                        if (moveSpeed == 0.0f) { // �������� ���� ���� Idle
                            Pstate = PlayerState::Idle;
                        }
                        else {
                            Pstate = PlayerState::Move;
                        }
                        isJumping = false;
                        break;
                    }

                    if (pos.y - size / 2 >= blockBottom && pos.y - size / 2 + jumpVelocity * deltaTime < blockBottom) {
                        jumpVelocity = 0.0f; // �浹 �� �ӵ� �ʱ�ȭ
                        pos.y = blockBottom + size / 2; // ĳ���͸� ��� �Ʒ��� ����
                        isLanding = false; // ���� ���� ����
                        break;
                    }

                    if (pos.y - size / 2 < blockBottom && pos.y + size / 2 > blockTop) {
                        pos.y = blockTop - size / 2; // ��� ���� ����
                        jumpVelocity = 0.0f; // �߷� �ӵ� �ʱ�ȭ
                        isLanding = true;
                        isJumping = false;
                        if (moveSpeed == 0.0f) { // �������� ���� ���� Idle
                            Pstate = PlayerState::Idle;
                        }
                        else {
                            Pstate = PlayerState::Move;
                        }
                        break;
                    }
                }
            } else if (SceneManager::GetInstance().GetSceneType() == SceneType::Stage3) {
                Stage3* stage = dynamic_cast<Stage3*>(SceneManager::GetInstance().GetCurrentScene());
                int(*currentMap)[8] = stage->GetMapData(stageNum);

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // ����� ���� ��� ��ŵ

                // ����� ��ǥ ���
                float blockTop = i * BLOCK_SIZE;
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // ���� ���� Ȯ��
                if (blockLeft < pos.x + size / 4 && pos.x - size / 4 < blockRight) {  // /4 ��, /2 �簢��
                    if (pos.y + size / 2 <= blockTop && pos.y + size / 2 + jumpVelocity * deltaTime >= blockTop) {
                        // ���� ó��
                        jumpVelocity = 0.0f;
                        pos.y = blockTop - size / 2; // ��� ���� ����
                        isLanding = true;
                        if (moveSpeed == 0.0f) { // �������� ���� ���� Idle
                            Pstate = PlayerState::Idle;
                        }
                        else {
                            Pstate = PlayerState::Move;
                        }
                        isJumping = false;
                        break;
                    }

                    if (pos.y - size / 2 >= blockBottom && pos.y - size / 2 + jumpVelocity * deltaTime < blockBottom) {
                        jumpVelocity = 0.0f; // �浹 �� �ӵ� �ʱ�ȭ
                        pos.y = blockBottom + size / 2; // ĳ���͸� ��� �Ʒ��� ����
                        isLanding = false; // ���� ���� ����
                        break;
                    }

                    if (pos.y - size / 2 < blockBottom && pos.y + size / 2 > blockTop) {
                        pos.y = blockTop - size / 2; // ��� ���� ����
                        jumpVelocity = 0.0f; // �߷� �ӵ� �ʱ�ȭ
                        isLanding = true;
                        isJumping = false;
                        if (moveSpeed == 0.0f) { // �������� ���� ���� Idle
                            Pstate = PlayerState::Idle;
                        }
                        else {
                            Pstate = PlayerState::Move;
                        }
                        break;
                    }
                }
            }
        }
        if (isLanding) break;
    }

    // ���� ó��
    if (input == KT::Up && !isJumping) {
        isJumping = true;
        jumpVelocity = -jumpStrength; // ���� ����
        Pstate = PlayerState::Jump;
    }

    // ���� ���� ó��
    if (!isLanding) {
        Pstate = PlayerState::Jump; // ���� ����
    }

    if (input == KT::Left || input == KT::Right) {
        moveSpeed = (input == KT::Left ? -speed : speed) * deltaTime;
        if (Pstate != PlayerState::Jump) {
            Pstate = PlayerState::Move;
        }
    }
}



void CPlayer::UpdateAnimation(float deltaTime) {
    animationTimer += deltaTime;

    if (animationTimer >= animationInterval) {
        Movement = (Movement + 1) % 2;
        animationTimer = 0.0f;
    }
}

RECT CPlayer::GetBoundingBox() const {
    RECT boundingBox;
    boundingBox.left = static_cast<int>(pos.x - size / 2);
    boundingBox.top = static_cast<int>(pos.y - size / 2);
    boundingBox.right = static_cast<int>(pos.x + size / 2);
    boundingBox.bottom = static_cast<int>(pos.y + size / 2);
    return boundingBox;
}

bool CPlayer::CheckCollisionWithBlocks(const RECT& area) {
    return false;
}

bool CPlayer::CheckVerticalCollision() {
    RECT boundingBox = GetBoundingBox();

    // �ٿ�� �ڽ��� �Ʒ��� �˻� ����
    RECT footBox = boundingBox;
    footBox.top = boundingBox.bottom - 5; // �Ʒ��� 5�ȼ� �˻�
    footBox.bottom = boundingBox.bottom;

    return CheckCollisionWithBlocks(footBox);
}

bool CPlayer::CheckHorizontalCollision(int direction) {
    RECT boundingBox = GetBoundingBox();

    // ���⿡ ���� �˻� ���� ����
    RECT sideBox = boundingBox;
    if (direction < 0) { // ����
        sideBox.right = boundingBox.left;
        sideBox.left = boundingBox.left - 1;
    }
    else { // ������
        sideBox.left = boundingBox.right;
        sideBox.right = boundingBox.right + 1;
    }

    return CheckCollisionWithBlocks(sideBox);
}