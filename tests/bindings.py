#!/usr/bin/python3.3
#

import interval_u32
from interval_u32 import interval_u32
from interval_u32 import interval_list_u32

def show_set(s):
	ret = ""
	for n in s:
		ret += str(n) + ","
	print(ret)

u0 = interval_u32()
u0.assign(4,11)

u1 = interval_u32()
u1.assign(20,40)

l = interval_list_u32()
l.add(u0)
l.add(u1)
l.aggregate()
l.random_sets(10, lambda s: show_set(s))
