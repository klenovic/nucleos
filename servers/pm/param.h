/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __PM_PARAM_H
#define __PM_PARAM_H

/* The following names are synonyms for the variables in the input message. */
#define exec_name	m_data4
#define exec_len	m_data1
#define grp_id		m_data1
#define grp_no		m_data1
#define groupsp		m_data4
#define namelen		m_data2
#define pid		m_data1
#define seconds		m_data1
#define which_timer	m_data1
#define new_val		m_data4
#define old_val		m_data5
#define stack_bytes	m_data2
#define stack_ptr	m_data5
#define status		m_data1
#define usr_id		m_data1
#define request		m_data2
#define data		m_data5
#define sig_nr		m_data2
#define sig_nsa		m_data4
#define sig_osa		m_data5
#define sig_ret		m_data6
#define stat_nr		m_data1
#define sig_set		m_data4
#define sig_how		m_data1
#define sig_context	m_data6
#define info_what	m_data1
#define info_where	m_data4
#define reboot_flag	m_data1
#define reboot_code	m_data4
#define reboot_strlen	m_data2
#define svrctl_req	m_data1
#define svrctl_argp	m_data6
#define stime		m_data4
#define memsize		m_data1
#define membase		m_data2
#define sysuname_req	m_data1
#define sysuname_field	m_data2
#define sysuname_len	m_data3
#define sysuname_value	m_data4

/* The following names are synonyms for the variables in a reply message. */
#define reply_res	m_type
#define reply_res2	m_data1
#define reply_res3	m_data2
#define reply_ptr	m_data6
#define reply_mask	m_data4
#define reply_trace	m_data5
#define reply_time	m_data4
#define reply_utime	m_data5
#define reply_t1	m_data1
#define reply_t2	m_data2
#define reply_t3	m_data3
#define reply_t4	m_data4
#define reply_t5	m_data5

#endif /* __PM_PARAM_H */
