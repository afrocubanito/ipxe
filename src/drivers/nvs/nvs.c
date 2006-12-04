/*
 * Copyright (C) 2006 Michael Brown <mbrown@fensystems.co.uk>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdint.h>
#include <assert.h>
#include <gpxe/nvs.h>

/** @file
 *
 * Non-volatile storage
 *
 */

/**
 * Read from non-volatile storage device
 *
 * @v nvs		NVS device
 * @v address		Address from which to read
 * @v data		Data buffer
 * @v len		Length of data buffer
 * @ret rc		Return status code
 */
int nvs_read ( struct nvs_device *nvs, unsigned int address,
	       void *data, size_t len ) {
	size_t frag_len;
	int rc;

	/* We don't even attempt to handle buffer lengths that aren't
	 * an integral number of words.
	 */
	assert ( ( len & ( ( 1 << nvs->word_len_log2 ) - 1 ) ) == 0 );

	while ( len ) {

		/* Calculate space remaining up to next block boundary */
		frag_len = ( ( nvs->block_size -
			       ( address & ( nvs->block_size - 1 ) ) )
			     << nvs->word_len_log2 );

		/* Limit to space remaining in buffer */
		if ( frag_len > len )
			frag_len = len;

		/* Read this portion of the buffer from the device */
		if ( ( rc = nvs->read ( nvs, address, data, frag_len ) ) != 0 )
			return rc;

		/* Update parameters */
		data += frag_len;
		address += ( frag_len >> nvs->word_len_log2 );
		len -= frag_len;
	}

	return 0;
}
