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

    float serverDeltaTime = TimeManager::GetInstance().GetDeltaTime(); // 서버 시간 기반 deltaTime

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

    // 중력 처리
    pos.y += jumpVelocity * deltaTime; // 점프 속도 적용
    jumpVelocity += gravity * deltaTime; // 중력 가속

    // 좌우 이동 속도
    float moveSpeed = 0.0f;

    if (input == KT::Left) {
        moveSpeed = -speed * deltaTime; // 좌측 이동
        if (Pstate != PlayerState::Jump) { // 점프 상태가 아닐 때만 Move로 전환
            Pstate = PlayerState::Move;
            UpdateAnimation(serverDeltaTime);
        }
    } else if (input == KT::Right) {
        moveSpeed = speed * deltaTime; // 우측 이동
        if (Pstate != PlayerState::Jump) { // 점프 상태가 아닐 때만 Move로 전환
            Pstate = PlayerState::Move;
            UpdateAnimation(serverDeltaTime);
        }
    }

    // 좌우 충돌 검사
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (SceneManager::GetInstance().GetSceneType()==SceneType::Stage1) {
                Stage1* stage = dynamic_cast<Stage1*>(SceneManager::GetInstance().GetCurrentScene());
                int(*currentMap)[8] = stage->GetMapData(stageNum);

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // 블록이 없는 경우 스킵

                // 블록의 좌표 계산
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockTop = i * BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // y축 충돌 범위 확인
                if (!(blockTop < pos.y + size / 2 && pos.y - size / 2 < blockBottom)) continue;

                if (moveSpeed < 0.f) { // 좌측 이동
                    if (pos.x - size / 2 < blockRight) continue; // 오른쪽에 있는 블록 제외

                    if (abs(blockRight - (pos.x - size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // 정확히 맞닿으면 이동 멈춤
                        break;
                    }

                    if (pos.x - size / 2 + moveSpeed < blockRight) {
                        float gap = blockRight - (pos.x - size / 2);
                        moveSpeed = gap; // 이동 간격 조정
                    }
                } else if (moveSpeed > 0.f) { // 우측 이동
                    if (pos.x + size / 2 > blockLeft) continue; // 왼쪽에 있는 블록 제외

                    if (abs(blockLeft - (pos.x + size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // 정확히 맞닿으면 이동 멈춤
                        break;
                    }

                    if (pos.x + size / 2 + moveSpeed > blockLeft) {
                        float gap = blockLeft - (pos.x + size / 2);
                        moveSpeed = gap; // 이동 간격 조정
                    }
                }
            } else if (SceneManager::GetInstance().GetSceneType() == SceneType::Stage2) {
                Stage2* stage = dynamic_cast<Stage2*>(SceneManager::GetInstance().GetCurrentScene());
                int(*currentMap)[8] = stage->GetMapData(stageNum);

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // 블록이 없는 경우 스킵

                // 블록의 좌표 계산
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockTop = i * BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // y축 충돌 범위 확인
                if (!(blockTop < pos.y + size / 2 && pos.y - size / 2 < blockBottom)) continue;

                if (moveSpeed < 0.f) { // 좌측 이동
                    if (pos.x - size / 2 < blockRight) continue; // 오른쪽에 있는 블록 제외

                    if (abs(blockRight - (pos.x - size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // 정확히 맞닿으면 이동 멈춤
                        break;
                    }

                    if (pos.x - size / 2 + moveSpeed < blockRight) {
                        float gap = blockRight - (pos.x - size / 2);
                        moveSpeed = gap; // 이동 간격 조정
                    }
                }
                else if (moveSpeed > 0.f) { // 우측 이동
                    if (pos.x + size / 2 > blockLeft) continue; // 왼쪽에 있는 블록 제외

                    if (abs(blockLeft - (pos.x + size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // 정확히 맞닿으면 이동 멈춤
                        break;
                    }

                    if (pos.x + size / 2 + moveSpeed > blockLeft) {
                        float gap = blockLeft - (pos.x + size / 2);
                        moveSpeed = gap; // 이동 간격 조정
                    }
                }
            }
            else if (SceneManager::GetInstance().GetSceneType() == SceneType::Stage3) {
                Stage3* stage = dynamic_cast<Stage3*>(SceneManager::GetInstance().GetCurrentScene());
                int(*currentMap)[8] = stage->GetMapData(stageNum);

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // 블록이 없는 경우 스킵

                // 블록의 좌표 계산
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockTop = i * BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // y축 충돌 범위 확인
                if (!(blockTop < pos.y + size / 2 && pos.y - size / 2 < blockBottom)) continue;

                if (moveSpeed < 0.f) { // 좌측 이동
                    if (pos.x - size / 2 < blockRight) continue; // 오른쪽에 있는 블록 제외

                    if (abs(blockRight - (pos.x - size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // 정확히 맞닿으면 이동 멈춤
                        break;
                    }

                    if (pos.x - size / 2 + moveSpeed < blockRight) {
                        float gap = blockRight - (pos.x - size / 2);
                        moveSpeed = gap; // 이동 간격 조정
                    }
                }
                else if (moveSpeed > 0.f) { // 우측 이동
                    if (pos.x + size / 2 > blockLeft) continue; // 왼쪽에 있는 블록 제외

                    if (abs(blockLeft - (pos.x + size / 2)) < 0.001f) {
                        moveSpeed = 0.0f; // 정확히 맞닿으면 이동 멈춤
                        break;
                    }

                    if (pos.x + size / 2 + moveSpeed > blockLeft) {
                        float gap = blockLeft - (pos.x + size / 2);
                        moveSpeed = gap; // 이동 간격 조정
                    }
                }
            }
        }
    }

    // 이동 적용
    pos.x += moveSpeed;

    // 착지 상태 처리 및 블록 내부 판정
    bool isLanding = false;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (SceneManager::GetInstance().GetSceneType() == SceneType::Stage1) {
                Stage1* stage = dynamic_cast<Stage1*>(SceneManager::GetInstance().GetCurrentScene());
                int(*currentMap)[8] = stage->GetMapData(stageNum);

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // 블록이 없는 경우 스킵

                // 블록의 좌표 계산
                float blockTop = i * BLOCK_SIZE;
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // 착지 조건 확인
                if (blockLeft < pos.x + size / 4 && pos.x - size / 4 < blockRight) {  // /4 발, /2 사각형
                    if (pos.y + size / 2 <= blockTop && pos.y + size / 2 + jumpVelocity * deltaTime >= blockTop) {
                        // 착지 처리
                        jumpVelocity = 0.0f;
                        pos.y = blockTop - size / 2; // 블록 위로 정렬
                        isLanding = true;
                        if (moveSpeed == 0.0f) { // 움직임이 없을 때만 Idle
                            Pstate = PlayerState::Idle;
                        }
                        else {
                            Pstate = PlayerState::Move;
                        }
                        isJumping = false;
                        break;
                    }

                    if (pos.y - size / 2 >= blockBottom && pos.y - size / 2 + jumpVelocity * deltaTime < blockBottom) {
                        jumpVelocity = 0.0f; // 충돌 시 속도 초기화
                        pos.y = blockBottom + size / 2; // 캐릭터를 블록 아래로 정렬
                        isLanding = false; // 공중 상태 유지
                        break;
                    }

                    if (pos.y - size / 2 < blockBottom && pos.y + size / 2 > blockTop) {
                        pos.y = blockTop - size / 2; // 블록 위로 고정
                        jumpVelocity = 0.0f; // 중력 속도 초기화
                        isLanding = true;
                        isJumping = false;
                        if (moveSpeed == 0.0f) { // 움직임이 없을 때만 Idle
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

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // 블록이 없는 경우 스킵

                // 블록의 좌표 계산
                float blockTop = i * BLOCK_SIZE;
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // 착지 조건 확인
                if (blockLeft < pos.x + size / 4 && pos.x - size / 4 < blockRight) {  // /4 발, /2 사각형
                    if (pos.y + size / 2 <= blockTop && pos.y + size / 2 + jumpVelocity * deltaTime >= blockTop) {
                        // 착지 처리
                        jumpVelocity = 0.0f;
                        pos.y = blockTop - size / 2; // 블록 위로 정렬
                        isLanding = true;
                        if (moveSpeed == 0.0f) { // 움직임이 없을 때만 Idle
                            Pstate = PlayerState::Idle;
                        }
                        else {
                            Pstate = PlayerState::Move;
                        }
                        isJumping = false;
                        break;
                    }

                    if (pos.y - size / 2 >= blockBottom && pos.y - size / 2 + jumpVelocity * deltaTime < blockBottom) {
                        jumpVelocity = 0.0f; // 충돌 시 속도 초기화
                        pos.y = blockBottom + size / 2; // 캐릭터를 블록 아래로 정렬
                        isLanding = false; // 공중 상태 유지
                        break;
                    }

                    if (pos.y - size / 2 < blockBottom && pos.y + size / 2 > blockTop) {
                        pos.y = blockTop - size / 2; // 블록 위로 고정
                        jumpVelocity = 0.0f; // 중력 속도 초기화
                        isLanding = true;
                        isJumping = false;
                        if (moveSpeed == 0.0f) { // 움직임이 없을 때만 Idle
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

                if (currentMap[i][j] != 2 && currentMap[i][j] != 6) continue; // 블록이 없는 경우 스킵

                // 블록의 좌표 계산
                float blockTop = i * BLOCK_SIZE;
                float blockLeft = j * BLOCK_SIZE;
                float blockRight = blockLeft + BLOCK_SIZE;
                float blockBottom = blockTop + BLOCK_SIZE;

                // 착지 조건 확인
                if (blockLeft < pos.x + size / 4 && pos.x - size / 4 < blockRight) {  // /4 발, /2 사각형
                    if (pos.y + size / 2 <= blockTop && pos.y + size / 2 + jumpVelocity * deltaTime >= blockTop) {
                        // 착지 처리
                        jumpVelocity = 0.0f;
                        pos.y = blockTop - size / 2; // 블록 위로 정렬
                        isLanding = true;
                        if (moveSpeed == 0.0f) { // 움직임이 없을 때만 Idle
                            Pstate = PlayerState::Idle;
                        }
                        else {
                            Pstate = PlayerState::Move;
                        }
                        isJumping = false;
                        break;
                    }

                    if (pos.y - size / 2 >= blockBottom && pos.y - size / 2 + jumpVelocity * deltaTime < blockBottom) {
                        jumpVelocity = 0.0f; // 충돌 시 속도 초기화
                        pos.y = blockBottom + size / 2; // 캐릭터를 블록 아래로 정렬
                        isLanding = false; // 공중 상태 유지
                        break;
                    }

                    if (pos.y - size / 2 < blockBottom && pos.y + size / 2 > blockTop) {
                        pos.y = blockTop - size / 2; // 블록 위로 고정
                        jumpVelocity = 0.0f; // 중력 속도 초기화
                        isLanding = true;
                        isJumping = false;
                        if (moveSpeed == 0.0f) { // 움직임이 없을 때만 Idle
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

    // 점프 처리
    if (input == KT::Up && !isJumping) {
        isJumping = true;
        jumpVelocity = -jumpStrength; // 위로 점프
        Pstate = PlayerState::Jump;
    }

    // 공중 상태 처리
    if (!isLanding) {
        Pstate = PlayerState::Jump; // 공중 상태
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

    // 바운딩 박스의 아래쪽 검사 영역
    RECT footBox = boundingBox;
    footBox.top = boundingBox.bottom - 5; // 아래쪽 5픽셀 검사
    footBox.bottom = boundingBox.bottom;

    return CheckCollisionWithBlocks(footBox);
}

bool CPlayer::CheckHorizontalCollision(int direction) {
    RECT boundingBox = GetBoundingBox();

    // 방향에 따라 검사 영역 설정
    RECT sideBox = boundingBox;
    if (direction < 0) { // 왼쪽
        sideBox.right = boundingBox.left;
        sideBox.left = boundingBox.left - 1;
    }
    else { // 오른쪽
        sideBox.left = boundingBox.right;
        sideBox.right = boundingBox.right + 1;
    }

    return CheckCollisionWithBlocks(sideBox);
}