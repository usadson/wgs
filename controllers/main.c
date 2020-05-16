/**
 * BSD-2-Clause
 *
 * Copyright (c) 2020 Tristan
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND  ANY  EXPRESS  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED  WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE  DISCLAIMED.  IN  NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE   FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
 * CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT  LIMITED  TO,  PROCUREMENT  OF
 * SUBSTITUTE  GOODS  OR  SERVICES;  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT,  STRICT  LIABILITY,  OR  TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING  IN  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/socket.h>

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main(void) {
	inquiry_info *info = NULL;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;

	char addr[19] = { 0 };
	char name[248] = { 0 };

	dev_id = hci_get_route(NULL);
	sock = hci_open_dev(dev_id);

	if (dev_id < 0 || sock < 0) {
		if (errno == ENODEV)
			err(EXIT_FAILURE, "No bluetooth adapter found");

		perror("opening socket");
		printf("errno is %i\n", errno);
		return EXIT_FAILURE;
	}

	len = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	info = (inquiry_info *) malloc(max_rsp * sizeof(inquiry_info));

	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &info, flags);
	if (num_rsp < 0) {
		perror("hci_inquiry");
		return EXIT_FAILURE;
	}

	for (i = 0; i < num_rsp; i++) {
		ba2str(&info[i].bdaddr, addr);
		memset(name, 0, sizeof(name));

		if (hci_read_remote_name(sock, &info[i].bdaddr, sizeof(name), name, 0)
			< 0) {
			strcpy(name, "[unknown]");
		}

		if (hci_read_remote_features(sock, uint16_t handle, uint8_t *features, int to) < 0) {
			
		}
		
		//int hci_read_remote_ext_features(int dd, uint16_t handle, uint8_t page, uint8_t *ma

		printf("%s %s\n", addr, name);
	}

	free(info);
	close(sock);

	return 0;
}
