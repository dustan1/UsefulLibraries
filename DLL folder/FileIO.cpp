
#include "FileIO.h"

namespace MyWin32DLL
{

FileIO::FileIO()
{
   lineCounter = 0;
   dataCounter = 0;
   isOpen = false;
   dataInLineBuffer = false;
};

FileIO::~FileIO()
{
   closeFile(true);
};


//Goes to the end of the file, checks the length, and then goes to the start.
unsigned long long FileIO::getFileLength()
{
   fileLength = 0; unsigned long long returnInt;
   if(!isBinary){//Not a binary file, therefore it is a text file
      goStart(1);
      std::string line;
      while (std::getline(myfile, line)){
         fileLength = fileLength + line.length() + 1;//Length of string plus null terminator
      }
      goStart(1);
   }

   if (isBinary){
      std::streampos fsize = 0;
      goEnd(1);
      fileLength = myfile.tellg() - fsize;
      goStart(1);
   }
returnInt = fileLength; return returnInt;
}

void FileIO::fileConstructor()
{
   lineCounter = 0;
   dataCounter = 0;
   isOpen = true;
   getFileLength();
}

int FileIO::deleteFile(std::string *fileName)
{

   if( remove( (*fileName).c_str() ) != 0 ){
      perror( "Error deleting file" );
   }
   else
   {
      std::cout<<"File successfully deleted"<<std::endl;
   }
   return 1;
}

//Takes a string, and opens the file at that location.
int FileIO::textOpenFile(std::string filePath, bool isFirstTime)
{
   if (isFirstTime){
      myfile.open(filePath.c_str(), std::ios::out);//creates the file
      myfile.close();
      myfile.open(filePath.c_str(), std::ios::out | std::ios::in);
      isBinary=false;
      fileLength=0;
      fileConstructor();
      return 1;
   }

   myfile.open(filePath.c_str(), std::ios::out | std::ios::in);

   if (myfile.is_open()){
      isBinary=false;
      fileConstructor();
      return 1;
   }

return 0;//file is not open
}

//Takes a string, and opens the file at that location.
int FileIO::dataOpenFile(std::string filePath, bool isFirstTime)
{
   if (isFirstTime){
      myfile.open(filePath.c_str(), std::ios::out);//creates the file
      myfile.close();
      myfile.open(filePath.c_str(), std::ios::out | std::ios::in);
      isBinary=true;
      fileLength=0;
      fileConstructor();
      return 1;
   }

   myfile.open(filePath.c_str(), std::ios::out | std::ios::in);

   if (myfile.is_open()){
      isBinary=true;
      fileConstructor();
      return 1;
   }

return 0;//file is not open
}

//When isRead is 1 or higher, moves the get position. If it is 0, moves the put pointer. Else, returns 0.
int FileIO::goStart(int isRead)
{
   myfile.clear();
   if (isRead > 0){
      myfile.seekg(0, std::ios::beg);
      return 1;
   }
   if (isRead == 0){
      myfile.seekp(0, std::ios::beg);
      return 1;
   }

   return 0;
}

int FileIO::goEnd(int isRead)
{
   myfile.clear();
   if (isRead > 0){
      myfile.seekg(0, std::ios::end);
      return 1;
   }
   if (isRead == 0){
      myfile.seekp(0, std::ios::end);
      return 1;
   }

   return 0;
}

int FileIO::goPos(int isRead, int position)
{

   if (position<0||position>fileLength){
      return 0;
   }

   if (isRead > 0){
      myfile.seekg(0, std::ios_base::beg);
      myfile.seekg(position);
      return 1;
   }
   if (isRead == 0){
      myfile.seekp(0, std::ios_base::beg);
      myfile.seekp(position);
      return 1;
   }

   return 0;

}

//Reads one line from the file, and returns it.
int FileIO::readLine(std::string *output)
{
   if (!myfile.good()){
      return 0;
   }

   std::string input;

   getline( myfile, input );
   if (input.length()==0){
      return 1;
   }

   *output = input;
   return 2;
}

//Reads a line from the file, and then appends an endline to it.
int FileIO::readWholeLine(std::string *output)
{
   if (!myfile.good()){
      return 0;
   }

   std::string input;https://www.youtube.com/watch?v=KJMXZ2T3t3E

   getline( myfile, input );
   if (input.length()==0){
      return 1;//Empty line...
   }
   std::stringstream ss;
   ss<<input<<std::endl;
   *output = ss.str();
   return 2;
}

// It is limited to MAX_BUFFER bytes per pull.
// Multiple executions may be required to get all data.
// http://stackoverflow.com/questions
// /1579719/variable-number-of-parameters-in-function-in-c
int FileIO::readData(int dataLength, int arrayLength, int* errorNum, ...)
{
//For dataLength,
//this affects how many bytes are taken from the file per arrayLength.

//returns 0 for failure, else 1

   char* buffer= new char [MAX_BUFFER];
   //char * buffer = new char [length];
   //This stores 512 4 byte objects (such as integers), or 256 doubles.
   char* output; //void * temp;
   int bytesToGet=dataLength;
   int totalBytesToGet=0;
   int j = 0;
   *errorNum = 0;
   if (dataLength>=MAX_DATA_LENGTH||dataLength<=-1){
      delete [] buffer;
      return 0;
   }

   va_list ap;
   va_start(ap, errorNum);//Gets the pointer to the function parameter list

   output = va_arg(ap, char*);//This allows for modification, and pointer arithmetic
   va_end(ap);//closes list, important...

   if (output == NULL){
      std::cout<<"???"<<std::endl;
      delete [] buffer;
      *errorNum = -1;//Input error
      return 0;
   }


   totalBytesToGet = (bytesToGet * arrayLength);
   if (totalBytesToGet>MAX_BUFFER){
      delete [] buffer;
      *errorNum = -1;//Input error
      return 0;
   }

   //goStart(1);//Temporary
   //read bytes, chance to fail...
   myfile.read(buffer, totalBytesToGet);
   int bytesRead = myfile.gcount();

   if (bytesRead!=totalBytesToGet){
      //goStart(1);//Goes to start, temporary.
      myfile.clear();
      std::cout<<"asdf"<<std::endl;
      system("PAUSE");
      delete [] buffer;
      *errorNum = -2;//Read error, not enough bytes to get.
      return -1 * bytesRead;//Bad stuff, note that all bad returns are less than one.
   }

   //if (myfile.eof()&&myfile.fail()) return 0; //hit end of file...

   for (int i=0;i<(arrayLength*bytesToGet);i++){
      *(output+i) = buffer[i];
   }
   //system("PAUSE");
   *errorNum = 1;
   delete [] buffer;
   return 1;

}

//Puts a newline after the output
int FileIO::writeLine(std::string output)
{

   if (myfile.is_open()){
      myfile << output << std::endl;
      return 1;
   }

return 0;
}

//Does not put a newline after the output
int FileIO::writeStringText(std::string output)
{
   if (myfile.is_open()){
      myfile << output;
      return 1;
   }
return 0;
}

//Adds to the buffer, and then increments the line counter.
void FileIO::bufferLines(std::string input)
{
   if (lineBuffer.empty()){
      lineBuffer.push_back(input);
   }
   if (!dataInLineBuffer){
      lineBuffer.push_back(input);
   }
   std::string temp;
   //temp = lineBuffer[lineCounter];//.back();
   lineBufferBuffer+=input;
   if (lineBuffer.capacity()<=lineCounter+2){
      lineBuffer.reserve(lineCounter+(lineCounter/2)+1);
   }

   lineBuffer[lineCounter] = lineBufferBuffer;//push_back(input);[lineCounter]+=input;
   lineCounter++;
   dataInLineBuffer=false;
   lineBufferBuffer.clear();
}

//Clears entire buffer, and resets counter
void FileIO::clearBuffer()
{
   dataInLineBuffer = false;
   lineBuffer.clear();
   lineBufferBuffer.clear();
   lineCounter=0;
   dataBuffer.clear();
}

//clears the buffer after line #(line), and sets the counter to that line.
int FileIO::clearBuffer(int line)
{
   if (line>lineCounter||line<0){
      return 0;
   }
   lineBuffer[line].clear();
   lineCounter = line;
   dataInLineBuffer = false;
   lineBufferBuffer.clear();
   if (lineBuffer[line].empty())return 1;
   return 0;
}

//Writes the buffer to the file, and then clears it.
void FileIO::writeBuffer()
{
   for (int i=0;i<lineCounter;i++){
      myfile << lineBuffer[i] << std::endl;
   }
   if (dataInLineBuffer){
      myfile << lineBufferBuffer;
   }
clearBuffer();
}

//Does not clear the buffer, allowing for multiple calls.
void FileIO::writeBuffer(bool clearData)
{
   for (int i=0;i<lineCounter;i++){
      myfile << lineBuffer[i] << std::endl;
   }
   if (dataInLineBuffer){
      myfile << lineBufferBuffer;
   }
}

//Does not increment the counter, and instead adds to the currently stored line.
void FileIO::bufferAddition(std::string input)
{
   //lineBuffer[lineCounter]+=input;
   lineBufferBuffer+=input;
   dataInLineBuffer = true;
}


//Takes any array (including single value pointers) and writes it to the file.
//Returns the number of indexes written to the file.
//int FileIO::writeData(int dataLength, int arrayLength, ...){

void FileIO::bufferData(const void* input, int sizeOfData, int arrayLength)
{
   int dataSize = sizeOfData * arrayLength; char* charInput = (char*)input;
   for (int i=0; i<dataSize; i++){
      dataBuffer.push_back(*(charInput + i));
   }
}

int FileIO::writeBufferData()
{
int temp = 0;
if (dataBuffer.size()!=0){
   temp = writeData(sizeof(char), dataBuffer.size(), &(dataBuffer[0]));
   if (temp>0){
      clearDataBuffer();
      return temp;
   }
}
return 0;

}

int FileIO::writeBufferData(int dummy)//Does not clear buffer
{
if (dataBuffer.size()!=0){
   return writeData(sizeof(char), dataBuffer.size(), &(dataBuffer[0]));
}
return 0;

}



void FileIO::clearDataBuffer()
{
   if (dataBuffer.size()>=1){
      clearDataBuffer(0);
   }
}

void FileIO::clearDataBuffer(int dummy)
{
   dataBuffer.clear();//Clearing buffer
   dataBuffer.resize(64);//Resizing buffer
}



//Takes data already
int FileIO::writeDataToFile(const char* data, int length)
{
   if (myfile.is_open()){
      myfile.write(data, length); return !myfile.fail();
   }
return 0;
}

//Takes any array (including single value pointers) and writes it to the file.
//Returns the number of indexes written to the file.
int FileIO::writeData(int dataLength, int arrayLength, ...){

if (dataLength<=0||arrayLength<=0){
   return 0;
}

va_list ap;
const char* dataBytes;
va_start(ap, arrayLength);

   dataBytes = va_arg(ap, const char*);

va_end(ap);

if (!(writeDataToFile(dataBytes, dataLength * arrayLength))){
   return 0;
}
   dataBytes=dataBytes+dataLength;//I forgot to change the variable. Derp.
   return 1;
//return arrayLength;
}

bool FileIO::checkIfOpen()
{
   if (myfile.is_open()){
      isOpen = true;
      return isOpen;
   }

   isOpen = false;
   return isOpen;
}

//Closes the file if and only if there is one open
int FileIO::closeFile()
{
   if (isOpen){
      closeFile(true);
      return 1;
   }
   return 0;

}

//Closes the file, clears the buffer, etc...
void FileIO::closeFile(bool asdf)
{
   lineCounter = 0;
   dataCounter = 0;
   dataInLineBuffer = false;
   myfile.close();
   clearBuffer();
   isOpen = false;
}



}





