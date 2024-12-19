
# CuteWebserv

## 시연 영상
[output.webm](https://github.com/CuteWebserv/webserv/assets/28525747/ba7dc6e8-e8d1-4683-88be-364980727c70)

## 프로젝트 개요

nginx를 샘플링하여 만든 HTTP/1.1 표준을 준수하는 웹 서버(Web Server) 입니다.

RFC 문서(7230~7235) 기반으로 HTTP Method(GET, POST, PUT, DELETE) 요청 처리 및 응답을 반환하도록 구현했습니다.

CGI 프로토콜 규격에 따라 동적 페이지 생성을 위한 CGI 프로그램 실행 기능을 지원합니다.

## 실행 방법
1. **설치**: 프로젝트를 클론
```bash
  git clone https://github.com/CuteWebserv/webserv.git
```
2. **실행**: 프로젝트를 실행
```bash
  cd /webserv
  make # 주의사항: gcc98, makefile 라이브러리가 필요합니다!
```

# 팀원
| 이름 | 담당 업무 |
|------|-----------|
| [이수린](https://github.com/Elineely) | HTTP Processor 개발 |
| [곽진솔](https://github.com/solgito) | HTTP Processor 개발 |
| [김호권](https://github.com/404yonara) | HTTP Processor 개발 |
| [박기선](https://github.com/Arkingco) | Parser & I/O Multiplexing 개발 |
| [한준혁](https://github.com/Han-Joon-Hyeok) | Parser & I/O Multiplexing 개발 |

### 피그마 링크 

[프로그램 설계](https://www.figma.com/file/vilFckAR6FPMNpEhJ69usT/%EC%9B%B9%EC%84%9C%EB%B8%8C-%EC%84%A4%EA%B3%84?type=whiteboard&node-id=284%3A580&t=dbAoxn87qNj6w2mq-1)
