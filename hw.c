int fifo = 0, lru = 0;

void loop() {
    char* input[100];
    char** args;
    do {
        printf("> ");
        fgets(input, 80, stdin);
        args = tokenize((char *) input);
        if (strcmp(args[0], "quit") == 0)
            exit(0);
        execute(args);
    } while (1);
}

int main(int argc, char** argv) {
    if (argv[1] == NULL || strcmp (argv[1], "FIFO") == 0)
        fifo = 1;
    else if (strcmp (argv[1], "LRU") == 0)
        lru = 1;
    init();
    loop();
    return 0;
}
