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

int fifoCounter = 0;
int fifo = 0, lru = 0;
int main_mem_counter = 0; //keeps track of which main memory page to load to for given virtual address

//Main calls this so we can initialize whatever we need here
//Unsure if this is correct
void init(){
    //Initialize virtual_memory
    for(int i = 0; i < 128; i++){
        virtual_memory[i].address = i;
        virtual_memory[i].data = -1;
    }

    //initialize main memory
    for(int i = 0; i < 32; i++){
        main_memory[i].address = i;
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

//UNUSED DEBUG FUNCTION THAT PRINTS FIRST 51 items of virtual memory
void printVMemory(){
    for(int i = 0; i < 51; i++){
        printf("%d: %d\n", virtual_memory[i].address, virtual_memory[i].data);
    }
}


void pageFault(int virtual_page){
    printf("A Page Fault Has Occurred\n");
    int main_mem_page = main_mem_counter;
    //all pages have been loaded, we use 
    if(main_mem_counter>3){
        //assuming that we incrementing after
        //based on whether FIFO or LRU
        //  1) we update the main_memory page
        //  2) update the virtual page we last used

        //All that we need to implement fifo and lru is the current main memory page that we need to evict and replace
        if(fifo) //DONE
        {
            main_mem_page = (main_mem_counter % 4);
        }
        else if(lru)
        {
            //DO THIS
        }

        //Process eviction into virtual memory
        for (int i = 0; i < 16; i++){
            //find virtual page number of current page to be evicted in main
            if (p_table[i].page_num == main_mem_page && p_table[i].valid_bit == 1){
                int evicted_page_num = p_table[i].v_page_num;
                //reset page table for evicted main page
                p_table[i].dirty_bit = 0;
                p_table[i].valid_bit = 0;
                p_table[i].page_num = p_table[i].v_page_num;
                //store evicted main page in virtual memory
                for (int j = 0; j < 8; j++){
                    int curData = main_memory[main_mem_page * 8 + j].data;
                    virtual_memory[evicted_page_num * 8 + j].data = curData;
                }
                break;
            }
        }
    }
    //Regardless of eviction or not, we load in from the virtual memory
    //Probably could be in the if block but does not hurt to be after it

    //Set values in main memory from virtual memory
    for (int i = 0; i < 8; i++){
        int curData = virtual_memory[virtual_page * 8 + i].data;
        main_memory[main_mem_page * 8 + i].data = curData;
    }
    
    //moved logic found in read and write to here
    //set corresponding main mem page number to virtual page and set valid bit to 1 (active in main memory)
    p_table[virtual_page].page_num = main_mem_page;
    p_table[virtual_page].valid_bit = 1;
    main_mem_counter++;
}

// right now input is routed to call these functions, these print statements are for your understanding
// page fault handles page table information and main memory allocation 
// we convert the virtual address to a virtual page number by dividing it by 8, and based on the page, we check
// if it is loaded in memory, if not, then
void read(int vAddress){
   int virtual_page = vAddress/8;
   if (p_table[virtual_page].valid_bit==0)
    {
        //this means that the page hasn't been loaded from main_memory
        //causes page fault 
        pageFault(virtual_page);
    }
    int main_memory_address = vAddress - 8*virtual_page; //page_num gives us starting page num
    //gets the address from the main memory page and reads it out
    int value = main_memory[p_table[virtual_page].page_num*8 + main_memory_address].data;
    printf("%d\n", value);
}

void write(int vAddress, int num){
    int virtual_page = vAddress/8;
    if (p_table[virtual_page].valid_bit==0)
    {
        //this means that the page hasn't been loaded from main_memory
        //causes page fault, we can't write into memory
        pageFault(virtual_page);
    }
    p_table[virtual_page].dirty_bit = 1; //since we are writing into it
    int main_memory_address = vAddress - 8*virtual_page; //page_num gives us starting page num
    main_memory[p_table[virtual_page].page_num*8 + main_memory_address].data = num;
}

//main memory has 4 pages, each page 8 addresses
// 0 - 7 : 0
// 8 - 15: 1
// 16 - 23: 2
// 24 - 31 : 3 
void showmain(int ppn){
    if(ppn>= 0 && ppn<=3){
        int start = 8 * (ppn);
        for(int i = start; i<start+8; i++)
        {
            printf("%d: %d\n", main_memory[i].address, main_memory[i].data);
        }
    }
    else{
        printf("Invalid: Unable to print page %d from main memory\n", ppn);
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
