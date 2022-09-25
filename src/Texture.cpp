#include "Texture.h"


/* Function to load a BMP image passing back width, height and pixel data as char* */  
bool Texture::LoadBMP(std::string filename, int & width, int & height, char * &data)
{
	std::ifstream input;
	input.open(filename.c_str(), std::ifstream::binary);
	assert(!input.fail() || !"Could not find file");
	char buffer[2];
	input.read(buffer, 2);
	assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Not a bitmap file");
	input.ignore(8);
	int dataOffset = readInt(input);
	
	// Read the header
	int headerSize = readInt(input);
	switch(headerSize)
	{
		case 40:
			// V3
			width = readInt(input);
			height = readInt(input);
			input.ignore(2);
			assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			assert(readShort(input) == 0 || !"Image is compressed");
			break;
		case 12:
			// OS/2 V1
			width = readShort(input);
			height = readShort(input);
			input.ignore(2);
			assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			break;
		case 64:
			// OS/2 V2
			assert(!"Can't load OS/2 V2 bitmaps");
			break;
		case 108:
			// Windows V4
			assert(!"Can't load Windows V4 bitmaps");
			break;
		case 124:
			// Windows V5
			assert(!"Can't load Windows V5 bitmaps");
			break;
		default:
			assert(!"Unknown bitmap format");
	}
	
	// Read the data
	int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
	int size = bytesPerRow * height;
	auto_array<char> pixels(new char[size]);
	input.seekg(dataOffset, std::ios_base::beg);
	input.read(pixels.get(), size);
	
	// Get the data into the right format
	data = new char[width * height * 3];
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			for(int c = 0; c < 3; c++) {
				data[3 * (width * y + x) + c] = pixels[bytesPerRow * y + 3 * x + (2 - c)];
			}
		}
	}
	
	// Close input stream
	input.close();
	
	std::cout << "Loaded " << filename << " size " << width << "x" << height << std::endl;
	return true;
}


GLuint Texture::LoadBMP(std::string filename)
{
	std::ifstream input;
	input.open(filename.c_str(), std::ifstream::binary);
	assert(!input.fail() || !"Could not find file");
	char buffer[2];
	input.read(buffer, 2);
	assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Not a bitmap file");
	input.ignore(8);
	int dataOffset = readInt(input);
	
	// Read the header
	int headerSize = readInt(input);
	int width;
	int height;
	switch(headerSize)
	{
		case 40:
			// V3
			width = readInt(input);
			height = readInt(input);
			input.ignore(2);
			assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			assert(readShort(input) == 0 || !"Image is compressed");
			break;
		case 12:
			// OS/2 V1
			width = readShort(input);
			height = readShort(input);
			input.ignore(2);
			assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			break;
		case 64:
			// OS/2 V2
			assert(!"Can't load OS/2 V2 bitmaps");
			break;
		case 108:
			// Windows V4
			assert(!"Can't load Windows V4 bitmaps");
			break;
		case 124:
			// Windows V5
			assert(!"Can't load Windows V5 bitmaps");
			break;
		default:
			assert(!"Unknown bitmap format");
	}
	
	// Read the data
	int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
	int size = bytesPerRow * height;
	auto_array<char> pixels(new char[size]);
	input.seekg(dataOffset, std::ios_base::beg);
	input.read(pixels.get(), size);
	
	// Get the data into the right format
	auto_array<char> pixelData(new char[width * height * 3]);
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			for(int c = 0; c < 3; c++) {
				pixelData[3 * (width * y + x) + c] = pixels[bytesPerRow * y + 3 * x + (2 - c)];
			}
		}
	}
	
	input.close();
	
	GLuint texture;
	glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelData.release());
	
	std::cout << "Loaded " << filename << " into Texture " <<  texture << " with size " << width << "x" << height << std::endl;
	
	return texture;
}


