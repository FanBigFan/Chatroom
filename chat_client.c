#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void *
child_func (void *arg)
{
  int nfp = (int)arg;
  int ret;
  char buf[256];

  while (1)
    {
      bzero (buf, sizeof (buf));
      ret = recv (nfp, buf, sizeof (buf), 0);
      if (ret <= 0)
        {
          printf ("recv error.\n");
          break;
        }
      else
        printf ("%s\n", buf);
    }
  close (nfp);
  return NULL;
}

int
main ()
{
  int cfp;
  struct sockaddr_in saddr;
  int sin_size, ret;
  unsigned short port = 0x9999;
  char buf[256];
  pthread_t tid;

  cfp = socket (AF_INET, SOCK_STREAM, 0);
  if (cfp == -1)
    {
      printf ("socket error.\n");
      return -1;
    }
  printf ("connecting to server... \n");

  bzero (&saddr, sizeof (saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
  saddr.sin_port = htons (port);
  ret = connect (cfp, (struct sockaddr *)&saddr, sizeof (struct sockaddr));
  if (ret == -1)
    {
      printf ("connect error.\n");
      close (cfp);
      return -1;
    }
  printf ("connect success.\n");

  printf ("login: ");
  scanf ("%s", buf);
  ret = send (cfp, buf, strlen (buf), 0);
  if (ret <= 0)
    {
      printf ("send error.\n");
      close (cfp);
      return -1;
    }

  if (strcmp (buf, "over") == 0)
    {
      close (cfp);
      return 0;
    }

  pthread_create (&tid, NULL, &child_func, (void *)cfp);
  while (1)
    {
      bzero (buf, 256);
      scanf ("%s", buf);
      ret = send (cfp, buf, strlen (buf), 0);
      if (ret <= 0)
        {
          printf ("send error.\n");
          break;
        }
      if (strcmp (buf, "over") == 0)
        break;
    }
  printf ("client over\n");
  close (cfp);
  return 0;
}
