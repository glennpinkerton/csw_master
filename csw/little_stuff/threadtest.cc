
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "logger.h"

using namespace logger;

void *print_message_function (void *ptr);

int main ()
{

    pthread_t  thread1, thread2;

    MsgLog *plog1 = new MsgLog ();

    plog1->LogInfoMsg ("Main Thread Info message");
    plog1->LogWarningMsg ("Main Thread Warning message");
    plog1->LogErrorMsg ("Main Thread Error message");

    char *message1 = "Application Thread 1";
    char *message2 = "Application Thread 2";

    int  iret1, iret2;

    iret1 = pthread_create (&thread1, NULL, print_message_function, (void *)message1);
    iret2 = pthread_create (&thread2, NULL, print_message_function, (void *)message2);

    pthread_join (thread1, NULL);
    printf ("application thread 1 is finished\n");
    pthread_join (thread2, NULL);
    printf ("application thread 2 is finished\n");

    exit (0);

    return 0;
}


void *print_message_function (void *ptr)
{
    char   *message;

    message = (char *)ptr;

    MsgLog *plog1 = new MsgLog ();

    std::string s1 = message;
    std::string s2;

    s2 = " Info message";
    plog1->LogInfoMsg (s1 + s2);
    s2 = " Warning message";
    plog1->LogWarningMsg (s1 + s2);
    s2 = " Error message";
    plog1->LogErrorMsg (s1 + s2);

    return NULL;
}

