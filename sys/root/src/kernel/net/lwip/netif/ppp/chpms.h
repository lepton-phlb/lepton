
#ifndef CHPMS_H
#define CHPMS_H

#define MAX_NT_PASSWORD 256 /* Maximum number of (Unicode) chars in an NT password */

void ChapMS (chap_state *, char *, int, char *, int);

#endif /* CHPMS_H */
