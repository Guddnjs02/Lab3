# 🐧 Lab 3

이 저장소는 **오픈소스프로젝트** 과목의 **Lab 3 (Advanced System Programming)** 실습 결과물입니다.  
리눅스 환경에서 **멀티 쓰레드(Multi-thread)**, **네트워크 소켓(Socket)**, 그리고 **GUI(GTK+)** 프로그래밍 기술을 습득하고 구현하는 것을 목표로 합니다.

## 📂 프로젝트 구조 (Directory Structure)

프로젝트는 주제별로 3개의 디렉토리로 구성되어 있습니다.

| 폴더명 | 주제 | 주요 내용 |
| :--- | :--- | :--- |
| **`01_Threads`** | **Multi-Threading** | Pthread 생성, 뮤텍스/조건변수 동기화, 생산자-소비자 문제 해결 |
| **`02_Network`** | **Socket Programming** | TCP 에코 서버, 다중 접속 채팅 서버(I/O Multiplexing), 웹 서버 구현 |
| **`03_GUI`** | **GTK+ Programming** | GTK+ 3.0 기반 윈도우 생성, 시그널 처리, 계산기 프로그램 구현 |

## ✨ 주요 구현 내용 (Features)

### 1. 멀티 쓰레드 (01_Threads)
* **기초 쓰레드:** `pthread_create`, `pthread_join`을 이용한 쓰레드 생명주기 관리.
* **생산자-소비자 문제 (Bounded Buffer):**
    * **뮤텍스(Mutex)**와 **조건변수(Condition Variable)**를 사용하여 경쟁 상태(Race Condition) 해결.
    * 다수의 생산자와 소비자가 제한된 버퍼를 안전하게 공유하도록 동기화 구현.
* **쓰레드 순서 제어:** 이진 플래그와 조건변수를 활용하여 부모-자식 쓰레드가 정확히 번갈아 가며 실행되도록 구현.

### 2. 네트워크 프로그래밍 (02_Network)
* **TCP Echo Server:** 기본적인 소켓 연결(`bind`, `listen`, `accept`) 및 데이터 송수신 흐름 구현.
* **다중 접속 채팅 서버:**
    * **I/O 멀티플렉싱 (`select`)** 기법을 사용하여 단일 프로세스로 다수의 클라이언트를 동시 처리.
    * 클라이언트가 보낸 메시지를 접속된 모든 사용자에게 전송(Broadcasting).
* **웹 서버 (HTTP Web Server):**
    * **HTTP 프로토콜(GET, POST)** 요청 파싱 및 처리.
    * **CGI(Common Gateway Interface)**를 통해 외부 프로그램 실행 결과를 웹페이지로 전송.

### 3. GUI 프로그래밍 (03_GUI)
* **GTK+ 윈도우:** `gtk_init`, `gtk_window_new`를 사용한 기본 윈도우 생성 및 종료 이벤트 처리.
* **계산기 (Calculator):**
    * **GtkGrid** 레이아웃을 사용하여 버튼 배치.
    * 시그널-콜백 메커니즘을 통해 사칙연산 로직 구현.

## ⚙️ 빌드 및 실행 방법 (Build & Run)

### 사전 준비 (Prerequisites)
Ubuntu 환경에서 GTK+ 개발 라이브러리 설치가 필요합니다.
```bash
sudo apt update
sudo apt install libgtk-3-dev
