#include <drivers/keyboard.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void clearScreen();
void printf(char*);
void printfHex(uint8_t);

// Trạng thái: 0 - nhập a, 1 - nhập b, 2 - nhập c
static uint16_t flag = 0;
static int64_t a = 0, b = 0, c = 0;
static int sign_a = 1, sign_b = 1, sign_c = 1; // Để nhập số âm

// Hàm chia nguyên cho 64-bit, trả về đúng kiểu C/C++ (lấy phần nguyên về 0)
int64_t divide(int64_t x, int64_t y) {
    if (y == 0) return 0; // Tránh chia 0
    int neg = 0;
    if (x < 0) { x = -x; neg = !neg; }
    if (y < 0) { y = -y; neg = !neg; }
    int64_t q = 0;
    while (x >= y) {
        x -= y;
        q++;
    }
    return neg ? -q : q;
}

// Hàm lấy phần dư đúng chuẩn C/C++
int64_t mod(int64_t x, int64_t y) {
    if (y == 0) return 0;
    int neg = 0;
    if (x < 0) { x = -x; neg = 1; }
    if (y < 0) y = -y;
    while (x >= y) x -= y;
    return neg ? -x : x;
}

// Hàm in số nguyên ra màn hình (hỗ trợ số âm)
void printInt(int64_t n) {
    char buffer[32];
    uint16_t index = 0;
    int negative = 0;
    if(n < 0) {
        negative = 1;
        n = -n;
    }
    if(n == 0) buffer[index++] = '0';
    while(n > 0) {
        buffer[index++] = '0' + mod(n, 10);
        n = divide(n, 10);
    }
    if(negative) buffer[index++] = '-';
    buffer[index] = '\0';
    // đảo ngược chuỗi
    for(uint16_t i = 0; i < index / 2; i++) {
        char t = buffer[i];
        buffer[i] = buffer[index-1-i];
        buffer[index-1-i] = t;
    }
    printf(buffer);
}

// Hàm căn bậc hai nguyên (Newton) - dùng divide
int64_t isqrt(int64_t n) {
    if(n < 0) return 0; // Không hỗ trợ căn số âm
    int64_t x = n, y = divide(x + 1, 2);
    while(y < x) {
        x = y;
        int64_t temp = divide(n, x);
        y = divide(x + temp, 2);
    }
    return x;
}

// Hàm giải phương trình bậc hai và in kết quả (dùng divide thay cho /)
void solveQuadratic(int64_t a, int64_t b, int64_t c) {
    printf("phuong trinh: ");
    printInt(a); printf("x^2 ");
    if (b >= 0) printf("+ ");
    printInt(b); printf("x ");
    if (c >= 0) printf("+ ");
    printInt(c); printf(" = 0\n");

    if (a == 0) {
        // bx + c = 0 => x = -c/b
        printf("phuong trinh la phuong trinh bac nhat:\n ");
        int64_t root = divide(-c, b);
        printf("co nghiem la: ");
        printInt(root);
        printf("\n");
        return;
    }
    int64_t delta = b*b - 4*a*c;
    printf("Delta = ");
    printInt(delta); printf("\n");
    if (delta < 0) {
        printf("phuong trinh vo nghiem\n");
        return;
    }
    int64_t sqrt_delta = isqrt(delta);
    if (sqrt_delta*sqrt_delta != delta) {
        printf("delta la khong phai la so chinh phuong\n");
        return;
    }
    int64_t denom = 2 * a;
    int64_t x1 = divide(-b + sqrt_delta, denom);
    int64_t x2 = divide(-b - sqrt_delta, denom);
    if (x1 == x2) {
        printf("nghiem kep: x = ");
        printInt(x1); printf("\n");
    } else {
        printf("nghiem 1: ");
        printInt(x1); printf("\n");
        printf("nghiem 2: ");
        printInt(x2); printf("\n");
    }
}

// --- Phần driver bàn phím giữ nguyên ---
KeyboardEventHandler::KeyboardEventHandler() {}
void KeyboardEventHandler::OnKeyDown(char) {}
void KeyboardEventHandler::OnKeyUp(char) {}

KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler *handler)
: InterruptHandler(manager, 0x21),
dataport(0x60),
commandport(0x64) {
    this->handler = handler;
}
KeyboardDriver::~KeyboardDriver() {}

void KeyboardDriver::Activate() {
    while(commandport.Read() & 0x1)
        dataport.Read();
    commandport.Write(0xae); // activate interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);
    dataport.Write(0xf4);
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp) {
    uint8_t key = dataport.Read();
    if(handler == 0)
        return esp;
    if(key < 0x80) {
        switch(key) {
            // Các phím số
            case 0x02: printf("1");
                if(flag == 0) a = a*10 + 1;
                else if(flag == 1) b = b*10 + 1;
                else if(flag == 2) c = c*10 + 1;
                break;
            case 0x03: printf("2");
                if(flag == 0) a = a*10 + 2;
                else if(flag == 1) b = b*10 + 2;
                else if(flag == 2) c = c*10 + 2;
                break;
            case 0x04: printf("3");
                if(flag == 0) a = a*10 + 3;
                else if(flag == 1) b = b*10 + 3;
                else if(flag == 2) c = c*10 + 3;
                break;
            case 0x05: printf("4");
                if(flag == 0) a = a*10 + 4;
                else if(flag == 1) b = b*10 + 4;
                else if(flag == 2) c = c*10 + 4;
                break;
            case 0x06: printf("5");
                if(flag == 0) a = a*10 + 5;
                else if(flag == 1) b = b*10 + 5;
                else if(flag == 2) c = c*10 + 5;
                break;
            case 0x07: printf("6");
                if(flag == 0) a = a*10 + 6;
                else if(flag == 1) b = b*10 + 6;
                else if(flag == 2) c = c*10 + 6;
                break;
            case 0x08: printf("7");
                if(flag == 0) a = a*10 + 7;
                else if(flag == 1) b = b*10 + 7;
                else if(flag == 2) c = c*10 + 7;
                break;
            case 0x09: printf("8");
                if(flag == 0) a = a*10 + 8;
                else if(flag == 1) b = b*10 + 8;
                else if(flag == 2) c = c*10 + 8;
                break;
            case 0x0A: printf("9");
                if(flag == 0) a = a*10 + 9;
                else if(flag == 1) b = b*10 + 9;
                else if(flag == 2) c = c*10 + 9;
                break;
            case 0x0B: printf("0");
                if(flag == 0) a = a*10;
                else if(flag == 1) b = b*10;
                else if(flag == 2) c = c*10;
                break;
            // Phím trừ để nhập số âm
            case 0x0C: // '-'
                if(flag == 0 && a == 0) sign_a = -sign_a;
                else if(flag == 1 && b == 0) sign_b = -sign_b;
                else if(flag == 2 && c == 0) sign_c = -sign_c;
                printf("-");
                break;
            // Phím r để reset
            case 0x13: // r
                clearScreen();
                printf("nhap bien a: ");
                a = b = c = 0;
                sign_a = sign_b = sign_c = 1;
                flag = 0;
                break;
            // Phím Enter để chuyển bước nhập
            case 0x1C: // Enter
                if(flag == 0) {
                    a = a * sign_a;
                    printf("\n nhap bien b: ");
                    flag = 1;
                } else if(flag == 1) {
                    b = b * sign_b;
                    printf("\n nhap bien c: ");
                    flag = 2;
                } else if(flag == 2) {
                    c = c * sign_c;
                    printf("\n");
                    solveQuadratic(a, b, c);
                    a = b = c = 0;
                    sign_a = sign_b = sign_c = 1;
                    flag = 0;
                    printf("\n nhap 'r' de reset nhap lai:  \n");
                }
                break;
            // Có thể thêm xử lý xóa/backspace nếu cần
        }
    }
    return esp;
}