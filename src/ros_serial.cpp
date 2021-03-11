
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "serialport.h"
#include "error-log.h"

#include <ros/ros.h>

pthread_t read_tid;  /**< read thread */
pthread_t exit_tid;  /**< exit thread */

/** The data we write to the port. */
//char *buf = "Are you going to die?\r\n";
char *buf = "Are you going to die? To be or not to be, that is the question.\r\n";
/** data we receive */
char tmp[512];

/**
 * write_port_thread - A thread that writes data to the port
 *
 * @param argc : Here means the port(specified by the fd).
 *
 * @note
 * This is only a test, not the @e real one.
 */

void *read_port_thread(void *argc)
{
    int num;
    int fd;

    //fd = (int)argc;
    fd = 3;
    while (1)
    {
        while ((num = read(fd, tmp, 512)) > 0)
        {
            debug_msg("read num: %d\n", num);
            tmp[num+1] = '\0';
            printf("%s\n", tmp);
        }
        sleep(0.1);
        if (num < 0)
            pthread_exit(NULL);

        int robot_id = atoi(&tmp[0]);
        ROS_INFO("robot id %d", robot_id);
        ROS_INFO("start char %c", tmp[0]);

  }
    pthread_exit(NULL);
}

void* exit_thread(void* argc)
{
    while (1)
    {
        int c = getchar();
        if ('q' == c)
        {
            printf("You exit, not myself.\n");
            exit(0);
        }
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "ros_serial");

    int fd;
    int ret;
    char dev_name[32] = {0};

    strcpy(dev_name, "/dev/ttyS10");
    if (argc == 2)
    {
      sprintf(dev_name,"%s",argv[1]);
    }

    fd = open_port(dev_name);          /* open the port */
    if (fd < 0)
    {
      printf("open %s err\n",dev_name);
      exit(0);
    }

    ret = setup_port(fd, 115200, 8, 'N', 1);  /* setup the port */
    if (ret<0)
      exit(0);

    ret = pthread_create(&read_tid, NULL, read_port_thread, (void*)fd);
    if (ret < 0)
      unix_error_exit("Create read thread error.");

    ret = pthread_create(&exit_tid, NULL, exit_thread, NULL);
    if (ret < 0)
        unix_error_exit("Create exit thread error.");

    pthread_join(read_tid, NULL);
    pthread_join(exit_tid, NULL);
    close_port(fd);
    //ros::spin();

    return 0;
}
