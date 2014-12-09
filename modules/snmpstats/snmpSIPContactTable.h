/*
 * $Id$
 *
 * SNMPStats Module 
 * Copyright (C) 2006 SOMA Networks, INC.
 * Written by: Jeffrey Magder (jmagder@somanetworks.com)
 *
 * This file is part of Kamailio, a free SIP server.
 *
 * Kamailio is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * Kamailio is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 * USA
 *
 * History:
 * --------
 * 2006-11-23 initial version (jmagder)
 * 2007-02-16 Moved all OID registrations from the experimental branch to 
 *            Kamailio's IANA assigned enterprise branch. (jmagder)
 * 
 * Note: this file was originally auto-generated by mib2c using
 * mib2c.array-user.conf
 *
 * This file contains the implementation of the kamailioSIPContact Table.  For a
 * full description of this structure, please see the KAMAILIO-SIP-SERVER-MIB.
 *
 * Some important notes on implementation follow:
 *
 * We require Kamailios usrloc module to inform us when a contact is
 * added/removed.  The general callback process works as follows:
 *
 * 1) On startup, we register handleContactCallbacks() for USRLOC callbacks, so 
 *    we can be informed whenever a contact is added/removed from the system.  
 *    This registration happens with a call to registerForUSRLOCCallbacks().  
 *    (This is actually called when the SNMPStats module is initialized)
 *
 * 2) Whenever we receive a contact callback, handleContactCallbacks() will 
 *    quickly add the contact information and operation type to the
 *    interprocess buffer.  
 *
 * 3) When we receive an SNMP request for user/contact information, we consume
 *    the interprocess buffer with consumeInterprocessBuffer().  The function
 *    will add/delete rows to the tables, and then service the SNMP request.
 *
 * Notes: 
 *
 * - The interprocess buffer was necessary, because NetSNMP's containers can be
 *   very inefficient at adding large amounts of data at a time, such as when
 *   Kamailio first starts up.  It was decided its better to make an SNMP manager
 *   wait for data, instead of slowing down the rest of Kamailio while the
 *   sub-agent processes the data. 
 *
 * - It is important to send periodic SNMP requests to this table (or the user
 *   table), to make sure the process buffer doesn't get too large.  
 */

#ifndef KAMAILIOSIPCONTACTTABLE_H
#define KAMAILIOSIPCONTACTTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

#include "interprocess_buffer.h"

#include "../usrloc/ucontact.h"
#include "../../config.h"

/* This strucutre is actually quite different from what was generated by mib2c.
 * Specifically, the scalars kamailioSIPContactURI, kamailioSIPContactExpiry, and
 * kamailioSIPContactPreference have been removed from the rows definition.  This
 * was done because the values change over time.  Therefore we retrieve the data
 * only when it is requested for, instead of storing stale data.
 */
typedef struct kamailioSIPContactTable_context_s 
{
	netsnmp_index index; /** THIS MUST BE FIRST!!! */

	unsigned long kamailioSIPContactIndex;

	unsigned char *kamailioSIPContactURI;
	long           kamailioSIPContactURI_len;

	/* A pointer to the rest of the contact structure, giving us access to
	 * kamailioSIPContactURI, kamailioSIPContactExpirty, and
	 * kamailioSIPContactPreference. */
	ucontact_t *contactInfo;

	void * data;

} kamailioSIPContactTable_context;

/******************************/
/* Customized SNMP Prototypes */
/******************************/

/* 
 * Creates an SNMP row and inserts it into the contact table. This function
 * should only be called when the interprocess buffer is being consumed.
 *
 * Returns: 1 on success, and 0 otherwise. 
 */
int  createContactRow(int userIndex, int contactIndex, char *contactName,
		ucontact_t *contactInfo);

/* 
 * Removes the row indexed by userIndex and contactIndex, and free's up the
 * memory allocated to it.  If the row could not be found, then nothing is done.
 */
void deleteContactRow(int userIndex, int contactIndex);


/* 
 * This function adds a new contactToIndexStruct_t record to the front of
 * 'contactRecord'.  
 *
 * The structure is used to map a contact name to the SNMPStats modules integer
 * indexing scheme.  It will be used later when a delete command comes in, and
 * we need to find out which SNMP row the information is stored under.
 */
int  insertContactRecord(contactToIndexStruct_t **contactRecord, int index, 
		char *name);

/*
 * This function will remove the contactToIndexStruct_T record matching
 * 'contactName' from the users contactToIndexStruct_t linked-list, and return
 * the records index.  In the event that the record could not be found, 0 will
 * be returned. 
 */
int  deleteContactRecord(contactToIndexStruct_t **contactRecord, 
		char *contactName);


/********************************/
/* Normal SNMP Table Prototypes */
/********************************/

/*
 * Initializes the kamailioSIPContactTable module.  This involves:
 *
 *  1) Registering the tables OID with the master agent
 *
 *  2) Creating a default row, so that there is a row to query to trigger the
 *     consumption of the interprocess buffer.
 */
void  init_kamailioSIPContactTable(void);


/*
 * Initialize the kamailioSIPContactTable table by defining its contents and how
 * it's structured.
 *
 * This function is mostly auto-generated.
 */
void  initialize_table_kamailioSIPContactTable(void);

const kamailioSIPContactTable_context * kamailioSIPContactTable_get_by_idx(
		netsnmp_index *);

const kamailioSIPContactTable_context * kamailioSIPContactTable_get_by_idx_rs(
		netsnmp_index *, int row_status);

/* This routine is called to process get requests for elements of the table. */
int   kamailioSIPContactTable_get_value(netsnmp_request_info *, netsnmp_index *,
		netsnmp_table_request_info *);


/* oid declarations */
extern oid    kamailioSIPContactTable_oid[];
extern size_t kamailioSIPContactTable_oid_len;

#define kamailioSIPContactTable_TABLE_OID KAMAILIO_OID,3,1,2,1,5,7
    
/*************************************************************
 * column number definitions for table kamailioSIPContactTable
 */
#define COLUMN_KAMAILIOSIPCONTACTINDEX       1
#define COLUMN_KAMAILIOSIPCONTACTDISPLAYNAME 2
#define COLUMN_KAMAILIOSIPCONTACTURI         3
#define COLUMN_KAMAILIOSIPCONTACTLASTUPDATED 4
#define COLUMN_KAMAILIOSIPCONTACTEXPIRY      5
#define COLUMN_KAMAILIOSIPCONTACTPREFERENCE  6

#define kamailioSIPContactTable_COL_MIN 2
#define kamailioSIPContactTable_COL_MAX 6

#ifdef __cplusplus
}
#endif

#endif /** KAMAILIOSIPCONTACTTABLE_H */
