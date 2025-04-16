# 팀명 : Clean Code
# 과제 : SSD Project

# 커밋 메시지 규칙 :ex. #{issue_no} FEAT blah blah

1. 이슈 번호 지정
2. 유형 지정
```
FEAT : 새로운 기능의 추가
FIX: 버그 수정
DOCS: 문서 수정
STYLE: 스타일 관련 기능(코드 포맷팅, 세미콜론 누락, 코드 자체의 변경이 없는 경우)
REFACTOR: 코드 리펙토링
TEST: 테스트 코트, 리펙토링 테스트 코드 추가
CHORE: 빌드 업무 수정, 패키지 매니저 수정(ex .gitignore 수정 같은 경우)
```
3. 제목 행을 50자로 제한
4. 제목 행의 첫 글자는 대문자로 시작
5. 마침표를 넣지 않는다


# 브랜치 네이밍 : feature/blah

# Ground Rule
- 출퇴근 시 서로 인사하기
- 리뷰시 비속어 사용하지 않기
- 쉬는 시간에는 푹 쉬기
- 어려운 부분은 서로 돕기
- 시간 잘지키기
- 커밋시 디스크립션만 보고 리뷰 가능하게 쓰기
- 리뷰는 10자 이상 써주기
- 1인당 1일 1커밋, 1리뷰 하기
- Pull request 2명 이상에게 리뷰받기
- 코드 한 줄에 100자 이상 넘지 않기

# 프로젝트 진행 관련 안내/문의
- 문서 : https://url.kr/3pioxm
- 코드 제출은 git url
- project 기능 관련 : sejin.choi@mincoding.co.kr
- TDD 관련 문의 : jeonghwan.seo@mincoding.co.kr


# 기능 추가
- https://url.kr/fu5vxh
- https://forms.gle/ppaWbXsxgt6Ac8Uy8
1. SSD - Erase 명령어 추가
    1. SSD : E [ LBA ] [ SIZE ]
        1. 특정 LBA 부터 SIZE 만큼 삭제
        2. SIZE는 최대 10 (1~10)
        3. 삭제된 공간은 0x00000000
    2. SHELL : erase [ LBA ] [ SIZE ]
        1. SIZE는 최대 10 (1~10)
    3. SHELL : erase range [ start lba ] [ end lba ]
        1. 이건 size 제한 없음
2. Logger 기능 추가
    1. 기능 1
        1. 출력 포맷 : [날짜 시간] 함수명() : 로그메세지
        2. 함수명 포맷 30칸 지정해서 이쁘게
        3. logger.print(”Bye Bye”);
        4. [26.01.01 17:04] release() : Bye bye.
    2. 로그 파일 관리
        1. 10kb 단위로 파일 관리
        2. latest.log으로 최신 로그 저장하고 있다가
        3. 10kb 초과시 until_날짜_시간.log으로 이름 변경
        4. 10kb 파일이 2개되면 오래된 파일 순으로 zip으로 확장자 변경
3. Runner 기능 추가 (테스트 스크립트 관련)
    1. cmd들을 txt 파일로 저장하고 txt 파일을 입력하면 순서대로 테스트 기능을 실행해줌.
    2. 실행 커맨드 : shell.exe run_list.lst
    3. Runner 실행중 콘솔에는 Runner의 출력 외의 다른 출력은 없어야 한다.
        1. TestApp1 —- Run…Pass
        2. TestApp2 —- Run…FAIL!
            1. FAIL인 경우 Runner 종료
4. SSD - Write Buffer 기능 추가
    1. 기능 1 - Buffering
        1. write / erase 명령어를 모아서 한번에 수행한다.
        2. ssd 자체 기능
        3. 10개 명령어까지 buffering
        4. 10개 명령어 차면 명령을 한번에 수행
            1. nand.txt 에 write/erase 할 때, 최대 10개의 cmd를 한번에 처리
            2. 10 개 cmd 중 중복되는 cmd의 경우 최종 cmd만 실행
    2. 기능 2 - Flush
        1. 10개 cmd 안차더라도 flush 가능
    3. 기능 3 - Fast Read
        1. Fast Write
            1. 연속적인 range에 erase 있으면, erase range으로 대체
            2. 같은 lba에 write/erase 있으면 write는 skip 
        2. Fast Read
            1. read할때 write buffer 부터 확인, nana 접근 최소화
5. 시나리오 추가 시, 재빌드 이슈 해결
    1. Test Shell 기능 추가하면 Test Shell 빌드 해야한다.
    2. Test Scenario 1 / 2 / 3 / … 분리
    3. Test Scenario 추가되어도 Test Shell 빌드는 할 필요가 없어야 한다.

# 발표 양식
- template : https://url.kr/6dnhvs
- 발표 내용
  - 필수
  - 조원 소개 및 역할
  - 기능 구현 소개
  - TDD 활용 예시
  - Mocking 활용 예시
  - 리팩토링 통한 클린코드 전후 결과 비교
  - 소감
  - 프로젝트 시연도 포함되어야 함
    - ex) buffer 파일로 구현 -> 시연하고 buffer역할의 파일 보여주고 이런거?
