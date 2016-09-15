     /*
      *
      *  Modified for AF_INET by Pedro Roque
      *
      *  <roque@di.fc.ul.pt>
      *
      *  Original copyright notice included bellow
      */
     
    /*
     * Copyright (c)  The Regents of the University of California.
     * All rights reserved.
     *
     * This code is derived from software contributed to Berkeley by
     * Mike Muuss.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * . Redistributions of source code must retain the above copyright
     *    notice, this list of conditions and the following disclaimer.
     * . Redistributions in binary form must reproduce the above copyright
     *    notice, this list of conditions and the following disclaimer in the
     *    documentation and/or other materials provided with the distribution.
     * . All advertising materials mentioning features or use of this software
     *    must display the following acknowledgement:
     *  This product includes software developed by the University of
     *  California, Berkeley and its contributors.
     * . Neither the name of the University nor the names of its contributors
     *    may be used to endorse or promote products derived from this software
     *    without specific prior written permission.
     *
     * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
     * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
     * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
     * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
     * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
     * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
     * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
     * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
     * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
     * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
     * SUCH DAMAGE.
     */
    
    #ifndef lint
    char copyright[] =
    "@(#) Copyright (c)  The Regents of the University of California.\n\
     All rights reserved.\n";
    #endif /* not lint */
    
    /*
     *          P I N G . C
     *
     * Using the InterNet Control Message Protocol (ICMP) "ECHO" facility,
     * measure round-trip-delays and packet loss across network paths.
     *
     * Author -
     *  Mike Muuss
     *  U. S. Army Ballistic Research Laboratory
     *  December, 
     *
     * Status -
     *  Public Domain.  Distribution Unlimited.
     * Bugs -
     *  More statistics could always be gathered.
     *  This program has to run SUID to ROOT to access the ICMP socket.
     */
    #include "ping_common.h"
    
    #include <linux/in.h>
    #include <linux/ipv.h>
    #include <linux/icmpv.h>
    
    #define BIT_CLEAR(nr, addr) do { ((__u *)(addr))[(nr) >> ] &= ~(U << ((nr) & )); } while()
    #define BIT_SET(nr, addr) do { ((__u *)(addr))[(nr) >> ] |= (U << ((nr) & )); } while()
    #define BIT_TEST(nr, addr) do { (__u *)(addr))[(nr) >> ] & (U << ((nr) & )); } while()
    
    #define ICMPV_FILTER_WILLPASS(type, filterp) \
        (BIT_TEST((type), filterp) == )
    
    #define ICMPV_FILTER_WILLBLOCK(type, filterp) \
        BIT_TEST((type), filterp)
    
    #define ICMPV_FILTER_SETPASS(type, filterp) \
        BIT_CLEAR((type), filterp)
    
    #define ICMPV_FILTER_SETBLOCK(type, filterp) \
        BIT_SET((type), filterp)
    
    #define ICMPV_FILTER_SETPASSALL(filterp) \
        memset(filterp, , sizeof(struct icmp_filter));
    
    #define ICMPV_FILTER_SETBLOCKALL(filterp) \
        memset(filterp, xFF, sizeof(struct icmp_filter));
    
    
    #define MAXPACKET         /* max packet size */
    
   #ifdef SO_TIMESTAMP
   #define HAVE_SIN_SCOPEID 
   #endif
   
   
   __u flowlabel;
   __u tclass;
   struct cmsghdr *srcrt;
   
   struct sockaddr_in whereto;    /* who to ping */
   u_char outpack[MAXPACKET];
   int maxpacket = sizeof(outpack);
   
   static unsigned char cmsgbuf[];
   static int cmsglen = ;
   
   static char * pr_addr(struct in_addr *addr);
   static char * pr_addr_n(struct in_addr *addr);
   static int pr_icmph(__u type, __u code, __u info);
   static void usage(void) __attribute((noreturn));
   
   struct sockaddr_in source;
   char *device;
   int pmtudisc=-;
   
   static int icmp_sock;
   
   
   
   static struct in_addr in_anyaddr;
   static __inline__ int ipv_addr_any(struct in_addr *addr)
   {
       return (memcmp(addr, &in_anyaddr, ) == );
   }
   
   size_t inet_srcrt_space(int type, int segments)
   {
       if (type !=  || segments > )
           return ;
   
       return (sizeof(struct cmsghdr) + sizeof(struct rt_hdr) +
           segments * sizeof(struct in_addr));
   }
   
   extern struct cmsghdr * inet_srcrt_init(void *bp, int type)
   {
       struct cmsghdr *cmsg;
   
       if (type)
           return NULL;
   
       memset(bp, , sizeof(struct cmsghdr) + sizeof(struct rt_hdr));
       cmsg = (struct cmsghdr *) bp;
   
       cmsg->cmsg_len = sizeof(struct cmsghdr) + sizeof(struct rt_hdr);
       cmsg->cmsg_level = SOL_IPV;
       cmsg->cmsg_type = IPV_RTHDR;
   
       return cmsg;
   }
   
   int inet_srcrt_add(struct cmsghdr *cmsg, const struct in_addr *addr)
   {
       struct rt_hdr *hdr;
       
       hdr = (struct rt_hdr *) CMSG_DATA(cmsg);
   
       cmsg->cmsg_len += sizeof(struct in_addr);
       hdr->rt_hdr.hdrlen += sizeof(struct in_addr) / ;
   
       memcpy(&hdr->addr[hdr->rt_hdr.segments_left++], addr,
              sizeof(struct in_addr));
           
       return ;
   }
   
   int main(int argc, char *argv[])
   {
       int ch, hold, packlen;
       u_char *packet;
       char *target;
       struct sockaddr_in firsthop;
       int socket_errno;
       struct icmp_filter filter;
       int err, csum_offset, sz_opt;
   
       icmp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMPV);
       socket_errno = errno;
   
       uid = getuid();
       setuid(uid);
   
       source.sin_family = AF_INET;
       memset(&firsthop, , sizeof(firsthop));
       firsthop.sin_family = AF_INET;
   
       preload = ;
       while ((ch = getopt(argc, argv, COMMON_OPTSTR "F:")) != EOF) {
           switch(ch) {
           case 'F':
               sscanf(optarg, "%x", &flowlabel);
               options |= F_FLOWINFO;
               break;
           case 'Q':
               sscanf(optarg, "%x", &tclass);
               options |= F_TCLASS;
               break;
           case 'I':
               if (strchr(optarg, ':')) {
                   if (inet_pton(AF_INET, optarg, (char*)&source.sin_addr) <= ) {
                       fprintf(stderr, "ping: invalid source address %s\n", optarg);
                       exit();
                   }
                   options |= F_STRICTSOURCE;
               } else {
                   device = optarg;
               }
               break;
           case 'M':
               if (strcmp(optarg, "do") == )
                   pmtudisc = IPV_PMTUDISC_DO;
               else if (strcmp(optarg, "dont") == )
                   pmtudisc = IPV_PMTUDISC_DONT;
               else if (strcmp(optarg, "want") == )
                   pmtudisc = IPV_PMTUDISC_WANT;
               else {
                   fprintf(stderr, "ping: wrong value for -M: do, dont, want are valid ones.\n");
                   exit();
               }
               break;
           case 'V':
               printf("ping utility, iputils-ss%s\n", SNAPSHOT);
               exit();
           COMMON_OPTIONS
               common_options(ch);
               break;
           default:
               usage();
           }
       }
       argc -= optind;
       argv += optind;
   
       while (argc > ) {
           struct in_addr addr;
   
           if (srcrt == NULL) {
               int space;
               
               space = inet_srcrt_space(IPV_SRCRT_TYPE_, argc - );
   
               if (space == ) {
                   fprintf(stderr, "srcrt_space failed\n");
                   exit();
               }
               if (space + cmsglen > sizeof(cmsgbuf)) {
                   fprintf(stderr, "no room for options\n");
                   exit();
               }
   
               srcrt = (struct cmsghdr*)(cmsgbuf+cmsglen);
               cmsglen += CMSG_ALIGN(space);
               inet_srcrt_init(srcrt, IPV_SRCRT_TYPE_);
           }
   
           target = *argv;
   
           if (inet_pton(AF_INET, target, &addr) <= ) {
               struct hostent *hp;
   
               hp = gethostbyname(target, AF_INET);
   
               if (hp == NULL) {
                   fprintf(stderr, "unknown host %s\n", target);
                   exit();
               }
   
               memcpy(&addr, hp->h_addr_list[], );
           }
   
           inet_srcrt_add(srcrt, &addr);
           if (ipv_addr_any(&firsthop.sin_addr))
               memcpy(&firsthop.sin_addr, &addr, );
   
           argv++;
           argc--;
       }
   
       if (argc != )
           usage();
       target = *argv;
   
       memset(&whereto, , sizeof(struct sockaddr_in));
       whereto.sin_family = AF_INET;
       whereto.sin_port = htons(IPPROTO_ICMPV);
   
       if (inet_pton(AF_INET, target, &whereto.sin_addr) <= ) {
           struct hostent *hp;
   
           hp = gethostbyname(target, AF_INET);
   
           if (hp == NULL) {
               fprintf(stderr, "unknown host\n");
               exit();
           }
           
           memcpy(&whereto.sin_addr, hp->h_addr_list[], );
       } else {
           options |= F_NUMERIC;
       }
       if (ipv_addr_any(&firsthop.sin_addr))
           memcpy(&firsthop.sin_addr, &whereto.sin_addr, );
   
       hostname = target;
   
       if (ipv_addr_any(&source.sin_addr)) {
           int alen;
           int probe_fd = socket(AF_INET, SOCK_DGRAM, );
   
           if (probe_fd < ) {
               perror("socket");
               exit();
           }
           if (device) {
               struct ifreq ifr;
               memset(&ifr, , sizeof(ifr));
               strncpy(ifr.ifr_name, device, IFNAMSIZ-);
               if (setsockopt(probe_fd, SOL_SOCKET, SO_BINDTODEVICE, device, strlen(device)+) == -) {
   #ifdef HAVE_SIN_SCOPEID
                   if ((firsthop.sin_addr.s_addr[]&htons(xffc)) == htons (xfe) ||
                       (firsthop.sin_addr.s_addr[]&htons(xffff)) == htons (xff)) {
                       if (ioctl(probe_fd, SIOCGIFINDEX, &ifr) < ) {
                           fprintf(stderr, "ping: unknown iface %s\n", device);
                           exit();
                       }
                       firsthop.sin_scope_id = ifr.ifr_ifindex;
                   }
   #endif
               }
           }
           firsthop.sin_port = htons();
           if (connect(probe_fd, (struct sockaddr*)&firsthop, sizeof(firsthop)) == -) {
               perror("connect");
               exit();
           }
           alen = sizeof(source);
           if (getsockname(probe_fd, (struct sockaddr*)&source, &alen) == -) {
               perror("getsockname");
               exit();
           }
           source.sin_port = ;
           close(probe_fd);
       }
   
       if (icmp_sock < ) {
           errno = socket_errno;
           perror("ping: icmp open socket");
           exit();
       }
   
       if (device) {
           struct ifreq ifr;
           struct cmsghdr *cmsg;
           struct in_pktinfo *ipi;
                   
           memset(&ifr, , sizeof(ifr));
           strncpy(ifr.ifr_name, device, IFNAMSIZ-);
           if (ioctl(icmp_sock, SIOCGIFINDEX, &ifr) < ) {
               fprintf(stderr, "ping: unknown iface %s\n", device);
               exit();
           }
           cmsg = (struct cmsghdr*)cmsgbuf;
           cmsglen += CMSG_SPACE(sizeof(*ipi));
           cmsg->cmsg_len = CMSG_LEN(sizeof(*ipi));
           cmsg->cmsg_level = SOL_IPV;
           cmsg->cmsg_type = IPV_PKTINFO;
                   
           ipi = (struct in_pktinfo*)CMSG_DATA(cmsg);
           memset(ipi, , sizeof(*ipi));
           ipi->ipi_ifindex = ifr.ifr_ifindex;
       }
   
       if ((whereto.sin_addr.s_addr[]&htons(xff)) == htons (xff)) {
           if (uid) {
               if (interval < ) {
                   fprintf(stderr, "ping: multicast ping with too short interval.\n");
                   exit();
               }
               if (pmtudisc >=  && pmtudisc != IPV_PMTUDISC_DO) {
                   fprintf(stderr, "ping: multicast ping does not fragment.\n");
                   exit();
               }
           }
           if (pmtudisc < )
               pmtudisc = IPV_PMTUDISC_DO;
       }
   
       if (pmtudisc >= ) {
           if (setsockopt(icmp_sock, SOL_IPV, IPV_MTU_DISCOVER, &pmtudisc, sizeof(pmtudisc)) == -) {
               perror("ping: IPV_MTU_DISCOVER");
               exit();
           }
       }
   
       if ((options&F_STRICTSOURCE) &&
           bind(icmp_sock, (struct sockaddr*)&source, sizeof(source)) == -) {
           perror("ping: bind icmp socket");
           exit();
       }
   
       if (datalen >= sizeof(struct timeval))  /* can we time transfer */
           timing = ;
       packlen = datalen +  +  +  + ; /*  for rthdr */
       if (!(packet = (u_char *)malloc((u_int)packlen))) {
           fprintf(stderr, "ping: out of memory.\n");
           exit();
       }
   
       working_recverr = ;
       hold = ;
       if (setsockopt(icmp_sock, SOL_IPV, IPV_RECVERR, (char *)&hold, sizeof(hold))) {
           fprintf(stderr, "WARNING: your kernel is veeery old. No problems.\n");
           working_recverr = ;
       }
   
       /* Estimate memory eaten by single packet. It is rough estimate.
        * Actually, for small datalen's it depends on kernel side a lot. */
       hold = datalen+;
       hold += ((hold+)/)*(+++);
       sock_setbufs(icmp_sock, hold);
   
       csum_offset = ;
       sz_opt = sizeof(int);
   
       err = setsockopt(icmp_sock, SOL_RAW, IPV_CHECKSUM, &csum_offset, sz_opt);
       if (err < ) {
           perror("setsockopt(RAW_CHECKSUM)");
           exit();
       }
   
       /*
        *  select icmp echo reply as icmp type to receive
        */
   
       ICMPV_FILTER_SETBLOCKALL(&filter);
   
       if (!working_recverr) {
           ICMPV_FILTER_SETPASS(ICMPV_DEST_UNREACH, &filter);
           ICMPV_FILTER_SETPASS(ICMPV_PKT_TOOBIG, &filter);
           ICMPV_FILTER_SETPASS(ICMPV_TIME_EXCEED, &filter);
           ICMPV_FILTER_SETPASS(ICMPV_PARAMPROB, &filter);
       }
   
       ICMPV_FILTER_SETPASS(ICMPV_ECHO_REPLY, &filter);
   
       err = setsockopt(icmp_sock, SOL_ICMPV, ICMPV_FILTER, &filter,
                sizeof(struct icmp_filter));
   
       if (err < ) {
           perror("setsockopt(ICMPV_FILTER)");
           exit();
       }
   
       if (options & F_NOLOOP) {
           int loop = ;
           if (setsockopt(icmp_sock, IPPROTO_IPV, IPV_MULTICAST_LOOP,
                               &loop, sizeof(loop)) == -) {
               perror ("can't disable multicast loopback");
               exit();
           }
       }
       if (options & F_TTL) {
           if (setsockopt(icmp_sock, IPPROTO_IPV, IPV_MULTICAST_HOPS,
                      &ttl, sizeof(ttl)) == -) {
               perror ("can't set multicast hop limit");
               exit();
           }
           if (setsockopt(icmp_sock, IPPROTO_IPV, IPV_UNICAST_HOPS,
                      &ttl, sizeof(ttl)) == -) {
               perror ("can't set unicast hop limit");
               exit();
           }
       }
   
       if () {
           int on = ;
           if (setsockopt(icmp_sock, IPPROTO_IPV, IPV_HOPLIMIT,
                      &on, sizeof(on)) == -) {
               perror ("can't receive hop limit");
               exit();
           }
       }
   
       if (options&F_FLOWINFO) {
   #ifdef IPV_FLOWLABEL_MGR
           char freq_buf[CMSG_ALIGN(sizeof(struct in_flowlabel_req)) + cmsglen];
           struct in_flowlabel_req *freq = (struct in_flowlabel_req *)freq_buf;
           int freq_len = sizeof(*freq);
           if (srcrt)
               freq_len = CMSG_ALIGN(sizeof(*freq)) + srcrt->cmsg_len;
           memset(freq, , sizeof(*freq));
           freq->flr_label = htonl(flowlabel&xFFFFF);
           freq->flr_action = IPV_FL_A_GET;
           freq->flr_flags = IPV_FL_F_CREATE;
           freq->flr_share = IPV_FL_S_EXCL;
           memcpy(&freq->flr_dst, &whereto.sin_addr, );
           if (srcrt)
               memcpy(freq_buf + CMSG_ALIGN(sizeof(*freq)), srcrt, srcrt->cmsg_len);
           if (setsockopt(icmp_sock, IPPROTO_IPV, IPV_FLOWLABEL_MGR,
                      freq, freq_len) == -) {
               perror ("can't set flowlabel");
               exit();
           }
           flowlabel = freq->flr_label;
           if (srcrt) {
               cmsglen = (char*)srcrt - (char*)cmsgbuf;
               srcrt = NULL;
           }
   #else
           fprintf(stderr, "Flow labels are not supported.\n");
           exit();
   #endif
       }
       if (options&(F_FLOWINFO|F_TCLASS)) {
   #ifdef IPV_FLOWINFO_SEND
           int on = ;
           whereto.sin_flowinfo = flowlabel | htonl((tclass&xFF)<<);
           if (setsockopt(icmp_sock, IPPROTO_IPV, IPV_FLOWINFO_SEND,
                      &on, sizeof(on)) == -) {
               perror ("can't send flowinfo");
               exit();
           }
   #else
           fprintf(stderr, "Flowinfo is not supported.\n");
           exit();
   #endif
       }
   
       printf("PING %s(%s) ", hostname, pr_addr(&whereto.sin_addr));
       if (flowlabel)
           printf(", flow x%x, ", (unsigned)ntohl(flowlabel));
       if (device || (options&F_STRICTSOURCE)) {
           printf("from %s %s: ",
                  pr_addr_n(&source.sin_addr), device ? : "");
       }
       printf("%d data bytes\n", datalen);
   
       setup(icmp_sock);
   
       main_loop(icmp_sock, packet, packlen);
   }
   
   int receive_error_msg()
   {
       int res;
       char cbuf[];
       struct iovec  iov;
       struct msghdr msg;
       struct cmsghdr *cmsg;
       struct sock_extended_err *e;
       struct icmphdr icmph;
       struct sockaddr_in target;
       int net_errors = ;
       int local_errors = ;
       int saved_errno = errno;
   
       iov.iov_base = &icmph;
       iov.iov_len = sizeof(icmph);
       msg.msg_name = (void*)&target;
       msg.msg_namelen = sizeof(target);
       msg.msg_iov = &iov;
       msg.msg_iovlen = ;
       msg.msg_flags = ;
       msg.msg_control = cbuf;
       msg.msg_controllen = sizeof(cbuf);
   
       res = recvmsg(icmp_sock, &msg, MSG_ERRQUEUE|MSG_DONTWAIT);
       if (res < )
           goto out;
   
       e = NULL;
       for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
           if (cmsg->cmsg_level == SOL_IPV) {
               if (cmsg->cmsg_type == IPV_RECVERR)
                   e = (struct sock_extended_err *)CMSG_DATA(cmsg);
           }
       }
       if (e == NULL)
           abort();
   
       if (e->ee_origin == SO_EE_ORIGIN_LOCAL) {
           local_errors++;
           if (options & F_QUIET)
               goto out;
           if (options & F_FLOOD)
               write(STDOUT_FILENO, "E", );
           else if (e->ee_errno != EMSGSIZE)
               fprintf(stderr, "ping: local error: %s\n", strerror(e->ee_errno));
           else
               fprintf(stderr, "ping: local error: Message too long, mtu=%u\n", e->ee_info);
           nerrors++;
       } else if (e->ee_origin == SO_EE_ORIGIN_ICMP) {
           struct sockaddr_in *sin = (struct sockaddr_in*)(e+);
   
           if (res < sizeof(icmph) ||
               memcmp(&target.sin_addr, &whereto.sin_addr, ) ||
               icmph.icmp_type != ICMPV_ECHO_REQUEST ||
               icmph.icmp_identifier != ident) {
               /* Not our error, not an error at all. Clear. */
               saved_errno = ;
               goto out;
           }
   
           net_errors++;
           nerrors++;
           if (options & F_QUIET)
               goto out;
           if (options & F_FLOOD) {
               write(STDOUT_FILENO, "\bE", );
           } else {
               printf("From %s icmp_seq=%u ", pr_addr(&sin->sin_addr), ntohs(icmph.icmp_sequence));
               pr_icmph(e->ee_type, e->ee_code, e->ee_info);
               putchar('\n');
               fflush(stdout);
           }
       }
   
   out:
       errno = saved_errno;
       return net_errors ? : -local_errors;
   }
   
   /*
    * pinger --
    *  Compose and transmit an ICMP ECHO REQUEST packet.  The IP packet
    * will be added on by the kernel.  The ID field is our UNIX process ID,
    * and the sequence number is an ascending integer.  The first  bytes
    * of the data portion are used to hold a UNIX "timeval" struct in VAX
    * byte-order, to compute the round-trip time.
    */
   int send_probe(void)
   {
       struct icmphdr *icmph;
       int cc;
       int i;
   
       icmph = (struct icmphdr *)outpack;
       icmph->icmp_type = ICMPV_ECHO_REQUEST;
       icmph->icmp_code = ;
       icmph->icmp_cksum = ;
       icmph->icmp_sequence = htons(ntransmitted+);
       icmph->icmp_identifier = ident;
   
       CLR((ntransmitted+) % mx_dup_ck);
   
       if (timing)
           gettimeofday((struct timeval *)&outpack[],
               (struct timezone *)NULL);
   
       cc = datalen + ;           /* skips ICMP portion */
   
       if (cmsglen == ) {
           i = sendto(icmp_sock, (char *)outpack, cc, confirm,
                  (struct sockaddr *) &whereto,
                  sizeof(struct sockaddr_in));
       } else {
           struct msghdr mhdr;
           struct iovec iov;
   
           iov.iov_len  = cc;
           iov.iov_base = outpack;
   
           mhdr.msg_name = &whereto;
           mhdr.msg_namelen = sizeof(struct sockaddr_in);
           mhdr.msg_iov = &iov;
           mhdr.msg_iovlen = ;
           mhdr.msg_control = cmsgbuf;
           mhdr.msg_controllen = cmsglen;
   
           i = sendmsg(icmp_sock, &mhdr, confirm);
       }
       confirm = ;
   
       return (cc == i ?  : i);
   }
   
   /*
    * parse_reply --
    *  Print out the packet, if it came from us.  This logic is necessary
    * because ALL readers of the ICMP socket get a copy of ALL ICMP packets
    * which arrive ('tis only fair).  This permits multiple copies of this
    * program to be run without having intermingled output (or statistics!).
    */
   int
   parse_reply(struct msghdr *msg, int cc, void *addr, struct timeval *tv)
   {
       struct sockaddr_in *from = addr;
       __u *buf = msg->msg_iov->iov_base;
       struct cmsghdr *c;
       struct icmphdr *icmph;
       int hops = -;
   
       for (c = CMSG_FIRSTHDR(msg); c; c = CMSG_NXTHDR(msg, c)) {
           if (c->cmsg_level != SOL_IPV ||
               c->cmsg_type != IPV_HOPLIMIT)
               continue;
           if (c->cmsg_len < CMSG_LEN(sizeof(int)))
               continue;
           hops = *(int*)CMSG_DATA(c);
       }
   
   
       /* Now the ICMP part */
   
       icmph = (struct icmphdr *) buf;
       if (cc < ) {
           if (options & F_VERBOSE)
               fprintf(stderr, "ping: packet too short (%d bytes)\n", cc);
           return ;
       }
   
       if (icmph->icmp_type == ICMPV_ECHO_REPLY) {
           if (icmph->icmp_identifier != ident)
               return ;
           if (gather_statistics((__u*)(icmph+), cc,
                         ntohs(icmph->icmp_sequence),
                         hops, , tv, pr_addr(&from->sin_addr)))
               return ;
       } else {
           int nexthdr;
           struct ipvhdr *iph = (struct ipvhdr*)(icmph+);
           struct icmphdr *icmph = (struct icmphdr *)(iph+);
   
           /* We must not ever fall here. All the messages but
            * echo reply are blocked by filter and error are
            * received with IPV_RECVERR. Ugly code is preserved
            * however, just to remember what crap we avoided
            * using RECVRERR. :-)
            */
   
           if (cc < +sizeof(struct ipvhdr)+)
               return ;
   
           if (memcmp(&iph->daddr, &whereto.sin_addr, ))
               return ;
   
           nexthdr = iph->nexthdr;
   
           if (nexthdr == ) {
               nexthdr = *(__u*)icmph;
               icmph++;
           }
           if (nexthdr == IPPROTO_ICMPV) {
               if (icmph->icmp_type != ICMPV_ECHO_REQUEST ||
                   icmph->icmp_identifier != ident)
                   return ;
               acknowledge(ntohs(icmph->icmp_sequence));
               if (working_recverr)
                   return ;
               nerrors++;
               if (options & F_FLOOD) {
                   write(STDOUT_FILENO, "\bE", );
                   return ;
               }
               printf("From %s: icmp_seq=%u ", pr_addr(&from->sin_addr), ntohs(icmph->icmp_sequence));
           } else {
               /* We've got something other than an ECHOREPLY */
               if (!(options & F_VERBOSE) || uid)
                   return ;
               printf("From %s: ", pr_addr(&from->sin_addr));
           }
           pr_icmph(icmph->icmp_type, icmph->icmp_code, ntohl(icmph->icmp_mtu));
       }
   
       if (!(options & F_FLOOD)) {
           if (options & F_AUDIBLE)
               putchar('\a');
           putchar('\n');
           fflush(stdout);
       }
       return ;
   }
   
   
   int pr_icmph(__u type, __u code, __u info)
   {
       switch(type) {
       case ICMPV_DEST_UNREACH:
           printf("Destination unreachable: ");
           switch (code) {
           case ICMPV_NOROUTE:
               printf("No route");
               break;
           case ICMPV_ADM_PROHIBITED:
               printf("Administratively prohibited");
               break;
           case ICMPV_NOT_NEIGHBOUR:
               printf("Not neighbour");
               break;
           case ICMPV_ADDR_UNREACH:
               printf("Address unreachable");
               break;
           case ICMPV_PORT_UNREACH:
               printf("Port unreachable");
               break;
           default:    
               printf("Unknown code %d", code);
               break;
           }
           break;
       case ICMPV_PKT_TOOBIG:
           printf("Packet too big: mtu=%u", info);
           if (code)
               printf(", code=%d", code);
           break;
       case ICMPV_TIME_EXCEED:
           printf("Time exceeded: ");
           if (code == ICMPV_EXC_HOPLIMIT)
               printf("Hop limit");
           else if (code == ICMPV_EXC_FRAGTIME)
               printf("Defragmentation failure");
           else
               printf("code %d", code);
           break;
       case ICMPV_PARAMPROB:
           printf("Parameter problem: ");
           if (code == ICMPV_HDR_FIELD)
               printf("Wrong header field ");
           else if (code == ICMPV_UNK_NEXTHDR)
               printf("Unknown header ");
           else if (code == ICMPV_UNK_OPTION)
               printf("Unknown option ");
           else
               printf("code %d ", code);
           printf ("at %u", info);
           break;
       case ICMPV_ECHO_REQUEST:
           printf("Echo request");
           break;
       case ICMPV_ECHO_REPLY:
           printf("Echo reply");
           break;
       case ICMPV_MGM_QUERY:
           printf("MLD Query");
           break;
       case ICMPV_MGM_REPORT:
           printf("MLD Report");
           break;
       case ICMPV_MGM_REDUCTION:
           printf("MLD Reduction");
           break;
       default:
           printf("unknown icmp type");
           
       }
       return ;
   }
   
   #include <linux/filter.h>
   
   void install_filter(void)
   {
       static int once;
       static struct sock_filter insns[] = {
           BPF_STMT(BPF_LD|BPF_H|BPF_ABS, ),  /* Load icmp echo ident */
           BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, xAAAA, , ),  /* Ours? */
           BPF_STMT(BPF_RET|BPF_K, ~U),  /* Yes, it passes. */
           BPF_STMT(BPF_LD|BPF_B|BPF_ABS, ),  /* Load icmp type */
           BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, ICMPV_ECHO_REPLY, , ), /* Echo? */
           BPF_STMT(BPF_RET|BPF_K, ~U), /* No. It passes. This must not happen. */
           BPF_STMT(BPF_RET|BPF_K, ), /* Echo with wrong ident. Reject. */
       };
       static struct sock_fprog filter = {
           sizeof insns / sizeof(insns[]),
           insns
       };
   
       if (once)
           return;
       once = ;
   
       /* Patch bpflet for current identifier. */
       insns[] = (struct sock_filter)BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, __constant_htons(ident), , );
   
       if (setsockopt(icmp_sock, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter)))
           perror("WARNING: failed to install socket filter\n");
   }
   
   
   /*
    * pr_addr --
    *  Return an ascii host address as a dotted quad and optionally with
    * a hostname.
    */
   char * pr_addr(struct in_addr *addr)
   {
       struct hostent *hp = NULL;
   
       if (!(options&F_NUMERIC))
           hp = gethostbyaddr((__u*)addr, sizeof(struct in_addr), AF_INET);
   
       return hp ? hp->h_name : pr_addr_n(addr);
   }
   
   char * pr_addr_n(struct in_addr *addr)
   {
       static char str[];
       inet_ntop(AF_INET, addr, str, sizeof(str));
       return str;
   }
   
   void usage(void)
   {
       fprintf(stderr,
   "Usage: ping [-LUdfnqrvVaA] [-c count] [-i interval] [-w deadline]\n"
   "             [-p pattern] [-s packetsize] [-t ttl] [-I interface]\n"
   "             [-M mtu discovery hint] [-S sndbuf]\n"
   "             [-F flow label] [-Q traffic class] [hop ...] destination\n");
       exit();
   }

