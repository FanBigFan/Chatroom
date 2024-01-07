#include <assert.h>
#include <netinet/in.h>  //tcp/ip
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
// #include <typedef>

pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER;

typedef struct client_node {
  int sock;
  char name[64];
  struct client_node **prev;  // shao zuo yi bi pan duan
  struct client_node *next;
} client_node_t;
client_node_t *g_client = NULL;
void add_client(client_node_t *p) {
  p->next = g_client;
  p->prev = &g_client;
  g_client = p;
}
void remove_client(client_node_t *p) {
  *(p->prev) = p->next;
  if (p->next) {
    p->next->prev = p->prev;
  }
}
void log_message(FILE *fp, char *name, char *buf) {
  fp = fopen("./chatlog.log", "a");
  if (fp == NULL) {
    printf("fopen error");
    return;
  }

  pthread_mutex_lock(&MUTEX);
  fprintf(fp, "%s: %s\n", name, buf);
  pthread_mutex_unlock(&MUTEX);

  fclose(fp);
}

void *child_func(void *arg) {
  int nfp = (int)arg;
  int ret;
  char buf[256];
  client_node_t *cur;  // dang qian de client
  FILE *fp;

  bzero(buf, sizeof(buf));
  ret = recv(nfp, buf, sizeof(buf), 0);
  // xian shou yi ge yong hu ming
  if (ret <= 0) {
    printf("recv error!\n");
    close(nfp);
    return NULL;
  } else {
    printf("%s joined\n", buf);
  }

  cur = (client_node_t *)malloc(sizeof(client_node_t));
  cur->sock = nfp;
  strcpy(cur->name, buf);
  add_client(cur);
  while (1) {
    bzero(buf, sizeof(buf));
    ret = recv(nfp, buf, sizeof(buf), 0);
    if (ret <= 0) {
      printf("recv error.\n");
      break;
    } else if (ret == 0) {
      printf("recv null.\n");
      log_message(fp, cur->name, "NULL");
      fclose(fp);
    } else {
      printf("received: %s\n", buf);
      log_message(fp, cur->name, buf);
    }

    // send(nfp, "OK", 2 ,0);
    pthread_mutex_lock(&MUTEX);
    client_node_t *p = g_client;
    while (p) {
      if (p != cur) {
        char msg[256];
        sprintf(msg, "%s:%s\n", cur->name, buf);
        send(p->sock, msg, sizeof(msg), 0);
      }
      p = p->next;
    }
    pthread_mutex_unlock(&MUTEX);
    if (strcmp(buf, "over") == 0) break;
  }
  remove_client(cur);
  free(cur);
  close(nfp);
  return NULL;
}
int main() {
  int sfp, nfp;  // sfp:
  struct sockaddr_in saddr, caddr;
  int sin_size, ret;
  unsigned short port = 0x9999;
  pthread_t tid;

  printf("server start^^^^...\n");
  sfp = socket(AF_INET, SOCK_STREAM, 0);
  if (sfp == -1) {
    printf("socket error>>>.\n");
    return -1;
  }
  bzero(&saddr, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons(port);
  ret = bind(sfp, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
  // assert(ret != -1);
  if (ret == -1) {
    printf("bind  error!\n");
    close(sfp);  // shifang sfp
    return -1;
  }

  ret = listen(sfp, 5);  // jianting   5:dengdaiduiilie
  if (ret == -1) {
    printf("listen error.\n");
    close(sfp);
    return -1;
  }

  while (1) {
    printf("waiting connection...\n");
    sin_size = sizeof(struct sockaddr_in);
    nfp = accept(sfp, (struct sockaddr *)&caddr, &sin_size);
    if (nfp == -1) {
      printf("accept error.\n");
      close(sfp);
      return -1;
    }
    printf("aceept a connection.\n");
    pthread_create(&tid, NULL, &child_func, (void *)nfp);
  }
  close(sfp);
  pthread_mutex_destroy(&MUTEX);
  return 0;
}