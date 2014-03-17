
#ifndef MAGIC_H
#define MAGIC_H

/*****************************************************************************
************************** PUBLIC FUNCTIONS **********************************
*****************************************************************************/

/* Initialize the magic number generator */
void  magicInit(void);

/* Returns the next magic number */
u32_t magic(void);

#endif /* MAGIC_H */
