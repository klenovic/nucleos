/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VFS_PARAM_H
#define __SERVERS_VFS_PARAM_H

/* The following names are synonyms for the variables in the input message. */
#define addr	      m_data3
#define buffer	      m_data4
#define child_endpt      m_data2
#define co_mode	      m_data1
#define fd	      m_data1
#define fd2	      m_data2
#define group	      m_data3
#define ls_fd	      m_data1
#define mk_mode	      m_data2
#define mk_z0	      m_data3
#define mode	      m_data2
#define c_mode        m_data3
#define c_name        m_data4
#define name	      m_data3
#define flength       m_data4
#define name1	      m_data4
#define name2	      m_data5
#define	name_length   m_data1
#define name1_length  m_data1
#define name2_length  m_data2
#define nbytes        m_data2
#define owner	      m_data2
#define pathname      m_data10
#define pid	      m_data3
#define ENDPT	      m_data1
#define offset_lo     m_data4
#define offset_high   m_data5
#define ctl_req       m_data1
#define driver_nr     m_data2
#define dev_nr	      m_data3
#define dev_style     m_data4
#define m_force	      m_data5
#define rd_only	      m_data3
#define request       m_data2
#define sig	      m_data2
#define endpt1	      m_data1
#define fs_endpt      m_data6
#define tp	      m_data4
#define utime_actime  m_data4
#define utime_modtime m_data5
#define utime_file    m_data6
#define utime_length  m_data1
#define utime_strlen  m_data2
#define whence	      m_data2
#define svrctl_req    m_data1
#define svrctl_argp   m_data6
#define info_what     m_data1
#define info_where    m_data4
#define md_label	m_data6
#define md_label_len	m_data4
#define md_major	m_data1
#define md_style	m_data2
#define md_force	m_data3

/* The following names are synonyms for the variables in the output message. */
#define reply_type    m_type
#define reply_l1      m_data4
#define reply_l2      m_data5
#define reply_i1      m_data1
#define reply_i2      m_data2

#endif /* __SERVERS_VFS_PARAM_H */
