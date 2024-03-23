### 3/20
-  ~~메모리 할당하기~~
    - ~~map을 사용할듯?~~
- ~~함수화~~
- 이후에 instructions 작성. -> Ok
### 3/23
- ~~map 할당은 끝까지.~~
- 마지막에 나오는 라벨에 주소를 추가하기
- **라벨이 정확한 주소를 가르키게 해야함**
    - ~~.text 전까지는 제대로 작동하는듯함.~~
    - 아님! 벡터의 특성상 주소가 유동적임..
        - 이를 해결해야함..
        - **일단 Pass**
- ~~instruction_index는 없애도 될 듯 함.~~

## parsing workflow
- data인지 확인
    - continue.
- text인지 확인
    - in_test 변수 on
    - continue

- in_text가 0ff일 경우
    - 라벨이 나올 경우 (:이 포함 되는가.)
        - is_label을 on
        - label_name을 기록
        - lable_index를 기록
    - .word가 나올 경우
        - i에 +1 
        - +1된 i번째 words 원소를 data_seg에 저장.
            - 저장시 hex인지 확인
- in_text가 on일 경우
    - 라벨이 나올 경우 (:이 포함되는가)
        - is_label을 on
        - label_name을 기록
        - lable_index를 기록
    - instructions를 포함한 데이터를 text_seg에 저장
        - 

- 만약 label_index가 0이 아니고, i보다 작을 경우
    - in_text가 off라면
        - data_seg의 맨 마지막 원소의 주소를 밸류로, 
            라벨 이름을 키 값으로 매핑함
    - on이라면 text_seg의 맨 마지막 원소의 주소를 밸류로, 
        라벨 이름을 키 값으로 매핑

    - label_index = 0

- 인스트럭션 구현 시작
    - if is_instruction
        - 메모리 넘김.
# 포인터란 이런것인가!!!!! (3/24)
- addiu 먼저 구현해 보겠음.
    - 포인터로 불러오는 것 까지 구현 완료.
    - 로직은 이제 짜야함

## 현재 해야하는 것들
[ ] 벡터로 인한 주소 변경 문제 해결
[ ] 인스트럭터들 구현