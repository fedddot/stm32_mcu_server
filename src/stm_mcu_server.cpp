extern "C" {
    int main(void);
    void Reset_Handler(void);
}

void Reset_Handler(void) {
    main();
}

int main(void) {
    while (1) {
    }
}