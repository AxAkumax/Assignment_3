#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Memory {
 int address, data;
};
struct PageTable {
 int v_page_num, valid_bit, dirty_bit, page_num; //, time_stamp; this was in discussion code but not in any instructions
};
struct Memory main_memory[32];
struct Memory virtual_memory[128];
struct PageTable p_table[16];

int fifo = 0, lru = 0;

//Main calls this so we can initialize whatever we need here
//Unsure if this is correct
void init(){
    //Initialize main_memory
    for(int i = 0; i < 128; i++){
        virtual_memory[i].address = i;
        virtual_memory[i].data = -1;
    }

    //initialize virtual memory (idk what to do)
    for(int i = 0; i < 32; i++){
        // virtual_memory[i].address = i;
        main_memory[i].data = -1;
    }

    //initialize p_table 
    for (int i = 0; i < 16; i++){
        p_table[i].v_page_num = i;
        p_table[i].valid_bit = 0;
        p_table[i].dirty_bit = 0;
        p_table[i].page_num = i;
    }
} 

void pageFault(){
    printf("A Page Fault Has Occurred\n");
}

//right now input is routed to call these functions, these print statements are for your understanding
void read(int vAddress){
    printf("READ vAddress %d\n", vAddress);
}

void write(int vAddress, int num){
    printf("WRITE vAddress: %d, num: %d\n", vAddress, num);
}

//main memory has 4 pages, each page 8 addresses
// 0 - 7 : 0
// 8 - 15: 1
// 16 - 23: 2
// 24 - 31 : 3 
void showmain(int ppn){
    printf("SHOWMAIN ppn %d\n", ppn);
    // 0 + 8(ppn) = start value
    //not sure if this is right
    int start = 8(ppn);
    for(int i = start; i<start+8; i++)
    {
        printf("%d: %d", main_memory[i].address, main_memory[i].data);
    }
}

void showptable(){ //DONE
    for (int i = 0; i < 16; i++){
        printf("%d:%d:%d:%d\n", p_table[i].v_page_num, p_table[i].valid_bit, p_table[i].dirty_bit, p_table[i].page_num);
    }
}


//Handle user input
void loop() { //DONE
    char input[100];
    char* token;
    while(1) {
        printf("> ");
        fgets(input, 80, stdin);
        input[strcspn(input, "\n")] = 0;
        token = strtok(input, " ");
        if (strcmp(token, "quit") == 0){
            exit(0);
        }
        else if (strcmp(token, "read") == 0){
            token = strtok(NULL, " ");
            int vAddress = atoi(token);
            read(vAddress);
        }
        else if (strcmp(token, "write") == 0){
            token = strtok(NULL, " ");
            int vAddress = atoi(token);
            token = strtok(NULL, " ");
            int num = atoi(token);
            write(vAddress, num);
        }
        else if (strcmp(token, "showmain") == 0){
            token = strtok(NULL, " ");
            int ppn = atoi(token);
            showmain(ppn);
        }
        else if (strcmp(token, "showptable") == 0){
            showptable();
        }
    };
}

int main(int argc, char** argv) { //DONE
    //fifo is chosen given no args or if chosen
    if (argv[1] == NULL || strcmp (argv[1], "FIFO") == 0){
        fifo = 1;
    }
    else if (strcmp (argv[1], "LRU") == 0){ 
        lru = 1;
    }
    init();
    loop();
    return 0;
}
