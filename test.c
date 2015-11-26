int main();
int *string_buffer;
int CHAR_LF;
int CHAR_TAB;
int CHAR_CR;
int INT_MIN;
int INT_MAX;
int result;
int *power_of_two_table;
int a;



void printString(int c0, int c1, int c2, int c3, int c4,
                 int c5, int c6, int c7, int c8, int c9,
                 int c10, int c11, int c12, int c13, int c14,
                 int c15, int c16, int c17, int c18, int c19);
void assignString(int *s, int c0, int c1, int c2, int c3, int c4,
                  int c5, int c6, int c7, int c8, int c9,
                  int c10, int c11, int c12, int c13, int c14,
                  int c15, int c16, int c17, int c18, int c19);
void print(int *s);


void assignString(int *s, int c0, int c1, int c2, int c3, int c4,
                  int c5, int c6, int c7, int c8, int c9,
                  int c10, int c11, int c12, int c13, int c14,
                  int c15, int c16, int c17, int c18, int c19) {
    
    *(s+0) = c0; *(s+1) = c1; *(s+2) = c2; *(s+3) = c3; *(s+4) = c4;
    *(s+5) = c5; *(s+6) = c6; *(s+7) = c7; *(s+8) = c8; *(s+9) = c9;
    *(s+10) = c10; *(s+11) = c11; *(s+12) = c12; *(s+13) = c13; *(s+14) = c14;
    *(s+15) = c15; *(s+16) = c16; *(s+17) = c17; *(s+18) = c18; *(s+19) = c19;
    *(s+20) = 0;
}

void printString(int c0, int c1, int c2, int c3, int c4,
                 int c5, int c6, int c7, int c8, int c9,
                 int c10, int c11, int c12, int c13, int c14,
                 int c15, int c16, int c17, int c18, int c19) {

    assignString(string_buffer, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9,c10, c11, c12, c13, c14, c15, c16, c17, c18, c19);
    
    print(string_buffer);
}

void print(int *s) {
    while (*s != 0) {
        putchar(*s);
        s = s + 1;
    }
}

int stringLength(int *s) {
    int i;
    
    i = 0;
    
    while (*(s+i) != 0)
        i = i + 1;
    
    return i;
}

void reverseString(int *s) {
    int tmp;
    int i;
    int j;
    
    i = 0;
    j = stringLength(s) - 1;
    
    while (i < j) {
        tmp = *(s+i);
        *(s+i) = *(s+j);
        *(s+j) = tmp;
        i = i + 1;
        j = j - 1;
    }
}

int* itoa(int n, int *s, int b, int a) {
    // assert: b in {2,4,8,10,16}
    
    int i;
    int sign;
    
    i = 0;
    
    sign = 0;
    
    if (n == 0) {
        *s = '0';
        
        i = 1;
    } else if (n < 0) {
        sign = 1;
        
        if (b == 10) {
            if (n == INT_MIN) {
                *s = '8'; // rightmost decimal digit of 32-bit INT_MIN
                
                n = -(n / 10);
                i = i + 1;
            } else
                n = -n;
        } else {
            if (n == INT_MIN) {
                *s = '0'; // rightmost non-decimal digit of INT_MIN
                
                n = (rightShift(INT_MIN, 1) / b) * 2;
                i = i + 1;
            } else
                n = rightShift(leftShift(n, 1), 1);
        }
    }
    
    while (n != 0) {
        *(s+i) = n % b;
        
        if (*(s+i) > 9)
            *(s+i) = *(s+i) - 10 + 'A';
        else
            *(s+i) = *(s+i) + '0';
        
        n = n / b;
        i = i + 1;
        
        if (i == 1) {
            if (sign) {
                if (b != 10)
                    n = n + (rightShift(INT_MIN, 1) / b) * 2;
            }
        }
    }
    
    if (b != 10) {
        while (i < a) {
            *(s+i) = '0'; // align with zeros
            
            i = i + 1;
        }
    } else if (sign) {
        *(s+i) = '-';
        
        i = i + 1;
    }
    
    *(s+i) = 0; // null terminated string
    
    reverseString(s);
    
    return s;
}

int twoToThePowerOf(int p) {
    // assert: 0 <= p < 31
    return *(power_of_two_table + p);
}

int leftShift(int n, int b) {
    // assert: b >= 0;
    
    if (b > 30)
        return 0;
    else
        return n * twoToThePowerOf(b);
}

int rightShift(int n, int b) {
    // assert: b >= 0
    
    if (b > 30)
        return 0;
    else if (n >= 0)
        return n / twoToThePowerOf(b);
    else
        // works even if n == INT_MIN
        return ((n + 1) + INT_MAX) / twoToThePowerOf(b) +
        (INT_MAX / twoToThePowerOf(b) + 1);
}


void init (){
    int i;
    
    power_of_two_table = malloc(31*4);
    
    *power_of_two_table = 1; // 2^0
    
    i = 1;
    
    while (i < 31) {
        *(power_of_two_table + i) = *(power_of_two_table + (i - 1)) * 2;
        
        i = i + 1;
    }
    
    // computing INT_MAX and INT_MIN without overflows
    INT_MAX = (twoToThePowerOf(30) - 1) * 2 + 1;
    INT_MIN = -INT_MAX - 1;
    
    CHAR_TAB = 9;  // ASCII code 9  = tabulator
    CHAR_LF  = 10; // ASCII code 10 = linefeed
    CHAR_CR  = 13; // ASCII code 13 = carriage return
    
    string_buffer = malloc(33*4);
    

}





int main(){
    int* head;
    int* number_buffer;

    number_buffer = (int*) malloc (10 * 4);

    init();
    
    //lock();
    //print("something");
    //putchar(CHAR_LF);
    
    printString('W','i','t','h',' ','L','o','c','k',' ',0,0,0,0,0,0,0,0,0,0);
    putchar(CHAR_LF);

    //unlock();

    //lock();
    //print("somethingElse");

    
    printString('W','i','t','h','o','u','t',' ','L','o','c','k',' ',0,0,0,0,0,0,0);
    
    putchar(CHAR_LF);


    print(itoa(1, string_buffer, 10, 0));

    print(itoa(2, string_buffer, 10, 0));
    print(itoa(3, string_buffer, 10, 0));
    print(itoa(4, string_buffer, 10, 0));
    print(itoa(5, string_buffer, 10, 0));
    putchar(CHAR_LF);
    sched_yield();
    print(itoa(6, string_buffer, 10, 0));
    print(itoa(7, string_buffer, 10, 0));
    print(itoa(8, string_buffer, 10, 0));
    print(itoa(9, string_buffer, 10, 0));
    putchar(CHAR_LF);

    //switch();
    sched_yield();


    
    exit(0);
}