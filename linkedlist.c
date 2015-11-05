//erzeugt listenelement
int* create(int* next, int data) {
	int* s;
	s = (int*)malloc( 2 * 4 );

	// next == 0 entspricht dem letzten element, ansonsten pointer aufs naechste
	*(s+0) = next;

	// payload, daten kommen hier rein, zB 'a'
	*(s+1) = data;

	return s;
}

// entfernt element von liste
// sucht element in liste rekursiv
void remove(int* head, int* element) {
	int* next;
	next = *(head+0);

	if (next == element) {
		// found, remove element. memleak todo: free next element's mem but not impl' yet in c*c
		*(head + 0) = *(next + 0);
		return;
	}

	if ((int)next == 0 ) {
                // abbruchbedinung: das letzte element wurde gefunden, der next pointer zeigt auf (int)0
                return;
        }
	remove(next,element);


}

// gibt liste auf konsole aus, zB a,b,c,
// ruft sich dabei selbst rekursiv fuer alle elemente auf und bricht bei letztem element ab
void printll( int* element) {
	int* next;
	putchar(*(element + 1));

	next = *(element+0);
	if ((int)next == 0 ) {
		// abbruchbedinung: das letzte element wurde gefunden, der next pointer zeigt auf (int)0
		putchar(10); // newline (at least under *nix)
		return;
	}
	putchar(',');
	printll(next);
}

int main() {
	int* head;
	int* toberemoved;
	head = create(0, 'a');
	head = create(head, 'b');
	toberemoved = create(head, 'c');
	head = toberemoved;
	head = create(head, 'd');
	printll(head);

	remove(head, toberemoved);
	
	printll(head);

	putchar('d');
	putchar('o');
	putchar('n');
	putchar('e');
	exit(6);
}
