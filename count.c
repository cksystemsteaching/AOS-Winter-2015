int main() {
	int i;
	int pid;

	int rr_pid;

	pid = (int)getpid();

	if (pid == 0 ) {
		rr_pid = 0;
		while (1) {

			// kernel process
			putchar('K');
			putchar('E');
			putchar('R');
			putchar('N');
			putchar('E');
			putchar('L');
			
			rr_pid = rr_pid + 1;
			if (rr_pid > 2)
				rr_pid = 1;
			sched_switch(0, rr_pid);
			putchar('_');
			putchar('N');
			putchar('E');
			putchar('X');
			putchar('T');


			//sched_yield();
		}
		exit(6);
	} else {
		// non-kernel processes
		i=48;

		// print '0' to 'Z' (in asciitable)
		while (i<90) {
			putchar(i);
			i = i+1;
		}
		exit(8);
	}

}
