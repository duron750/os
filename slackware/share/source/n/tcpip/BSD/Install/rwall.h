/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _RWALL_H_RPCGEN
#define _RWALL_H_RPCGEN

#include <rpc/rpc.h>


#define WALLPROG ((u_long)100008)
#define WALLVERS ((u_long)1)

#ifdef __cplusplus
#define WALLPROC_WALL ((u_long)2)
extern "C" void * wallproc_wall_1(char **, CLIENT *);
extern "C" void * wallproc_wall_1_svc(char **, struct svc_req *);

#elif __STDC__
#define WALLPROC_WALL ((u_long)2)
extern  void * wallproc_wall_1(char **, CLIENT *);
extern  void * wallproc_wall_1_svc(char **, struct svc_req *);

#else /* Old Style C */ 
#define WALLPROC_WALL ((u_long)2)
extern  void * wallproc_wall_1();
extern  void * wallproc_wall_1_svc();
#endif /* Old Style C */ 

#endif /* !_RWALL_H_RPCGEN */
