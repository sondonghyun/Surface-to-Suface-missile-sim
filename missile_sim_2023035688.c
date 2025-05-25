#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define GRAVITY 9.8  
#define TRACK_LENGTH 50

typedef struct {
    double speed;
    double angle;
    double vx, vy;
    double x, y;
    double launchTime;
    bool isLaunched;
} Missile;

typedef struct {
    bool isArmed;
    bool electronicFailed;
    double impactForce;
    double threshold;
    bool detonated;
} Fuze;

typedef struct {
    double x;
    double y;
} Target;

double calculateLaunchAngle(double speed, double targetX, double targetY) {
    double v = speed, x = targetX, y = targetY, g = GRAVITY;
    double a = g * x * x / (2 * v * v);
    double b = -x;
    double c = y + g * x * x / (2 * v * v);

    double discrim = b * b - 4 * a * c;
    if (discrim < 0) {
        printf("목표물 타격을 위한 발사 각도 계산이 불가능합니다!\n");
        return -1.0;
    }

    double tanTheta1 = (-b + sqrt(discrim)) / (2 * a);
    double tanTheta2 = (-b - sqrt(discrim)) / (2 * a);

    double theta1 = atan(tanTheta1) * 180.0 / M_PI;
    double theta2 = atan(tanTheta2) * 180.0 / M_PI;

    printf("Possible angles: %.2f degrees, %.2f degrees\n", theta1, theta2);

    double selectedTheta = -1.0;
    if (theta1 > 0 && theta1 < 90) {
        selectedTheta = theta1;
    }
    if (theta2 > 0 && theta2 < 90 && (selectedTheta < 0 || theta2 < selectedTheta)) {
        selectedTheta = theta2;
    }

    if (selectedTheta < 0) {
        printf("0-90도 사이에 가능한 발사 각도 없음!\n");
        return -1.0;
    }

    return selectedTheta;
}

void initMissile(Missile* missile, double speed, double angle) {
    missile->speed = speed;
    missile->angle = angle * M_PI / 180.0;
    missile->vx = speed * cos(missile->angle);
    missile->vy = speed * sin(missile->angle);
    missile->x = 0.0;
    missile->y = 0.0;
    missile->launchTime = 0.0;
    missile->isLaunched = false;
}

void initFuze(Fuze* fuze, double threshold, bool electronicFailed) {
    fuze->isArmed = false;
    fuze->electronicFailed = electronicFailed;
    fuze->impactForce = 0.0;
    fuze->threshold = threshold;
    fuze->detonated = false;
}

void initTarget(Target* target, double x, double y) {
    target->x = x;
    target->y = y;
}

void armFuze(Fuze* fuze, double currentTime, double launchTime) {
    if (currentTime - launchTime > 1.0) {
        fuze->isArmed = true;
        printf("Fuze armed at %.2f seconds!\n", currentTime);
    }
}

void updateMissilePosition(Missile* missile, double currentTime) {
    if (missile->isLaunched) {
        double t = currentTime - missile->launchTime;
        missile->x = missile->vx * t;
        missile->y = missile->vy * t - 0.5 * GRAVITY * t * t;
        if (missile->y < 0.0) missile->y = 0.0;
        printf("Debug: Position updated to (%.2f, %.2f) m at time %.2f s\n", missile->x, missile->y, currentTime);
    }
}

void visualize(Missile* missile, Target* target, double targetDistance, double currentTime) {
    char track[TRACK_LENGTH + 1];
    for (int i = 0; i < TRACK_LENGTH; i++) {
        track[i] = '-';
    }
    track[TRACK_LENGTH] = '\0';

    int missilePos = (int)(missile->x / targetDistance * TRACK_LENGTH);
    if (missilePos >= TRACK_LENGTH) missilePos = TRACK_LENGTH - 1;
    if (missilePos < 0) missilePos = 0;

    int targetPos = TRACK_LENGTH - 1;

    if (missilePos == targetPos && fabs(missile->x - target->x) <= 5.0 && fabs(missile->y - target->y) <= 5.0) {
        track[missilePos] = 'X';
    }
    else {
        track[targetPos] = 'T';
        if (missilePos != targetPos) {
            track[missilePos] = '*';
        }
    }

    printf("Time: %.2f s, Position: (%.2f, %.2f) m\n", currentTime - missile->launchTime, missile->x, missile->y);
    printf("[%s]\n\n", track);
}

bool checkCollision(Missile* missile, Target* target) {
    double dx = fabs(missile->x - target->x);
    double dy = fabs(missile->y - target->y);
    printf("Debug: 충돌 감지 확인 - dx: %.2f, dy: %.2f\n", dx, dy);
    return dx <= 5.0 && dy <= 5.0;
}

void processImpact(Fuze* fuze, double impactForce) {
    fuze->impactForce = impactForce;
    printf("Debug: Fuze - isArmed: %d, impactForce: %.2f, threshold: %.2f\n", fuze->isArmed, fuze->impactForce, fuze->threshold);
    if (!fuze->isArmed) {
        printf("퓨즈가 불능 상태입니다, 폭발 불가.\n");
        return;
    }
    if (!fuze->electronicFailed) {
        if (fuze->impactForce >= fuze->threshold) {
            fuze->detonated = true;
            printf("Electronic fuze: 탄두 폭파 (force: %.2f)!\n", impactForce);
        }
        else {
            printf("Electronic fuze: 실제 충격력 (%.2f)이 임계값(%.2f) 보다 작습니다, 탄두 폭파 불가.\n", impactForce, fuze->threshold);
        }
    }
    else {
        printf("Electronic fuze failed, using mechanical backup.\n");
        if (fuze->impactForce >= fuze->threshold) {
            fuze->detonated = true;
            printf("Mechanical backup: 탄두 폭파 (force: %.2f)!\n", impactForce);
        }
        else {
            printf("Mechanical backup: 실제 충격력 (%.2f)이 임계값(%.2f) 보다 작습니다, 탄두 폭파 불가.\n", impactForce, fuze->threshold);
        }
    }
}

int main() {
    Missile missile;
    Fuze fuze;
    Target target;
    double speed, targetX, targetY;
    bool electronicFailed;
    char failInput;

    printf("Enter missile speed (m/s): ");
    scanf("%lf", &speed);
    printf("Enter target x-coordinate (meters): ");
    scanf("%lf", &targetX);
    printf("Enter target y-coordinate (meters, height): ");
    scanf("%lf", &targetY);
    printf("Is electronic fuze failed? (y/n): ");
    scanf(" %c", &failInput);
    electronicFailed = (failInput == 'y' || failInput == 'Y');

    if (speed <= 0 || targetX <= 0 || targetY < 0) {
        printf("잘못된 입력입니다! 속도는 0m/s 이하 이거나 목표물까지의 x축 거리, 목표물의 y축 높이는 음수일 수 없습니다.\n");
        return 1;
    }

    double angle = calculateLaunchAngle(speed, targetX, targetY);
    if (angle < 0) {
        printf("잘못된 발사 각도로 인하여 시뮬레이션이 중단됩니다.\n");
        return 1;
    }
    printf("Selected launch angle: %.2f degrees\n", angle);

    initMissile(&missile, speed, angle);
    initFuze(&fuze, 1000.0, electronicFailed);
    initTarget(&target, targetX, targetY);

    printf("\n--- Missile Launch Simulation ---\n");
    missile.isLaunched = true;
    missile.launchTime = 0.0;
    double currentTime = 0.0;
    double targetDistance = targetX;

    while (!fuze.detonated && missile.x <= targetDistance + 5.0) {
        updateMissilePosition(&missile, currentTime);
        visualize(&missile, &target, targetDistance, currentTime);
        armFuze(&fuze, currentTime, missile.launchTime);
        if (checkCollision(&missile, &target)) {
            printf("충돌이 (%.2f, %.2f) m 에서 감지되었습니다!\n", missile.x, missile.y);
            processImpact(&fuze, 1500.0);
            break;
        }
        currentTime += 0.01; 
        for (volatile int i = 0; i < 5000000; i++); // 지연 조정
    }

    if (!fuze.detonated) {
        printf("미사일이 목표물을 놓쳤거나 폭발 실패!\n");
        printf("최종 위치: (%.2f, %.2f) m\n", missile.x, missile.y);
    }
    else {
        printf("시뮬레이션 완료: 목표물 타격 및 폭발 완료!\n");
    }

    return 0;
}