//
// Created by d.tremasov on 18.10.2022.
//

// Тремасов Даниил 20 номер в списке, группа М22-524; 544 % 4 = 0  Способ А; N = 4 + (544 % 5) = 8;

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include<windows.h>

using namespace std;

CRITICAL_SECTION cs;

const int M = 219490; //438980   ÷   2
const int N = 8;

struct task {
    int from;
    int to;
    int number;
};

int global_res1 = 0;
int global_res2 = 0;
short wav_numbers[M];

DWORD __stdcall f(void *arg) {
    task *t = (task *) arg;
    int res1 = 0;
    int res2 = 0;
    for (int i = t->from; i < t->to; i++) {
        wav_numbers[i] > 16000 ? res1++ : res2++;
    }

    //synchronized
    EnterCriticalSection(&cs);
    global_res1 += res1;
    global_res2 += res2;
    printf("Thread %d ended\n", t->number);
    LeaveCriticalSection(&cs);
    return 0;
}

HANDLE th[N];
DWORD tid[N];
task t[N];

// Wav-header structure
struct WAVHEADER {
    char                 chunkId[4];
    unsigned long        chunkSize;
    char                 format[4];
    char                 subchunk1Id[4];
    unsigned long        subchunk1Size;
    unsigned short       audioFormat;
    unsigned short       numChannels;
    unsigned long        sampleRate;
    unsigned long        byteRate;
    unsigned short       blockAlign;
    unsigned short       bitsPerSample;
    char                 subchunk2Id[4];
    unsigned long        subchunk2Size;
};

int main() {
    InitializeCriticalSection(&cs);
    WAVHEADER header;
    int j = 0;
    FILE *file;

    fopen_s(&file, "../Wav.wav", "rb");
    fread(&header, sizeof(WAVHEADER), 1, file);

    short a;
    //printf("\nA: %d\n", a);
    while ((fread(&a, 1, 2, file)) > 0) {
        wav_numbers[j] = a;
        // printf("\nA: %d\n", a);
        j++;
    };

    for (int j = 0; j < N; j++) {
        t[j].from = (j * M / N);
        t[j].to = ((j + 1) * M / N);
        t[j].number = j;

        th[j] = CreateThread(NULL, M, f, (void *) &t[j], 0, &tid[j]);
    }

    for (int i = 0; i < N; i++) {
        WaitForSingleObject(th[i], INFINITE);
    }

    printf("\nCount more than 16000: %d\n", global_res1);
    printf("Count less than 16000: %d\n", global_res2);
    DeleteCriticalSection(&cs);
}
