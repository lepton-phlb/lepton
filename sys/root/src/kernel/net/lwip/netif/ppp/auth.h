
#ifndef AUTH_H
#define AUTH_H

/***********************
*** PUBLIC FUNCTIONS ***
***********************/

/* we are starting to use the link */
void link_required (int);

/* we are finished with the link */
void link_terminated (int);

/* the LCP layer has left the Opened state */
void link_down (int);

/* the link is up; authenticate now */
void link_established (int);

/* a network protocol has come up */
void np_up (int, u16_t);

/* a network protocol has gone down */
void np_down (int, u16_t);

/* a network protocol no longer needs link */
void np_finished (int, u16_t);

/* peer failed to authenticate itself */
void auth_peer_fail (int, u16_t);

/* peer successfully authenticated itself */
void auth_peer_success (int, u16_t, char *, int);

/* we failed to authenticate ourselves */
void auth_withpeer_fail (int, u16_t);

/* we successfully authenticated ourselves */
void auth_withpeer_success (int, u16_t);

/* check authentication options supplied */
void auth_check_options (void);

/* check what secrets we have */
void auth_reset (int);

/* Check peer-supplied username/password */
int  check_passwd (int, char *, int, char *, int, char **, int *);

/* get "secret" for chap */
int  get_secret (int, char *, char *, char *, int *, int);

/* check if IP address is authorized */
int  auth_ip_addr (int, u32_t);

/* check if IP address is unreasonable */
int  bad_ip_adrs (u32_t);

#endif /* AUTH_H */
