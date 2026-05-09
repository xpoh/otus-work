#define pr_fmt(fmt) KBUILD_MODNAME ": kprobe: " fmt

#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/ktime.h>
#include <net/inet_sock.h>
#include <net/sock.h>

#include "../inc/kprobe_traffic.h"
#include "../inc/stats.h"

static char probe_send[KSYM_NAME_LEN] = "tcp_sendmsg";
module_param_string(probe_send, probe_send, KSYM_NAME_LEN, 0644);
MODULE_PARM_DESC(probe_send, "function to probe for sent bytes");

static char probe_recv[KSYM_NAME_LEN] = "tcp_cleanup_rbuf";
module_param_string(probe_recv, probe_recv, KSYM_NAME_LEN, 0644);
MODULE_PARM_DESC(probe_recv, "function to probe for received bytes");

struct send_data {
  __be32 daddr;
};

struct recv_data {
  __be32 daddr;
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
  }
#else
  data->daddr = 0;
#endif
  return 0;
}

NOKPROBE_SYMBOL(send_entry_handler);

static int send_ret_handler(struct kretprobe_instance *ri,
                            struct pt_regs *regs) {
  unsigned long retval = regs_return_value(regs);
  struct send_data *data = (struct send_data *)ri->data;

  if (retval > 0 && data->daddr)
    stats_record_traffic(data->daddr, (size_t)retval);

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
  }
#else
  data->daddr = 0;
#endif
  return 0;
}

NOKPROBE_SYMBOL(recv_entry_handler);

static int recv_ret_handler(struct kretprobe_instance *ri,
                            struct pt_regs *regs) {
  unsigned long retval = regs_return_value(regs);
  struct recv_data *data = (struct recv_data *)ri->data;

  if (retval > 0 && data->daddr)
    stats_record_traffic(data->daddr, (size_t)retval);

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

  kretp_send.kp.symbol_name = probe_send;
  ret = register_kretprobe(&kretp_send);
  if (ret < 0) {
    pr_err("register_kretprobe(%s) failed: %d\n", probe_send, ret);
    return ret;
  }

  pr_info("planted kretprobe at %s: %p\n", probe_send, kretp_send.kp.addr);

  kretp_recv.kp.symbol_name = probe_recv;

  ret = register_kretprobe(&kretp_recv);
  if (ret < 0) {
    pr_err("register_kretprobe(%s) failed: %d\n", probe_recv, ret);
    unregister_kretprobe(&kretp_send);
    return ret;
  }

  pr_info("planted kretprobe at %s: %p\n", probe_recv, kretp_recv.kp.addr);

  return 0;
}

void kprobe_traffic_unregister(void) {
  unregister_kretprobe(&kretp_send);
  unregister_kretprobe(&kretp_recv);
  pr_info("kretprobes unregistered\n");
}
