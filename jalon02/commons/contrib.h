/*
 * contrib.h
 *
 *  Created on: Sep 26, 2014
 *      Author: nicolas
 */

#ifndef CONTRIB_H_
#define CONTRIB_H_

#include <unistd.h>

/**
 * function to read a line on a fd
 */
ssize_t
readLine(int fd, void *buffer, size_t n);

#endif /* CONTRIB_H_ */
