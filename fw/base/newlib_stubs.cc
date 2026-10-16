extern "C" {
int _kill(int, int sig) { return 0; }

int _getpid(void) { return 1; }
}