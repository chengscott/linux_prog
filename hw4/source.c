#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(void) {
  printf("Enumerated network interfaces:\n");
  struct ifaddrs *ifap, *it;
  getifaddrs(&ifap);
  // MAC address
  char MAC[12][18];
  int index = 0;
  for (it = ifap; it; it = it->ifa_next) {
    unsigned char *mac = ((struct sockaddr_ll *)it->ifa_addr)->sll_addr;
    if (it->ifa_addr->sa_family == AF_INET && (it->ifa_flags & IFF_UP) == 1)
      index = if_nametoindex(it->ifa_name);
    if (it->ifa_addr->sa_family == AF_PACKET)
      sprintf(MAC[if_nametoindex(it->ifa_name)],
              "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3],
              mac[4], mac[5]);
  }
  // Network address
  for (it = ifap; it; it = it->ifa_next) {
    if (it->ifa_addr->sa_family == AF_INET) {
      char inet_addr[INET_ADDRSTRLEN], netmask_addr[INET_ADDRSTRLEN],
          masked_addr[INET_ADDRSTRLEN];
      struct in_addr *it_addr = &((struct sockaddr_in *)it->ifa_addr)->sin_addr,
                     *it_netmask =
                         &((struct sockaddr_in *)it->ifa_netmask)->sin_addr,
                     masked_in;
      inet_ntop(AF_INET, it_addr, inet_addr, INET_ADDRSTRLEN);
      inet_ntop(AF_INET, it_netmask, netmask_addr, INET_ADDRSTRLEN);
      masked_in.s_addr = it_addr->s_addr | (~it_netmask->s_addr);
      inet_ntop(AF_INET, &masked_in, masked_addr, INET_ADDRSTRLEN);
      unsigned int idx = if_nametoindex(it->ifa_name);
      printf("%u - %-8s %16s %#x (%s) %s\n", idx, it->ifa_name, inet_addr,
             ntohl(it_netmask->s_addr), masked_addr, MAC[idx]);
    }
  }
  freeifaddrs(ifap);
  // username
  srand(time(NULL));
  char username[48] = "[";
  sprintf(username, "%05d%05d[", rand() % 100000, rand() % 100000);
  printf("Enter your name: ");
  scanf("%s", username + 11);
  printf("Welcome, '%s'!\n", username + 11);
  struct ether_header *hdr;
  const size_t len_ether_header = sizeof(struct ether_header),
               user_len = strlen(username);
  const unsigned short protocol = htons(ETH_P_IP + 1);
  // child receive message
  unsigned int pid;
  if ((pid = fork()) == 0) {
    char recv_msg[4096];
    int recv_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    while (1) {
      recv(recv_sock, recv_msg, 4096, 0);
      hdr = (struct ether_header *)recv_msg;
      if (hdr->ether_type == protocol &&
          strncmp(username, recv_msg + len_ether_header, user_len) != 0) {
        unsigned char *mac = hdr->ether_shost;
        printf(">>> <%02x:%02x:%02x:%02x:%02x:%02x> %s", mac[0], mac[1], mac[2],
               mac[3], mac[4], mac[5], recv_msg + len_ether_header + 10);
      }
    }
    return 0;
  }
  // link layer address
  struct sockaddr_ll addr;
  addr.sll_family = AF_PACKET;
  addr.sll_protocol = ETH_P_IP + 1;
  addr.sll_halen = ETH_ALEN;
  addr.sll_ifindex = index;
  for (size_t i = 0; i < 6; ++i) addr.sll_addr[i] = 0xff;
  // ethernet header
  char send_msg[4096] = {};
  hdr = (struct ether_header *)send_msg;
  for (size_t i = 0; i < 6; ++i) {
    char m1 = MAC[index][i * 3], m2 = MAC[index][i * 3 + 1];
    int d1 = m1 >= '0' && m1 <= '9' ? m1 - '0' : m1 - 'a' + 10,
        d2 = m2 >= '0' && m2 <= '9' ? m2 - '0' : m2 - 'a' + 10;
    hdr->ether_shost[i] = d1 * 16 + d2;
    hdr->ether_dhost[i] = 0xff;
  }
  hdr->ether_type = htons(ETH_P_IP + 1);
  // send message
  size_t len = len_ether_header, msg_len = 0;
  for (size_t i = 0; i < user_len; ++i) send_msg[len++] = username[i];
  send_msg[len++] = ']';
  send_msg[len++] = ':';
  send_msg[len++] = ' ';
  int send_sock = socket(AF_PACKET, SOCK_RAW, protocol);
  while (fgets(send_msg + len, 4000, stdin)) {
    msg_len = strlen(send_msg + len) + 1;
    if (msg_len == 2) continue;
    send_msg[len + msg_len] = '\0';
    sendto(send_sock, send_msg, len + msg_len, 0, (struct sockaddr *)&addr,
           sizeof(struct sockaddr_ll));
  }
  kill(pid, SIGTERM);
  return 0;
}
