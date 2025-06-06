# 2차원 지대지 미사일 시뮬레이션 (C언어)

![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)

이 프로젝트는 C언어로 구현한 2차원 지대지 미사일 시뮬레이션으로, 원점(0,0)에서 발사된 미사일이 사용자 입력으로 지정한 목표물의 x좌표(거리)와 y좌표(높이, 예: 1000m, 50m)에 도달하도록 최적 발사 각도를 계산합니다. 미사일은 중력(9.8m/s²)을 고려한 포물선 궤적을 따르며, 진행 상황은 ASCII 기반 1차원 트랙(`[*----------------------------------------T]`)으로 시각화됩니다. 접촉 퓨즈(전자 및 기계식 백업)는 발사 1초 후 활성화되며, 충격 강도가 임계값을 초과하면 폭발합니다. 이 프로젝트는 공학 전공 학부생을 위한 교육용으로, 물리학(포물선 운동), 프로그래밍(구조체, 포인터, 수치 계산), 디버깅 기술을 학습하는 데 목적이 있습니다.

## 기능
- **물리 기반 궤적 계산**: 2차 방정식을 사용해 목표물(x, y)에 도달하는 발사 각도 계산.
- **1차원 시각화**: 50칸 트랙에 미사일('*'), 목표물('T'), 명중('X') 표시.
- **퓨즈 로직**: 전자 퓨즈(고장 시 기계식 백업), 1초 후 활성화, 충격 강도(1500)와 임계값(1000) 비교.
- **입력 검증**: 속도(>0), 목표물 x(>0), y(≥0) 유효성 확인.
- **디버깅 출력**: 미사일 위치, 충돌 검사, 퓨즈 상태 로깅.

## 필수 요구사항
- **C 컴파일러**: Visual Studio 2022(MSVC) 권장, GCC 또는 Clang도 호환.
- **운영체제**: Windows(Visual Studio), Linux, macOS(GCC/Clang).
- **종속성**: 표준 C 라이브러리(`stdio.h`, `stdlib.h`, `math.h`).
- **선택 사항**: 비-Windows 환경에서는 `M_PI`를 위해 `_USE_MATH_DEFINES` 정의 필요.

## 설치 방법
1. **저장소 복제**:
   ```bash
   git clone https://github.com/your-username/2d-missile-simulation.git
   cd 2d-missile-simulation

## 사용 방법
1. **프로그램 실행**:
   - Visual Studio 2022: 솔루션 파일(`missile_simulation.sln`)을 열고 빌드 후 실행(F5)하거나, 새 프로젝트에 `missile.c`를 추가하여 실행.
   - 터미널(Linux/macOS): `./missile` 명령어 실행.
   - 터미널(Windows): `missile.exe` 실행.
2. **입력 매개변수**:
   - **미사일 속도(m/s)**: 미사일의 초기 속도(예: 200). 양수 입력.
   - **목표물 x좌표(m)**: 목표물까지의 수평 거리(예: 1000). 양수 입력.
   - **목표물 y좌표(m, 높이)**: 목표물의 높이(예: 50). 0 이상 입력.
   - **전자 퓨즈 고장 여부(y/n)**: 전자 퓨즈 고장 시 ‘y’, 정상 시 ‘n’(예: y).
3. **출력**:
   - 계산된 발사 각도(예: 43.67도).
   - 실시간 1차원 트랙: 미사일('*')이 목표물('T')로 이동, 명중 시 ‘X’ 표시.
   - 충돌 및 퓨즈 상태: “목표물 명중 및 폭발!” 또는 “미사일 실패” 메시지.
   - 디버깅 정보: 미사일 위치, 충돌 오차(dx, dy), 퓨즈 상태 출력.

**실행 예시**:
```bash
미사일 속도를 입력하세요 (m/s): 200
목표물 x좌표를 입력하세요 (m): 1000
목표물 y좌표를 입력하세요 (m, 높이): 50
전자 퓨즈 고장 여부 (y/n): y
가능한 발사 각도: 82.86도, 9.96도
선택된 발사 각도: 9.96도
--- 미사일 발사 시뮬레이션 ---
시간: 0.00초, 위치: (0.00, 0.00) m
[*----------------------------------------T]
...
시간: 7.07초, 위치: (1000.02, 50.03) m
[-----------------------------------------X]
충돌 감지: (1000.02, 50.03) m!
전자 퓨즈 고장, 기계식 백업 사용.
기계식 백업: 탄두 폭발 (충격 강도: 1500.00)!
시뮬레이션 완료: 목표물 명중 및 폭발!
```

## 코드 구조
- **파일**:
  - `missile.c`: 시뮬레이션의 모든 로직을 포함한 단일 소스 파일.
- **구조체**:
  - `Missile`: 미사일의 속도, 발사 각도(라디안), 속도 성분(vx, vy), 현재 위치(x, y), 발사 시간, 발사 상태 저장.
  - `Fuze`: 퓨즈의 활성화 여부, 전자 퓨즈 고장 여부, 충격 강도, 임계값, 폭발 여부 관리.
  - `Target`: 목표물의 x, y 좌표 저장.
- **주요 함수**:
  - `calculateLaunchAngle`: 목표물 좌표(x, y)에 도달하는 발사 각도를 2차 방정식으로 계산, 낮은 각도 선택.
  - `initMissile`, `initFuze`, `initTarget`: 각각 미사일, 퓨즈, 목표물 구조체 초기화.
  - `updateMissilePosition`: 포물선 운동 공식으로 미사일 위치(x, y) 갱신.
  - `visualize`: 미사일의 x좌표를 스케일링하여 1차원 트랙에 미사일('*'), 목표물('T'), 명중('X') 렌더링.
  - `checkCollision`: 미사일과 목표물의 x, y 좌표가 ±5m 이내인지 확인.
  - `processImpact`: 퓨즈 로직 처리(전자/기계식, 충격 강도 비교).
  - `armFuze`: 발사 1초 후 퓨즈 활성화.
- **프로그램 흐름**:
  1. 사용자 입력(속도, 목표물 좌표, 퓨즈 상태) 검증 및 구조체 초기화.
  2. 시뮬레이션 루프(0.01초 간격): 위치 갱신, 트랙 시각화, 퓨즈 상태 확인, 충돌 감지, 퓨즈 폭발 처리.
  3. 충돌 시 폭발 또는 목표물 초과 시 실패로 종료.
