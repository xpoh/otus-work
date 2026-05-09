#define pr_fmt(fmt) KBUILD_MODNAME ": kprobe: " fmt

#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/ktime.h>
#include <net/inet_sock.h>
#include <net/sock.h>

#include <kprobe_traffic.h>
#include <stats.h>

#define PROBE_SEND "tcp_sendmsg"
#define PROBE_RECV "tcp_cleanup_rbuf"

struct send_data {
  __be32 daddr;
  __be32 saddr;
};

struct recv_data {
  __be32 daddr;
  __be32 saddr;
  int copied;
};

static int send_entry_handler(struct kretprobe_instance *ri,
                              struct pt_regs *regs) {
  struct send_data *data;

  if (!current->mm)
    return 1;

  data = (struct send_data *)ri->data;
#ifdef CONFIG_X86
  {
    struct sock *sk = (struct sock *)regs->di;

    data->daddr = inet_sk(sk)->inet_daddr;
    data->saddr = inet_sk(sk)->inet_saddr;
  }
#else
  data->daddr = 0;
  data->saddr = 0;
#endif
  return 0;
}

NOKPROBE_SYMBOL(send_entry_handler);

static int send_ret_handler(struct kretprobe_instance *ri,
                            struct pt_regs *regs) {
  unsigned long retval = regs_return_value(regs);
  struct send_data *data = (struct send_data *)ri->data;

  if (retval > 0 && data->saddr && data->daddr)
    stats_record_traffic(data->saddr, data->daddr, true, (size_t)retval);

  return 0;
}

NOKPROBE_SYMBOL(send_ret_handler);

static int recv_entry_handler(struct kretprobe_instance *ri,
                              struct pt_regs *regs) {
  struct recv_data *data;

  if (!current->mm)
    return 1;

  data = (struct recv_data *)ri->data;
#ifdef CONFIG_X86
  {
    struct sock *sk = (struct sock *)regs->di;

    data->daddr = inet_sk(sk)->inet_daddr;
    data->saddr = inet_sk(sk)->inet_saddr;
    data->copied = (int)regs->si;
  }
#else
  data->daddr = 0;
  data->saddr = 0;
  data->copied = 0;
#endif
  return 0;
}

NOKPROBE_SYMBOL(recv_entry_handler);

static int recv_ret_handler(struct kretprobe_instance *ri,
                            struct pt_regs *regs) {
  struct recv_data *data = (struct recv_data *)ri->data;

  if (data->copied > 0 && data->saddr && data->daddr)
    stats_record_traffic(data->saddr, data->daddr, false,
                         (size_t)data->copied);

  return 0;
}

NOKPROBE_SYMBOL(recv_ret_handler);

static struct kretprobe kretp_send = {
    .handler = send_ret_handler,
    .entry_handler = send_entry_handler,
    .data_size = sizeof(struct send_data),
    .maxactive = 20,
};

static struct kretprobe kretp_recv = {
    .handler = recv_ret_handler,
    .entry_handler = recv_entry_handler,
    .data_size = sizeof(struct recv_data),
    .maxactive = 20,
};

int kprobe_traffic_register(void) {
  int ret;

  kretp_send.kp.symbol_name = PROBE_SEND;

  ret = register_kretprobe(&kretp_send);
  if (ret < 0) {
    pr_err("register_kretprobe(%s) failed: %d\n", PROBE_SEND, ret);

    return ret;
  }

  pr_info("planted kretprobe at %s: %p\n", PROBE_SEND, kretp_send.kp.addr);

  kretp_recv.kp.symbol_name = PROBE_RECV;

  ret = register_kretprobe(&kretp_recv);
  if (ret < 0) {
    pr_err("register_kretprobe(%s) failed: %d\n", PROBE_RECV, ret);
    unregister_kretprobe(&kretp_send);
    return ret;
  }

  pr_info("planted kretprobe at %s: %p\n", PROBE_RECV, kretp_recv.kp.addr);

  return 0;
}

void kprobe_traffic_unregister(void) {
  unregister_kretprobe(&kretp_send);
  unregister_kretprobe(&kretp_recv);
  pr_info("kretprobes unregistered\n");
}
