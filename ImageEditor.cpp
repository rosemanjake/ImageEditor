#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstddef>
#include <sstream>
#include <bitset>
#include <numeric>
#include <utility>

//Class for pixel objects with RGB methods and get/set methods for flexible manipulation
class Pixel
{
    public:
    int8_t B;
    int8_t G;
    int8_t R;

    Pixel() {} // default constructor for resizing vectors.

    //I'm using BGR as the order as that's how the bytes appear in the file sequentially on account of little endianness.
    Pixel(int8_t aB, int8_t aG, int8_t aR)
    {
        B = aB;
        G = aG;
        R = aR;
    }
};

//Class for image objects containing everything we need to manipulate and save images etc.
class Image
{
    public:
    std::vector<int8_t> Header;
    std::vector<Pixel> Pixels;
    int Height;
    int Width;

    Image(std::vector<int8_t> aHeader, std::vector<Pixel> aPixels, int aHeight, int aWidth)
    {
        Header = aHeader;
        Pixels = aPixels;
        Height = aHeight;
        Width = aWidth;
    }
};

//Get file names for input and output
std::string filenamefetcher(std::string type)
{
    std::cout << "What do you want the name of your " + type + " file to be?\n";
    std::string filename;
    std::cin >> filename;
    return filename;
}

//Turn 8 bit int into a string with the actual binary value
std::string toBinary(int8_t byte)
{
    std::string binary = std::bitset<8>(byte).to_string(); //bitset is width 8 because each byte going in is 8 bits long, convert to string of binary
    return binary;
}

//Turn two string with binary values into a single 16 bit decimal value
unsigned long toLong(std::string bin2, std::string bin) //might be useful to make this more than 16 bit
{
    std::string binary = bin2 + bin; //need to be the second value ahead of the first becuase the files reads right to left on account of endianness (relevant for image dimensions)
    unsigned long decimal = std::bitset<16>(binary).to_ulong(); //need to be 16 bit now as that's the size of the value
    return decimal;
}

//Turn single 8-bit value into a single long - you can use this to get the unsigned value of an int8_t
unsigned long longatron(std::string bin)
{
    unsigned long decimal = std::bitset<8>(bin).to_ulong();
    return decimal;
}

//Convert back from a binary string to an 8 bit int
uint8_t to8bitint(std::string bin)
{
    std::string binary = bin;
    uint8_t decimal = std::bitset<8>(binary).to_ulong();
    return decimal;
}

//Write an image to disk, using the image object
void imagewriter_obj(Image& image)
{
    std::string filename{ filenamefetcher("output") };
    std::ofstream file(filename, std::ios_base::binary | std::ios_base::out);

    for (size_t i = 0; i < image.Header.size(); i++)
    {
        file << image.Header[i];
    }

    for (size_t i = 0; i < image.Pixels.size(); i++)
    {
        file << image.Pixels[i].B;
        file << image.Pixels[i].G;
        file << image.Pixels[i].R;
    }
}

//Turn bytes into objects with the RGB values in signed 8bit decimals
std::vector<Pixel> bytestopix(std::vector<int8_t> vector)
{
    std::cout << "Creating pixel objects...\n";
    std::vector<Pixel> pixels;

    int8_t R = 0;
    int8_t G = 0;
    int8_t B = 0;

    for (size_t i = 54; i < vector.size(); i++)//will just be the pixels now
    {
        int index = i % 3;//divide i by 3 and get the remainder so we can sort the list of pixels into sets of three

        if (index == 0)
        {
            B = vector[i]; //In the file, the order is BGR/right to left because it's little endian
        }
        if (index == 1)
        {
            G = vector[i];
        }
        if (index == 2)
        {
            R = vector[i];
            pixels.push_back(Pixel(B, G, R));
        }
    }

    std::cout << "Number of pixels: " << pixels.size() << std::endl;

    return pixels;
}

//Extract header (first 54 bytes) of the file/bytestream
std::vector<int8_t> headerfetcher(std::vector<int8_t> vector)
{
    std::cout << "Fetching image header..." << std::endl;

    std::vector<int8_t> header;

    for (size_t i = 0; i < 54; i++)//will just be the pixels now
    {
        header.push_back(vector[i]);
    }

    return header;
}

//Read .bmp image and store each byte as an int8_t (signed 8 bit int)
std::vector<int8_t> imagereader()
{
    std::string filename{ filenamefetcher("input") };
    int8_t byte;
    std::vector<int8_t> bytes;

    std::ifstream file(filename, std::ios_base::binary | std::ios_base::out); //input file
    file >> std::noskipws;
    
    std::cout << "Reading your file..." << std::endl;

    while (file >> byte)//for each byte in file is what this actually does
    {
        bytes.push_back(byte);//add to end of vector bytes
    }
    return bytes;
}

//Get the width and height of the image from the header
std::vector<int> measure(std::vector<int8_t> bytes) //get the height and width by reading the byes in the image header
{
    std::vector<int> dimensions;

    std::string bin;
    std::string bin2;

    bin = toBinary(bytes[18]); //these are the bytes in the .bmp header that record the image height, the next are for width
    bin2 = toBinary(bytes[19]);
    dimensions.push_back((int)toLong(bin2, bin));

    bin = toBinary(bytes[22]);
    bin2 = toBinary(bytes[23]);
    dimensions.push_back((int)toLong(bin2, bin));

    std::cout << "Height of the image: " << dimensions[1] << std::endl << "Width of the image: " << dimensions[0] << std::endl;
    
    return dimensions;
}

//Flips any vector of pixel objects - you can use this for individual rows, or to rotate the full image
std::vector<Pixel> flipper(std::vector<Pixel> Pixels)
{
    std::vector<Pixel> flippedpixels;

    for (size_t i = 0; i < Pixels.size(); i++)
    {
        flippedpixels.insert(flippedpixels.begin(), Pixels[i]);
    }

    return flippedpixels;
}

//Transforms array of pixels into a 2d array to track the rows and columns
std::vector<std::vector<Pixel>> arraymaker (std::vector<Pixel> Pixels, int Height, int Width)
{
    std::vector<std::vector<Pixel>> array;

    array.resize(Height);
    for (int i = 0; i < Height; i++)//top level array contains rows
    {
        array[i].resize(Width);
    }

    int p = 0;
    for (size_t i = 0; i < array.size(); i++)//for each row
    {
        for (size_t j = 0; j < array[i].size(); j++, p++)//for each pixel in the row
        {
            array[i][j] = Pixels[p];
        }
    }

    return array;
}

//Transform 2d array to single array of pixels - makes the image object simpler
std::vector<Pixel> arraytopix(std::vector<std::vector<Pixel>> array)
{
    std::vector<Pixel> Pixels;

    for (size_t i = 0; i < array.size(); i++)//for each row
    {
        for (size_t j = 0; j < array[i].size(); j++)//for each pixel in the row
        {
            Pixels.push_back(array[i][j]);
        }
    }

    return Pixels;
}

//Flip image horizontally
Image hflip(Image& image)
{
    std::vector<std::vector<Pixel>> multiarray = arraymaker(image.Pixels, image.Height, image.Width);

    for (size_t i = 0; i < multiarray.size(); i++)
    {
        multiarray[i] = flipper(multiarray[i]);
    }

    return Image(image.Header, arraytopix(multiarray), image.Height, image.Width);
}

//Flip image vertically
Image vflip(Image& image)
{
    std::vector<std::vector<Pixel>> multiarray = arraymaker(image.Pixels, image.Height, image.Width);
    std::vector<std::vector<Pixel>> flipped;

    for (size_t i = 0; i < multiarray.size(); i++)
    {
        flipped.insert(flipped.begin(), multiarray[i]);
    }
    return Image(image.Header, arraytopix(flipped), image.Height, image.Width);
}

//Turn image greyscale
Image greyscale(Image& image)
{
    for (size_t i = 0; i < image.Pixels.size(); i++)
    {
        int8_t greyvalue = ((to8bitint(toBinary(image.Pixels[i].R)) * 0.3)+(to8bitint(toBinary(image.Pixels[i].G)) * 0.59)+(to8bitint(toBinary(image.Pixels[i].B)) * 0.11));
        //int8_t greyvalue = ((to8bitint(toBinary(image.Pixels[i].R)) + to8bitint(toBinary(image.Pixels[i].G)) + to8bitint(toBinary(image.Pixels[i].B))) / 3);
        image.Pixels[i].R = greyvalue;
        image.Pixels[i].G = greyvalue;
        image.Pixels[i].B = greyvalue;
    }
    return Image(image.Header, image.Pixels, image.Height, image.Width);
}

//Function for modifying colour values - here I'm using RGB as the order for the parameters rather than GBR to make mods a bit more user friendly 
Image colourmod(Image& image, double red, double green, double blue)
{
    for (size_t i = 0; i < image.Pixels.size(); i++)
    {
        image.Pixels[i].B = ((int8_t)image.Pixels[i].B * blue);
        image.Pixels[i].G = ((int8_t)image.Pixels[i].G * green);
        image.Pixels[i].R = ((int8_t)image.Pixels[i].R * red);
    }

    return Image(image.Header, image.Pixels, image.Height, image.Width);
}

int main()
{
    std::vector<std::int8_t> bytes = imagereader(); //Read image as bytes.

    std::vector<Pixel> pixels = bytestopix(bytes);//Transform raw btes into pixel objects.
    std::vector<int8_t> header = headerfetcher(bytes);//Extract the header from the file as bytes.
    std::vector<int> dimensions = measure(bytes);//Extract the dimensions of the image from the header.

    Image plain = Image(header, pixels, dimensions[1], dimensions[0]);//Create image object with all the above. We can now transform it as we see fit.
    
    //Isolate red channel
    Image red = colourmod(plain, 1, 0, 0);
    std::cout << "Successfully isolated red channel. ";
    imagewriter_obj(red);

    //Isolate green channel
    Image green = colourmod(plain, 0, 1, 0);
    std::cout << "Successfully isolated green channel. ";
    imagewriter_obj(green);

    //Isolate blue channel
    Image blue = colourmod(plain, 0, 0, 1);
    std::cout << "Successfully isolated blue channel. ";
    imagewriter_obj(blue);

    //flip image horizontally and save
    Image h = hflip(plain);
    std::cout << "Successfully flipped image horizontally. ";
    imagewriter_obj(h);

    //flip image vertically and save
    Image v = vflip(plain);
    std::cout << "Successfully flipped image vertically. ";
    imagewriter_obj(v);
    
    //transform to greyscale and save
    Image g = greyscale(plain);
    std::cout << "Successfully converted image to greyscale. ";
    imagewriter_obj(g);

    return 0;
}
