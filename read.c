// The MIT License (MIT)
//
// Copyright (c) 2020 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//
// Purpose: Demonstrate the use of semaphore with a producer / consumers problem.
// In this example there are a number of cashiers to checkout customers.  A customer
// producer will create a random number of customers from 0 to 10 and add them to the 
// line. Cashier threads will check out the customers as they are available.

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // for bool
#include <unistd.h>

#define INITIAL_CUSTOMERS 1
#define BUFFER 4 // queue
#define NONSHARED 1
#define MAX 5000000

sem_t mutex;
sem_t customer_checked_out, customers_in_line;
int customers_waiting = INITIAL_CUSTOMERS;

FILE * fp;

//char c;
sem_t buffer_length, sem2;
char buffer[5];
int val;
char array[MAX];
bool full = false;
bool more;
int count = 0;

void * Producer(void * arg)
{
  char c;
  int i = 0;

  if ((fp = fopen("message.txt", "r")) == NULL)
  {
    printf("ERROR: can’t open message.txt!\n");
    return 0;
  }

  do
  {
    char c = fgetc(fp);
    // If c hits an EOF, the program will end by breaking out of this loop
    if (c == EOF)
    {
      more == false;
      break;
    }
    more = true;
    // If the count is bigger than size 5, then go back to consumer to let it
    // consume the 5 chars in the array. Let it print back
    // to the screen and then let it come back and do it again
    // until it hits an EOF.
    if (count > BUFFER)
    {
      full == true;
      count = 0;
      sem_wait( & buffer_length);
    }
    // If full and more are both true, then we can reset the count for the buffer size and give it to the consumer to consume
    if ((full == true) && (more == true))
    {
      //full == true;
      count = 0;
      sem_post( & buffer_length);
    }

    array[i] = c;
    i++;
    count++;
    sem_post( & sem2);
  } while (1);
}

void * Consumer(void * arg)
{
  char letter;
  int i = 0;

  if ((letter = fgetc(fp)) == EOF)
  {
    sem_post( & customer_checked_out);
  }

  do
  {
    if (array[i] == 0)
    {
      break;
    }

    sem_wait( & sem2);
    printf("%c", array[i]);
    fflush(stdout);
    i++;
    sleep(1);
    sem_post( & buffer_length);
  } while (1);
}

int main(int argc, char * argv[])
{
  pthread_t producer_tid;
  pthread_t consumer_tid;

  sem_init( & buffer_length, NONSHARED, BUFFER);
  sem_init( & sem2, NONSHARED, 0); // empty queue

  pthread_create( & producer_tid, NULL, Producer, NULL);

  sleep(1);

  pthread_create( & consumer_tid, NULL, Consumer, NULL);

  pthread_join(producer_tid, NULL);
  pthread_join(consumer_tid, NULL);
}
