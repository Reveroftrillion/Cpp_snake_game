C++ 프로그래밍 스테이크 게임 구현 10조

전체 코드 분석
- Block.h = 모든 게임 오브젝트의 초상 자료형 정의 
- Map.h = 맵 크기, 벽 생성, 스네이크 초기 배치 등 정적 월드 데이터를 관리
- Game.h = UI 그리기, 입력 처리, 아이템 & 게이트 & 미션 로직, 충돌 판정
- Main.cpp = 실행 진입

UI 분석
- 흰색 = 벽
- 흰 바탕에 검은 + 글자 = 면역 벽 = 게이트가 생성되지 않는 모서리
- 청록색 = 머리
- 초록색 = 몸통
- 파랑색 = Growth = 먹으면 몸통 1 증가
- 빨강색 = Poison = 먹으면 몸통 1 감소, 몸통이 3일 때 먹으면 게임 오버
- 자홍색 = Gate
- 노랑색 = TimeItem = 먹으면 40 tick 동안 속도가 1.5배 증가

- GrowthItem, PoisonItem의 스폰 주기 = 50 tick
- TimeItem의 스폰 주기 = 30 tick
- Gate의 스폰 주기 = 80 tick
- ShieldItem의 스폰 주기 = 40 tick

미션 - 다 깨면 다음 스테이지로 넘어간다.
- 몸 길이 >= 7
- GrowthItem 획득 >= 5
- PoisonItem 획득 >= 2
- Gate 통과 >= 1
- Shield는 미션에 포함시키지 않는다.

하고 있는 과제
1. 직사각형 모양으로 보이는 문제 = vscode의 문제, 굳이 코드를 건드릴 필요가 없음
2. ShieldItem 제작 - 주황색 = 지금 오류가 너무 많이 생김
3. RandomItem 제작 - 핑크색
4. ETC...
