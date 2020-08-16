#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>

#define PAGE_SIZE 256
#define FRAME_SIZE 256     
#define FRAMES 256   
#define TLB_LENGTH 16       
#define OFFSET_BITS 8
#define OFFSET_MASK 255
#define MEMORY_SIZE 65536
#define PAGE_NUMBER_MASK 255

int page_table[PAGE_SIZE][PAGE_SIZE];
int tlb[TLB_LENGTH][TLB_LENGTH];
int physical_mem[FRAMES][FRAME_SIZE]; 
signed char value_memory;
char input_buffer[10];
int virtual_address;
int next_frame = 0; 
int next_page = 0;  
FILE *fptr1;
int mmapfile_fd;
signed char *mmapfptr;
signed char b[MEMORY_SIZE];
int faults = 0;  
int hits = 0; 
int spots = 0;             


void addTLB(unsigned int page_number,unsigned int frame_number){
	int i; 
    	for(i=0; i<spots; i++){
        	if(tlb[i][0] == page_number)
            		break;
    	}
    
    	if(i!= spots){
		for(i=i; i <spots-1; i++){      
            		tlb[i][0] = tlb[i+1][0];      
            		tlb[i][1] = tlb[i+1][1];
        	}
        	if(spots<TLB_LENGTH){                
            		tlb[spots][0] = page_number;
            		tlb[spots][1] = frame_number;
        	}else{                                              
            		tlb[spots-1][0] = page_number;
            		tlb[spots-1][1] = frame_number;
        	}
        	        
    	}else{
		if(spots<TLB_LENGTH){ 
            		tlb[spots][0] = page_number;    
            		tlb[spots][1] = frame_number;
        	} else{                                           
            		for(i=0; i<TLB_LENGTH-1; i++){
                		tlb[i][0] = tlb[i+1][0];
                		tlb[i][1] = tlb[i+1][1];
            		}
            		tlb[spots-1][0]= page_number;  
            		tlb[spots-1][1]= frame_number;
        	}
    	}
    	if(TLB_LENGTH>spots){                   
        	spots++;
   	}    
}


void backing_store(int page_number){
	mmapfptr=mmap(0,MEMORY_SIZE,PROT_READ,MAP_PRIVATE,mmapfile_fd,0);
	memcpy(b,mmapfptr+(page_number*256),256);

	int j;
   	for(j=0; j<256; j++){
       		 physical_mem[next_frame][j] = b[j];
    	}
	
	page_table[next_page][0]=page_number;
	page_table[next_page][1]=next_frame;
    
    	next_frame++;
    	next_page++;

	if (munmap(mmapfptr,MEMORY_SIZE)==-1){
		printf("Error in munmap");
	}
}

void searchTLB(unsigned int virtual_address){
	unsigned int frame_number=-1;
	unsigned int page_number = (virtual_address >>OFFSET_BITS) & PAGE_NUMBER_MASK;
	unsigned int offset = (virtual_address & OFFSET_MASK);
    	int i;  
    	for(i=1; i<TLB_LENGTH; i++){
        	if(tlb[i][0] == page_number){ 
			hits++;   
            		frame_number = tlb[i][1];       
        	}
    	}
	if(frame_number==-1){
        	int i;  
        	for(i=0; i<next_page; i++){
            		if(page_table[i][0]==page_number){        
                		frame_number=page_table[i][1];          
            		}
        	}
        	if(frame_number==-1){
			backing_store(page_number); 
			faults++;            
			frame_number=next_frame-1;
        	}
    	}
    
    	addTLB(page_number, frame_number); 
    	value_memory = physical_mem[frame_number][offset]; 
	unsigned int physical_address=(frame_number << OFFSET_BITS)|offset;

    	printf("Virtual address: %d Physical address: %d Value: %d\n",virtual_address,physical_address, value_memory);
}

int main(int argc, char *argv[])
{
    	if (argc != 3) {
        	fprintf(stderr,"Usage: ./a.out <virtual address input file>\n");
        	return -1;
    	}
        fptr1 = fopen(argv[1], "r");
	mmapfile_fd=open(argv[2],O_RDONLY);
    	
    	if (mmapfile_fd<0)
		printf("Error in opening BACKING_STORE.bin");
	
    	if (fptr1 == NULL) {
        	printf("Error in opening addresses.txt\n");
        	return -1;
    	}
    
    	while (fgets(input_buffer, 10, fptr1) != NULL) {
        	virtual_address = atoi(input_buffer);
        	searchTLB(virtual_address);
    	}
	
	printf("Number of Translated Addresses = 1000\n");
    	printf("Page Faults = %d\n", faults);
    	printf("TLB Hits = %d\n", hits);

    	fclose(fptr1);
	close(mmapfile_fd);
    	return 0;
}












