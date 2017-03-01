/* "name" changed to protect the guilty */

int name_async_serial_start(char *pitch_file, char *roll_file, char *gps_file)
{
  if ((name_fds.pitch = open(pitch_file, O_RDONLY)) == -1) {
    fprintf(stderr, "open(%s): %s.\n", pitch_file, strerror(errno));
    goto open_pitch;
  }
  if ((name_fds.roll = open(roll_file, O_RDONLY)) == -1) {
    fprintf(stderr, "open(%s): %s.\n", roll_file, strerror(errno));
    goto open_roll;
  }
  if ((name_fds.gps = open(gps_file, O_RDONLY)) == -1) {
    fprintf(stderr, "open(%s): %s.\n", gps_file, strerror(errno));
    goto open_gps;
  }

  if (pthread_mutex_init(&pr_mutex, NULL)) {
    perror("pthread_mutex_init");
    goto pr_mutex_init;
  }
  if (pthread_cond_init(&pr_cond, NULL)) {
    perror("pthread_cond_init");
    goto pr_cond_init;
  }
  if (pthread_mutex_init(&gps_mutex, NULL)) {
    perror("pthread_mutex_init");
    goto gps_mutex_init;
  }
  if (pthread_cond_init(&gps_cond, NULL)) {
    perror("pthread_cond_init");
    goto gps_cond_init;
  }

  if (pthread_create(&thread_id, NULL, reader_thread, NULL)) {
    perror("pthread_create()");
    goto pthreads;
  }

  return 0;

 pthreads:
  pthread_cond_destroy(&gps_cond);
 gps_cond_init:
  pthread_mutex_destroy(&gps_mutex);
 gps_mutex_init:
  pthread_cond_destroy(&pr_cond);
 pr_cond_init:
  pthread_mutex_destroy(&pr_mutex);
 pr_mutex_init:
  close(name_fds.gps);
 open_gps:
  close(name_fds.roll);
 open_roll:
  close(name_fds.pitch);
 open_pitch:

  return -1;
}
