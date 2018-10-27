# 181020

<http://jake.dothome.co.kr/pci-2/>
<http://jake.dothome.co.kr/pci-3/>

pci에서는 pci host controller, pcie에서는 root complex라고 함.

디바이스 트리 예
cell은 다음 노드에서 쓰임. cells는 크기를 정하는 것
```
pci {
    compatible = "company,foo";
    device_type = "pci";        /* always be "pci" */
    #address-cells   = <3>;     /* always be 3 */
    #size-cells      = <2>;     /* 32bit=1, 64bit=2 */
    #interrupt-cells = <1>;     /* always be 1 for legacy pci irq */

    bus-range = <0x0 0x1>;

    /*    CPU_PHYSICAL(P#a)     SIZE(P#s) */
    reg = <0x0 0x40000000    0x0 0x1000000>;

    /*             PCI_PHYSICAL(#a)      CPU_PHYSICAL(P#a)   SIZE(#s)  outbound mapping pci가 나를 호출할때는 이렇게 해.*/
    ranges = <0x81000000 0x0 0x00000000   0x0 0x48000000    0x0 0x00010000>,  /* non-relocatable IO */
             <0x82000000 0x0 0x40000000   0x0 0x40000000    0x0 0x40000000>;  /* non-relocatable MEM */

    /*               PCI_DEVICE(#a)  INT#(#i)   IRQ-Controller   IRQ-UNIT#   IRQ_ARG(PI#i) */
    interrupt-map = <  0x0 0x0 0x0    0x0           &gic            0x0       0x4 0x1
                     0x800 0x0 0x0    0x1           &gic            0x0       0x5 0x1
                    0x1000 0x0 0x0    0x2           &gic            0x0       0x6 0x1
                    0x1800 0x0 0x0    0x4           &gic            0x0       0x7 0x1>;

    /*                    PCI_DEVICE(#a)    INT#(#i) */
    interrupt-map-mask = <0xf800 0x0 0x0     0x7>;
}
```
PCI_PHYSICAL에서 앞에거 0x81000000 얘는 타입
뒤에 두개가 이어서 시작 주소

npt000ss 는 타입에서의 앞에부터 비트로 

non-relocatable은 pci 주소를 못바꾼다는 소리임. (pci주소랑 cpu주소가 같다는 말이 아님 다를 수 있음.)

PCI bus scan 순서 -> bfs, PCI bus numbering할 때는 dfs

of_match_ptr : device tree를 쓸수도 ahci?일 수도 있음.

구조체 마지막에 `unsigned long private[0]` 이렇게 하고 다른 구조체와 이을때 마지막 변수가 다음 구조체 주소를 가리킴

regulator는 전원 관리

디바이스 트리에는 물리 주소만 있다. 
reg = <a, b, c, d> // 부모 주소와 크기
reg-names = "regs", "config"; // reg-names가 있을때는 뭐가 다른지

soc { compatible = "simple-bus" } // 1단계 이 안에는 2단계인데 2단계는 플랫폼 디바이스가 아닌데, simple-bus로 지정되면 2단계도 플랫폼 디바이스가 됨. soc하위에 있는 애들은 

interrupts = <a, b, c> // a : PCI면 0, 

gic : 인터럽트 컨트롤러. http://jake.dothome.co.kr/ic/

bus-range = <a, b> // 첫번째 노드면 a : 0x0 0부터 시작, b : 전

outbound - downstream, inbound - upstream. cpu -> pci downstream(99%)

gic에 msi기능이 있음 msi?? : 

# 181027

## Shared memory
다른 것들은 복사가 일어남. 얘는 안일어남.

* ftruncate : 크기를 늘리거나 자르거나. 메모리를 지정하는 쪽에서만 하는게 좋음.

mmap : memory 매핑해서 쓰는데 오픈 하는 것에 따라 달라짐. 파일이 될수도 있고, anonymous 메모리가 될 수도 있고, 물리 메모리가 될수도 있고.

glibc : gnu에서 만듬 malloc vss : virtual ... zero page 영역?. rss : residence .... 실제 할당 영역? brk : 이게 메모리와 메모리 매핑하는거 mmap : 파일 메모리 매핑 근데 brk는 이제 ㅂㅂ
mlibc : 

파일에 접근할 때 로컬리티를 고려해서 리드 어헤드 표시를 해두고 한번에 이용함. 옵션을 통해서 리드 어헤드 하지마 이런식으로 설정 가능

## PCI driver

<https://github.com/maquefel/virtual_gpio_basic>

pci 전원 상태 
    d0 : 노말
    d1 : 조금 절전
    d2 : 좀 더 절전
    d3 : 꺼버려 off

request_irq : 내 루틴이 길다. 이걸로 스레디드로 처리 
            빨리 끝날거 같으면 내 컨텍스트안에서 그냥 끝내면됨

인터럽트를 처리하는 방법이 많은데 내 루틴이 길다 싶으면 threaded_irq 또는 ?? bottom half... threaded를 쓰면 두개 핸들러 이용가능 irq context 에서는 빠르게 threaded에서 긴거 

인터럽트 - <http://egloos.zum.com/rousalome/v/9964791>, <http://egloos.zum.com/rousalome/v/9384612>

qemu 추가 옵션
    -device ivshmem-doorbell,chardev=myivshmem,vectors=1 \
    -chardev socket,path=/tmp/ivshmem_socket,id-myivshmem \

`./ivshmem-server -p /tmp/ivshmem_pid -S /tmp/ivshmem_socket -l 512k -n 10`

* qemu 네트워크 설정
<https://tthtlc.wordpress.com/2015/10/21/qemu-how-to-setup-tuntap-bridge-networking/>
<http://jake.dothome.co.kr/qemu/>

