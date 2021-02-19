#include <pthread.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

typedef struct {
	long * CurrentStatus; // variable derefences as the process indicator
	long InitialValue; //the starting value for computation
	long TerminationValue; //the value at which the computation is complete
} PROGRESS_STATUS;

// monitors wordcount
void *progress_monitor(void *statusMonitor) {
	PROGRESS_STATUS tempStaut = *(PROGRESS_STATUS*)statusMonitor;
	int i = 0;
	while(true) {
		// display one hyphen for every 1/40
		if (i < (40*(*(tempStaut.CurrentStatus)) / 
			(tempStaut.TerminationValue - tempStaut.InitialValue))){
			i++;
			cout <<"-";
			cout.flush();
		}
		//
		if (*tempStaut.CurrentStatus == tempStaut.TerminationValue && i == 40)
		{
			cout << endl;
			break;
		}
	}

	return 0;
}
// count the words
long wordcount(char* filenames) {
	ifstream infile;
	long numberofWord = 0;

	infile.open(filenames);
	//check the file
	if (!infile.is_open()) {
		cout<< "Failed to open the file." <<endl;
		exit(0);
	}
	char getCharacter;
	int errorchecker;
	pthread_t id;
	struct stat statBuff;
	// get the total bytes of the file
	int errnumber = lstat(filenames, &statBuff);
	if (errnumber == 0) {
		PROGRESS_STATUS progressStatus;
		progressStatus.CurrentStatus = new long();
		progressStatus.InitialValue = 0;
		progressStatus.TerminationValue = statBuff.st_size;
		errorchecker = 
			pthread_create(&id, NULL, &progress_monitor, &progressStatus);
		//check posix thread create
		if (errorchecker)
		{
			cout << "create pthread error!" << endl;
			exit(1);
		}
		//Check end of file
		while (!infile.eof()) {
			getCharacter = infile.get();

			if (infile.fail()) {
				break;
			}
			//counting the word
			if (!iswspace(getCharacter)) {
				numberofWord++;
				(*progressStatus.CurrentStatus)++;
				while (infile.get(getCharacter)) {

					if (iswspace(getCharacter)) {
						(*progressStatus.CurrentStatus)++;
						break;
					}
					else {
						(*progressStatus.CurrentStatus)++;
					}
				}
			}
			// if the first word is whitespace or mutiple whitespace
			else {
				(*progressStatus.CurrentStatus)++;
			}
		}
	}
	else
	{
		cout << "could not open file.";
		exit(0);
	}
	//waiting for process_monitors complete.
	pthread_join(id, NULL);
	infile.close();
	return numberofWord;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "no file specified.";
		return 0;
	}

	long number = wordcount(argv[1]);
	cout << "There are " << number << " words in "
		<< "\""<< argv[1] << "\"" <<"." <<endl;
	return 0;

}
