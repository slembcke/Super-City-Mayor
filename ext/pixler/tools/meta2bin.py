import sys
import struct
import base64
import xml.etree.ElementTree

filename = sys.argv[1]

map = xml.etree.ElementTree.parse(filename).getroot()
data = map.find('layer/data')
data_u32 = base64.b64decode(data.text).strip()

format = "{0}I".format(len(data_u32)/4)
values = struct.unpack(format, data_u32)
values = [(e - 1) & 0xFF for e in values]

meta0 = []
meta1 = []
meta2 = []
meta3 = []

for row in range(8):
	cursor = row*32 + 0
	meta0 += values[cursor+0 : cursor+16 : 2]
	meta1 += values[cursor+1 : cursor+16 : 2]
	cursor = row*32 + 16
	meta2 += values[cursor+0 : cursor+16 : 2]
	meta3 += values[cursor+1 : cursor+16 : 2]

meta_tiles = meta0 + meta1 + meta2 + meta3

format = "{0}B".format(len(meta_tiles))
data_u8 = struct.pack(format, *meta_tiles)

sys.stdout.write(data_u8)
