# gpio

## 참고소스
* lsk-4.14/drivers/gpio/gpio-lpc32xx.c
* lsk-4.14/drivers/gpio/gpio-mmio.c

## 실습
* 4개핀 dat, set, clr, dirout, dirin (reg 0x0 0x4 0x8 0xc, 0x10) 각 reg당 4비트씩 씀(4개 핀)
* 2개핀 dat, set, clr, dirout, dirin (reg 0x14 ...) 각 reg당 2비트 씩 씀(2개 핀)
* 변수를 걍 int로 선언??
* cpiochqadd_data??
* 출력 모드에서 set을 하면 dat이 1, clr하면 dat이 0이 되도록

## 확인
* echo 번호 > export // 하면 디렉토리 생김
* echo 번호 > value ?? // 하면 write
* cat value // read
