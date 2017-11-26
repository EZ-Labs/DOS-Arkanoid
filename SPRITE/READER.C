#include <STDIO.H>
#include <LIMITS.H>
#include "MYTYPES.H"

int main(int argc, char** argv) {
	
	int16_t data = 0;
	uint8_t color_counter = 0; /* Current count of repeating bytes from file */
	uint8_t color_index = 0;
	FILE* raw_file;
	FILE* lca_file;
	
	/* Create filepath for uncompressed file */
	char* filepath = (char*) malloc(strlen(argv[1]) + 5);
	strcpy(filepath, argv[1]);
	strcat(filepath, ".RAW");
	
	/* Open file for uncompressing and create destination file */
	lca_file = fopen(argv[1], "rb");
	raw_file = fopen(filepath, "wb");
	
	if(lca_file == NULL) {
		
		printf("File %s doesn't exist!\n", argv[1]);
		return 1;
	}
	
	/* Read file until EOF reached, writes uncompressed blocks on each loop */
	while(1) {
		
		size_t n;
		
		data = getc(lca_file);
		
		if(data == EOF) break;
		
		color_index = (uint8_t) data;
		
		data = getc(lca_file);
		
		if(data == EOF) break;
		
		color_counter = (uint8_t) data;
		
		for(n = 0; n < color_counter; n++) {
			
			fwrite(&color_index, sizeof(color_index), 1, raw_file);
		}
	}
	
	fclose(raw_file);
	fclose(lca_file);
	
	return 0;
}