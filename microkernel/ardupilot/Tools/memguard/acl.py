#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

class ACL:
    def __init__(self, name):
        self.name = name
        self.addrs = list()
        self.aces = list()

class ACE:
    def __init__(self, addr, size, perm, used):
        self.addr = addr
        self.size = size
        self.perm = perm
        self.used = used

    def __str__(self):
        s = "%08x-%08x %-3s" % (self.addr, self.addr+self.size, self.perm)
        if self.used is not None:
            s += (" # %d/%d (%.2f%%)" % \
                  (self.used, self.size, float(self.used)*100/self.size))
        return s

    def __cmp__(self, other):
        return cmp(self.addr, other.addr)
    
class Permission:
    def __init__(self, mod):
        if type(mod) is str:
            self.mod = 0
            if 'r' in mod:
                self.mod += 4
            if 'w' in mod:
                self.mod += 2
            if 'x' in mod:
                self.mod += 1
            return
        self.mod = int(mod)
    
    def __str__(self):
        s = str()
        if self.mod & 4 != 0:
            s += 'r'
        if self.mod & 2 != 0:
            s += 'w'
        if self.mod & 1 != 0:
            s += 'x'
        return s

    def __eq__(self, other):
        return self.mod == other.mod
    
    def __ne__(self, other):
        return self.mod != other.mod
    
    def __add__(self, other):
        return Permission(self.mod | other.mod)
