# Asio Network Libaray Example

## 특징
- C++ asio와 boost코루틴 라이브러리를 이용한 네트워크 라이브러리 예제
- TBB의 락-프리 자료구조 및 메모리 관리자 사용 ( 현재 Apache v2.0 라이센스로 변경되어, 라이센스 사용만 명치하면 무료로 사용 가능)
- 모던 C++를 적극 활용한 코드
- 코루틴 패턴을 이용하면 기존 TaskQueue 또는 JobQueue를 이용한 비동기 io 처리방식에 비해 좀 더 직관적인 코딩이 가능함
- 예제 코드의 방식을 이용한다면 Node.js 또는 C# 등에서 사용되는 async/await 패턴으로 코드 작성 가능
- 네트워크 io 스레드와 로직 채널 스레드의 수를 별도로 할당하여, 스레드간 경쟁을 최소화
- 같은 존에 있는 유저는 존 식별자 등을 통해서 같은 채널스레드에 할당하는 것을 전제로 작성
- asio::co_spawn과 co_await를 적극 사용하여 로직에 할당된 싱글스레드에서 비동기 처리가 가능하도록 하였음
- 코루틴을 위한 비동기 처리는 멀티스레드의 Context Switching보다 오버헤드가 적음
- 예를 들어, 데이터베이스, 캐시 서버, 인증 서버 등과의 통신을 비동기 코드로 효율적으로 처리할 수 있음
 
## 빌드 환경
- Microsoft Visual Studio Community 2019
- c++ 17 이상의 컴파일러, /await 옵션
- [Threading Building Blocks 2020 이상](https://github.com/oneapi-src/oneTBB/releases)
  - 설치 파일 중 bin 폴더의 파일은 디버그/릴리스에 있어야 실행 가능
- [boost libary 1.74 이상](https://www.boost.org/users/history/version_1_74_0.html)
- [msgpack](https://github.com/msgpack/msgpack-c/tree/cpp_master)
  - include only 라이브러리이기 때문에 빌드없이 include 만으로 사용
- google unit test package nuget 설치

## 실행 예
- 서버
  - 예) 43234 포트로 서버를 실행하되, 억셉트/송수신 io에는 1개의 스레드를 할당하고, 채널(컨텐츠)에는 2개의 스레드를 할당함
    - AsioNetServer.exe --port 43234 --ioThreadCnt 1 --channelThreadCnt 2
- 클라이언트
  - 예) 400개의 더미 클라이언트를 4개의 스레드로 실행하여, 200ms 마다 에코메시지를 보냄
    - AsioNetClient.exe --ip 127.0.0.1 --port 43234 --clientCnt 400 --threadCnt 4 --interval 200
 

## 프로젝트 구조
- AsioNetwork
  - AsioNetLib
    - 서버 네트워크 라이브러리
  - AsioNetCommon
    - 클라이언트/서버 공통(패킷 정의 등)
  - AsioNetServer
    - 서버 실행 프로세스
    - 패킷 컨트롤러 정의
  - AsioNetClient
    - 테스트용 클라이언트
  - AsioNetUnitTest
    - 라이브러리 관련 유닛테스트
  - ThirdParty
    - boost
    - msgpack
    - tbb


