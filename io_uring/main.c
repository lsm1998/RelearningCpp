#include <arpa/inet.h>
#include <fcntl.h>
#include <liburing.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define QUEUE_DEPTH 256
#define MAX_CONNS 1024
#define BUFFER_SIZE 4096

// 连接状态
enum conn_state {
  STATE_READ, // 等待读数据
  STATE_WRITE // 等待写数据（回显）
};

// 每个客户端连接对应的数据结构
struct conn {
  int fd;
  char buf[BUFFER_SIZE];
  int buf_len; // 当前缓冲区有效数据长度
  enum conn_state state;
  int need_close; // 标记是否需要关闭
};

static struct conn conns[MAX_CONNS];
static struct io_uring ring;

// 根据 fd 找到对应的 conn 指针（简单线性搜索，并发不高时足够）
static struct conn *find_conn_by_fd(int fd) {
  for (int i = 0; i < MAX_CONNS; i++) {
    if (conns[i].fd == fd && !conns[i].need_close)
      return &conns[i];
  }
  return NULL;
}

// 分配一个新的连接结构体
static struct conn *alloc_conn(int fd) {
  for (int i = 0; i < MAX_CONNS; i++) {
    if (conns[i].fd == -1) {
      conns[i].fd = fd;
      conns[i].buf_len = 0;
      conns[i].state = STATE_READ;
      conns[i].need_close = 0;
      return &conns[i];
    }
  }
  return NULL;
}

// 释放连接
static void free_conn(struct conn *c) {
  if (c->fd != -1) {
    close(c->fd);
    c->fd = -1;
  }
  c->need_close = 0;
}

// 提交一个 accept 请求
static void submit_accept(int listen_fd) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    fprintf(stderr, "Could not get SQE for accept\n");
    return;
  }
  io_uring_prep_accept(sqe, listen_fd, NULL, NULL, 0);
  // 设置 user_data 为特殊值（例如 NULL 表示 accept 完成）
  io_uring_sqe_set_data(sqe, NULL);
}

// 提交一个读请求
static void submit_read(struct conn *c) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    fprintf(stderr, "Could not get SQE for read\n");
    return;
  }
  io_uring_prep_recv(sqe, c->fd, c->buf, BUFFER_SIZE, 0);
  io_uring_sqe_set_data(sqe, c);
  c->state = STATE_READ;
}

// 提交一个写请求（回显）
static void submit_write(struct conn *c) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    fprintf(stderr, "Could not get SQE for write\n");
    return;
  }
  io_uring_prep_send(sqe, c->fd, c->buf, c->buf_len, 0);
  io_uring_sqe_set_data(sqe, c);
  c->state = STATE_WRITE;
}

int main() {
  int listen_fd;
  struct sockaddr_in addr;
  int ret;

  // 1. 初始化所有连接为 -1
  for (int i = 0; i < MAX_CONNS; i++) {
    conns[i].fd = -1;
  }

  // 2. 创建监听 socket
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    perror("socket");
    exit(1);
  }

  int reuse = 1;
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(listen_fd, 128) < 0) {
    perror("listen");
    exit(1);
  }

  printf("Echo server listening on port %d\n", PORT);

  // 3. 初始化 io_uring
  ret = io_uring_queue_init(QUEUE_DEPTH, &ring, 0);
  if (ret) {
    fprintf(stderr, "Unable to setup io_uring: %s\n", strerror(-ret));
    exit(1);
  }

  // 4. 提交第一个 accept 请求
  submit_accept(listen_fd);

  // 5. 事件主循环
  while (1) {
    // 等待至少一个完成事件
    struct io_uring_cqe *cqe;
    ret = io_uring_wait_cqe(&ring, &cqe);
    if (ret < 0) {
      fprintf(stderr, "io_uring_wait_cqe error: %s\n", strerror(-ret));
      break;
    }

    // 处理完成事件
    struct conn *c = (struct conn *)io_uring_cqe_get_data(cqe);
    int res = cqe->res; // 操作结果（类似于系统调用返回值）

    if (c == NULL) {
      // 这是 accept 完成事件
      if (res >= 0) {
        int client_fd = res;
        printf("New connection from fd %d\n", client_fd);
        struct conn *newc = alloc_conn(client_fd);
        if (!newc) {
          fprintf(stderr, "Too many connections, closing %d\n", client_fd);
          close(client_fd);
        } else {
          // 为该客户端提交第一个读请求
          submit_read(newc);
        }
      } else {
        fprintf(stderr, "accept error: %s\n", strerror(-res));
      }
      // 无论成功与否，都重新提交 accept 以接受更多连接
      submit_accept(listen_fd);
    } else {
      // 这是某个连接的读或写完成事件
      if (res < 0) {
        // 错误发生，关闭连接
        fprintf(stderr, "Operation error on fd %d: %s\n", c->fd,
                strerror(-res));
        free_conn(c);
        continue;
      }

      if (c->state == STATE_READ) {
        // 读完成
        if (res == 0) {
          // 客户端关闭连接
          printf("Client fd %d closed\n", c->fd);
          free_conn(c);
        } else {
          // 成功读取到数据
          c->buf_len = res;
          // 回显：立即提交写请求
          submit_write(c);
        }
      } else if (c->state == STATE_WRITE) {
        // 写完成（数据已回显给客户端）
        if (res != c->buf_len) {
          // 理想情况应处理部分发送，这里简单忽略并关闭
          fprintf(stderr, "Partial send on fd %d, closing\n", c->fd);
          free_conn(c);
        } else {
          // 写完成后，继续读下一个数据包
          submit_read(c);
        }
      } else {
        fprintf(stderr, "Invalid state\n");
        free_conn(c);
      }
    }

    // 标记完成队列项已处理
    io_uring_cqe_seen(&ring, cqe);
  }

  close(listen_fd);
  io_uring_queue_exit(&ring);
  return 0;
}