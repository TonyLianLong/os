import os,sys
argc = len(sys.argv)
if argc == 3:
	filesize = os.path.getsize(sys.argv[1])
	f = open(sys.argv[1], 'ab')
	i = 0
	if int(sys.argv[2])-filesize < 0:
		print "File is too big"
		exit(1)
	if (int(sys.argv[2])-filesize)%512 == 0:
		zero512 = ''
		while i < 512:
			zero512 += '\0'
			i += 1
		i = 0
		while i < (int(sys.argv[2])-filesize)/512:
			f.write(zero512)
			i=i+1
	else:
		while i < int(sys.argv[2])-filesize:
			f.write('\0')
			i=i+1
	f.close()
else:
	print "fill.py [file] [size]"
