#include <STDIO.H>
#include <LIMITS.H>
#include "MYTYPES.H"

int main(int argc, char** argv) {
	
	int16_t data = 0;
	uint8_t previous_data = 0;
	uint8_t color_counter = 0; /* Current count of repeating bytes from file */
	FILE* raw_file;
	FILE* lca_file;
	
	/* Create filepath for compressed file */
	char* filepath = (char*) malloc(strlen(argv[1]) + 5);
	strcpy(filepath, argv[1]);
	strcat(filepath, "_lca");
	
	/* Open file for compressing and create destination file */
	raw_file = fopen(argv[1], "rb");
	lca_file = fopen(filepath, "wb");
	
	if(raw_file == NULL) {
		
		printf("File %s doesn't exist!\n", argv[1]);
		return 1;
	}
	
	data = getc(raw_file);
	color_counter++;
	
	/* Read file until EOF reached, writes compressed blocks when different byte value appears */
	while(data != EOF) {
		
		previous_data = (uint8_t) data;
		data = getc(raw_file);
		
		if(previous_data == data) {
			color_counter++;
			if(color_counter == UCHAR_MAX) {
				fwrite(&previous_data, sizeof(previous_data), 1, lca_file);
				fwrite(&color_counter, sizeof(color_counter), 1, lca_file);
				color_counter = 0;
			}
		} else {
			fwrite(&previous_data, sizeof(previous_data), 1, lca_file);
			fwrite(&color_counter, sizeof(color_counter), 1, lca_file);
			color_counter = 1;
		}
	}
	
	fclose(raw_file);
	fclose(lca_file);
	
	return 0;
}