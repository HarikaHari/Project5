#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IMAGE_MAX_SIZE 1024

typedef struct pixel {
    float r, g, b;
} pixel;

typedef struct Image {
    int width, height;
    pixel* pixmap;
} Image;




int getNextWhiteSpace(FILE* fp, char buffer[], int buffer_size) {
    int c;
    int pos = 0;
    while (1) {
        if (pos > buffer_size - 1) {
            fprintf(stderr, "Error: Buffer not large enough to finish reading to whitespace\n");
            return -1;
        }
        c = getc(fp);
        if (c == EOF) {
            fprintf(stderr, "Error: Prematured End Of File");
            return -1;
        }
        if(c == '\r'||c == '\n'||c == ' '||c == '\t') {
            fseek(fp, -1, SEEK_CUR);
            buffer[pos] = '\0';
            return pos;
        }
        buffer[pos++] = (char)c;
    }
};



int skipComments(FILE* fp) {
    int temp;
    char in = 0;
    while (1) {
        temp = getc(fp);
        if (temp == EOF) {
            fprintf(stderr, "Error: The file is not a valid PPM3 image file\n");
            return 1;
        }
        if (in) {
            if (temp == '\n' || temp == '\r')
                in = 0;
        }
        else if (temp == '#')
            in = 1;
        else {
            fseek(fp, -1, SEEK_CUR);
            return 0;
        }
    }
};

int removeWhiteSpace(FILE* fp) {
    int c;
    do {
        c = getc(fp);
        // make sure we didn't read to the EOF
        if (c == EOF) {
            fprintf(stderr, "Error: Prematured End Of File");
            return 1;
        }
        if (c == '\n' || c == '\r') {
			if (skipComments(fp) != 0)
			return 1;
        }
    }
    while(c == '\r'||c == '\n'||c == ' '||c == '\t'); 
    fseek(fp, -1, SEEK_CUR);
    return 0;
}


int readP3(FILE* fp, Image* imagePtr, int MAX_COLOR, char buffer[]) {
    int height = imagePtr->height;
    int width = imagePtr->width;
    imagePtr->pixmap = malloc(sizeof(pixel) * width * height);

    // Read the actual image in
    int i, j, k;
    float value;
    int bytes_read;
    for (i=0; i<height; i++) {
        for (j=0; j<width; j++) {
            for (k=0; k<3; k++) {
                // start of reading the pixel data into buffer
                if (removeWhiteSpace(fp) != 0) {
                    fprintf(stderr, "Error: An error occurred skipping file whitespace\n");
                    return 1;
                }

                bytes_read = getNextWhiteSpace(fp, buffer, IMAGE_MAX_SIZE);
                if (bytes_read <= 0) {
                    fprintf(stderr, "Error: Expected a color value but read nothing\n");
                    return 1;
                }
                else if (atoi(buffer) > MAX_COLOR) {
                    fprintf(stderr, "Error: A color sample is greater than the maximum color (%i) value \n", MAX_COLOR);
                    return 1;
                }
                else if (atoi(buffer) < 0) {
                    fprintf(stderr, "Error: A negative color sample is not a valid value \n");
                    return 1;
                }
                value = (atoi(buffer)/(float)MAX_COLOR);

                if (k == 0)
                    imagePtr->pixmap[i*width + j].r = value;
                if (k == 1)
                    imagePtr->pixmap[i*width + j].g = value;
                if (k == 2)
                    imagePtr->pixmap[i*width + j].b = value;
            }
        }
    }
	//end of reading the pixel data into buffer
    return 0;
}

int readP6(FILE* fp, Image* imagePtr, int MAX_COLOR, char buffer[]) {
    int height = imagePtr->height;
    int width = imagePtr->width;
    imagePtr->pixmap = malloc(sizeof(pixel) * width * height);

    if (removeWhiteSpace(fp) != 0) {
        fprintf(stderr, "Error: An error occurred skipping file whitespace\n");
        return 1;
    }

    // start of reading the pixel data into buffer
    int i, j, k;
    size_t bytes_read;
    float value;
    for (i=0; i<height; i++) {
        for (j=0; j<width; j++) {
            for (k=0; k<3; k++) {
                if (MAX_COLOR < 256) {
                    bytes_read = fread(buffer, sizeof(char), 1, fp);
                    if (bytes_read < 1) {
                        fprintf(stderr, "Error: Expected a color value but read nothing\n");
                        return 1;
                    }
                    value = buffer[0] & 0xFF;
                }
                else {
                    bytes_read = fread(buffer, sizeof(char), 2, fp);
                    if (bytes_read < 2) {
                        fprintf(stderr, "Error: Expected a color value but read nothing\n");
                        return 1;
                    }
                    value = ((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF);
                }

                value = (value/(float)MAX_COLOR);

                if (k == 0)
                    imagePtr->pixmap[i*width + j].r = value;
                if (k == 1)
                    imagePtr->pixmap[i*width + j].g = value;
                if (k == 2)
                    imagePtr->pixmap[i*width + j].b = value;
            }
        }
    }
	//end of reading the pixel data into buffer
    return 0;
}

int readImage(Image* imagePtr, char* name) {
    FILE* fp = fopen(name, "r");
    if (fp) {
        int version = 0, width, height;
        char buffer[IMAGE_MAX_SIZE];
        int MAX_COLOR;

        if (getNextWhiteSpace(fp, buffer, IMAGE_MAX_SIZE) != 2) {
            fprintf(stderr, "ERROR: Invalid source file i.e. not a valid PPM file");
            fclose(fp);
            return 1;
        }
        if (strncmp("P3", buffer, 2) == 0) 
            version = 3;
        
        else if (strncmp("P6", buffer, 2) == 0) 
            version = 6;
        
        else {
            fprintf(stderr, "ERROR: Invalid source file i.e. not a valid PPM file");
            fclose(fp);
            return 1;
        }
		
        if (removeWhiteSpace(fp) != 0) {
            fprintf(stderr, "Error: An error occurred skipping file whitespace\n");
            fclose(fp);
            return 1;
        }

        // width
        if (getNextWhiteSpace(fp, buffer, IMAGE_MAX_SIZE) <= 0) {
            fprintf(stderr, "Error: Expected a width value but read nothing\n");
            return 1;
        }

        width = atoi(buffer);

        if (width < 0)
        {
            fprintf(stderr, "ERROR: Invalid source file i.e. not a valid PPM file");
            fclose(fp);
            return 1;
        }
		printf("\n got the version %d",width);
 
        if (removeWhiteSpace(fp) != 0) {
            fprintf(stderr, "Error: An error occurred skipping file whitespace\n");
            fclose(fp);
            return 1;
        }

        // height
        if (getNextWhiteSpace(fp, buffer, IMAGE_MAX_SIZE) <= 0) {
            fprintf(stderr, "Error: Expected a width value but read nothing\n");
            return 1;
        }

        height = atoi(buffer);

        if (height < 0)
        {
            fprintf(stderr, "ERROR: Invalid source file i.e. not a valid PPM file");
            fclose(fp);
            return 1;
        }


        if (removeWhiteSpace(fp) != 0) {
            fprintf(stderr, "Error: An error occurred skipping file whitespace\n");
            fclose(fp);
            return 1;
        }

        // MAX_COLOR
        if (getNextWhiteSpace(fp, buffer, IMAGE_MAX_SIZE) <= 0) {
            fprintf(stderr, "Error: Expected a maximum color value but read nothing\n");
            return 1;
        }

        MAX_COLOR = atoi(buffer);

        if (MAX_COLOR < 0 || MAX_COLOR > 65535)
        {
            fprintf(stderr, "Error: Expected maximum color value between 0 and 65536");
            fclose(fp);
            return 1;
        }

        imagePtr->width = (int) width;
        imagePtr->height = (int) height;
		printf("\n image pointer width %d",imagePtr->width);
		printf("\n image pointer height %d",imagePtr->height);
        int result;
        if (version == 6)
            result = readP6(fp, imagePtr, MAX_COLOR, buffer);

        if (version == 3)
            result = readP3(fp, imagePtr, MAX_COLOR, buffer);

        fclose(fp);
        return result;
    }
    else {
        fprintf(stderr, "Error: An error occurred while reading the image file.\n", name);
        return 1;
    }
}


int main (int argc, char *argv[]) {
    // Check input arguments
    if (argc != 2) {
        fprintf(stderr, "Error: Not enough arguments provided\n");
        return 1;
    }
	Image image;
    char *fileName = argv[1];
    // start of code to load ppm image file
  	int loadTemp = readImage(&image, fileName);
	
    if ( loadTemp != 0) {
        fprintf(stderr, "Error: An error occurred loading image file.\n");
        exit(1);
    }
	 // end of code to load ppm image file
    exit(EXIT_SUCCESS);
}