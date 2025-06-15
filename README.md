# Snake Game (Team 10, C++ / ncurses)

C++17과 **ncurses** 라이브러리로 구현한 콘솔 Snake Game입니다.  
간단한 `g++` 한 줄 빌드만으로 동작하도록 구성되어 있으며, 추가 편의용 도구(CMake, Makefile)는 **선택 사항**입니다.

---

## 목차
1. [파일 구조](#파일-구조)
2. [게임 오브젝트/색상 표](#게임-오브젝트색상-표)
3. [기본 Rules](#기본-rules)
4. [Custom Rules](#custom-rules)
5. [미션 시스템](#미션-시스템)
6. [빌드 & 실행](#빌드--실행)
7. [플레이 스크린샷](#플레이-스크린샷)

---

## 파일 구조
.
├── README.md ← (현재 파일)
└── src
├── block.h ← 게임 오브젝트 기반 클래스 & 상수
├── map.h ← 맵·벽·스네이크 초기화, 아이템 스폰
├── game.h ← 렌더링, 입력 처리, 충돌·미션 로직
└── main.cpp ← 진입점 (Game 객체 실행)

markdown
복사
편집

> **주의:**  
> 별도의 CMakeLists.txt 나 Makefile은 포함하지 않았습니다.  
> • IDE 통합을 원한다면 자유롭게 추가하세요.  
> • 그냥 터미널에서 한 줄로 빌드해도 문제없습니다.

---

## 게임 오브젝트/색상 표

| 코드 상수 | 표시 색 | 설명 | 효과 |
|-----------|---------|------|------|
| `Wall`              | **흰색**        | 외곽·내부 벽 | 충돌 = Game Over |
| `ImmunedWall`       | 흰 배경 ＋ `+`  | 네 모서리 면역벽 | Gate 스폰 불가 |
| `Snake Head`        | **청록색**      | 스네이크 머리 | 방향키로 이동 |
| `Snake Body`        | **초록색**      | 스네이크 몸통 | 머리와 충돌 불가 |
| `GrowthItem (+)`    | **파랑**        | 몸 +1 | 길이 증가 |
| `PoisonItem (-)`    | **빨강**        | 몸 −1 | 길이 3 이하 → Game Over |
| `TimeItem (T)`      | **노랑**        | 속도 ×1.5 (40 tick) | |
| `ShieldItem (S)`    | **형광 초록**   | 무적 40 tick | 벽·몸 무시, 맵 밖은 **불가** |
| `RandomItem (R)`    | **핑크**        | 위 4가지 중 랜덤 효과 | |
| `Gate`              | **자홍**        | 한 쌍 이동 통로 | 규칙 4 참조 |

---

## 기본 Rules
1. **이동**   
   * 방향키 (중복 입력 무시)  
   * 반대 방향 180° 급선회 불가  
   * 벽·몸 충돌 → Game Over
2. **아이템**   
   * 머리가 닿으면 즉시 적용 & 새 위치로 재스폰  
   * 스폰 주기  
     * Growth / Poison : 50 tick  
     * Time : 30 tick
3. **Gate**   
   * 언제나 **쌍**으로 스폰, 겹치지 않음  
   * 통과 중엔 소멸/이동 X  
   * 스폰 주기 : 80 tick
4. **Gate 방향 규칙**   
   * 가장자리 Wall → 항상 맵 안쪽으로  
   * 중앙 Wall → 진입 → 시계 방향 → 반시계 → 반대 순
5. **Wall** 충돌 시 Game Over
6. **점수판**  
   * `B`: 현재/최대 길이  
   * `+`, `-`, `G`, `S` 카운트  
   * `time`: 경과 초

---

## Custom Rules
|   | 내용 |
|---|------|
| **스테이지 벽** | 길이 4 ~ 10 랜덤 벽 2 ~ 6개 |
| **Tick**       | 스테이지마다 50 ~ 300 ms |
| **TimeItem**   | 속도 1.5 × (40 tick) |
| **ShieldItem** | 무적 40 tick (맵 밖 이동은 금지) |
| **RandomItem** | 70 tick 마다 스폰 · 4종 효과 중 랜덤 |

---

## 미션 시스템
스테이지별 **ALL CLEAR 조건** — 전부 달성 시 다음 스테이지 자동 생성
| 항목 | 조건 |
|------|------|
| 길이 (B) | ≥ 7 |
| GrowthItem | ≥ 5 |
| PoisonItem | ≥ 2 |
| Gate 통과 | ≥ 1 |
※ Shield / Random 아이템은 미션 집계 제외

---

## 빌드 & 실행

```bash
# ① 의존 라이브러리
sudo apt-get install libncurses5-dev     # Debian/Ubuntu 예시

# ② 빌드
g++ -std=c++17 src/main.cpp -lncurses -o snake

# ③ 실행
./snake
| **RandomItem** | 70 tick 마다 스폰 · 4종 효과 중 랜덤 |

---

## 플레이 스크린샷
