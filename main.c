#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TRUE 1
#define FALSE 0

/*Author: Haydn Gynn
Company: Staffordshire University 
Date: 17/03/2018
Version 1.0 
Purpose: 

*/

typedef struct TrackHeader
{
	char *FileName;
	unsigned int RIFFChunk;
    unsigned int FileSize;
    unsigned short int Channel;
    unsigned int SampleRate;
    unsigned int BytesPerSecond;
    unsigned short int BytesPerSample;
    unsigned short int BitsPerSample;
    unsigned int LengthOfData;
    unsigned int Numberofsamples;
    double DurationofTrack;
    FILE *FilePointer;
    float Percent;
    
}TrackHeader;


int main() {
	char prompt,Input[8],confirmation, OutputFileName[255];	
	int TrackCompare(),Track1Percent, Track2Percent;
	TrackHeader *Track1Header, *Track2Header,*LoadFile();
	float GetTrackPercent();

	if(sizeof(short int) != 2){
		printf("Short int should be configured for 2 bytes");
		return 0;
	}
	
	printf("\n\rTrack1 File:\n\r");
	Track1Header = LoadFile();
	printf("\n\rTrack2 File:\n\r");
	Track2Header = LoadFile();

	
	if(Track1Header != 0 && Track2Header != 0 ){
		if(TrackCompare(Track1Header,Track2Header)){ /* 1 for equal, 0 for not equal*/
			Track1Header->Percent = GetTrackPercent();
			Track2Header->Percent = 1-Track1Header->Percent;			
			
			printf("\n\rOutput File:\n\r");
			GetFileName(OutputFileName);
			
			
			if(Mix(Track1Header,Track2Header,OutputFileName)){
				printf("\n\rMix successfull!\n\r");
			
			}
			
											
		}
		
		fclose(Track1Header->FilePointer);
		fclose(Track2Header->FilePointer);
	}


	printf("\n\rPress any key to exit \n\r");
	scanf("%c",&prompt);
	return(0);
}

int Mix(TrackHeader *file1,TrackHeader *file2,char *OutputfileName){
	FILE *OutputTrack;
	short int sample1Int,sample2Int,ReadSample(),OutputSample;
	unsigned int counter, samples;
	unsigned char temp[2],*BigToLittleArray16(short int, unsigned char[4]);
	int TwoFiles = TRUE;

	
	OutputTrack = fopen(OutputfileName,"wb+");
	
	if(WriteHeaderFile(OutputTrack, file1,file2)){
		
		if(file1->Numberofsamples > file2->Numberofsamples){
			samples = file1->Numberofsamples;
		}else{
			samples = file2->Numberofsamples;
		}
		
		for(counter = 0; counter < samples;counter++){
			TwoFiles = TRUE;
			
			if(counter <= file1->Numberofsamples){
				sample1Int = ReadSample(file1);	
				
				if(counter > file2->Numberofsamples){
					TwoFiles = FALSE;
					OutputSample = sample1Int;
					/*printf("Track1 Finished");*/
				}
			}
			
			if(counter <= file2->Numberofsamples){
				sample2Int = ReadSample(file2);
				
				if(counter > file1->Numberofsamples){
					TwoFiles = FALSE;
					OutputSample = sample2Int;
					/*printf("Track2 Finished");*/
				}
			}
				
			/*printf("\n\r%d %d\n\r",sample1Int,sample2Int);*/
			
			if(TwoFiles == TRUE){
				/*printf("\n\r%f %f\n\r",(file1->Percent * sample1Int),(file2->Percent * sample2Int));*/								
				OutputSample =  (file1->Percent * sample1Int) +  (file2->Percent * sample2Int);
			}
									
			/*printf("\n\r%d\n\r",OutputSample);*/
			
			BigToLittleArray16(OutputSample,temp);
			/*printf("\n\r%x %x\n\r",temp[0],temp[1]);*/
			
			fwrite (temp, 1, 2,OutputTrack); 					
		}
													
		printf("\n\rSaving file...\n\r");
		fclose(OutputTrack);
		printf("\n\r%s Saved!\n\r",OutputfileName);	
	
	}
				
	return 1;
}

short int ReadSample(TrackHeader *file){
	unsigned char sample[2];
	short int sampleInt,NormLittleToint16(),Output;
	
	fread(sample,1, 2, file->FilePointer);
	sampleInt = NormLittleToint16(sample);
	
	return sampleInt;


}

 
int WriteHeaderFile(FILE *file,TrackHeader *file1,TrackHeader *file2){
	unsigned char temp4[4],temp2[2],*BigToLittleArray32(),*BigToLittleArray16(short int, unsigned char[4]);
	char TempString[8];
	unsigned short int byte2;

	/* RIFF */
	fwrite ("RIFF", 1, 4,file ); 
	
	/* SizeOf File */
	if(file1->FileSize > file2->FileSize){
		BigToLittleArray32(file1->FileSize,temp4);
	}else{
		BigToLittleArray32(file2->FileSize,temp4);
	}	
	fwrite(temp4, 1, 4,file ); 

	/* WAVE */
	fwrite ("WAVE", 1, 4,file ); 

	/* fmt_ */
	fwrite ("fmt ", 1, 4,file ); 
	
	
	/* format chunk size */
	BigToLittleArray32(0x00000010,temp4);
	fwrite (temp4, 1, 4,file ); 
	
	/* audioformat */
	byte2 = 0x0001;
	BigToLittleArray16(byte2,temp2);
	fwrite (temp2, 1, 2,file ); 
	
	/* channel */
	byte2 = 0x0001;
	BigToLittleArray16(byte2,temp2);
	fwrite (temp2, 1, 2,file ); 
	
	/* sampleRate */
	BigToLittleArray32(file1->SampleRate,temp4);
	fwrite(temp4, 1, 4,file ); 
	
	/* ByteRate */
	BigToLittleArray32(file1->BytesPerSecond,temp4);
	fwrite(temp4, 1, 4,file ); 
	
	/* Bytes per sample */
	BigToLittleArray16(file1->BytesPerSample,temp2);
	fwrite(temp2, 1, 2,file ); 
	
	/* Bits per sample */
	BigToLittleArray16(file1->BitsPerSample,temp2);
	fwrite(temp2, 1, 2,file ); 
	
	/* data  */
	fwrite ("data", 1, 4,file ); 
	
	/* ByteRate */
	if(file1->LengthOfData > file2->LengthOfData){
		BigToLittleArray32(file1->LengthOfData,temp4);
	}else{
		BigToLittleArray32(file2->LengthOfData,temp4);
	}
	fwrite(temp4, 1, 4,file ); 

	return 1;
}



float GetTrackPercent(){
	char Input[8],confirmation;	
	int InputLoop = TRUE,IsNumber(),ConfirmationLoop = TRUE;
	float Track1Percent, Track2Percent;
	void ClearBuffer();
	
	while(InputLoop){
		printf("\n\rHow much percentage of track 1 would you like?(0-100): ");
		scanf(" %s",Input);
		if(IsNumber(Input)){
			sscanf(Input, "%f", &Track1Percent);
			if(Track1Percent >= 0 && Track1Percent <= 100){
				Track2Percent = 100-Track1Percent;
									
				ConfirmationLoop = TRUE;
				while(ConfirmationLoop){
				
					printf("\n\rYou have chosen to mix:\n\r");
					printf("\n\rTrack1: %.2f%%",Track1Percent);
					printf("\n\rTrack2: %.2f%%\n\r",Track2Percent);
					printf("\n\rIs this correct? (Y/N): ");	
					scanf(" %c",&confirmation);
					if(confirmation == 'Y' || confirmation == 'y'){
						InputLoop = FALSE;
						ConfirmationLoop = FALSE;
					}else if(confirmation == 'N' || confirmation == 'n'){
						ConfirmationLoop = FALSE;						
					}else{
						ClearBuffer();
					}
					printf("\n\r");	
				}
																						
			}else{
				printf("\n\rNumer out of range!\n\r");
			}									
		}else{
			printf("\n\rNot a valid input!\n\r");
		}
	}
	return Track1Percent /100;

}

TrackHeader *LoadFile(){
	int FileFound = FALSE,GetFileName(),ConfirmationLoop = TRUE;
	char TrackFileName[255],confirmation;
	FILE *Track;
	TrackHeader *GetTrackHeader(),*header;
	void OutputTrackHeader(),ClearBuffer();	

	while(FileFound == FALSE){
		
		GetFileName(TrackFileName);
		Track = fopen(TrackFileName,"rb");
		if(Track == NULL){
			printf("\n\rCannot find file!\n\r\n\r");
		}else{			
			header = GetTrackHeader(Track);	
			if(header == 0){
				printf("\n\rHeader parse faled\n\r");
				fclose(Track);
			}else{
				header->FileName = TrackFileName;
				header->FilePointer = Track;
				OutputTrackHeader(header);	
				
				ConfirmationLoop = TRUE;
				while(ConfirmationLoop){
				
					printf("\n\rIs this the correct file? (Y/N): ");	
					scanf(" %c",&confirmation);
					if(confirmation == 'Y' || confirmation == 'y'){
						FileFound = TRUE;
						ConfirmationLoop = FALSE; 
					}else if(confirmation == 'N' || confirmation == 'n'){
						ConfirmationLoop = FALSE; 
						fclose(Track);
					
					}else{
						ClearBuffer();
					}
					printf("\n\r");	
				}
			}									
		}
		ClearBuffer();
	}
	return header;
}

int GetFileName(char *FileNamePointer){
	void ClearBuffer();
	printf("Enter a name of the file (Without file extension): ");
	scanf(" %s",FileNamePointer);
	strcat(FileNamePointer,".wav");
}

int TrackCompare(TrackHeader *header1,TrackHeader *header2){

	if(header1->Channel != header2->Channel) return 0;
	
	if(header1->SampleRate != header2->SampleRate) return 0;
	
	if(header1->BytesPerSample != header2->BytesPerSample) return 0;
	
	if(header1->BitsPerSample != header2->BitsPerSample) return 0;

	if(header1->BytesPerSecond != header2->BytesPerSecond) return 0;

	return 1;
}

void OutputTrackHeader(TrackHeader *header){
	printf("\n\rFileName: %s",header->FileName);
	printf("\n\rOverall size: bytes: %u, Kb:%u", header->FileSize + header->RIFFChunk, (header->FileSize + header->RIFFChunk)/1024);
	printf("\n\rDuration: %.2lfs\n\r", header->DurationofTrack);
	/*if(header->Channel == 0x1){
		printf("\n\rChannel: Mono\n\r");
	}
	else if(header->Channel == 0x2){
		printf("\n\rChannel: Stereo\n\r");
	}*/		
}

TrackHeader* GetTrackHeader(FILE *track){
	unsigned char temp[4];
	TrackHeader *header = (TrackHeader*) malloc(sizeof(TrackHeader));
	unsigned int LittleToint32();
	unsigned short int LittleToint16();
	
	/* RIFF Check*/
	fread(temp,1, 4, track);	
	/*printf("\n\r%s\n\r", temp);*/	
	if(strcmp(temp, "RIFF") != 0){ 
		printf("\n\rMissing RIFF\n\r");
		return 0;	
	}
	header->RIFFChunk = 64;
	
	/* File Size*/
	fread(temp,1, 4, track);	
	/*printf("\n\r%x %x %x %x\n\r", temp[0], temp[1], temp[2], temp[3]); */
	header->FileSize = LittleToint32(temp);	
	/*printf("\n\r%x\n\r", header->FileSize); */
	
	/* Wave Check*/
	fread(temp,1, 4, track);	
	/*printf("\n\r%s\n\r", temp);	*/
	if(strcmp(temp, "WAVE") != 0){ 
		printf("\n\rMissing WAVE\n\r");
		return 0;	
	}
	
	/* fmt_ Check*/
	fread(temp,1, 4, track);	
	/*printf("\n\r%s\n\r", temp);	*/
	if(strcmp(temp, "fmt ") != 0){ 
		printf("\n\rMissing fmt\n\r");
		return 0;	
	}
	
	/* Length of format chunk*/
	fread(temp,1, 4, track);	
	/*printf("\n\r%x\n\r", LittleToint32(temp)); */
	if(LittleToint32(temp) != 0x10){ 
		printf("\n\rFormat chunk too short\n\r");
		return 0;	
	}
	
	/* AudioFormat Check (1 for Linear quantization)*/
	fread(temp,1, 2, track);	
	/*printf("\n\r%x\n\r", LittleToint16(temp));*/
	if(LittleToint32(temp) != 0x1){ 
		printf("\n\rAudio format check failed\n\r");
		return 0;	
	}	
	
	/* Number of channels (1 = mono,2 = stereo)*/
	fread(temp,1, 2, track);		
	header->Channel = LittleToint16(temp);
	if(header->Channel != 0x1){
		printf("\n\rOnly supports mono audio\n\r");
		return 0;
	} 
	/*printf("\n\r%x\n\r", header->Channel);*/
	
	/* Sample Rate (Hz)*/
	fread(temp,1, 4, track);		
	header->SampleRate = LittleToint32(temp);
	/*printf("\n\r%d\n\r", header->SampleRate);*/
	
	/* Bytes Per Second*/
	fread(temp,1, 4, track);		
	header->BytesPerSecond = LittleToint32(temp);
	/*printf("\n\r%d\n\r", header->BytesPerSecond);*/
	
	/* Bytes Per Sample*/
	fread(temp,1, 2, track);		
	header->BytesPerSample = LittleToint16(temp);
	if(header->BytesPerSecond/header->BytesPerSample != header->SampleRate){
		printf("\n\rSample Rate Check failed\n\r");
		return 0;
	}	
	/*printf("\n\r%d\n\r", header->BytesPerSample);*/
	
	/* Bits Per Sample*/
	fread(temp,1, 2, track);
	header->BitsPerSample = LittleToint16(temp);
	if(header->BytesPerSample * 0x8 != header->BitsPerSample){
		printf("\n\rBit/Byte per sample comparison failed\n\r");
		return 0;
	}	
	if((header->SampleRate * header->Channel * (header->BitsPerSample/0x8)) != header->BytesPerSecond){
		printf("\n\rSample rate/Byte check failed\n\r");
		return 0;
	}
	/*printf("\n\r%d\n\r", header->BitsPerSample);*/
	
	/* DATA CHUNK*/
	
	/* data check*/
	fread(temp,1, 4, track);	
	/*printf("\n\r%s\n\r", temp);*/	
	if(strcmp(temp, "data") != 0){ 
		printf("\n\rMissing DATA\n\r");
		return 0;	
	}
	
	/* length of datas*/
	fread(temp,1, 4, track);
	header->LengthOfData = LittleToint32(temp);	
	/*printf("\n\r%d\n\r", header->LengthOfData);*/	
	
	/* Number of samples*/
	header->Numberofsamples = header->LengthOfData / header->BytesPerSample;
	/*printf("\n\r%d\n\r", header->Numberofsamples);*/	
	
	/* Duration of track*/
	header->DurationofTrack = (1.0f/header->SampleRate) * header->Numberofsamples; 
	/*printf("\n\r%.2lf\n\r", header->DurationofTrack);	*/
	
	return header;
	
}

unsigned int LittleToint32(unsigned char *array){
	
	return array[0] | (array[1]<<8) | (array[2]<<16) | (array[3]<<24);
	
}
unsigned short int LittleToint16(unsigned char *array){
		
	return array[0] | (array[1] << 8);
	
}

short int NormLittleToint16(unsigned char *array){
		
	return array[0] | (array[1] << 8);
	
}



unsigned char* BigToLittleArray32(unsigned int x,unsigned char array[4]){

	array[0] = (x<<24) >> 24;
	array[1] = ((x<<8) & 0x00ff0000) >> 16;
	array[2] = (((x>>8) & 0x0000ff00)) >> 8;
	array[3] = (x>>24);
	
	return array;	
}

unsigned char* BigToLittleArray16(short int x,unsigned char array[2]){

	array[0] = x & 0xff;
	array[1] = x >> 8;
	
	return array;	
}


void ClearBuffer(){ 
	while ((getchar()) != '\n');
}


int IsNumber(char Input[]){
	int Counter = 0;
	int length = strlen(Input);
	
	for(Counter; Counter < length; Counter++){
		if(Input[Counter] != '-'&& Input[Counter] != '.' && !isdigit(Input[Counter])){
			return 0;
			
		}
	}
	return 1;
}
