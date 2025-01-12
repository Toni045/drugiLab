#ifndef CONFIG_H
#define CONFIG_H

#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/list.h>

#define MAX_MSGS        16
#define MAX_MSG_SIZE    64
#define MAX_THREADS     5

struct msg {
    struct list_head list;   // lista za poruke
    size_t size;
    char d[1];               // prvi bajt poruke, ostali odmah iza
};

struct msg_queue {
    size_t max_msgs;         // maksimalan broj poruka u redu
    size_t msg_cnt;          // trenutni broj poruka
    size_t max_msg_size;     // maksimalna veličina poruke
    size_t max_threads;      // maksimalan broj dretvi
    size_t thread_cnt;       // trenutni broj dretvi

    struct list_head msgs;   // lista poruka
    struct semaphore readers; // semafor za čitače
    struct semaphore writers; // semafor za pisače
    struct mutex lock;        // zaključavanje za pristup redu
};

#endif // CONFIG_H
