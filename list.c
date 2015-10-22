int main();
int *string_buffer;
int CHAR_LF;
int CHAR_TAB;
int CHAR_CR;
int INT_MIN;
int INT_MAX;
int result;
int *power_of_two_table;



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


int * insert(int data, int * head){
    int * adr;
    
    adr = (int*)malloc (2*4);
    *adr = data;
    *(adr+1) = head;
    
    return adr;
    
}

void printList(int*head){
    int* number_buffer;
    
    number_buffer = (int*) malloc (10 * 4);
    
    while ((int)(head) != 0){
        print(itoa(*(head),number_buffer,10,0));
        putchar(CHAR_LF);
        head = *(head+1);
    }
}

void sortList(int *head){
    int i;
    int j;
    int help;
    int* temp;
    
    i = 0;
    j = 0;
  
    while(*(head+1) != 0){
        
        temp = *(head+1);
        
        while((int)(temp) != 0){
            
            if(*(head) < *(temp)){
                help = *(temp);
                *temp = *head;
                *head = help;
            }
            
            temp = *(temp+1);
        }
       
        head = *(head+1);
    }
}

void remove(int data, int * head){
    int* current;
    int* previous;
    int* help;
    
    current = head;
    previous = 0;
    
    if(*(current) == data){
        help = *(current+1);
        *current = *help;
        *(current+1) = *(help+1);
    }
    else{
        while((int)current != 0){
            if(*(current) == data){
                *(previous+1) =(int*)(*(current+1));
                current = 0;
            }
            else{
                previous = current;
                current = *(current+1);
            }
        }
    }
}



int main(){
    int* head;
    int* number_buffer;

    number_buffer = (int*) malloc (10 * 4);

    init();

    //inserting
    head = insert(7,head);
    head = insert(8,head);
    head = insert(3,head);
    head = insert(6,head);
    head = insert(9,head);
    head = insert(16,head);
    head = insert(18,head);
    head = insert(50,head);
    head = insert(42,head);
    head = insert(31,head);
    head = insert(39,head);
    head = insert(29,head);
    head = insert(13,head);
    head = insert(0,head);
    head = insert(72,head);
    head = insert(51,head);

    //printing
    putchar(CHAR_LF);
    printString('O','r','i','g','i','n','a','l',' ','L','i','s','t',' ',0,0,0,0,0,0);
    putchar(CHAR_LF);
    printList(head);
    putchar(CHAR_LF);

    //removing 9, inserting 4
    remove(9,head);
    head = insert(4,head);

    //printing
    printString('R','e','m','o','v','e','d',' ','9',' ','I','n','s','e','r','t','e','d',' ','4');
    putchar(CHAR_LF);
    printList(head);
    
    //sorting
    sortList(head);
    
    
    //printing
    putchar(CHAR_LF);
    printString('S','o','r','t','e','d',' ','L','i','s','t',' ',0,0,0,0,0,0,0,0);
    putchar(CHAR_LF);
    printList(head);

    exit(0);
}