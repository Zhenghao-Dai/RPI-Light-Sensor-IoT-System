/**
 * @ingroup     examples
 * @{
 *
 * @file        udp_rx.c
 * @brief       UDP receiver thread
 *
 * @author      Zhenghao Dai 
                Brian Chang
 *
 * Github Link: [https://github.com/usc-ee250-fall2019/finalproj-lucas-brian-11/tree/final/examples/ee250_lab11 ]
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "thread.h"
#include "msg.h"
#include "net/gnrc.h"
#include "udp_rx.h"
#include "timex.h"
#include "mutex.h"
#include "random.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/* TODO */
#define PRNG_INITVAL            67  /* please use a unique int */
#define SLEEP_MSG_STR           "sleep"
#define SLEEP_MSG_LEN           5
#define SLEEP_INTERVAL_SECS     (2)

#define RAND_USLEEP_MIN         (0)
#define RAND_USLEEP_MAX         (1000000)
#define MSG_QUEUE_SIZE 8
char udp_receive_stack [THREAD_STACKSIZE_DEFAULT];
static msg_t msg_udp_rx[MSG_QUEUE_SIZE];
static void *_udp_rx(void *arg)
{
    
    udp_rx_args_t * udp_rx_args = (udp_rx_args_t *)arg; 
    gnrc_pktsnip_t *snip;
    mutex_t *mutex = udp_rx_args->mutex;
    /* initialize PRNG */
    msg_init_queue(msg_udp_rx,MSG_QUEUE_SIZE);
    msg_t msg;
    gnrc_netreg_entry_t udp_rx_reg;
    udp_rx_reg.demux_ctx = udp_rx_args->udp_port;
    udp_rx_reg.target.pid = thread_getpid();
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &udp_rx_reg);
    
    random_init(PRNG_INITVAL);
    printf("PRNG initialized to current time: %d\n", PRNG_INITVAL);
    while (1) {
        msg_receive(&msg);
        mutex_lock(mutex);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                printf("udp_rx: data received\n");
                snip = gnrc_pktsnip_search_type(msg.content.ptr, GNRC_NETTYPE_UNDEF);
                //checking
                if(strncmp(snip->data,SLEEP_MSG_STR, SLEEP_MSG_LEN) == 0){
                    //lock here
                    printf("lock locked");
                    
                    printf("start sleeping");
                    //sleep for 4 sec
                    
                    xtimer_usleep(4000000);
                   
                    printf("end sleeping");
                    //unlock here
                    printf("lock unlocked");
                    
                    /* additional random sleep to reduce network collisions */
                    uint32_t interval = random_uint32_range(RAND_USLEEP_MIN, RAND_USLEEP_MAX);
                    xtimer_usleep(interval);
                }
                gnrc_pktbuf_release(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("udp_rx_thr: sending packets is not my job even though the \
                    way the library is built forwards this message type. \
                    ignoring.\n");
                gnrc_pktbuf_release(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
                DEBUG("udp_rx_thr: get commands are not my job. ignoring.\n");
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("udp_rx_thr: set commands are not my job. ignoring w/ the \
                    necessary reply as per msg.h specifications.\n");
                
                break;


            default:
                DEBUG("udp_rx_thr: received something unexpected");
                break;
        }
        mutex_unlock(mutex);
    }

    /* should never be reached */
    DEBUG("ERROR!\n");
    return 0;
}

kernel_pid_t udp_rx_init(void *args)
{
    kernel_pid_t udp_rx_pid;
    udp_rx_pid= thread_create(
        udp_receive_stack, sizeof(udp_receive_stack),
        THREAD_PRIORITY_MAIN+1 ,
        THREAD_CREATE_STACKTEST,
        _udp_rx,
        args,
        "udp_rx_thread"
        );
    return udp_rx_pid;
}