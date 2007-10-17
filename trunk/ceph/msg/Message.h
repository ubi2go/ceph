// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */

#ifndef __MESSAGE_H
#define __MESSAGE_H
 
#define MSG_CLOSE 0

#define MSG_STATFS       1
#define MSG_STATFS_REPLY 2
#define MSG_PGSTATS      3

#define MSG_PING        10
#define MSG_PING_ACK    11

#define MSG_SHUTDOWN    99999

#define MSG_MON_COMMAND            13
#define MSG_MON_COMMAND_ACK        14


#define MSG_MON_ELECTION           15

#define MSG_MON_OSDMAP_INFO            20
#define MSG_MON_OSDMAP_LEASE           21
#define MSG_MON_OSDMAP_LEASE_ACK       22
#define MSG_MON_OSDMAP_UPDATE_PREPARE  23
#define MSG_MON_OSDMAP_UPDATE_ACK      24
#define MSG_MON_OSDMAP_UPDATE_COMMIT   25

#define MSG_MON_PAXOS              30

#define MSG_OSD_OP           40    // delete, etc.
#define MSG_OSD_OPREPLY      41    // delete, etc.
#define MSG_OSD_PING         42

#define MSG_OSD_GETMAP       43
#define MSG_OSD_MAP          44

#define MSG_OSD_BOOT         45
#define MSG_OSD_MKFS_ACK     46

#define MSG_OSD_FAILURE      47

#define MSG_OSD_IN           48
#define MSG_OSD_OUT          49



#define MSG_OSD_PG_NOTIFY      50
#define MSG_OSD_PG_QUERY       51
#define MSG_OSD_PG_SUMMARY     52
#define MSG_OSD_PG_LOG         53
#define MSG_OSD_PG_REMOVE      54
#define MSG_OSD_PG_ACTIVATE_SET 55

// -- client --
// to monitor
#define MSG_CLIENT_MOUNT           60
#define MSG_CLIENT_UNMOUNT         61

// to mds
#define MSG_CLIENT_SESSION         70   // start or stop
#define MSG_CLIENT_RECONNECT       71

#define MSG_CLIENT_REQUEST         80
#define MSG_CLIENT_REQUEST_FORWARD 81
#define MSG_CLIENT_REPLY           82
#define MSG_CLIENT_FILECAPS        83



// *** MDS ***

#define MSG_MDS_GETMAP             102
#define MSG_MDS_MAP                103
#define MSG_MDS_HEARTBEAT          104  // for mds load balancer
#define MSG_MDS_BEACON             105  // to monitor

#define MSG_MDS_RESOLVE            106
#define MSG_MDS_RESOLVEACK         107

#define MSG_MDS_CACHEREJOIN        108

#define MSG_MDS_DISCOVER           110
#define MSG_MDS_DISCOVERREPLY      111

#define MSG_MDS_INODEGETREPLICA    112
#define MSG_MDS_INODEGETREPLICAACK 113

#define MSG_MDS_INODEFILECAPS      115

#define MSG_MDS_INODEUPDATE  120
#define MSG_MDS_DIRUPDATE    121
#define MSG_MDS_INODEEXPIRE  122
#define MSG_MDS_DIREXPIRE    123

#define MSG_MDS_DIREXPIREREQ 124

#define MSG_MDS_CACHEEXPIRE  125

#define MSG_MDS_ANCHOR 130

#define MSG_MDS_FRAGMENTNOTIFY 140

#define MSG_MDS_EXPORTDIRDISCOVER     149
#define MSG_MDS_EXPORTDIRDISCOVERACK  150
#define MSG_MDS_EXPORTDIRCANCEL       151
#define MSG_MDS_EXPORTDIRPREP         152
#define MSG_MDS_EXPORTDIRPREPACK      153
#define MSG_MDS_EXPORTDIRWARNING      154
#define MSG_MDS_EXPORTDIRWARNINGACK   155
#define MSG_MDS_EXPORTDIR             156
#define MSG_MDS_EXPORTDIRACK          157
#define MSG_MDS_EXPORTDIRNOTIFY       158
#define MSG_MDS_EXPORTDIRNOTIFYACK    159
#define MSG_MDS_EXPORTDIRFINISH       160

#define MSG_MDS_SLAVE_REQUEST         170

#define MSG_MDS_DENTRYUNLINK      200

#define MSG_MDS_LOCK             500

#define MSG_MDS_SHUTDOWNSTART  900
#define MSG_MDS_SHUTDOWNFINISH 901


#include <stdlib.h>
#include <cassert>

#include <iostream>
#include <list>
using std::list;

#include <ext/hash_map>


#include "include/types.h"
#include "include/buffer.h"
#include "msg_types.h"




// ======================================================

// abstract Message class


class Message {
 private:
  
 protected:
  ceph_message_header  env;    // envelope
  bufferlist      payload;        // payload
  list<int> chunk_payload_at;
  
  utime_t recv_stamp;

  friend class Messenger;
public:

 public:
  Message() { 
    env.source_port = env.dest_port = 0;
    env.nchunks = 0;
  };
  Message(int t) {
    env.source_port = env.dest_port = 0;
    env.nchunks = 0;
    env.type = t;
  }
  virtual ~Message() {
  }


  void clear_payload() { payload.clear(); }
  bool empty_payload() { return payload.length() == 0; }
  bufferlist& get_payload() {
    return payload;
  }
  void set_payload(bufferlist& bl) {
    payload.claim(bl);
  }
  void copy_payload(const bufferlist& bl) {
    payload = bl;
  }
  const list<int>& get_chunk_payload_at() const { return chunk_payload_at; }
  void set_chunk_payload_at(list<int>& o) { chunk_payload_at.swap(o); }
  ceph_message_header& get_envelope() {
    return env;
  }
  void set_envelope(ceph_message_header& env) {
    this->env = env;
  }


  void set_recv_stamp(utime_t t) { recv_stamp = t; }
  utime_t get_recv_stamp() { return recv_stamp; }

  // ENVELOPE ----

  // type
  int get_type() { return env.type; }
  void set_type(int t) { env.type = t; }
  virtual char *get_type_name() = 0;

  // source/dest
  entity_inst_t& get_dest_inst() { return *(entity_inst_t*)&env.dst; }
  void set_dest_inst(entity_inst_t& inst) { env.dst = *(ceph_entity_inst*)&inst; }

  entity_inst_t& get_source_inst() { return *(entity_inst_t*)&env.src; }
  void set_source_inst(entity_inst_t& inst) { env.src = *(ceph_entity_inst*)&inst; }

  entity_name_t& get_dest() { return *(entity_name_t*)&env.dst.name; }
  void set_dest(entity_name_t a, int p) { env.dst.name = *(ceph_entity_name*)&a; env.dest_port = p; }
  int get_dest_port() { return env.dest_port; }
  void set_dest_port(int p) { env.dest_port = p; }
  
  entity_name_t& get_source() { return *(entity_name_t*)&env.src.name; }
  void set_source(entity_name_t a, int p) { env.src.name = *(ceph_entity_name*)&a; env.source_port = p; }
  int get_source_port() { return env.source_port; }

  entity_addr_t& get_source_addr() { return *(entity_addr_t*)&env.src.addr; }
  void set_source_addr(const entity_addr_t &i) { env.src.addr = *(ceph_entity_addr*)&i; }

  // PAYLOAD ----
  void reset_payload() {
    payload.clear();
  }

  virtual void decode_payload() = 0;
  virtual void encode_payload() = 0;

  virtual void print(ostream& out) {
    out << get_type_name();
  }
  
};

extern Message *decode_message(ceph_message_header &env, bufferlist& bl);
inline ostream& operator<<(ostream& out, Message& m) {
  m.print(out);
  return out;
}

#endif
