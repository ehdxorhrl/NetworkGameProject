#include "stdafx.h"
#include "CPlayer.h"
#include "Stage1.h"

CPlayer::CPlayer() {}

CPlayer::~CPlayer() {}


void CPlayer::Init() {
    size = 50;
    speed = 350;
    input = KT::None;
    input_time = 0;
    playerID = 0;
    Ptype = PlayerType::P2;
}

void CPlayer::Update() {

    float deltaTime = (float)input_time / 1000;

    // �߷� ó��
    pos.y += jumpVelocity * deltaTime; // ���� �ӵ� ����
    jumpVelocity += gravity * deltaTime; // �߷� ����

    // �¿� �̵� �ӵ�
    float moveSpeed = 0.0f;
    if (input==KT::Left) {
        moveSpeed = -speed * deltaTime; // ���� �̵�
    }
    else if (input == KT::Right) {
        moveSpeed = speed * deltaTime; // ���� �̵�
    }

    // �¿� �浹 �˻�
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (Stage1* stage = dynamic_cast<Stage1*>(SceneManager::GetInstance().GetCurrentScene())) {
                int(*currentMap)[8] = stage->GetMapData(stage->GetCurrentMapID());

                if (currentMap[i][j] != 1) continue; // ����� ���� ��� ��ŵ

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
            if (Stage1* stage = dynamic_cast<Stage1*>(SceneManager::GetInstance().GetCurrentScene())) {
                int(*currentMap)[8] = stage->GetMapData(stage->GetCurrentMapID());

                if (currentMap[i][j] != 1) continue; // ����� ���� ��� ��ŵ

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
                        isJumping = false;
                        Pstate = PlayerState::Idle;
                        break;
                    }

                    if (pos.y - size / 2 >= blockBottom && pos.y - size / 2 + jumpVelocity * deltaTime < blockBottom) {
                        jumpVelocity = 0.0f; // �浹 �� �ӵ� �ʱ�ȭ
                        pos.y = blockBottom + size / 2; // ĳ���͸� ��� �Ʒ��� ����
                        isLanding = false; // ���� ���� ����
                        break;
                    }

                    // ��� ���ο� �ִ� ��� ó��
                    if (pos.y - size / 2 < blockBottom && pos.y + size / 2 > blockTop) {
                        pos.y = blockTop - size / 2; // ��� ���� ����
                        jumpVelocity = 0.0f; // �߷� �ӵ� �ʱ�ȭ
                        isLanding = true;
                        isJumping = false;
                        Pstate = PlayerState::Idle;
                        break;
                    }

                }
            }
        }
        if (isLanding) break;
    }

    // ���� ���� ó��
    if (!isLanding) {
        Pstate = PlayerState::Jump; // ���� ����
    }

    // ���� ó��
    if (input==KT::Up && !isJumping) {
        isJumping = true;
        jumpVelocity = -jumpStrength; // ���� ����
        Pstate = PlayerState::Jump;
    }
    input_time = 0;
    input = KT::None;
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