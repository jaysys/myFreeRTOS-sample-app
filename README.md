# myTaskApp 개발 가이드

이 문서는 FreeRTOS POSIX 시뮬레이터 환경에서 myTaskApp을 개발 및 빌드하는 방법을 안내합니다.

---

## 0. FreeRTOS 커널 소스 다운로드 방법

FreeRTOS 공식 커널 소스는 [GitHub FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel)에서 받을 수 있습니다.

### (1) git clone 이용
```sh
git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git
mv FreeRTOS-Kernel src/FreeRTOS-Kernel-src
```

### (2) ZIP 파일 다운로드
- [FreeRTOS-Kernel-src.zip](https://github.com/FreeRTOS/FreeRTOS-Kernel/archive/refs/heads/main.zip) 파일을 다운로드 후 압축 해제
- 압축 해제한 폴더를 `src/FreeRTOS-Kernel-src`으로 이동/이름 변경

> **중요:**
> - POSIX 포팅(`portable/ThirdParty/GCC/Posix`)과 메모리 관리(`portable/MemMang/heap_4.c`) 파일이 포함되어 있는지 확인하세요.

---

## 1. 폴더 구조 및 소스 배치

```
myTaskApp/
├── Makefile
├── README.md
├── build/                # 빌드 결과물이 생성되는 디렉터리
├── src/
│   ├── main.c            # 사용자 애플리케이션 진입점
│   └── FreeRTOS-Kernel-src/   # 다운받은 FreeRTOS 커널 소스
│       ├── tasks.c, queue.c, ...
│       ├── include/
│       └── portable/
│           └── ThirdParty/GCC/Posix/
│               ├── port.c
│               └── utils/
│                   └── wait_for_event.c
│           └── MemMang/
│               └── heap_4.c
```

- **FreeRTOS 커널 소스**는 `src/FreeRTOS-Kernel-src/`에 위치시킵니다.
- POSIX 포팅 관련 소스(`port.c`, `wait_for_event.c`)와 메모리 매니저(`heap_4.c`)도 해당 위치에 포함되어야 합니다.

---

## 2. Makefile 구성 요약

- **빌드 대상:** `build/myTaskApp`
- **컴파일러:** gcc (macOS 기준)
- **주요 변수:**
  - `FREERTOS_DIR = src/FreeRTOS-Kernel-src`
  - `APP_SRC_DIR = src`
  - `C_SRCS`: FreeRTOS 커널 및 포팅, 앱 소스 파일 목록
  - `VPATH`, `INCLUDES`: 소스 및 헤더 경로 지정
- **포팅 심볼 해결:**
  - `utils/wait_for_event.c`를 반드시 `C_SRCS`에 포함해야 POSIX 포팅 심볼 에러가 발생하지 않습니다.
- **하위 디렉터리 오브젝트 생성:**
  - `@mkdir -p $(dir $@)`로 자동 생성

---

## 3. main.c 작성 예시

`main.c`에는 FreeRTOS 태스크 생성 및 스케줄러 실행 코드가 포함되어야 합니다.

```c
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

void vTask1(void *pvParameters) {
    for (;;) {
        printf("Task 1 running!\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void) {
    xTaskCreate(vTask1, "Task1", 1000, NULL, 1, NULL);
    vTaskStartScheduler();
    // 만약 여기까지 오면 메모리 부족 등으로 스케줄러가 시작되지 않은 것
    printf("Scheduler exited!\n");
    return 0;
}
```

- POSIX 시뮬레이터 환경에서는 표준 입출력(`printf`)을 사용할 수 있습니다.
- 태스크 함수는 반드시 무한 루프를 가져야 하며, `vTaskDelay`로 주기적 동작을 구현할 수 있습니다.

---

## 4. 빌드 및 실행 방법

```sh
make clean
make
./build/myTaskApp
```

---

## 5. 참고 사항
- FreeRTOS POSIX 포팅은 macOS, Linux 등에서 테스트 가능합니다.
- 추가 태스크/기능 구현 시 `src/main.c`를 수정하세요.
- 커널 소스 업데이트 시 `src/FreeRTOS-Kernel-src`을 교체하면 됩니다.
- 빌드 실패 시, Makefile의 소스 경로와 파일 목록을 점검하세요.

---

문의 및 개선 사항은 언제든 코드에 코멘트로 남겨주세요.
